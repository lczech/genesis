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

#include "genesis/tree/mass_tree/functions.hpp"

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
//     Manipulate Masses
// =================================================================================================

MassTree mass_tree_merge_trees(
    MassTree const& lhs,
    MassTree const& rhs,
    double const scaler_lhs,
    double const scaler_rhs
) {
    auto copy = lhs;
    mass_tree_merge_trees_inplace( copy, rhs, scaler_lhs, scaler_rhs );
    return copy;
}

void mass_tree_merge_trees_inplace(
    MassTree& lhs,
    MassTree const& rhs,
    double const scaler_lhs,
    double const scaler_rhs
) {
    // Do at least a basic check.
    if( lhs.edge_count() != rhs.edge_count() ) {
        throw std::runtime_error( "Incompatible MassTrees for merging." );
    }

    // Only do the work if needed.
    if( scaler_lhs != 1.0 ) {
        mass_tree_scale_masses( lhs, scaler_lhs );
    }

    #pragma omp parallel for
    for( size_t i = 0; i < lhs.edge_count(); ++i ) {
        auto& lhs_masses = lhs.edge_at( i ).data<MassTreeEdgeData>().masses;
        for( auto const& rhs_mass : rhs.edge_at( i ).data<MassTreeEdgeData>().masses ) {
            lhs_masses[ rhs_mass.first ] += scaler_rhs * rhs_mass.second;
        }
    }
}

void mass_tree_clear_masses( MassTree& tree )
{
    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        tree.edge_at(i).data<MassTreeEdgeData>().masses.clear();
    }
}

void mass_tree_reverse_signs( MassTree& tree )
{
    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto& masses = tree.edge_at( i ).data<MassTreeEdgeData>().masses;
        for( auto& mass : masses ) {
            mass.second = -mass.second;
        }
    }
}

void mass_tree_scale_masses( MassTree& tree, double factor )
{
    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto& masses = tree.edge_at( i ).data<MassTreeEdgeData>().masses;
        for( auto& mass : masses ) {
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

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        for( auto& mass : tree.edge_at(i).data<MassTreeEdgeData>().masses ) {
            mass.second /= total_mass;
        }
    }
}

void mass_tree_transform_to_unit_branch_lengths( MassTree& tree )
{
    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto& edge_data = tree.edge_at(i).data<MassTreeEdgeData>();
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

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto& edge_data = tree.edge_at(i).data<MassTreeEdgeData>();

        double const branch_center = edge_data.branch_length / 2;
        double central_mass = 0.0;

        for( auto const& mass : edge_data.masses ) {
            #pragma omp atomic
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

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto& edge_data = tree.edge_at(i).data<MassTreeEdgeData>();

        // No masses on the edge. We need to skip the rest, otherwise we end up having a nan values
        // as mass centers, which leads to nan earth mover distance values, which leads to invalid
        // kmeans cluster centroid assigments, which leads to crashes. What a stupid bug that was.
        if( edge_data.masses.empty() ) {
            continue;
        }

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
            #pragma omp atomic
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

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {

        // Shorthands.
        auto& edge_data = tree.edge_at(i).data<MassTreeEdgeData>();
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

bool mass_tree_all_identical_topology( std::vector<MassTree> const& mass_trees )
{
    // If all pairs of two adjacent trees have same the topology, all of them have.
    // Thus, we do not need a complete pairwise comparision.
    for (size_t i = 1; i < mass_trees.size(); i++) {
        if( ! identical_topology( mass_trees[i-1], mass_trees[i] )) {
            return false;
        }
    }
    return true;
}

void mass_trees_make_average_branch_lengths( std::vector<MassTree>& mass_trees )
{
    // Nothing to do.
    if( mass_trees.size() < 2 ) {
        return;
    }

    // Store averages
    size_t const num_edges = mass_trees[0].edge_count();
    auto avg_br_lens = std::vector<double>( num_edges, 0.0 );

    // Accumulate averages
    for( auto const& tree : mass_trees ) {

        // Check
        if( tree.edge_count() != num_edges ) {
            throw std::runtime_error(
                "Cannot make average branch lengths, because trees have different size."
            );
        }

        // Accu
        assert( avg_br_lens.size() == tree.edge_count() );
        for( size_t edge_idx = 0; edge_idx < num_edges; ++edge_idx ) {
            avg_br_lens[edge_idx] += tree.edge_at( edge_idx ).data<MassTreeEdgeData>().branch_length;
        }
    }

    // Average
    for( auto& ae : avg_br_lens ) {
        ae /= static_cast<double>( mass_trees.size() );
    }

    // Set branch lengths and ajust masses.
    for( auto& tree : mass_trees ) {

        #pragma omp parallel for
        for( size_t edge_idx = 0; edge_idx < tree.edge_count(); ++edge_idx ) {

            // Setup.
            auto& edge_data = tree.edge_at( edge_idx ).data<MassTreeEdgeData>();
            auto new_masses = std::map<double, double>();

            // Branch position scaler.
            auto const scaler = avg_br_lens[ edge_idx ] / edge_data.branch_length;

            // Move masses proprotional to branch lengths change.
            for( auto const& mass : edge_data.masses ) {

                auto const new_pos = mass.first * scaler;
                new_masses[ new_pos ] += mass.second;
            }

            // Replace masses by new accumuated ones, and change br len
            edge_data.masses = new_masses;
            edge_data.branch_length = avg_br_lens[ edge_idx ];
        }
    }
}

std::vector<double> mass_tree_mass_per_edge( MassTree const& tree )
{
    auto result = std::vector<double>( tree.edge_count(), 0.0 );

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto const& edge = tree.edge_at(i);
        auto const& idx = edge.index();
        for( auto const& mass : edge.data<MassTreeEdgeData>().masses ) {
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
