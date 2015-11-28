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
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), Color(255, 0, 0)) );
    EXPECT_LE( 1, std::count(colors.begin(), colors.end(), Color(0, 255, 0)) );
}
