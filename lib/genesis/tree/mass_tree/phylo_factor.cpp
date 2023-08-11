/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

 #include "genesis/tree/mass_tree/phylo_factor.hpp"

#include "genesis/tree/drawing/functions.hpp"
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
#include "genesis/utils/color/color.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Phylogenetic Factorization
// =================================================================================================

std::unordered_set<size_t> phylo_factor_subtree_indices(
    Subtree const& subtree,
    std::unordered_set<size_t> const& candidate_edges
) {
    // Prepare result
    std::unordered_set<size_t> sub_indices;

    // Given a link, find the first other link of the same node that is a valid direction
    // to proceed the traversal. Valid means: it is in the candidate edges, and it is not
    // a leaf. If no such direction is found, a nullptr is returned.
    // If leaves are found (links of the node that lead to leaf nodes), we also need
    // to add them to our result list.
    // So, basically, this function is a replacement for the next() step in normal tree traversal,
    // which just might skip some nexts.
    auto find_valid_next = [&]( TreeLink const* link_ptr ){
        auto next_ptr = &link_ptr->next();
        while( next_ptr != link_ptr ) {

            if( is_leaf( next_ptr->edge() )) {

                // We do not need to move down to leaves, but store them in our result list.
                assert( sub_indices.count( next_ptr->edge().index() ) == 0 );
                sub_indices.insert( next_ptr->edge().index() );
            } else if( candidate_edges.count( next_ptr->edge().index() ) > 0 ) {

                // If we found an edge adjacent to the node that is in the candidates list,
                // we can stop here, as is is what we are looking for.
                break;
            }

            // If we are here, the edge is not in the candidates - either because it has been
            // removed for a previous factor, or because it is a leaf edge. In both cases, it is
            // not one we are looking for (no need to go down this edge), so move to the next.
            assert( candidate_edges.count( next_ptr->edge().index() ) == 0 );
            next_ptr = &next_ptr->next();
        }

        // We treat the special case of "no valid direction found" here, so that the caller
        // has it easier dealing with that case, and we make it easiert to catch accidents
        // this way (as they will lead to a segfault when trying to deref the nullptr,
        // instead of silently succeeding).
        if( next_ptr == link_ptr ) {
            next_ptr = nullptr;
        }
        return next_ptr;
    };

    // Iterate the subtree manually, so that we can easily skip parts of it.
    // We start at the outer of the given link, because this is what the loop expects:
    // start from an outer link, and move down in its (yet again) outer direction.
    // Basically, this loop does a normal outer -> next -> outer -> next ... traversal of the tree,
    // but might skip some nexts in between.
    auto link_ptr = &subtree.link().outer();
    while( link_ptr != &subtree.link() ) {

        // At the beginning of the loop, we are at the outer link of the node that we want
        // to consider. Move to the node.
        // This is the outer() part of normal tree traversal.
        link_ptr = &link_ptr->outer();
        assert( candidate_edges.count( link_ptr->edge().index() ) > 0 );
        assert( ! is_leaf( link_ptr->edge() ));

        // Unless we are in the first iteration, add it to the result list.
        // (In the first iteration, we are at the edge of our current candidate factor,
        // so we do not want to include it.)
        if( link_ptr != &subtree.link() ) {

            // Insert the edge. It might already have been inserted, if this iteration is
            // one that goes up the tree again after finishing with a subtree.
            sub_indices.insert( link_ptr->edge().index() );
        }

        // Find the first subtree of that node that is part of the candidates.
        // If there is none, we will then move up again.
        auto next_ptr = find_valid_next( link_ptr );

        if( next_ptr ) {
            // We found a direction where to go next. Set it as the new link pointer.
            // The subtree is in the candidates and not a leaf.
            // Use the link for the next iteration, so that we go down this subtree.
            // As we always start at the outer() link of the link where we want to continue,
            // this is simply the next node itself.

            assert( candidate_edges.count( next_ptr->edge().index() ) > 0 );
            assert( ! is_leaf( next_ptr->edge() ));
            link_ptr = next_ptr;

        } else {
            // We did not find a direction where to go next (a subtree that is in the
            // candidates). Either we move back and up the tree, or we are done.

            // Check that we are not done with the whole subtree yet. If so (the non-existent else
            // part of the following condition), no need to do anything, as the while loop will then
            // terminate in its next check anyway.
            // (Yes, the following condition could simply be an else if above, but this way,
            // it is more readable...)
            if( link_ptr != &subtree.link() ) {

                // We must have seen the edge before on our way down.
                assert( sub_indices.count( link_ptr->edge().index() ) > 0 );
                assert( candidate_edges.count( link_ptr->edge().index() ) > 0 );

                // Find a valid direction to go up again. We cannot simply use outer().next() here,
                // as this might be a leaf or not in the candidates, which we need to skip.
                // It will however definitely find some way to go, which in a bifurcating tree
                // is the way up - that is, the same way that got us here.
                link_ptr = find_valid_next( &link_ptr->outer() );

                // We got here somehow, so there must be a way back.
                assert( link_ptr );
                assert( ! is_leaf( link_ptr->edge() ));
            }
        }
    }

    return sub_indices;
}

PhyloFactor phylo_factor_find_best_edge(
    BalanceData const& data,
    std::unordered_set<size_t> const& candidate_edges,
    std::function<double( std::vector<double> const& balances )> objective
) {
    assert( ! data.tree.empty() );

    // Init a result that has an objective value smaller than all we will encounter.
    PhyloFactor result;
    result.objective_value = - std::numeric_limits<double>::infinity();
    result.all_objective_values = std::vector<double>(
        data.tree.edge_count(), std::numeric_limits<double>::quiet_NaN()
    );

    // We cheat for simplicity, and create a vector of the indices as well,
    // so that we can efficiently parallelize over it...
    // (There are ways to use OpenMP on unordered sets, but they are ugly as hell!)
    auto const cand_vec = std::vector<size_t>( candidate_edges.begin(), candidate_edges.end() );

    // Try out all candidate edges.
    #pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < cand_vec.size(); ++i ) {
        auto const ce_idx = cand_vec[i];

        assert( ce_idx < data.tree.edge_count() );
        auto const& edge = data.tree.edge_at( ce_idx );

        // The calling function already leaves out edges that lead to a leaf.
        assert( ! is_leaf( edge ));

        // Find the edges of the two subtrees induced by the split of the edge,
        // leaving out subtrees that are not connected (that is, which are connected by an
        // edge that is not in the candidates list).
        // This might give empty sets, because previous factors can lead to a subtree being
        // completely blocked.
        // This could be optimized: an edge that yields an empty set here will also do so
        // in all following phylo factors, so we can just completely remove it from lookup candidates.
        // We can however not remove it from the candidates completely, as it is still part of the
        // edges needed for calculating balances. So, we'd need another set of edges distinct from
        // the candidates for storing which edges to use for the lookup... too complex for now!
        auto const p_indices = phylo_factor_subtree_indices(
            Subtree{ edge.primary_link() }, candidate_edges
        );
        if( p_indices.empty() ) {
            continue;
        }
        auto const s_indices = phylo_factor_subtree_indices(
            Subtree{ edge.secondary_link() }, candidate_edges
        );
        if( s_indices.empty() ) {
            continue;
        }

        // We should not have added the actual candidate edge to either of the partitions.
        assert( s_indices.count( edge.index() ) == 0 );
        assert( p_indices.count( edge.index() ) == 0 );

        // Calculate the balances of this edge for all trees.
        auto const balances = mass_balance( data, s_indices, p_indices );

        // Calculate the objective function, and store it in the result.
        auto const ov = objective( balances );
        result.all_objective_values[ ce_idx ] = ov;

        // Update our greedy best hit if needed.
        #pragma omp critical( GENESIS_TREE_MASS_TREE_PHYLO_FACTOR_OBJECTIVE_UPDATE )
        {
            if( ov > result.objective_value ) {
                result.edge_index = ce_idx;
                result.edge_indices_primary   = p_indices;
                result.edge_indices_secondary = s_indices;
                result.balances = balances;
                result.objective_value = ov;
            }
        }
    }

    return result;
}

std::vector<PhyloFactor> phylogenetic_factorization(
    BalanceData const& data,
    std::function<double( std::vector<double> const& balances )> objective,
    size_t max_iterations,
    std::function<void( size_t iteration, size_t max_iterations )> log_progress
) {
    // Basic checks.
    if( data.tree.empty() ) {
        return std::vector<PhyloFactor>{};
    }

    // Start with all edges except for leaves as potential candidates for factors.
    std::unordered_set<size_t> candidate_edges;
    for( size_t i = 0; i < data.tree.edge_count(); ++i ) {
        if( ! is_leaf( data.tree.edge_at(i) )) {
            candidate_edges.insert( i );
        }
    }

    // Special value max iters == 0: get all factors. Also, reduce if too large.
    if( max_iterations == 0 || max_iterations > candidate_edges.size() ) {
        max_iterations = candidate_edges.size();
    }

    // Successively find factors. This cannot be parallelized,
    // as each iteration depends on all previous ones.
    std::vector<PhyloFactor> result;
    for( size_t it = 0; it < max_iterations; ++it ) {
        assert( candidate_edges.size() > 0 );

        // Log the progress, if needed.
        if( log_progress ) {
            log_progress( it + 1, max_iterations );
        }

        // Find and store the next (greedy) phylo factor.
        result.push_back( phylo_factor_find_best_edge( data, candidate_edges, objective ));

        // Remove its edge from the candiate list.
        assert( candidate_edges.count( result.back().edge_index ) > 0 );
        candidate_edges.erase( result.back().edge_index );
    }

    return result;
}

} // namespace tree
} // namespace genesis
