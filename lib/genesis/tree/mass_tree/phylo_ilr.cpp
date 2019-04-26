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
 * @brief
 *
 * @file
 * @ingroup tree
 */

 #include "genesis/tree/mass_tree/phylo_ilr.hpp"

#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/mass_tree/balances.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree/subtree.hpp"

#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/statistics.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Phylogenetic ILR Tranform
// =================================================================================================

utils::Matrix<double> phylogenetic_ilr_transform(
    BalanceData const& data,
    bool reverse_signs
) {
    // Basic checks specific for this function. More checks are done in mass_balance()
    if( data.tree.empty() ) {
        assert( data.edge_masses.size() == 0 );
        assert( data.taxon_weights.size() == 0 );
        return {};
    }
    if( ! is_rooted( data.tree )) {
        throw std::invalid_argument(
            "Tree is not rooted. Cannot calculate its Phylogenetic ILR tranform."
        );
    }
    if( ! is_bifurcating( data.tree )) {
        throw std::invalid_argument(
            "Tree is not bifurcating. Cannot calculate its Phylogenetic ILR tranform."
        );
    }

    // Helper to get the edge indices of a particular subtree, including the edge that leads to it.
    // It is slightly inefficient to first store the indices and then do the lookup of masses
    // later, but this way, we save code duplication of the balance calculation function...
    auto get_subtree_indices_ = []( Subtree const& subtree ){
        std::unordered_set<size_t> sub_indices;
        for( auto it : preorder( subtree )) {

            // The iterator visits each edge of the subtree once.
            assert( sub_indices.count( it.edge().index() ) == 0 );

            // The preorder iterator is node based, so we start at the root node of the subtree,
            // meaning that we automatically also insert its edge index! No special case needed.
            sub_indices.insert( it.edge().index() );
        }
        return sub_indices;
    };

    // Prepare result matrix.
    auto result = utils::Matrix<double>( data.edge_masses.rows(), data.tree.node_count(), 0.0 );

    // Calculate balance for every node of the tree.
    #pragma omp parallel for
    for( size_t node_idx = 0; node_idx < data.tree.node_count(); ++node_idx ) {
        auto const& node = data.tree.node_at( node_idx );
        assert( node.index() == node_idx );

        // For leaf nodes do nothing. They just keep their initial value of 0.0.
        auto const deg = degree( node );
        if( deg == 1 ) {
            continue;
        }

        // Get the indices of the edges in the two subtrees down from the given node.
        // We need a special case for the root, because its links are a bit different.
        // (If we ignore this, we'd get a flipped sign at the root.)
        std::unordered_set<size_t> lhs_indices;
        std::unordered_set<size_t> rhs_indices;
        if( deg == 2 ) {
            assert( is_root( node ));

            // The tree is rooted, so that the root node, the left hand side is the primary
            // link of the node itself, and the right hand side the next one.
            lhs_indices = get_subtree_indices_( Subtree{ node.link().outer() });
            rhs_indices = get_subtree_indices_( Subtree{ node.link().next().outer() });

            // After that, we should have all edges of the tree.
            assert( lhs_indices.size() + rhs_indices.size() == data.tree.edge_count() );
        } else {
            assert( deg == 3 );

            // At inner nodes, the primary link points towards the root, so we use the next two links.
            lhs_indices = get_subtree_indices_( Subtree{ node.link().next().outer() });
            rhs_indices = get_subtree_indices_( Subtree{ node.link().next().next().outer() });

            // We never have more edges than the tree.
            assert( lhs_indices.size() + rhs_indices.size() < data.tree.edge_count() );
        }

        // If needed, flip lhs and rhs.
        if( reverse_signs ) {
            std::swap( lhs_indices, rhs_indices );
        }

        // Calculate and store the balance for all rows (trees) of the data.
        result.col( node_idx ) = mass_balance( data, lhs_indices, rhs_indices );
    }

    return result;
}

utils::Matrix<double> edge_balances(
    BalanceData const& data,
    bool reverse_signs
) {
    // Basic checks specific for this function. More checks are done in mass_balance()
    if( data.tree.empty() ) {
        assert( data.edge_masses.size() == 0 );
        assert( data.taxon_weights.size() == 0 );
        return {};
    }

    // Helper to get the edge indices of a particular subtree, excluding the edge that leads to it.
    auto get_subtree_indices_ = []( Subtree const& subtree ){
        std::unordered_set<size_t> sub_indices;
        for( auto it : preorder( subtree )) {

            // The iterator visits each edge of the subtree once.
            assert( sub_indices.count( it.edge().index() ) == 0 );

            // Skip the top edge, where the subtree is attached.
            if( it.is_first_iteration() ) {
                continue;
            }

            sub_indices.insert( it.edge().index() );
        }
        return sub_indices;
    };

    // Prepare result matrix.
    auto result = utils::Matrix<double>( data.edge_masses.rows(), data.tree.edge_count(), 0.0 );

    // Calculate balance for every node of the tree.
    #pragma omp parallel for
    for( size_t edge_idx = 0; edge_idx < data.tree.edge_count(); ++edge_idx ) {
        auto const& edge = data.tree.edge_at( edge_idx );
        assert( edge.index() == edge_idx );

        // For leaf edges do nothing. They just keep their initial value of 0.0.
        if( is_leaf( edge )) {
            continue;
        }

        // Get the indices of the edges in the two subtrees away from the given edge.
        auto p_indices = get_subtree_indices_( Subtree{ edge.primary_link() });
        auto s_indices = get_subtree_indices_( Subtree{ edge.secondary_link() });

        // After that, we should have all edges of the tree except one.
        assert( p_indices.size() + s_indices.size() == data.tree.edge_count() - 1 );

        // If needed, flip lhs and rhs.
        if( reverse_signs ) {
            std::swap( p_indices, s_indices );
        }

        // Calculate and store the balance for all rows (trees) of the data.
        result.col( edge_idx ) = mass_balance( data, s_indices, p_indices );
    }

    return result;
}

} // namespace tree
} // namespace genesis
