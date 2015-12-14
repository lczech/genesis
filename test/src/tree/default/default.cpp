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
#include "lib/tree/default/newick_processor.hpp"
#include "lib/tree/io/newick/processor.hpp"
#include "lib/tree/tree.hpp"

#include "lib/utils/tools/color/operators.hpp"

#include <algorithm>

using namespace genesis;

TEST(DefaultTree, EdgeColorBranchLengthGradient)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    DefaultTree tree;
    std::string infile = environment->data_dir + "tree/distances.newick";
    DefaultTreeNewickProcessor().from_file(infile, tree);

    // Colorize the branches according to their length.
    auto colors = edge_color_branch_length_gradient(tree);

    // We expect at least one branch to have max color (red) and one to have min color (green).
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), color::Color(255, 0, 0)) );
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), color::Color(0, 255, 0)) );
}

TEST(DefaultTree, NodeNames)
{
    DefaultTree tree;

    // Using a tree with all names set to some value.
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    EXPECT_TRUE(DefaultTreeNewickProcessor().from_string(input, tree));

    EXPECT_EQ( 13, tree.node_count() );
    EXPECT_EQ(  7, tree.leaf_count() );

    auto names_all    = node_names(tree);
    auto names_leaves = node_names(tree, true);

    EXPECT_EQ( 13, names_all.size() );
    EXPECT_EQ(  7, names_leaves.size() );

    // Using a tree where some names are empty.
    input = "((A,(B,))E,((,(G,H))J,)L);";
    EXPECT_TRUE(DefaultTreeNewickProcessor().from_string(input, tree));

    EXPECT_EQ( 13, tree.node_count() );
    EXPECT_EQ(  7, tree.leaf_count() );

    names_all    = node_names(tree);
    names_leaves = node_names(tree, true);

    EXPECT_EQ(  7, names_all.size() );
    EXPECT_EQ(  4, names_leaves.size() );
}
