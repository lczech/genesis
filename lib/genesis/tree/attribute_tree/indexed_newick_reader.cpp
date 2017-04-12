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

#include "genesis/tree/attribute_tree/indexed_newick_reader.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Settings
// =================================================================================================

IndexedAttributeTreeNewickReaderPlugin& IndexedAttributeTreeNewickReaderPlugin::add_attribute(
    Source             source,
    size_t             index,
    Target             target,
    std::string const& target_key
) {
    indexed_attributes_.push_back({ source, index, target, target_key, "", false });
    return *this;
}

IndexedAttributeTreeNewickReaderPlugin& IndexedAttributeTreeNewickReaderPlugin::add_attribute(
    Source             source,
    size_t             index,
    Target             target,
    std::string const& target_key,
    std::string const& default_value
) {
    indexed_attributes_.push_back({ source, index, target, target_key, default_value, true });
    return *this;
}

IndexedAttributeTreeNewickReaderPlugin& IndexedAttributeTreeNewickReaderPlugin::add_catch_all(
    Source             source,
    Target             target,
    std::string const& target_key_prefix
) {
    catch_all_attributes_.push_back({ source, 0, target, target_key_prefix, "", false });
    return *this;
}

IndexedAttributeTreeNewickReaderPlugin& IndexedAttributeTreeNewickReaderPlugin::add_catch_all(
    Target target
) {
    add_catch_all( Source::kComment, target, "comment_" );
    add_catch_all( Source::kTag,     target, "tag_" );
    add_catch_all( Source::kValue,   target, "value_" );
    return *this;
}

void IndexedAttributeTreeNewickReaderPlugin::clear()
{
    indexed_attributes_.clear();
    catch_all_attributes_.clear();
}

// =================================================================================================
//     Plugin Functions
// =================================================================================================

void IndexedAttributeTreeNewickReaderPlugin::element_to_node(
    NewickBrokerElement const& element,
    TreeNode& node
) const {
    auto& attributes = node.data<AttributeTreeNodeData>().attributes;
    process_indexed_attributes_(   element, attributes, Target::kNode );
    process_catch_all_attributes_( element, attributes, Target::kNode );
}

void IndexedAttributeTreeNewickReaderPlugin::element_to_edge(
    NewickBrokerElement const& element,
    TreeEdge& edge
) const {
    auto& attributes = edge.data<AttributeTreeEdgeData>().attributes;
    process_indexed_attributes_(   element, attributes, Target::kEdge );
    process_catch_all_attributes_( element, attributes, Target::kEdge );
}

void IndexedAttributeTreeNewickReaderPlugin::register_with( NewickReader& reader ) const
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

std::vector<std::string> const& IndexedAttributeTreeNewickReaderPlugin::get_attribute_source_(
    NewickBrokerElement const& element,
    Source                     source
) const {
    switch( source ) {
        case Source::kValue: {
            return element.values;
        }
        case Source::kComment: {
            return element.comments;
        }
        case Source::kTag: {
            return element.tags;
        }
        default: {
            // This can only happen if we misuse the enum.
            throw std::runtime_error( "Invalid source type for indexed attribute." );
        }
    }

    // Make compiler happy. Need to do something here.
    throw std::runtime_error( "Invalid source type for indexed attribute." );
}

void IndexedAttributeTreeNewickReaderPlugin::process_indexed_attributes_(
    NewickBrokerElement const& element,
    AttributeTreeMap& attributes,
    Target target
) const {
    // Process indexed attributes...
    for( auto const& attrs : indexed_attributes_ ) {

        // ... but only if they are for our target.
        if( attrs.target != target ) {
            continue;
        }

        // Find the correct source element.
        auto const& src = get_attribute_source_( element, attrs.source );

        // Add the value at the index to the attribute map, or use default if index invalid.
        if( attrs.index < src.size() ) {
            attributes[ attrs.target_key ] = src[ attrs.index ];
        } else if( attrs.use_default ) {
            attributes[ attrs.target_key ] = attrs.default_value;
        }
    }
}

void IndexedAttributeTreeNewickReaderPlugin::process_catch_all_attributes_(
    NewickBrokerElement const& element,
    AttributeTreeMap& attributes,
    Target target
) const {
    // Process catch all attributes...
    for( auto const& attrs : catch_all_attributes_ ) {

        // ... but only if they are for our target.
        if( attrs.target != target ) {
            continue;
        }

        // Find the correct source element.
        auto const& src = get_attribute_source_( element, attrs.source );

        // Add all source attributes to the target map.
        size_t cnt = 0;
        for( auto const& elem : src ) {
            attributes[ attrs.target_key + std::to_string( cnt ) ] = elem;
            ++cnt;
        }
    }
}

} // namespace tree
} // namespace genesis
