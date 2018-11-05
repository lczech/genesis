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

#include "genesis/utils/math/statistics.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/text/string.hpp"

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

double mass_balance(
    std::vector<double> const& edge_masses,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices
) {
    if( numerator_edge_indices.empty() || denominator_edge_indices.empty() ) {
        throw std::runtime_error( "Cannot calculate mass balance of empty edge sets." );
    }

    // Helper function that calculates the geom mean of the `edge_masses` of the given edge indices.
    // (This would be a perfect fit for range filters... but we don't have them in CPP11...)
    auto calc_mass_mean = [ &edge_masses ]( std::unordered_set<size_t> const& indices ){
        std::vector<double> sub_masses;
        for( auto idx : indices ) {
            if( idx >= edge_masses.size() ) {
                throw std::runtime_error( "Invalid edge index in mass balance calculation." );
            }
            sub_masses.push_back( edge_masses[idx] );
        }
        return utils::geometric_mean( sub_masses );
    };

    // Get geometric means of edge subset masses.
    double const num_mean = calc_mass_mean( numerator_edge_indices );
    double const den_mean = calc_mass_mean( denominator_edge_indices );

    // Calculate the balance.
    double const size_l  = static_cast<double>( numerator_edge_indices.size() );
    double const size_r  = static_cast<double>( denominator_edge_indices.size() );
    double const scaling = std::sqrt(( size_l * size_r ) / ( size_l + size_r ));
    double const balance = scaling * std::log( num_mean / den_mean );
    assert( std::isfinite( balance ));

    // LOG_DBG << "num_mean " << num_mean << " den_mean " << den_mean;
    // LOG_DBG << "size_l " << size_l << " size_r " << size_r;
    // LOG_DBG << "scaling " << scaling << " balance " << balance;
    // LOG_DBG;

    return balance;
}

std::vector<double> phylogenetic_ilr_transform( MassTree const& tree )
{
    // Edge cases and input checks.
    if( tree.empty() ) {
        return std::vector<double>();
    }
    if( ! is_rooted( tree )) {
        throw std::invalid_argument(
            "Tree is not rooted. Cannot calculate its Phylogenetic ILR tranform."
        );
    }
    if( ! is_bifurcating( tree )) {
        throw std::invalid_argument(
            "Tree is not bifurcating. Cannot calculate its Phylogenetic ILR tranform."
        );
    }
    if( ! tree_data_is< MassTreeNodeData, MassTreeEdgeData >( tree ) ) {
        throw std::invalid_argument( "Tree is not a MassTree. cannot calculate its Phylogenetic ILR transform." );
    }

    // Prepare result list for each node.
    auto result = std::vector<double>( tree.node_count(), 0.0 );

    // Get per-edge masses, stored in edge index order.
    auto edge_masses = mass_tree_mass_per_edge( tree );

    // Not needed right now:
    // Get the mass per branch as well as their average position on the branch.
    // (mass second value, position first value of the returned pairs.)
    // auto const masses = mass_tree_mass_per_edge_averaged( tree );

    // Check that we do not use normalized masses here.
    // For numerical reasons, we use a threshold of 1.1 here, to be sure.
    // This means that metagenomic samples with less than 1.1 sequences cannot be processed.
    // We can live with that.
    if( std::accumulate( edge_masses.begin(), edge_masses.end(), 0.0 ) < 1.1 ) {
        throw std::runtime_error(
            "Cannot calculate Phylogenetic ILR transform on Trees with normalized masses."
        );
    }

    // Add compensation of zero values.
    for( auto& e : edge_masses ) {
        e += 0.65;
    }

    // Get the edge indices of a particular subtree, including the edge that leads to it.
    // It is slightly inefficient to first store the indices and then do the lookup of masses
    // later, but this way, we save code duplication of the balance calculation function...
    auto get_subtree_indices = []( Subtree const& subtree ){
        std::unordered_set<size_t> sub_indices;
        for( auto it : preorder( subtree )) {

            // The preorder iterator is node based, so we start at the root node of the subtree,
            // meaning that we automatically also insert its edge index! No special case needed.
            sub_indices.insert( it.edge().index() );
        }
        return sub_indices;
    };

    // Calculate balance for every node of the tree.
    #pragma omp parallel for
    for( size_t node_idx = 0; node_idx < tree.node_count(); ++node_idx ) {
        auto const& node = tree.node_at( node_idx );

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
            lhs_indices = get_subtree_indices({ node.link().outer() });
            rhs_indices = get_subtree_indices({ node.link().next().outer() });
        } else {
            assert( deg == 3 );

            // At inner nodes, the primary link points towards the root, so we use the next two links.
            lhs_indices = get_subtree_indices({ node.link().next().outer() });
            rhs_indices = get_subtree_indices({ node.link().next().next().outer() });
        }

        // We never have more edges than the tree, unless its the root node, then we have all.
        assert(
            ( lhs_indices.size() + rhs_indices.size()  < tree.edge_count() && ! is_root( node )) ||
            ( lhs_indices.size() + rhs_indices.size() == tree.edge_count() && is_root( node ))
        );

        // Calculate and store balance.
        result[ node_idx ] = mass_balance( edge_masses, lhs_indices, rhs_indices );
    }

    return result;
}

utils::Matrix<double> phylogenetic_ilr_transform( std::vector<MassTree> const& trees )
{
    // Basic check. All other checks are done in the per-tree function.
    if( ! identical_topology( trees )) {
        throw std::invalid_argument(
            "Trees do not have identical topology. Cannot calculate their Phylogenetic ILR tranform matrix."
        );
    }
    if( trees.empty() ) {
        return {};
    }

    auto result = utils::Matrix<double>( trees.size(), trees[0].node_count(), 0.0 );

    #pragma omp parallel for
    for( size_t i = 0; i < trees.size(); ++i ) {
        result.row( i ) = phylogenetic_ilr_transform( trees[i] );
    }

    return result;
}

} // namespace tree
} // namespace genesis
