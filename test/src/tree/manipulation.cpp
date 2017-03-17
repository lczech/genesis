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
 * @brief Testing Newick class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/tree/function/manipulation.hpp"

#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/default/newick_reader.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Reroot
// =================================================================================================

void TestReroot( std::string root_node_name, std::string out_nodes, size_t nexts = 0 )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree = DefaultTreeNewickReader().from_string( input );

    auto root_node = find_node( tree, root_node_name );
    ASSERT_NE( nullptr, root_node );

    // Move to the 'next' link of the new root link, so that we get different root links
    // of the new root node.
    auto root_link = &root_node->link();
    for( size_t i = 0; i < nexts; ++i ) {
        root_link = &root_link->next();
    }

    // Root the tree at the given link, validate its pointers.
    reroot( tree, *root_link );
    EXPECT_TRUE( validate_topology( tree ));

    // Build a string of the nodes in levelorder, starting from the new root.
    for( auto it : levelorder( tree ) ) {
        nodes += std::to_string( it.depth() )
              +  it.node().data<DefaultNodeData>().name
              +  " ";
    }

    // Check if the levelorder outcome is correct.
    EXPECT_EQ( out_nodes, utils::trim( nodes )) << " with start node " << root_node_name;
}

TEST( TreeManipulation, Reroot )
{
    // Test normal rerooting.
    TestReroot( "R", "0R 1A 1F 1G 2B 2C 2H 2I 3D 3E" );
    TestReroot( "A", "0A 1R 1B 1C 2F 2G 2D 2E 3H 3I" );
    TestReroot( "B", "0B 1A 2C 2R 3D 3E 3F 3G 4H 4I" );
    TestReroot( "C", "0C 1A 1D 1E 2R 2B 3F 3G 4H 4I" );
    TestReroot( "D", "0D 1C 2E 2A 3R 3B 4F 4G 5H 5I" );
    TestReroot( "E", "0E 1C 2A 2D 3R 3B 4F 4G 5H 5I" );
    TestReroot( "F", "0F 1R 2G 2A 3H 3I 3B 3C 4D 4E" );
    TestReroot( "G", "0G 1R 1H 1I 2A 2F 3B 3C 4D 4E" );
    TestReroot( "H", "0H 1G 2I 2R 3A 3F 4B 4C 5D 5E" );
    TestReroot( "I", "0I 1G 2R 2H 3A 3F 4B 4C 5D 5E" );

    // Test rerooting with a specific link.
    TestReroot( "R", "0R 1F 1G 1A 2H 2I 2B 2C 3D 3E", 1 );
    TestReroot( "R", "0R 1G 1A 1F 2H 2I 2B 2C 3D 3E", 2 );
    TestReroot( "A", "0A 1B 1C 1R 2D 2E 2F 2G 3H 3I", 1 );
    TestReroot( "A", "0A 1C 1R 1B 2D 2E 2F 2G 3H 3I", 2 );
    TestReroot( "C", "0C 1D 1E 1A 2R 2B 3F 3G 4H 4I", 1 );
    TestReroot( "C", "0C 1E 1A 1D 2R 2B 3F 3G 4H 4I", 2 );
    TestReroot( "G", "0G 1H 1I 1R 2A 2F 3B 3C 4D 4E", 1 );
    TestReroot( "G", "0G 1I 1R 1H 2A 2F 3B 3C 4D 4E", 2 );
}

TEST( TreeManipulation, AddNewNodeNodeA )
{
    // We use input with branch length, in order to make sure that new edges have a default one.
    std::string input = "((B:2.0,(D:2.0,E:2.0)C:2.0)A:2.0,F:2.0,(H:2.0,I:2.0)G:2.0)R:2.0;";
    Tree tree = DefaultTreeNewickReader().from_string( input );

    // Find an inner node.
    auto node = find_node( tree, "A" );
    ASSERT_NE( nullptr, node );

    // Add a node.
    auto& edge = add_new_node( tree, *node );

    // Check all indices and validate tree.
    EXPECT_EQ( 9, edge.index() );
    EXPECT_EQ( 5, edge.primary_node().index() );
    EXPECT_EQ( 18, edge.primary_link().index() );
    EXPECT_EQ( 9, edge.primary_link().next().index() );
    EXPECT_EQ( 10, edge.secondary_node().index() );
    EXPECT_EQ( 19, edge.secondary_link().index() );
    EXPECT_TRUE( validate_topology( tree ));

    // Check whether the data pointers were set correctly.
    ASSERT_NO_THROW( edge.secondary_node().data<DefaultNodeData>() );
    EXPECT_EQ( "", edge.secondary_node().data<DefaultNodeData>().name );
    ASSERT_NO_THROW( edge.data<DefaultEdgeData>() );
    EXPECT_EQ( 0.0, edge.data<DefaultEdgeData>().branch_length );
}

TEST( TreeManipulation, AddNewNodeNodeB )
{
    // We use input with branch length, in order to make sure that new edges have a default one.
    std::string input = "((B:2.0,(D:2.0,E:2.0)C:2.0)A:2.0,F:2.0,(H:2.0,I:2.0)G:2.0)R:2.0;";
    Tree tree = DefaultTreeNewickReader().from_string( input );

    // Find a leaf node.
    auto node = find_node( tree, "B" );
    ASSERT_NE( nullptr, node );

    // Add a node.
    auto& edge = add_new_node( tree, *node );

    // Check all indices and validate tree.
    EXPECT_EQ( 9, edge.index() );
    EXPECT_EQ( 9, edge.primary_node().index() );
    EXPECT_EQ( 18, edge.primary_link().index() );
    EXPECT_EQ( 17, edge.primary_link().next().index() );
    EXPECT_EQ( 10, edge.secondary_node().index() );
    EXPECT_EQ( 19, edge.secondary_link().index() );
    EXPECT_TRUE( validate_topology( tree ));

    // Check whether the data pointers were set correctly.
    ASSERT_NO_THROW( edge.secondary_node().data<DefaultNodeData>() );
    EXPECT_EQ( "", edge.secondary_node().data<DefaultNodeData>().name );
    ASSERT_NO_THROW( edge.data<DefaultEdgeData>() );
    EXPECT_EQ( 0.0, edge.data<DefaultEdgeData>().branch_length );
}

TEST( TreeManipulation, AddNewNodeEdge )
{
    // We use input with branch length, in order to make sure that new edges have a default one.
    std::string input = "((B:2.0,(D:2.0,E:2.0)C:2.0)A:2.0,F:2.0,(H:2.0,I:2.0)G:2.0)R:2.0;";
    Tree tree = DefaultTreeNewickReader().from_string( input );

    // Find a leaf node.
    auto node = find_node( tree, "C" );
    ASSERT_NE( nullptr, node );

    // Add a node.
    auto& edge = add_new_node( tree, node->primary_link().edge() );

    // Check all indices and validate tree.
    EXPECT_EQ( 10, edge.index() );
    EXPECT_EQ( 10, edge.primary_node().index() );
    EXPECT_EQ( 20, edge.primary_link().index() );
    EXPECT_EQ( 18, edge.primary_link().next().index() );
    EXPECT_EQ( 11, edge.secondary_node().index() );
    EXPECT_EQ( 21, edge.secondary_link().index() );
    EXPECT_TRUE( validate_topology( tree ));

    // Check whether the data pointers were set correctly: New leaf.
    ASSERT_NO_THROW( edge.secondary_node().data<DefaultNodeData>() );
    EXPECT_EQ( "", edge.secondary_node().data<DefaultNodeData>().name );
    ASSERT_NO_THROW( edge.data<DefaultEdgeData>() );
    EXPECT_EQ( 0.0, edge.data<DefaultEdgeData>().branch_length );

    // Check whether the data pointers were set correctly: New secondary edge.
    ASSERT_NO_THROW( edge.primary_link().next().next().node().data<DefaultNodeData>() );
    EXPECT_EQ( "", edge.primary_link().next().next().node().data<DefaultNodeData>().name );
    ASSERT_NO_THROW( edge.primary_link().next().next().edge().data<DefaultEdgeData>() );
    EXPECT_EQ( 0.0, edge.primary_link().next().next().edge().data<DefaultEdgeData>().branch_length );
}
