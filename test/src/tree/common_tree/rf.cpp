/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @brief Testing Tree distance methods.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/tree/bipartition/rf.hpp"
#include "genesis/tree/common_tree/distances.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

using namespace genesis;
using namespace tree;

TEST(CommonTree, RFDistance)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // TreeSet trees;
    // for( size_t i = 0; i < 10; ++i ) {
    //     // Read and process tree.
    //     std::string infile = environment->data_dir + "tree/random-tree-" + std::to_string(i) + ".newick";
    //     Tree tree = CommonTreeNewickReader().read( utils::from_file( infile ));
    //
    //     // Basic checks.
    //     EXPECT_EQ( 10, leaf_node_count( tree ));
    //     EXPECT_TRUE( is_bifurcating( tree ));
    //     EXPECT_TRUE( validate_topology( tree ));
    //
    //     trees.add( tree );
    // }

    TreeSet trees;
    std::string const infile = environment->data_dir + "tree/random-trees.newick";
    CommonTreeNewickReader().read( utils::from_file( infile ), trees );
    EXPECT_EQ( 10, trees.size() );

    auto const rf_mat = rf_distance_absolute( trees );
    // for( size_t i = 0; i < rf_mat.rows(); ++i ) {
    //     for( size_t j = i + 1; j < rf_mat.cols(); ++j ) {
    //         LOG_DBG << i << "\t" << j << "\t" << rf_mat( i, j );
    //     }
    // }
    // LOG_BOLD << rf_mat;

    auto const rf_mat_exp = utils::Matrix<size_t>( 10, 10, {
        0, 14, 14, 12, 14, 14, 14, 10, 14, 10,
        14, 0, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 0, 14, 12, 14, 10, 14, 14, 14,
        12, 14, 14, 0, 14, 14, 12, 12, 14, 14,
        14, 14, 12, 14, 0, 14, 14, 14, 12, 14,
        14, 14, 14, 14, 14, 0, 14, 12, 12, 14,
        14, 14, 10, 12, 14, 14, 0, 12, 14, 14,
        10, 14, 14, 12, 14, 12, 12, 0, 14, 12,
        14, 14, 14, 14, 12, 12, 14, 14, 0, 14,
        10, 14, 14, 14, 14, 14, 14, 12, 14, 0
    });
    EXPECT_EQ( rf_mat_exp, rf_mat );

    auto const rf_vec = rf_distance_absolute( trees[0], trees );
    // for( size_t i = 0; i < rf_vec.size(); ++i ) {
    //     LOG_DBG << i << "\t" << rf_vec[i];
    // }

    auto const rf_vec_exp = std::vector<size_t>{
        0, 14, 14, 12, 14, 14, 14, 10, 14, 10
    };
    EXPECT_EQ( rf_vec_exp, rf_vec );
}
