/**
 * @brief Testing Tree distance methods.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/tree/default/distances.hpp"
#include "lib/tree/default_tree_newick_adapter.hpp"
#include "lib/tree/distances.hpp"
#include "lib/tree/newick_processor.hpp"
#include "lib/tree/tree.hpp"
#include "lib/utils/math/matrix.hpp"

using namespace genesis;

TEST(Tree, Distances)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA

    // Read and process tree.
    DefaultTree tree;
    std::string infile = environment->data_dir + "tree/distances.newick";
    DefaultTreeNewickProcessor().from_file(infile, tree);

    // Basic checks.
    EXPECT_EQ(7, tree.leaf_count());
    EXPECT_EQ(6, tree.inner_count());
    EXPECT_EQ(13, tree.node_count());
    EXPECT_TRUE(tree.is_bifurcating());
    EXPECT_TRUE(tree.validate());

    Matrix<double> exp (12, 12, {
        0,	1536,	1280,	1664,	1856,	1824,	1552,	1032,	1044,	1050,	1049,	1040.5,
        1536,	0,	768,	1152,	1344,	1312,	1040,	2568,	2580,	2586,	2585,	2576.5,
        1280,	768,	0,	384,	576,	544,	272,	2312,	2324,	2330,	2329,	2320.5,
        1664,	1152,	384,	0,	192,	160,	144,	2696,	2708,	2714,	2713,	2704.5,
        1856,	1344,	576,	192,	0,	96,	336,	2888,	2900,	2906,	2905,	2896.5,
        1824,	1312,	544,	160,	96,	0,	304,	2856,	2868,	2874,	2873,	2864.5,
        1552,	1040,	272,	144,	336,	304,	0,	2584,	2596,	2602,	2601,	2592.5,
        1032,	2568,	2312,	2696,	2888,	2856,	2584,	0,	12,	18,	17,	8.5,
        1044,	2580,	2324,	2708,	2900,	2868,	2596,	12,	0,	6,	5,	4.5,
        1050,	2586,	2330,	2714,	2906,	2874,	2602,	18,	6,	0,	3,	10.5,
        1049,	2585,	2329,	2713,	2905,	2873,	2601,	17,	5,	3,	0,	9.5,
        1040.5,	2576.5,	2320.5,	2704.5,	2896.5,	2864.5,	2592.5,	8.5,	4.5,	10.5,	9.5,	0
    });

    EXPECT_TRUE(edge_distance_matrix(tree) == exp);
    EXPECT_EQ(exp, edge_distance_matrix(tree));
}
