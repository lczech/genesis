/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/tree/mass_tree/functions.hpp"

#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"

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

double earth_movers_distance( MassTree const& lhs, MassTree const& rhs )
{
    // We make an extra copy, which is super expensive. In our our code, thus better do not
    // use this function, but the one-tree version directly!
    // auto copy = lhs;
    // mass_tree_reverse_signs( copy );
    // mass_tree_merge_trees_inplace( copy, rhs );
    // return earth_movers_distance( copy ).first;

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
            work += std::abs( current_mass ) * ( current_pos - mass_rit->first );

            // Update the current position and mass.
            current_pos   = mass_rit->first;
            current_mass += mass_rit->second;
        }

        // After we finished moving along the branch, we need extra work to move the remaining mass
        // to the node at the top end of the branch. Also, add the remaining mass to this node, so
        // that it is available for when we process the upper part of that node (towards the root).
        work                          += std::abs( current_mass ) * current_pos;
        node_masses[ pri_node_index ] += current_mass;
    }

    // Now we need to be done with both trees, otherwise we have a problem.
    if( lhs_it != lhs_end || rhs_it != rhs_end ) {
        throw std::invalid_argument( "Incompatible MassTrees." );
    }

    return work;
}

utils::Matrix<double> earth_movers_distance( std::vector<MassTree> const& trees )
{
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
            auto const emd = earth_movers_distance( trees[i], trees[j] );
            result( i, j ) = emd;
            result( j, i ) = emd;
        }

    // If no threads are available at all, use serial version.
    #else

        for( size_t i = 0; i < trees.size(); ++i ) {

            // The result is symmetric - we only calculate the upper triangle.
            for( size_t j = i + 1; j < trees.size(); ++j ) {

                auto const emd = earth_movers_distance( trees[i], trees[j] );
                result( i, j ) = emd;
                result( j, i ) = emd;
            }
        }

    #endif

    return result;
}

std::pair<double, double> earth_movers_distance( MassTree const& tree )
{
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
            // that we move it.
            work += std::abs( current_mass ) * ( current_pos - mass_rit->first );

            // Update the current position and mass.
            current_pos   = mass_rit->first;
            current_mass += mass_rit->second;
        }

        // After we finished moving along the branch, we need extra work to move the remaining mass
        // to the node at the top end of the branch. Also, add the remaining mass to this node, so
        // that it is available for when we process the upper part of that node (towards the root).
        work                          += std::abs( current_mass ) * current_pos;
        node_masses[ pri_node_index ] += current_mass;
    }

    return { work, node_masses[ tree.root_node().index() ] };
}

// =================================================================================================
//     Manipulate Masses
// =================================================================================================

MassTree mass_tree_merge_trees( MassTree const& lhs, MassTree const& rhs )
{
    auto copy = lhs;
    mass_tree_merge_trees_inplace( copy, rhs );
    return copy;
}

void mass_tree_merge_trees_inplace( MassTree& lhs, MassTree const& rhs )
{
    // Do at least a basic check.
    if( lhs.edge_count() != rhs.edge_count() ) {
        throw std::runtime_error( "Incompatible MassTrees for merging." );
    }

    #pragma omp parallel for
    for( size_t i = 0; i < lhs.edge_count(); ++i ) {
        auto& lhs_masses = lhs.edge_at( i ).data<MassTreeEdgeData>().masses;
        for( auto const& rhs_mass : rhs.edge_at( i ).data<MassTreeEdgeData>().masses ) {
            lhs_masses[ rhs_mass.first ] += rhs_mass.second;
        }
    }
}

void mass_tree_clear_masses( MassTree& tree )
{
    for( auto& edge : tree.edges() ) {
        edge->data<MassTreeEdgeData>().masses.clear();
    }
}

void mass_tree_reverse_signs( MassTree& tree )
{
    for( auto& edge : tree.edges() ) {
        for( auto& mass : edge->data<MassTreeEdgeData>().masses ) {
            mass.second = -mass.second;
        }
    }
}

void mass_tree_scale_masses( MassTree& tree, double factor )
{
    for( auto& edge : tree.edges() ) {
        for( auto& mass : edge->data<MassTreeEdgeData>().masses ) {
            mass.second *= factor;
        }
    }
}

void mass_tree_normalize_masses( MassTree& tree )
{
    double const total_mass = mass_tree_sum_of_masses( tree );

    if( total_mass == 0.0 ) {
        return;
    }

    for( auto& edge : tree.edges() ) {
        for( auto& mass : edge->data<MassTreeEdgeData>().masses ) {
            mass.second /= total_mass;
        }
    }
}

void mass_tree_transform_to_unit_branch_lengths( MassTree& tree )
{
    for( auto& edge : tree.edges() ) {
        auto& edge_data = edge->data<MassTreeEdgeData>();
        std::map<double, double> relative;

        for( auto& mass : edge_data.masses ) {
            relative[ mass.first / edge_data.branch_length ] += mass.second;
        }

        edge_data.masses = relative;
        edge_data.branch_length = 1.0;
    }
}

double mass_tree_center_masses_on_branches( MassTree& tree )
{
    double work = 0.0;
    for( auto& edge : tree.edges() ) {
        auto& edge_data = edge->data<MassTreeEdgeData>();

        double const branch_center = edge_data.branch_length / 2;
        double central_mass = 0.0;

        for( auto const& mass : edge_data.masses ) {
            work         += mass.second * std::abs( branch_center - mass.first );
            central_mass += mass.second;
        }

        edge_data.masses.clear();
        edge_data.masses[ branch_center ] = central_mass;
    }
    return work;
}

double mass_tree_center_masses_on_branches_averaged( MassTree& tree )
{
    double work = 0.0;
    for( auto& edge : tree.edges() ) {
        auto& edge_data = edge->data<MassTreeEdgeData>();

        double mass_center = 0.0;
        double mass_sum    = 0.0;

        // Accumulate the mass center by adding the weighted positions,
        // and accumulate the total sum of weights.
        for( auto const& mass : edge_data.masses ) {
            mass_center += mass.first * mass.second;
            mass_sum    += mass.second;
        }

        // Find average mass center by dividing by weight sum.
        mass_center /= mass_sum;

        // Calculate work.
        for( auto const& mass : edge_data.masses ) {
            work += mass.second * std::abs( mass_center - mass.first );
        }

        // Set the new mass at the mass center.
        edge_data.masses.clear();
        edge_data.masses[ mass_center ] = mass_sum;
    }
    return work;
}

double mass_tree_binify_masses( MassTree& tree, size_t number_of_bins )
{
    if( number_of_bins == 0 ) {
        throw std::invalid_argument( "Cannot use number_of_bins == 0." );
    }

    // Take a number and return the closest bin position. The bins are found as the mid points
    // of equally sized intervals distributed over the branch length, with number_of_bins being
    // the number of those intervals/bins.
    auto get_bin_pos = [ number_of_bins ]( double pos, double bl )
    {
        auto const nb = static_cast<double>( number_of_bins );

        // Trim and scale pos to be in interval [ 0.0, nb )
        auto const pn = std::min( std::max( 0.0, pos / bl * nb ), std::nextafter( nb, 0.0 ));

        // Floor it to get to the interval start, then scale back, and add half the interval size,
        // so that we end up at the mid point of the interval.
        return ( std::floor( pn ) * bl / nb ) + ( bl / nb / 2.0 );
    };

    double work = 0.0;
    for( auto& edge : tree.edges() ) {

        // Shorthands.
        auto& edge_data = edge->data<MassTreeEdgeData>();
        auto new_masses = std::map<double, double>();

        // Accumulate masses at the closest bins, and accumulate the work needed to do so.
        for( auto const& mass : edge_data.masses ) {
            auto const bin = get_bin_pos( mass.first, edge_data.branch_length );

            work              += mass.second * std::abs( bin - mass.first );
            new_masses[ bin ] += mass.second;
        }

        // Replace masses by new accumuated ones.
        edge_data.masses = new_masses;
    }

    return work;
}

// =================================================================================================
//     Others
// =================================================================================================

std::vector<double> mass_tree_mass_per_edge( MassTree const& tree )
{
    auto result = std::vector<double>( tree.edge_count(), 0.0 );

    for( auto const& edge : tree.edges() ) {
        auto const& idx = edge->index();
        for( auto const& mass : edge->data<MassTreeEdgeData>().masses ) {
            result[ idx ] += mass.second;
        }
    }

    return result;
}

double mass_tree_sum_of_masses( MassTree const& tree )
{
    double total_mass = 0.0;
    for( auto const& edge : tree.edges() ) {
        for( auto const& mass : edge->data<MassTreeEdgeData>().masses ) {
            total_mass += mass.second;
        }
    }
    return total_mass;
}

bool mass_tree_validate( MassTree const& tree, double valid_total_mass_difference )
{
    // Check tree.
    if( ! validate_topology( tree ) ) {
        LOG_INFO << "Invalid Tree topology.";
        return false;
    }
    if( ! tree_data_is< MassTreeNodeData, MassTreeEdgeData >( tree )) {
        LOG_INFO << "Tree does not only contain Mass Node and Edge data types.";
        return false;
    }

    // Check masses.
    double mass_sum = 0.0;
    for( auto const& edge : tree.edges() ) {
        auto const edge_data = dynamic_cast< MassTreeEdgeData const* >( edge->data_ptr() );
        if( edge_data == nullptr ) {
            LOG_INFO << "Edge data type is not 'MassTreeEdgeData'.";
            return false;
        }

        for( auto const& mass : edge_data->masses ) {
            if( mass.first < 0.0 ) {
                LOG_INFO << "Mass with branch position < 0.0";
                return false;
            }
            if( mass.first > edge_data->branch_length ) {
                LOG_INFO << "Mass with branch position > branch_length";
                return false;
            }

            mass_sum += mass.second;
        }
    }

    if( valid_total_mass_difference >= 0.0 && std::abs(mass_sum) > valid_total_mass_difference ) {
        LOG_INFO << "Total mass difference " << mass_sum
                 << " is higher than " << valid_total_mass_difference;
        return false;
    }
    return true;
}

} // namespace tree
} // namespace genesis
