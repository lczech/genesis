/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/mass_tree/balances.hpp"

#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree/subtree.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/logging.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Phylogenetic ILR Tranform
// =================================================================================================

utils::Matrix<signed char> sign_matrix( Tree const& tree, bool compressed )
{
    // Edge cases and input checks.
    if( tree.empty() ) {
        return utils::Matrix<signed char>();
    }
    if( ! is_rooted( tree )) {
        throw std::invalid_argument( "Tree is not rooted. Cannot calculate its sign matrix." );
    }
    if( ! is_bifurcating( tree )) {
        throw std::invalid_argument( "Tree is not bifurcating. Cannot calculate its sign matrix." );
    }

    // Prepare a result matrix of the full size. For the compressed version,
    // we later replate it again.
    auto result = utils::Matrix<signed char>( tree.node_count(), tree.node_count(), 0 );

    // Helper function that fills all columns of a subtree with a given sign.
    auto fill_subtree_indices = [&]( size_t row_idx, Subtree const& st, signed char sign ){
        for( auto const& it : preorder(st) ) {
            result( row_idx, it.node().index() ) = sign;
        }
    };

    // Fill every row of the matrix.
    #pragma omp parallel for
    for( size_t i = 0; i < tree.node_count(); ++i ) {
        auto const& row_node = tree.node_at(i);
        auto const  row_idx  = row_node.index();

        if( row_idx == tree.root_node().index() ) {

            // The root node is special: we use its two subtrees directly.
            assert( &row_node.link().next().next() == &row_node.link() );
            fill_subtree_indices( row_idx, Subtree{ row_node.link().outer() },        +1 );
            fill_subtree_indices( row_idx, Subtree{ row_node.link().next().outer() }, -1 );

        } else if( is_inner( row_node )) {

            // All other inner nodes are filled using their subtrees.
            assert( &row_node.link().next().next().next() == &row_node.link() );
            fill_subtree_indices( row_idx, Subtree{ row_node.link().next().outer() },        +1 );
            fill_subtree_indices( row_idx, Subtree{ row_node.link().next().next().outer() }, -1 );
        }
    }

    // For the compressed version, we re-use the previous result matrix,
    // and simply fill a new one with the needed rows and columns.
    // The data is not too big, and this is way easier and cleaner to implement.
    if( compressed ) {
        // Create a matrix with rows for each inner node and columns for each tip node.
        auto const in_node_idcs = inner_node_indices( tree );
        auto const lf_node_idcs = leaf_node_indices( tree );
        auto result_cmpr = utils::Matrix<signed char>( in_node_idcs.size(), lf_node_idcs.size(), 0 );

        // Fill the matrix at the indices that belong to inner nodes (for rows) and
        // leaf nodes (for columns).
        for( size_t r = 0; r < in_node_idcs.size(); ++r ) {
            for( size_t c = 0; c < lf_node_idcs.size(); ++c ) {
                result_cmpr( r, c ) = result( in_node_idcs[r], lf_node_idcs[c] );
            }
        }

        // Replace the result matrix efficiently.
        using std::swap;
        swap( result, result_cmpr );
    }

    return result;
}

} // namespace tree
} // namespace genesis
