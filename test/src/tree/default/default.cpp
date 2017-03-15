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
 * @brief Testing Default Tree methods.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/edge_color.hpp"
#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/default/newick_reader.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/operators.hpp"

#include <algorithm>

using namespace genesis;
using namespace tree;

TEST(DefaultTree, EdgeColorBranchLengthGradient)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/distances.newick";
    Tree tree = DefaultTreeNewickReader().from_file( infile );

    // Colorize the branches according to their length.
    auto colors = edge_color_branch_length_gradient(tree);

    // We expect at least one branch to have max color (red) and one to have min color (green).
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), utils::Color(255, 0, 0)) );
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), utils::Color(0, 255, 0)) );
}

TEST(DefaultTree, NodeNames)
{
    Tree tree;

    // Using a tree with all names set to some value.
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    tree = DefaultTreeNewickReader().from_string( input );

    EXPECT_EQ( 13, tree.node_count() );
    EXPECT_EQ(  7, leaf_node_count(tree) );

    auto names_all    = node_names(tree);
    auto names_leaves = node_names(tree, true);

    EXPECT_EQ( 13, names_all.size() );
    EXPECT_EQ(  7, names_leaves.size() );

    // Using a tree where some names are empty.
    input = "((A,(B,))E,((,(G,H))J,)L);";
    tree = DefaultTreeNewickReader().from_string( input );

    EXPECT_EQ( 13, tree.node_count() );
    EXPECT_EQ(  7, leaf_node_count(tree) );

    names_all    = node_names(tree);
    names_leaves = node_names(tree, true);

    EXPECT_EQ(  7, names_all.size() );
    EXPECT_EQ(  4, names_leaves.size() );
}
