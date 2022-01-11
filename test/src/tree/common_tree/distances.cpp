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
 * @brief Testing Tree distance methods.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/tree/common_tree/distances.hpp"
#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

using namespace genesis;
using namespace tree;

TEST(CommonTree, Distances)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/distances.newick";
    Tree tree =  CommonTreeNewickReader().read( utils::from_file( infile ));

    // Basic checks.
    EXPECT_EQ(7, leaf_node_count(tree));
    EXPECT_EQ(6, inner_node_count(tree));
    EXPECT_EQ(13, tree.node_count());
    EXPECT_TRUE( is_bifurcating( tree ));
    EXPECT_TRUE( validate_topology( tree ));

    utils::Matrix<double> exp (12, 12, {
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

    EXPECT_TRUE( exp == edge_branch_length_distance_matrix(tree) );
    EXPECT_EQ(   exp,   edge_branch_length_distance_matrix(tree) );
}

TEST( CommonTree, PatristicDistances )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string const infile = environment->data_dir + "tree/distances.newick";
    Tree const tree =  CommonTreeNewickReader().read( utils::from_file( infile ));

    utils::Matrix<double> exp (13, 13, {
        0, 2048, 3072, 2560, 2816, 2944, 2880, 2592, 16, 24, 28, 26, 17,
        2048, 0, 1024, 512, 768, 896, 832, 544, 2064, 2072, 2076, 2074, 2065,
        3072, 1024, 0, 1536, 1792, 1920, 1856, 1568, 3088, 3096, 3100, 3098, 3089,
        2560, 512, 1536, 0, 256, 384, 320, 32, 2576, 2584, 2588, 2586, 2577,
        2816, 768, 1792, 256, 0, 128, 64, 288, 2832, 2840, 2844, 2842, 2833,
        2944, 896, 1920, 384, 128, 0, 192, 416, 2960, 2968, 2972, 2970, 2961,
        2880, 832, 1856, 320, 64, 192, 0, 352, 2896, 2904, 2908, 2906, 2897,
        2592, 544, 1568, 32, 288, 416, 352, 0, 2608, 2616, 2620, 2618, 2609,
        16, 2064, 3088, 2576, 2832, 2960, 2896, 2608, 0, 8, 12, 10, 1,
        24, 2072, 3096, 2584, 2840, 2968, 2904, 2616, 8, 0, 4, 2, 9,
        28, 2076, 3100, 2588, 2844, 2972, 2908, 2620, 12, 4, 0, 6, 13,
        26, 2074, 3098, 2586, 2842, 2970, 2906, 2618, 10, 2, 6, 0, 11,
        17, 2065, 3089, 2577, 2833, 2961, 2897, 2609, 1, 9, 13, 11, 0
    });

    auto const mat = node_branch_length_distance_matrix( tree );
    // LOG_DBG << mat;

    EXPECT_EQ( exp, mat );
    EXPECT_FLOAT_EQ( 3100.0, diameter(tree) );
}
