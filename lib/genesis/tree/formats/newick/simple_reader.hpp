#ifndef GENESIS_TREE_FORMATS_NEWICK_SIMPLE_READER_H_
#define GENESIS_TREE_FORMATS_NEWICK_SIMPLE_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/formats/newick/simple_tree.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Simple Newick Tree Newick Reader Plugin
// =================================================================================================

/**
 * @brief
 */
class SimpleNewickTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    SimpleNewickTreeNewickReaderPlugin() = default;
    virtual ~SimpleNewickTreeNewickReaderPlugin() = default;

    SimpleNewickTreeNewickReaderPlugin(SimpleNewickTreeNewickReaderPlugin const&) = default;
    SimpleNewickTreeNewickReaderPlugin(SimpleNewickTreeNewickReaderPlugin&&)      = default;

    SimpleNewickTreeNewickReaderPlugin& operator= (
        SimpleNewickTreeNewickReaderPlugin const&
    ) = default;
    SimpleNewickTreeNewickReaderPlugin& operator= (
        SimpleNewickTreeNewickReaderPlugin&&
    ) = default;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void element_to_node( NewickBrokerElement const& element, TreeNode& node ) const
    {
        // Get the node comments and store them in our tree
        auto& node_data = node.data<SimpleNewickNodeData>();
        node_data.comments = element.comments;
    }

    void element_to_edge( NewickBrokerElement const& element, TreeEdge& edge ) const
    {
        // Get the edge values (including branch lenght, but then remove it), and tags.
        auto& edge_data = edge.data<SimpleNewickEdgeData>();
        edge_data.values = element.values;
        if( edge_data.values.size() > 0 ) {
            edge_data.values.erase( edge_data.values.begin() );
        }
        edge_data.tags = element.tags;
    }

    void register_with( NewickReader& reader ) const
    {
        // Set node data creation function.
        reader.create_node_data_plugin = []( TreeNode& node ){
            node.reset_data( SimpleNewickNodeData::create() );
        };

        // Set edge data creation function.
        reader.create_edge_data_plugin = []( TreeEdge& edge ){
            edge.reset_data( SimpleNewickEdgeData::create() );
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
};

// =================================================================================================
//     Simple Newick Tree Newick Reader
// =================================================================================================

class SimpleNewickTreeNewickReader
    : public NewickReader
    , public CommonTreeNewickReaderPlugin
    , public SimpleNewickTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    SimpleNewickTreeNewickReader()
    {
        // We explicitly use tags here. Activate them!
        enable_tags( true );

        // We first register the default reader, then the tree reader, because the latter
        // overwrites the data creation functions.
        CommonTreeNewickReaderPlugin::register_with( *this );
        SimpleNewickTreeNewickReaderPlugin::register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
