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
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Phylogenetic ILR Tranform
// =================================================================================================

std::vector<double> mass_balance_edge_weights(
    std::vector<MassTree> const& trees,
    BalanceSettings balance_settings
) {
    if( trees.empty() ) {
        return std::vector<double>();
    }

    // Prepare result. Default to 1.0, that is, no weights.
    auto result = std::vector<double>( trees[0].edge_count(), 1.0 );

    // Shortcut: If no weights are to be calcualted, we can immediately return.
    if(
        balance_settings.tendency == BalanceSettings::WeightTendency::kNone &&
        balance_settings.norm     == BalanceSettings::WeightNorm::kNone
    ) {
        return result;
    }

    // Get masses per edge of all tress.
    auto const edge_masses = mass_tree_mass_per_edge( trees );
    assert( edge_masses.rows() == trees.size() );
    assert( edge_masses.cols() == result.size() );

    // Calculate the weight for each edge.
    #pragma omp parallel for
    for( size_t c = 0; c < trees[0].edge_count(); ++c ) {
        auto counts = edge_masses.col(c).to_vector();

        // Get a central tendency of counts.
        double tendency = 1.0;
        switch( balance_settings.tendency ) {
            case BalanceSettings::WeightTendency::kNone: {
                break;
            }
            case BalanceSettings::WeightTendency::kMedian: {
                tendency = utils::median( counts );
                break;
            }
            case BalanceSettings::WeightTendency::kArithmeticMean: {
                tendency = utils::arithmetic_mean( counts );
                break;
            }
            case BalanceSettings::WeightTendency::kGeometricMean: {
                // For the geometric mean of the raw counts, we add one to avoid zeros.
                auto raw = counts;
                for( auto& e : raw ) {
                    e += 1.0;
                }
                tendency = utils::geometric_mean( raw );
                break;
            }
            default: {
                assert( false );
            }
        }

        // Get a norm of the relative abundances of the taxon across all trees.
        // This is calcualted on the relative abundances, so build the closure of the masses first.
        utils::closure( counts );
        double norm = 1.0;
        switch( balance_settings.norm ) {
            case BalanceSettings::WeightNorm::kNone: {
                break;
            }
            case BalanceSettings::WeightNorm::kManhattan: {
                norm = utils::manhattan_norm( counts );
                break;
            }
            case BalanceSettings::WeightNorm::kEuclidean: {
                norm = utils::euclidean_norm( counts );
                break;
            }
            case BalanceSettings::WeightNorm::kMaximum: {
                norm = utils::maximum_norm( counts );
                break;
            }
            case BalanceSettings::WeightNorm::kAitchison: {
                norm = utils::aitchison_norm( counts );
                break;
            }
            default: {
                assert( false );
            }
        }

        result[c] = tendency * norm;
        assert( std::isfinite( result[c] ) && result[c] >= 0.0 );
    }

    return result;
}

double mass_balance(
    std::vector<double> const& edge_masses,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices,
    std::vector<double> const& edge_weights
) {
    if( numerator_edge_indices.empty() || denominator_edge_indices.empty() ) {
        throw std::runtime_error( "Cannot calculate mass balance of empty edge sets." );
    }
    if( ! edge_weights.empty() && edge_weights.size() != edge_masses.size() ) {
        throw std::invalid_argument(
            "Edge weights need to have same size as edge masses."
        );
    }

    // Helper struct for more expressive code.
    struct BalanceTerms
    {
        double mean;
        double scaling;
    };

    // Helper function that calculates the geom mean of the `edge_masses` of the given edge indices,
    // and the scaling term n used for normalization of the balance.
    // This would be a perfect fit for range filters. But we don't have them in CPP11,
    // so we need to make copies instead...
    auto calc_mass_mean_and_scaling_ = [ &edge_masses, &edge_weights ](
        std::unordered_set<size_t> const& indices
    ){
        std::vector<double> sub_masses;
        std::vector<double> sub_weights;
        for( auto idx : indices ) {

            // Collect masses at the edge indices.
            if( idx >= edge_masses.size() ) {
                throw std::runtime_error( "Invalid edge index in mass balance calculation." );
            }
            sub_masses.push_back( edge_masses[idx] );

            // Collect weights at the edge indices. If we do not have weights, use 1.0 instead.
            if( edge_weights.empty() ) {
                sub_weights.push_back( 1.0 );
            } else {
                assert( idx < edge_weights.size() );
                sub_weights.push_back( edge_weights[idx] );
            }
        }
        assert( sub_masses.size()  == indices.size() );
        assert( sub_weights.size() == indices.size() );

        // Calcualte the mean and the n for scaling.
        double geom_mean = utils::weighted_geometric_mean( sub_masses, sub_weights );
        double scaling_n = std::accumulate( sub_weights.begin(), sub_weights.end(), 0.0 );

        // If we have no edge weights, the scaling terms should be the same as the number of edges.
        assert(
            ! edge_weights.empty() ||
            utils::almost_equal_relative( scaling_n, indices.size(), 0.1 )
        );

        // return utils::geometric_mean( sub_masses );
        return BalanceTerms{ geom_mean, scaling_n };
    };

    // Get geometric means of edge subset masses, and the weighted scaling terms.
    auto const num = calc_mass_mean_and_scaling_( numerator_edge_indices );
    auto const den = calc_mass_mean_and_scaling_( denominator_edge_indices );
    assert( num.mean > 0.0 && den.mean > 0.0 );

    // Scaling terms without taking weights into account. Not longer needed.
    // double const size_l  = static_cast<double>( numerator_edge_indices.size() );
    // double const size_r  = static_cast<double>( denominator_edge_indices.size() );

    // Calculate the balance.
    double const scaling = std::sqrt(( num.scaling * den.scaling ) / ( num.scaling + den.scaling ));
    double const balance = scaling * std::log( num.mean / den.mean );
    assert( std::isfinite( balance ));

    // LOG_DBG << "num_mean " << num_mean << " den_mean " << den_mean;
    // LOG_DBG << "size_l " << size_l << " size_r " << size_r;
    // LOG_DBG << "scaling " << scaling << " balance " << balance;
    // LOG_DBG;

    return balance;
}

std::vector<double> phylogenetic_ilr_transform(
    MassTree const& tree,
    BalanceSettings balance_settings,
    std::vector<double> const& edge_weights
) {
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
        throw std::invalid_argument(
            "Tree is not a MassTree. Cannot calculate its Phylogenetic ILR transform."
        );
    }
    if( ! edge_weights.empty() && edge_weights.size() != tree.edge_count() ) {
        throw std::invalid_argument(
            "Edge weights need to have same size as the edge count of the provided Tree."
        );
    }
    if(
        ! std::isfinite( balance_settings.pseudo_count_summand_all   ) ||
        ! std::isfinite( balance_settings.pseudo_count_summand_zeros ) ||
        balance_settings.pseudo_count_summand_all   < 0.0              ||
        balance_settings.pseudo_count_summand_zeros < 0.0
    ) {
        throw std::invalid_argument(
            "Pseudo-count summands in the balance settings have to be non-negative numbers."
        );
    }

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

    // Add compensation of zero values, then calculate the closure (relative abundances)
    // of the masses, and if needed, take weights into account.
    for( auto& e : edge_masses ) {
        assert( std::isfinite( e ) && e >= 0.0 );
        if( e == 0.0 ) {
            e += balance_settings.pseudo_count_summand_zeros;
        }
        e += balance_settings.pseudo_count_summand_all;
    }
    utils::closure( edge_masses );
    if( ! edge_weights.empty() ) {
        assert( edge_weights.size() == edge_masses.size() );
        for( size_t i = 0; i < edge_weights.size(); ++i ) {
            edge_masses[i] /= edge_weights[i];
        }
    }

    // Get the edge indices of a particular subtree, including the edge that leads to it.
    // It is slightly inefficient to first store the indices and then do the lookup of masses
    // later, but this way, we save code duplication of the balance calculation function...
    auto get_subtree_indices = []( Subtree const& subtree ){
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

    // Prepare result list for each node.
    auto result = std::vector<double>( tree.node_count(), 0.0 );

    // Calculate balance for every node of the tree.
    #pragma omp parallel for
    for( size_t node_idx = 0; node_idx < tree.node_count(); ++node_idx ) {
        auto const& node = tree.node_at( node_idx );
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
            lhs_indices = get_subtree_indices({ node.link().outer() });
            rhs_indices = get_subtree_indices({ node.link().next().outer() });

            // After that, we should have all edges of the tree.
            assert( lhs_indices.size() + rhs_indices.size() == tree.edge_count() );
        } else {
            assert( deg == 3 );

            // At inner nodes, the primary link points towards the root, so we use the next two links.
            lhs_indices = get_subtree_indices({ node.link().next().outer() });
            rhs_indices = get_subtree_indices({ node.link().next().next().outer() });

            // We never have more edges than the tree.
            assert( lhs_indices.size() + rhs_indices.size() < tree.edge_count() );
        }

        // Calculate and store balance.
        if( balance_settings.reverse_signs ) {
            result[ node_idx ] = mass_balance( edge_masses, rhs_indices, lhs_indices, edge_weights );
        } else {
            result[ node_idx ] = mass_balance( edge_masses, lhs_indices, rhs_indices, edge_weights );
        }
    }

    return result;
}

utils::Matrix<double> phylogenetic_ilr_transform(
    std::vector<MassTree> const& trees,
    BalanceSettings balance_settings
) {
    // Basic check. All other checks are done in the per-tree function.
    if( ! identical_topology( trees )) {
        throw std::invalid_argument(
            "Trees do not have identical topology. Cannot calculate their Phylogenetic ILR tranform matrix."
        );
    }
    if( trees.empty() ) {
        return {};
    }

    // Calculate edge weights, and prepare result matrix.
    auto const edge_weights = mass_balance_edge_weights( trees, balance_settings );
    auto result = utils::Matrix<double>( trees.size(), trees[0].node_count(), 0.0 );

    #pragma omp parallel for
    for( size_t i = 0; i < trees.size(); ++i ) {
        result.row( i ) = phylogenetic_ilr_transform( trees[i], balance_settings, edge_weights );
    }

    return result;
}

} // namespace tree
} // namespace genesis
