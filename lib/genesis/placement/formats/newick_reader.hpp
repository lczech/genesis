#ifndef GENESIS_PLACEMENT_FORMATS_NEWICK_READER_H_
#define GENESIS_PLACEMENT_FORMATS_NEWICK_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <cassert>
#include <stdexcept>
#include <set>

#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/function/helper.hpp"

#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/formats/newick/reader.hpp"

#include "genesis/utils/core/logging.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Newick Reader Plugin
// =================================================================================================

/**
 * @brief
 */
class PlacementTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PlacementTreeNewickReaderPlugin() = default;
    virtual ~PlacementTreeNewickReaderPlugin() = default;

    PlacementTreeNewickReaderPlugin(PlacementTreeNewickReaderPlugin const&) = default;
    PlacementTreeNewickReaderPlugin(PlacementTreeNewickReaderPlugin&&)      = default;

    PlacementTreeNewickReaderPlugin& operator= (PlacementTreeNewickReaderPlugin const&) = default;
    PlacementTreeNewickReaderPlugin& operator= (PlacementTreeNewickReaderPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void element_to_edge( tree::NewickBrokerElement const& element, tree::TreeEdge& edge ) const
    {
        // Set defaults.
        auto& edge_data = edge.data<PlacementEdgeData>();
        edge_data.reset_edge_num( -1 );

        // Depending on the setting, we either use Newick tags {} or comments [] to get the edge nums.
        auto const& en_vec = ( get_edge_num_from_comments_ ? element.comments : element.tags );

        // Process the edge num.
        if( en_vec.size() == 1 ) {
            edge_data.reset_edge_num( std::stoi( en_vec[0] ));

        } else {
            // Error cases.
            // Check which error occured and report accordingly.

            // Get a nice readable name.
            auto name = element.name;
            if( name.empty() ) {
                name = "inner node";
            } else {
                name = "node '" + name + "'";
            }

            // If there is no edge num value at all, we keep it at the default
            // of -1, which will then be fixed later in finish_reading().
            // We do not warn about this here, as this might just clutter the output too much
            // if there are several edges with missing edge_nums.
            // if( en_vec.size() == 0 ) {
            //     LOG_WARN << "Jplace document contains a Newick tree where the edge at " << name
            //              << " does not contain an edge_num value. "
            //              << "This might be because the document uses an old jplace standard "
            //              << "where edges at the root do not have an edge_num. "
            //              << "We can still work with this tree, but it might also indicate "
            //              << "a more severe issue with the data.";
            // }

            // Cannot cope with multiple values, as we would not know which one is the correct
            // one intended to be used as edge num.
            if( en_vec.size() > 1 ) {
                if( get_edge_num_from_comments_ ) {
                    throw std::invalid_argument(
                        "Edge at " + name + " contains more than one comment value such as "
                        "'[xyz]'. Expecting only one for the placement edge_num of this edge."
                    );
                } else {
                    throw std::invalid_argument(
                        "Edge at " + name + " contains more than one tag value such as "
                        "'{xyz}'. Expecting only one for the placement edge_num of this edge."
                    );
                }
            }
        }
    }

    void finish_reading( tree::Tree& tree ) const
    {
        // Get a list of all used edge nums and check their uniqueness.
        // This is a bit wasteful, as we later do a similar check in the JplaceReader,
        // but we kind of need this here anyway to correctly fix missing edge nums.
        std::set<PlacementEdgeData::EdgeNumType> edge_nums;
        for( auto const& edge : tree.edges() ) {
            auto const edge_num = edge.data<PlacementEdgeData>().edge_num();

            // Check for uniqueness. We leave out -1 here, just in case that there are multiple
            // edges that did not get a proper edge num in the file. This will be fixed later anyway.
            if( edge_num > -1 && edge_nums.count( edge_num ) > 0 ) {
                throw std::invalid_argument(
                    "Jplace document contains a Newick tree where the edge_num '"
                    + std::to_string( edge_num ) + "' occurs more than once, "
                    "and hence cannot be used to uniquely identify edges of the tree. "
                    "This indicates a severe issue with the program that created the jplace file."
                );
            }
            edge_nums.insert( edge_num );
        }

        // Some more safety for the user.
        if( tree.empty() ) {
            throw std::invalid_argument( "Jplace document contains an empty Newick tree." );
        }
        if( tree.edge_count() >= 3 && edge_nums.size() < tree.edge_count() - 3 ) {
            // We "allow" 3 edges without edge num before we warn. This can for example be
            // edges at the root. While having edges without edge num seems to be a thing that only
            // occurs with SEPP and the old jplace standard version 1, we still allow for this,
            // just to be nice. But anything above this is highly suspicious (even more so than
            // not having proper edge nums in the first place), as all these edges without edge nums
            // are not identifiable and hence cannot receive any placements.
            // So, let's stop being nice.
            throw std::invalid_argument(
                "Jplace document contains too many edges without an edge_num. We can cope with a few "
                "of them missing. But as none of them can receive any placements, it does not make "
                "sense if too many are missing. This hence indicates a severe issue with the program "
                "that created the jplace file. Possibly, the provided jplace version (1-3) does not "
                "match the format used to specify the edge_num values in the tree."
            );
        }

        // If there are edge nums that were not set by the element_to_edge() function,
        // we assume that those are some weird edge cases such as an edge at the root.
        // This seems to be allowed in jplace version 1, or at least seems to be created by SEPP.
        // It is not a good thing, but we try to cope and fix this.
        // As these edge cannot have any placements (because there is no way of referring to them
        // from the pqueries), we simply set a dummy value here, in order for the rest of our code
        // to have working edge nums.
        if( edge_nums.count( -1 ) > 0 ) {
            LOG_WARN << "Jplace document contains a Newick tree where not all edges have a proper "
                     << "edge_num assigned to them. This might be because the document uses "
                     << "an old jplace standard (version 1), where the edge at the root does not "
                     << "have an edge_num. We can still work with this tree, but it might also "
                     << "indicate a more severe issue with the data.";

            for( auto& edge : tree.edges() ) {
                auto& edge_data = edge.data<PlacementEdgeData>();
                if( edge_data.edge_num() == -1 ) {

                    // Get the next bigger number that is not yet used as an edge num.
                    // There must be values in the edge_num set, as we checked at least for
                    // element -1 above.
                    // Use it for the edge, and make sure that it is added to the set for later
                    // iteration.
                    assert( edge_nums.size() > 0 );
                    auto const next_avail = *(edge_nums.rbegin()) + 1;
                    assert( edge_nums.count( next_avail ) == 0 );
                    edge_data.reset_edge_num( next_avail );
                    edge_nums.insert( next_avail );
                }
            }
        }

        if( ! has_correct_edge_nums( tree )) {
            LOG_WARN << "Jplace document has a Newick tree where the edge_num tags are non standard. "
                     << "They are expected by the jplace standard to be assigned in ascending order "
                     << "via post-order traversal of the tree. We can still work with this tree, "
                     << "but it might indicate an issue with the data.";
        }
    }

    void register_with( tree::NewickReader& reader ) const
    {
        // Set node data creation function.
        reader.create_node_data_plugin = []( tree::TreeNode& node ){
            node.reset_data( PlacementNodeData::create() );
        };

        // Set edge data creation function.
        reader.create_edge_data_plugin = []( tree::TreeEdge& edge ){
            edge.reset_data( PlacementEdgeData::create() );
        };

        // Add edge manipulation functions.
        reader.element_to_edge_plugins.push_back(
            [&]( tree::NewickBrokerElement const& element, tree::TreeEdge& edge ) {
                element_to_edge( element, edge );
            }
        );

        // Add finish reading plugin.
        reader.finish_reading_plugins.push_back(
            [&]( tree::Tree& tree ) {
                finish_reading( tree );
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Set whether to use tags or comments of the Newick tree for the edge nums.
     *
     * In version 1 of the `jplace` standard, the edge num properties of the reference tree
     * were noted down as Newick comments in brackets (e.g., `[42]`) instead of the curly braces
     * used in later versions of the standard (e.g., `{42}`). By default, we use the newer tags.
     * Set this to `true` in order to fall back to the old version using comments instead.
     */
    PlacementTreeNewickReaderPlugin& get_edge_num_from_comments( bool value )
    {
        get_edge_num_from_comments_ = value;
        return *this;
    }

    /**
     * @brief Get whether to use tags or comments of the Newick tree for the edge nums.
     *
     * See the setter function for details.
     */
    bool get_edge_num_from_comments() const
    {
        return get_edge_num_from_comments_;
    }

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

private:

    bool get_edge_num_from_comments_ = false;

};

// =================================================================================================
//     Placement Tree Newick Reader
// =================================================================================================

class PlacementTreeNewickReader
    : public tree::NewickReader
    , public tree::CommonTreeNewickReaderPlugin
    , public PlacementTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PlacementTreeNewickReader()
    {
        // Jplace files use tags. Activate them!
        enable_tags( true );

        // We first register the default reader, then the placement reader, because the latter
        // overwrites the data creation functions.
        CommonTreeNewickReaderPlugin::register_with( *this );
        PlacementTreeNewickReaderPlugin::register_with( *this );
    }
};

} // namespace placement
} // namespace genesis

#endif // include guard
