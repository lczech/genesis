#ifndef GENESIS_PLACEMENT_FORMATS_NEWICK_READER_H_
#define GENESIS_PLACEMENT_FORMATS_NEWICK_READER_H_

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <assert.h>
#include <stdexcept>

#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/function/helper.hpp"

#include "genesis/tree/default/newick_reader.hpp"
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
        auto& edge_data = edge.data<PlacementEdgeData>();

        // Process the edge num.
        edge_data.reset_edge_num(-1);
        if (element.tags.size() == 0) {
            throw std::invalid_argument(
                "Edge at node '" + element.name + "' does not contain a tag value like '{42}'" +
                " for the placement edge_num of this edge."
            );
        }
        if (element.tags.size() > 1) {
            throw std::invalid_argument(
                "Edge at node '" + element.name + "' contains more than one tag value like " +
                "'{xyz}'. Expecting only one for the placement edge_num of this edge."
            );
        }
        assert(element.tags.size() == 1);
        edge_data.reset_edge_num( std::stoi( element.tags[0] ));
    }

    void finish_reading( tree::NewickBroker const& broker, tree::Tree& tree ) const
    {
        (void) broker;
        if( ! has_correct_edge_nums( tree )) {
            LOG_INFO << "Placement Tree does not have edge_nums that are increasing with a "
                     << "post-order traversal of the tree, as is demanded by the jplace standard. "
                     << "Genesis can still work with this tree, but it might indicate an issue "
                     << "with the data.";
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
            [&]( tree::NewickBroker const& broker, tree::Tree& tree ) {
                finish_reading( broker, tree );
            }
        );
    }

};

// =================================================================================================
//     Placement Tree Newick Reader
// =================================================================================================

class PlacementTreeNewickReader
    : public tree::NewickReader
    , public tree::DefaultTreeNewickReaderPlugin
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
        DefaultTreeNewickReaderPlugin::register_with( *this );
        PlacementTreeNewickReaderPlugin::register_with( *this );
    }
};

} // namespace placement
} // namespace genesis

#endif // include guard
