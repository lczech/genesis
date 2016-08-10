/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include <string>
#include <vector>

#include "lib/tree/default/functions.hpp"
#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/default/tree.hpp"
#include "lib/tree/formats/newick/reader.hpp"
#include "lib/tree/function/functions.hpp"
#include "lib/tree/tree.hpp"
#include "lib/utils/text/string.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Subtree Size
// =================================================================================================

void TestSubtreeSize( size_t link_index, size_t out_subtree_size )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";

    Tree tree;
    DefaultTreeNewickReader().from_string( input, tree );

    auto st_size = subtree_size( tree, tree.link_at( link_index ));
    EXPECT_EQ( out_subtree_size, st_size ) << " with link index " << link_index;
}

TEST( TreeFunctions, SubtreeSize )
{
    TestSubtreeSize(  0, 5 );
    TestSubtreeSize(  1, 1 );
    TestSubtreeSize(  2, 3 );
    TestSubtreeSize(  3, 7 );
    TestSubtreeSize(  4, 1 );
    TestSubtreeSize(  5, 1 );
    TestSubtreeSize(  6, 9 );
    TestSubtreeSize(  7, 9 );
    TestSubtreeSize(  8, 9 );
    TestSubtreeSize(  9, 5 );
    TestSubtreeSize( 10, 1 );
    TestSubtreeSize( 11, 3 );
    TestSubtreeSize( 12, 7 );
    TestSubtreeSize( 13, 1 );
    TestSubtreeSize( 14, 1 );
    TestSubtreeSize( 15, 9 );
    TestSubtreeSize( 16, 9 );
    TestSubtreeSize( 17, 9 );
}

// =================================================================================================
//     Subtree Sizes
// =================================================================================================

void TestSubtreeSizes( std::string node_name, std::vector<size_t> out_sizes )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";

    Tree tree;
    DefaultTreeNewickReader().from_string( input, tree );

    auto node = find_node( tree, node_name );
    ASSERT_NE( nullptr, node );

    auto st_sizes = subtree_sizes( tree, *node );
    EXPECT_EQ( out_sizes, st_sizes ) << " with start node " << node_name;
}

TEST( TreeFunctions, SubtreeSizes )
{
    TestSubtreeSizes( "R", { 9, 2, 0, 0, 0, 4, 2, 0, 0, 0 } );
    TestSubtreeSizes( "A", { 4, 2, 0, 0, 0, 9, 2, 0, 0, 0 } );
    TestSubtreeSizes( "B", { 4, 2, 0, 0, 0, 8, 2, 0, 0, 9 } );
    TestSubtreeSizes( "C", { 4, 2, 0, 0, 0, 6, 9, 0, 0, 0 } );
    TestSubtreeSizes( "D", { 4, 2, 0, 0, 0, 6, 8, 0, 9, 0 } );
    TestSubtreeSizes( "E", { 4, 2, 0, 0, 0, 6, 8, 9, 0, 0 } );
    TestSubtreeSizes( "F", { 8, 2, 0, 0, 9, 4, 2, 0, 0, 0 } );
    TestSubtreeSizes( "G", { 6, 9, 0, 0, 0, 4, 2, 0, 0, 0 } );
    TestSubtreeSizes( "H", { 6, 8, 0, 9, 0, 4, 2, 0, 0, 0 } );
    TestSubtreeSizes( "I", { 6, 8, 9, 0, 0, 4, 2, 0, 0, 0 } );
}

// =================================================================================================
//     Subtree Max Path Height
// =================================================================================================

void TestSubtreeMaxPathHeight( std::string node_name, size_t out_size )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";

    Tree tree;
    DefaultTreeNewickReader().from_string( input, tree );

    auto node = find_node( tree, node_name );
    ASSERT_NE( nullptr, node );

    // We are lazy and only evaluate the links towards the root.
    auto st_size = subtree_max_path_height( tree, node->link().outer() );
    EXPECT_EQ( out_size, st_size ) << " with start node " << node_name;
}

TEST( TreeFunctions, SubtreeMaxPathHeight )
{
    // TestSubtreeMaxPathHeight( "R", 3 );
    TestSubtreeMaxPathHeight( "A", 2 );
    TestSubtreeMaxPathHeight( "B", 0 );
    TestSubtreeMaxPathHeight( "C", 1 );
    TestSubtreeMaxPathHeight( "D", 0 );
    TestSubtreeMaxPathHeight( "E", 0 );
    TestSubtreeMaxPathHeight( "F", 0 );
    TestSubtreeMaxPathHeight( "G", 1 );
    TestSubtreeMaxPathHeight( "H", 0 );
    TestSubtreeMaxPathHeight( "I", 0 );
}

// =================================================================================================
//     Misc
// =================================================================================================

void TestTreeLCA( std::string node_name_a, std::string node_name_b, std::string node_name_lca )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";

    Tree tree;
    DefaultTreeNewickReader().from_string( input, tree );

    auto node_a = find_node( tree, node_name_a );
    auto node_b = find_node( tree, node_name_b );
    ASSERT_NE( nullptr, node_a );
    ASSERT_NE( nullptr, node_b );

    auto& node_lca = lowest_common_ancestor( *node_a, *node_b );
    EXPECT_EQ( node_name_lca, node_lca.data<DefaultNodeData>().name )
        << " with nodes " << node_name_a << ", " << node_name_b << " and LCA " << node_name_lca;
}

TEST( TreeFunctions, LCA )
{
    TestTreeLCA( "A", "A", "A" );
    TestTreeLCA( "A", "B", "A" );
    TestTreeLCA( "A", "F", "R" );
    TestTreeLCA( "E", "C", "C" );
    TestTreeLCA( "E", "H", "R" );
    TestTreeLCA( "H", "I", "G" );
}
