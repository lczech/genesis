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

#include "genesis/tree/mass_tree/emd.hpp"

#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Earth Movers Distance
// =================================================================================================

double earth_movers_distance( MassTree const& lhs, MassTree const& rhs, double const p )
{
    // Check.
    if( p <= 0.0 ) {
        throw std::runtime_error(
            "Invalid exponent value p for earth mover's distance calculation. Has to be > 0.0."
        );
    }

    // We make an extra copy, which is super expensive. In our our code, thus better do not
    // use this function, but the one-tree version directly!
    // auto copy = lhs;
    // mass_tree_reverse_signs( copy );
    // mass_tree_merge_trees_inplace( copy, rhs );
    // return earth_movers_distance( copy, p ).first;

    // We don't do a full check for compatile topologies, but at least this check is cheap.
    if( lhs.edge_count() != rhs.edge_count() ) {
        throw std::invalid_argument( "MassTrees need to have same size." );
    }

    // Keep track of the total resulting work (the distance we moved the masses).
    // This is the result returned in the end.
    double work = 0.0;

    // Store a list of masses for each processed node. It maps from node indices to the total
    // mass that comes from the subtree below that node. Thus, for the root node, it should be
    // the same value as sum_of_masses(). Both values should be close to zero (except for numerical
    // issues), in order for the result of this function to be meaningful.
    auto node_masses = std::vector<double>( lhs.node_count(), 0.0 );

    // Do a postorder traversal over both trees in parallel, starting at the root.
    // In theory, it does not matter where we start the traversal - however, the positions of the
    // masses are given as "proximal_length" on their branch, which always points away from the
    // root. Thus, if we decided to traverse from a different node than the root, we would have to
    // take this into account. So, we do start at the root, to keep it simple.
    auto lhs_it  = postorder( lhs ).begin();
    auto rhs_it  = postorder( rhs ).begin();
    auto lhs_end = postorder( lhs ).end();
    auto rhs_end = postorder( rhs ).end();
    for( ; lhs_it != lhs_end && rhs_it != rhs_end ; ++lhs_it, ++rhs_it ) {

        // If we are at the last iteration, we reached the root. Thus, we have moved all masses
        // and don't need to proceed. If we did, we would count an edge of the root again
        // (because the iterator traverses nodes, not edges, so the root node itself is traversed,
        // although it has no proper edge that we would need to process).
        if( lhs_it.is_last_iteration() || rhs_it.is_last_iteration() ) {
            // If one iterator is at the end, but not the other, something is wrong.
            if( ! lhs_it.is_last_iteration() || ! rhs_it.is_last_iteration() ) {
                throw std::invalid_argument( "Incompatible MassTrees." );
            }
            continue;
        }

        // Some shorthands.
        const size_t pri_node_index = lhs_it.edge().primary_node().index();
        const size_t sec_node_index = lhs_it.edge().secondary_node().index();

        // More checks.
        if(
            pri_node_index != rhs_it.edge().primary_node().index() ||
            sec_node_index != rhs_it.edge().secondary_node().index()
        ) {
            throw std::invalid_argument( "Incompatible MassTrees." );
        }

        // The iterator should guarantee that its edge is always the one pointing towards the root.
        // Still, better check this!
        assert( sec_node_index == lhs_it.node().index() );
        assert( sec_node_index == rhs_it.node().index() );

        // Add both masses to a common map, one of them with negative sign.
        // This is faster than merging into a vector, and easier than doing a parallel iteration
        // over the values in sorted order.
        auto edge_masses = lhs_it.edge().data<MassTreeEdgeData>().masses;
        for( auto const& mass : rhs_it.edge().data<MassTreeEdgeData>().masses ) {
            edge_masses[ mass.first ] -= mass.second;
        }

        // We now start a "normal" earth movers distance caluclation along the current edge.
        // We start at the end of the branch, with the mass that comes from the subtree below it...
        double current_pos  = std::max(
            lhs_it.edge().data<MassTreeEdgeData>().branch_length,
            rhs_it.edge().data<MassTreeEdgeData>().branch_length
        );
        double current_mass = node_masses[ sec_node_index ];

        // ... and move the mass along the branch, balancing it with the masses found on the branch.
        // We use a reverse iterator in order to traverse the branch from end to start.
        for(
            auto mass_rit = edge_masses.crbegin();
            mass_rit != edge_masses.crend();
            ++mass_rit
        ) {
            // The work is accumulated: The mass that we are currently moving times the distances
            // that we move it.
            work += std::pow( std::abs( current_mass ), p ) * ( current_pos - mass_rit->first );

            // Update the current position and mass.
            current_pos   = mass_rit->first;
            current_mass += mass_rit->second;
        }

        // After we finished moving along the branch, we need extra work to move the remaining mass
        // to the node at the top end of the branch. Also, add the remaining mass to this node, so
        // that it is available for when we process the upper part of that node (towards the root).
        work += std::pow( std::abs( current_mass ), p ) * current_pos;
        node_masses[ pri_node_index ] += current_mass;
    }

    // Now we need to be done with both trees, otherwise we have a problem.
    if( lhs_it != lhs_end || rhs_it != rhs_end ) {
        throw std::invalid_argument( "Incompatible MassTrees." );
    }

    // Apply the outer exponent.
    if( p > 1.0 ) {
        work = std::pow( work, 1.0 / p );
    }

    return work;
}

utils::Matrix<double> earth_movers_distance( std::vector<MassTree> const& trees, double const p )
{
    // Check.
    if( p <= 0.0 ) {
        throw std::runtime_error(
            "Invalid exponent value p for earth mover's distance calculation. Has to be > 0.0."
        );
    }

    // Init result matrix.
    auto result = utils::Matrix<double>( trees.size(), trees.size(), 0.0 );

    // Parallel specialized code.
    #ifdef GENESIS_OPENMP

        // We only need to calculate the upper triangle. Get the number of indices needed
        // to describe this triangle.
        size_t const max_k = utils::triangular_size( trees.size() );

        // Use dynamic parallelization, as trees might be of different size (in terms of number
        // of mass points).
        #pragma omp parallel for schedule( dynamic )
        for( size_t k = 0; k < max_k; ++k ) {

            // For the given linear index, get the actual position in the Matrix.
            auto const ij = utils::triangular_indices( k, trees.size() );
            auto const i = ij.first;
            auto const j = ij.second;

            // Calculate EMD and fill symmetric Matrix.
            auto const emd = earth_movers_distance( trees[i], trees[j], p );
            result( i, j ) = emd;
            result( j, i ) = emd;
        }

    // If no threads are available at all, use serial version.
    #else

        for( size_t i = 0; i < trees.size(); ++i ) {

            // The result is symmetric - we only calculate the upper triangle.
            for( size_t j = i + 1; j < trees.size(); ++j ) {

                auto const emd = earth_movers_distance( trees[i], trees[j], p );
                result( i, j ) = emd;
                result( j, i ) = emd;
            }
        }

    #endif

    return result;
}

std::pair<double, double> earth_movers_distance( MassTree const& tree, double const p )
{
    // Check.
    if( p <= 0.0 ) {
        throw std::runtime_error(
            "Invalid exponent value p for earth mover's distance calculation. Has to be > 0.0."
        );
    }

    // Keep track of the total resulting work (the distance we moved the masses).
    // This is the result returned in the end.
    double work = 0.0;

    // Store a list of masses for each processed node. It maps from node indices to the total
    // mass that comes from the subtree below that node. Thus, for the root node, it should be
    // the same value as sum_of_masses(). Both values should be close to zero (except for numerical
    // issues), in order for the result of this function to be meaningful.
    auto node_masses = std::vector<double>( tree.node_count(), 0.0 );

    // Do a postorder traversal of the tree, starting at the root.
    // In theory, it does not matter where we start the traversal - however, the positions of the
    // masses are given as "proximal_length" on their branch, which always points away from the
    // root. Thus, if we decided to traverse from a different node than the root, we would have to
    // take this into account. So, we do start at the root, to keep it simple.
    for( auto tree_it : postorder(tree) ) {

        // If we are at the last iteration, we reached the root. Thus, we have moved all masses
        // and don't need to proceed. If we did, we would count an edge of the root again
        // (because the iterator traverses nodes, not edges, so the root node itself is traversed,
        // although it has no proper edge that we would need to process).
        if( tree_it.is_last_iteration() ) {
            continue;
        }

        // Some shorthands.
        const size_t pri_node_index = tree_it.edge().primary_node().index();
        const size_t sec_node_index = tree_it.edge().secondary_node().index();

        // The iterator should guarantee that its edge is always the one pointing towards the root.
        // Still, better check this!
        assert( sec_node_index == tree_it.node().index() );

        // We now start a "normal" earth movers distance caluclation along the current edge.
        // We start at the end of the branch, with the mass that comes from the subtree below it...
        double current_pos  = tree_it.edge().data<MassTreeEdgeData>().branch_length;
        double current_mass = node_masses[ sec_node_index ];

        // ... and move the mass along the branch, balancing it with the masses found on the branch.
        // We use a reverse iterator in order to traverse the branch from end to start.
        auto const& edge_masses = tree_it.edge().data<MassTreeEdgeData>().masses;
        for(
            auto mass_rit = edge_masses.crbegin();
            mass_rit != edge_masses.crend();
            ++mass_rit
        ) {
            // The work is accumulated: The mass that we are currently moving times the distances
            // that we move it, taking the exponent into account.
            work += std::pow( std::abs( current_mass ), p ) * ( current_pos - mass_rit->first );

            // Update the current position and mass.
            current_pos   = mass_rit->first;
            current_mass += mass_rit->second;
        }

        // After we finished moving along the branch, we need extra work to move the remaining mass
        // to the node at the top end of the branch. Also, add the remaining mass to this node, so
        // that it is available for when we process the upper part of that node (towards the root).
        // Here again we need to take the exponent into account.
        work += std::pow( std::abs( current_mass ), p ) * current_pos;
        node_masses[ pri_node_index ] += current_mass;
    }

    // Apply the outer exponent.
    if( p > 1.0 ) {
        work = std::pow( work, 1.0 / p );
    }

    // Finally, return the needed work, and the mass at the root, as a way of correctness checking.
    return { work, node_masses[ tree.root_node().index() ] };
}

} // namespace tree
} // namespace genesis
