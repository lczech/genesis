#ifndef GENESIS_TREE_FORMATS_NEWICK_SIMPLE_WRITER_H_
#define GENESIS_TREE_FORMATS_NEWICK_SIMPLE_WRITER_H_

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

#include "genesis/tree/common_tree/newick_writer.hpp"
#include "genesis/tree/formats/newick/simple_tree.hpp"
#include "genesis/tree/formats/newick/writer.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Simple Newick Tree Newick Writer Plugin
// =================================================================================================

/**
 * @brief
 */
class SimpleNewickTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    SimpleNewickTreeNewickWriterPlugin() = default;
    virtual ~SimpleNewickTreeNewickWriterPlugin() = default;

    SimpleNewickTreeNewickWriterPlugin(SimpleNewickTreeNewickWriterPlugin const&) = default;
    SimpleNewickTreeNewickWriterPlugin(SimpleNewickTreeNewickWriterPlugin&&)      = default;

    SimpleNewickTreeNewickWriterPlugin& operator= (
        SimpleNewickTreeNewickWriterPlugin const&
    ) = default;
    SimpleNewickTreeNewickWriterPlugin& operator= (
        SimpleNewickTreeNewickWriterPlugin&&
    ) = default;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void node_to_element( TreeNode const& node, NewickBrokerElement& element ) const
    {
        // Add all comments stored in the node to the element.
        // We push them back, in case that some other plugin also wants to add data.
        auto& node_data = node.data<SimpleNewickNodeData>();
        for( auto const& comment : node_data.comments ) {
            element.comments.push_back( comment );
        }
    }

    void edge_to_element( TreeEdge const& edge, NewickBrokerElement& element ) const
    {
        // Add all additiona values and tags to the element.
        // We push them back, in case that some other plugin also wants to add data.
        auto& edge_data = edge.data<SimpleNewickEdgeData>();
        for( auto const& value : edge_data.values ) {
            element.values.push_back( value );
        }
        for( auto const& tag : edge_data.tags ) {
            element.tags.push_back( tag );
        }
    }

    void register_with( NewickWriter& writer ) const
    {
        writer.node_to_element_plugins.push_back(
            [&]( TreeNode const& node, NewickBrokerElement& element ) {
                node_to_element( node, element );
            }
        );

        writer.edge_to_element_plugins.push_back(
            [&]( TreeEdge const& edge, NewickBrokerElement& element ) {
                edge_to_element( edge, element );
            }
        );
    }
};

// =================================================================================================
//     SimpleNewick Tree Newick Writer
// =================================================================================================

class SimpleNewickTreeNewickWriter
    : public NewickWriter
    , public CommonTreeNewickWriterPlugin
    , public SimpleNewickTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    SimpleNewickTreeNewickWriter()
    {
        CommonTreeNewickWriterPlugin::register_with( *this );
        SimpleNewickTreeNewickWriterPlugin::register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
