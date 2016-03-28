/**
 * @brief Testing Default Tree methods.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/tree/default/distances.hpp"
#include "lib/tree/default/edge_color.hpp"
#include "lib/tree/default/functions.hpp"
#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/function/functions.hpp"
#include "lib/tree/io/newick/reader.hpp"
#include "lib/tree/tree.hpp"

#include "lib/utils/tools/color/operators.hpp"

#include <algorithm>

using namespace genesis;
using namespace tree;

TEST(DefaultTree, EdgeColorBranchLengthGradient)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    DefaultTree tree;
    std::string infile = environment->data_dir + "tree/distances.newick";
    DefaultTreeNewickReader().from_file(infile, tree);

    // Colorize the branches according to their length.
    auto colors = edge_color_branch_length_gradient(tree);

    // We expect at least one branch to have max color (red) and one to have min color (green).
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), utils::Color(255, 0, 0)) );
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), utils::Color(0, 255, 0)) );
}

TEST(DefaultTree, NodeNames)
{
    DefaultTree tree;

    // Using a tree with all names set to some value.
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    EXPECT_TRUE(DefaultTreeNewickReader().from_string(input, tree));

    EXPECT_EQ( 13, tree.node_count() );
    EXPECT_EQ(  7, leaf_nodes_count(tree) );

    auto names_all    = node_names(tree);
    auto names_leaves = node_names(tree, true);

    EXPECT_EQ( 13, names_all.size() );
    EXPECT_EQ(  7, names_leaves.size() );

    // Using a tree where some names are empty.
    input = "((A,(B,))E,((,(G,H))J,)L);";
    EXPECT_TRUE(DefaultTreeNewickReader().from_string(input, tree));

    EXPECT_EQ( 13, tree.node_count() );
    EXPECT_EQ(  7, leaf_nodes_count(tree) );

    names_all    = node_names(tree);
    names_leaves = node_names(tree, true);

    EXPECT_EQ(  7, names_all.size() );
    EXPECT_EQ(  4, names_leaves.size() );
}
