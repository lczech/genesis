/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of NewickBroker class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/attribute_tree/keyed_newick_reader.hpp"

#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Static Member Initialization
// =================================================================================================

std::vector<KeyedAttributeTreeNewickReaderPlugin::AttributeDescriptor>
KeyedAttributeTreeNewickReaderPlugin::nhx_attributes_ ={
    { "AC", Target::kNode, "AC", "", false }, // sequence accession
    { "Co", Target::kNode, "Co", "", false }, // collapse this node when drawing the tree (default is not to collapse)
    { "D",  Target::kNode, "D",  "", false }, // duplication event
    { "E",  Target::kNode, "E",  "", false }, // EC number
    { "GN", Target::kNode, "GN", "", false }, // gene name
    { "O",  Target::kNode, "O",  "", false }, // orthologous to this external node
    { "S",  Target::kNode, "S",  "", false }, // species name
    { "SO", Target::kNode, "SO", "", false }, // "super orthologous" (no duplications on paths)
    { "T",  Target::kNode, "T",  "", false }, // taxonomy ID

    { "B",  Target::kEdge, "B",  "", false }, // confidence value for parent branch
    { "L",  Target::kEdge, "L",  "", false }, // log likelihood value on parent branch
    { "Sw", Target::kEdge, "Sw", "", false }, // placing a subtree on the parent branch of this node
                                              // makes the tree significantly worse according to
                                              // Kishino/Hasegawa test (or similar)
};

// =================================================================================================
//     Settings
// =================================================================================================

KeyedAttributeTreeNewickReaderPlugin& KeyedAttributeTreeNewickReaderPlugin::add_keyed_attribute(
    std::string const& key,
    Target             target
) {
    keyed_attributes_.push_back({ key, target, key, "", false });
    return *this;
}

KeyedAttributeTreeNewickReaderPlugin& KeyedAttributeTreeNewickReaderPlugin::add_keyed_attribute(
    std::string const& source_key,
    Target             target,
    std::string const& target_key
) {
    keyed_attributes_.push_back({ source_key, target, target_key, "", false });
    return *this;
}

KeyedAttributeTreeNewickReaderPlugin& KeyedAttributeTreeNewickReaderPlugin::add_keyed_attribute(
    std::string const& source_key,
    Target             target,
    std::string const& target_key,
    std::string const& default_value
) {
    keyed_attributes_.push_back({ source_key, target, target_key, default_value, true });
    return *this;
}

KeyedAttributeTreeNewickReaderPlugin& KeyedAttributeTreeNewickReaderPlugin::add_catch_all(
    Target target
) {
    catch_all_attributes_.push_back({ "", target, "", "", true });
    return *this;
}

KeyedAttributeTreeNewickReaderPlugin& KeyedAttributeTreeNewickReaderPlugin::set_nhx_delimiters()
{
    prefix(    "&&NHX" );
    separator( ":" );
    assigner(  "=" );
    return *this;
}

KeyedAttributeTreeNewickReaderPlugin& KeyedAttributeTreeNewickReaderPlugin::add_nhx_attributes()
{
    set_nhx_delimiters();
    for( auto const& attrib : nhx_attributes_ ) {
        keyed_attributes_.push_back( attrib );
    }
    return *this;
}

// =================================================================================================
//     Plugin Functions
// =================================================================================================

void KeyedAttributeTreeNewickReaderPlugin::element_to_node( NewickBrokerElement const& element, TreeNode& node ) const
{
    auto data = get_data_( element );
    auto& attributes = node.data<AttributeTreeNodeData>().attributes;

    process_keyed_attributes_(     data, attributes, Target::kNode );
    process_catch_all_attributes_( data, attributes, Target::kNode );
}

void KeyedAttributeTreeNewickReaderPlugin::element_to_edge( NewickBrokerElement const& element, TreeEdge& edge ) const
{
    auto data = get_data_( element );
    auto& attributes = edge.data<AttributeTreeEdgeData>().attributes;

    process_keyed_attributes_(     data, attributes, Target::kEdge );
    process_catch_all_attributes_( data, attributes, Target::kEdge );
}

void KeyedAttributeTreeNewickReaderPlugin::register_with( NewickReader& reader ) const
{
    // Set node data creation function.
    reader.create_node_data_plugin = []( TreeNode& node ){
        node.reset_data( AttributeTreeNodeData::create() );
    };

    // Set edge data creation function.
    reader.create_edge_data_plugin = []( TreeEdge& edge ){
        edge.reset_data( AttributeTreeEdgeData::create() );
    };

    // Add node manipulation functions.
    reader.element_to_node_plugins.push_back(
        [&]( NewickBrokerElement const& element, TreeNode& node ) {
            element_to_node( element, node );
        }
    );

    // Add edge manipulation functions.
    reader.element_to_edge_plugins.push_back(
        [&]( NewickBrokerElement const& element, TreeEdge& edge ) {
            element_to_edge( element, edge );
        }
    );
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

KeyedAttributeTreeNewickReaderPlugin::PairList KeyedAttributeTreeNewickReaderPlugin::get_data_(
    NewickBrokerElement const& element
) const {
    auto result = PairList();

    // Process all comments and see whether they have the desireed prefix.
    for( auto const& comment : element.comments ) {
        if( ! utils::starts_with( comment, prefix_ ) ) {
            continue;
        }

        // Skip the prefix...
        size_t pos      = prefix_.size();
        size_t last_pos = prefix_.size();

        // ... then split the rest of the comment into key value pairs.
        while( pos < comment.length() ) {

            // Find the next occurence of the separator, limited to the end of the string.
            pos = comment.find( separator_, last_pos );
            if( pos == std::string::npos ) {
               pos = comment.length();
            }

            // We found a key value pair.
            if( pos != last_pos ) {
                // Get the pair. This is a copy. Could be avoided, but I am lazy today.
                assert( pos >= last_pos );
                auto entry = std::string( comment.data() + last_pos, pos - last_pos );

                // Split it according to the assign string.
                auto ass_pos = entry.find( assigner_ );

                // If there is an assign sign, use it to split.
                if( ass_pos != std::string::npos ) {
                    result.emplace_back(
                        utils::trim( entry.substr( 0, ass_pos )),
                        utils::trim( entry.substr( ass_pos + 1 ))
                    );
                }
            }

            last_pos = pos + 1;
        }
    }
    return result;
}

void KeyedAttributeTreeNewickReaderPlugin::process_keyed_attributes_(
    PairList const& data,
    AttributeTreeMap& attributes,
    Target target
) const {
    // Process indexed attributes...
    for( auto const& attrs : keyed_attributes_ ) {

        // ... but only if they are for our target.
        if( attrs.target != target ) {
            continue;
        }

        // Process all data, and add it if it fits the key.
        bool found_key = false;
        for( auto const& datum : data ) {
            if( datum.first == attrs.source_key ) {
                attributes[ attrs.target_key ] = datum.second;
                found_key = true;
            }
        }

        // If we did not find the key in the data, but want to use default, set it.
        if( ! found_key && attrs.use_default ) {
            attributes[ attrs.target_key ] = attrs.default_value;
        }
    }
}

void KeyedAttributeTreeNewickReaderPlugin::process_catch_all_attributes_(
    PairList const& data,
    AttributeTreeMap& attributes,
    Target target
) const {
    // Process catch all attributes...
    for( auto const& attrs : catch_all_attributes_ ) {

        // ... but only if they are for our target.
        if( attrs.target != target ) {
            continue;
        }

        // Process all data and add the key value pairs to the target.
        for( auto const& datum : data ) {
            attributes[ datum.first ] = datum.second;
        }
    }
}

} // namespace tree
} // namespace genesis
