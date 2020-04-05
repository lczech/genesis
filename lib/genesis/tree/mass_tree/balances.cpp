/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/operators.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/distance.hpp"
#include "genesis/utils/math/statistics.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Balances Data Calculation
// =================================================================================================

BalanceData mass_balance_data(
    std::vector<MassTree> const& trees,
    BalanceSettings settings
) {
    // Prepare (empty) result.
    BalanceData result;

    // Basic checks.
    if( trees.empty() ) {
        return result;
    }
    for( auto const& tree : trees ) {
        if( ! tree_data_is< MassTreeNodeData, MassTreeEdgeData >( tree ) ) {
            throw std::invalid_argument(
                "Tree is not a MassTree. Cannot calculate its balance data."
            );
        }
    }
    if( ! identical_topology( trees )) {
        throw std::invalid_argument(
            "Trees do not have identical topology. Cannot calculate their balance data."
        );
    }
    if(
        ! std::isfinite( settings.pseudo_count_summand_all   ) ||
        ! std::isfinite( settings.pseudo_count_summand_zeros ) ||
        settings.pseudo_count_summand_all   < 0.0              ||
        settings.pseudo_count_summand_zeros < 0.0
    ) {
        throw std::invalid_argument(
            "Pseudo-count summands in balance settings have to be non-negative numbers."
        );
    }

    // Make a nice copy of the tree.
    result.tree = convert_to_common_tree( average_branch_length_tree( trees ));

    // First, get the raw (total) masses. We make the relative later on,
    // because first we might need the raw ones for the tendency and norm terms of the taxon weights.
    result.edge_masses = mass_tree_mass_per_edge( trees );
    assert( result.edge_masses.rows() == trees.size() );
    assert( result.edge_masses.cols() == result.tree.edge_count() );

    // Check that we do not use normalized masses here.
    // For numerical reasons, we use a threshold of 1.1 here, to be sure.
    // This means that metagenomic samples with less than 1.1 sequences cannot be processed.
    // We can live with that.
    for( size_t r = 0; r < result.edge_masses.rows(); ++r ) {
        auto const sum = std::accumulate(
            result.edge_masses.row(r).begin(), result.edge_masses.row(r).end(), 0.0
        );
        if( sum < 1.1 ) {
            throw std::runtime_error(
                "Cannot calculate balance data on trees with normalized masses."
            );
        }
    }

    // Calculate the central tendency of each taxon (column) of the edge masses.
    // If we do not use the tendency term, we default to 1.0.
    // If we only have one tree, we cannot calculate any weights, so we can also skip this.
    auto tendencies = std::vector<double>( result.tree.edge_count(), 1.0 );
    if( trees.size() > 1 && settings.tendency != BalanceSettings::WeightTendency::kNone ) {

        #pragma omp parallel for
        for( size_t c = 0; c < tendencies.size(); ++c ) {
            switch( settings.tendency ) {
                case BalanceSettings::WeightTendency::kNone: {
                    // Can't happen, as we exluded this alreay above.
                    assert( false );
                    break;
                }
                case BalanceSettings::WeightTendency::kMedian: {
                    tendencies[c] = utils::median(
                        result.edge_masses.col(c).begin(), result.edge_masses.col(c).end()
                    );
                    break;
                }
                case BalanceSettings::WeightTendency::kArithmeticMean: {
                    tendencies[c] = utils::arithmetic_mean(
                        result.edge_masses.col(c).begin(), result.edge_masses.col(c).end()
                    );
                    break;
                }
                case BalanceSettings::WeightTendency::kGeometricMean: {
                    // For the geometric mean of the raw counts, we add one to avoid zeros.
                    // This is in accordance with Silverman et al.
                    auto raw = result.edge_masses.col(c).to_vector();
                    for( auto& e : raw ) {
                        e += 1.0;
                    }
                    tendencies[c] = utils::geometric_mean( raw );
                    break;
                }
                default: {
                    assert( false );
                }
            }
            assert( std::isfinite( tendencies[c] ));
            assert( tendencies[c] >= 0.0 );
        }
    }

    // Caluclate the norm of the relative abundances across all trees. In Silverman et al.,
    // it is not explicitly stated whether they calculate the norm on relative abundances with or
    // without pseudo counts. Probably they do not use pseudo counts. So, we do the same here,
    // which requires to make a copy of the masses...
    // If we do not use the norm term, we default to 1.0.
    auto norms = std::vector<double>( result.tree.edge_count(), 1.0 );
    if( trees.size() > 1 && settings.norm != BalanceSettings::WeightNorm::kNone ) {

        // Make a copy of the masses and calcualte relative abundances per row
        // without any pseudo counts.
        auto edge_masses_cpy = result.edge_masses;
        assert( edge_masses_cpy.cols() == result.tree.edge_count() );
        assert( edge_masses_cpy.cols() == norms.size() );
        for( size_t r = 0; r < edge_masses_cpy.rows(); ++r ) {
            utils::closure( edge_masses_cpy.row(r).begin(), edge_masses_cpy.row(r).end() );
        }

        // Calculate the norm on these masses.
        #pragma omp parallel for
        for( size_t c = 0; c < norms.size(); ++c ) {

            // Get iterators, so that we avoid copying the vectors.
            auto em_beg = edge_masses_cpy.col(c).begin();
            auto em_end = edge_masses_cpy.col(c).end();

            switch( settings.norm ) {
                case BalanceSettings::WeightNorm::kNone: {
                    // Can't happen, as we exluded this alreay above.
                    assert( false );
                    break;
                }
                case BalanceSettings::WeightNorm::kManhattan: {
                    norms[c] = utils::manhattan_norm( em_beg, em_end );
                    break;
                }
                case BalanceSettings::WeightNorm::kEuclidean: {
                    norms[c] = utils::euclidean_norm( em_beg, em_end );
                    break;
                }
                case BalanceSettings::WeightNorm::kMaximum: {
                    norms[c] = utils::maximum_norm( em_beg, em_end );
                    break;
                }
                case BalanceSettings::WeightNorm::kAitchison: {
                    norms[c] = utils::aitchison_norm( em_beg, em_end );
                    break;
                }
                default: {
                    assert( false );
                }
            }
            assert( std::isfinite( norms[c] ));
            assert( norms[c] >= 0.0 );
        }
    }

    // Calculate taxon weights as the product of tendency and norm per edge.
    // We only do that for more than one tree, otherwise we leave the weights empty.
    if( trees.size() > 1 ) {
        result.taxon_weights = std::vector<double>( result.tree.edge_count() );
        assert( result.taxon_weights.size() == tendencies.size() );
        assert( result.taxon_weights.size() == norms.size() );
        assert( result.taxon_weights.size() == result.tree.edge_count() );

        for( size_t c = 0; c < result.taxon_weights.size(); ++c ) {
            assert( std::isfinite( tendencies[c] ) && tendencies[c] >= 0.0 );
            assert( std::isfinite( norms[c] ) && norms[c] >= 0.0 );

            result.taxon_weights[c] = tendencies[c] * norms[c];
            assert( std::isfinite( result.taxon_weights[c] ) && result.taxon_weights[c] >= 0.0 );
        }
    }

    // Now, we can add compensation of zero values in form of pseudo counts.
    bool found_zero_mass = false;
    for( auto& em : result.edge_masses ) {
        assert( std::isfinite( em ) && em >= 0.0 );
        if( em == 0.0 ) {
            found_zero_mass = true;
            em += settings.pseudo_count_summand_zeros;
        }
        em += settings.pseudo_count_summand_all;
    }
    if(
        found_zero_mass &&
        settings.pseudo_count_summand_zeros == 0.0 &&
        settings.pseudo_count_summand_all == 0.0
    ) {
        throw std::runtime_error(
            "The balance data contains edge masses with value 0, but no pseudo counts were used. "
            "As such 0 values cannot be used in the balances calculations, you must add pseudo "
            "counts to them, e.g., via the BalanceSettings."
        );
    }

    // So far, we have only added pseudo counts to the edge masses, but no other transformations.
    // This is what we want for the raw masses data. Store it.
    // Then, calculate the closure (relative abundances) per row (tree) of the masses.
    result.raw_edge_masses = result.edge_masses;
    for( size_t r = 0; r < result.edge_masses.rows(); ++r ) {
        utils::closure( result.edge_masses.row(r).begin(), result.edge_masses.row(r).end() );
    }

    // If needed, take taxon weights into account for the masses.
    if( ! result.taxon_weights.empty() ) {
        assert( trees.size() > 1 );
        assert( result.taxon_weights.size() == result.edge_masses.cols() );

        // Get the minimum, which we use as a dummy for taxon weights of zero.
        auto const em_min = utils::finite_minimum(
            result.edge_masses.begin(), result.edge_masses.end()
        );

        #pragma omp parallel for
        for( size_t r = 0; r < result.edge_masses.rows(); ++r ) {
            for( size_t c = 0; c < result.taxon_weights.size(); ++c ) {
                auto& edge_mass = result.edge_masses( r, c );
                auto const& taxon_weight = result.taxon_weights[c];

                // We already made sure that this holds, in the part where pseudo counts are added.
                assert( std::isfinite( edge_mass ) && ( edge_mass > 0.0 ));

                // We also already made this sure.
                assert( std::isfinite( taxon_weight ) && taxon_weight >= 0.0 );

                // The weights are divided instead of multiplied, which is counter-intuitive, but
                // correct according to pers. comm. with Justin Silverman, who referred to
                // > J. J. Egozcue and V. Pawlowsky-Glahn,
                // > "Changing the Reference Measure in the Simplex and its Weighting Effects,"
                // > Austrian J. Stat., vol. 45, no. 4, p. 25, Jul. 2016.
                // for details.
                // We also need a special case for taxon weights of 0. Those cannot happen in the
                // original code, because they base their masses on OTU counts of the data,
                // so there is at least one sample that has a mass > 0 for the taxon.
                // However, as we use a fixed reference tree, it might happen that there are
                // branches/taxa where there is not a single placement in any of the samples.
                // In that case, the taxon weight is 0, and we instead use the pseudo counts as
                // replacement masses in order to avoid numerical issues with infinities etc.
                // Basically, it doesn't matter what (finite) value we set the mass to in that case,
                // as it will be completely ignored because of the 0 weight later on in the
                // geometric mean calculation anyway... but still, better to set it to something
                // reasonable!
                if( taxon_weight == 0.0 ) {
                    // auto const& ps_zeros = settings.pseudo_count_summand_zeros;
                    // auto const& ps_all   = settings.pseudo_count_summand_all;

                    // The pseudo counts cannot be both 0 if we are here. We know that we are at a
                    // taxon with no masses at all, so the above found_zero_mass check was triggered.
                    // If then above both pseudo counts were 0, we'd have had an exception by now.
                    // assert(( ps_zeros > 0.0 ) || ( ps_all > 0.0 ));
                    // edge_mass = ps_zeros + ps_all;

                    // Use a dummy weight.
                    edge_mass = em_min;
                } else {
                    edge_mass /= taxon_weight;
                }
                assert( std::isfinite( edge_mass ) && ( edge_mass > 0.0 ));
            }
        }
    }

    // Assert the result sizes again, just to have that stated explicitly somewhere.
    assert( result.edge_masses.rows() == trees.size() );
    assert( result.edge_masses.cols() == result.tree.edge_count() );
    assert(( trees.size() == 1 ) xor ( result.taxon_weights.size() == result.tree.edge_count() ));

    return result;
}

BalanceData mass_balance_data(
    MassTree const& tree,
    BalanceSettings settings
) {
    // Ugly, but we need to make a copy to avoid a lot of code duplication.
    // At least, it's just a single tree.
    auto const result = mass_balance_data( std::vector<MassTree>{ tree }, settings );

    assert( identical_topology( result.tree, tree ));
    assert( result.edge_masses.rows() == 1 );
    assert( result.edge_masses.cols() == tree.edge_count() );
    assert( result.taxon_weights.empty() );

    return result;
}

// =================================================================================================
//     Balance Calculation
// =================================================================================================

std::vector<double> mass_balance(
    BalanceData const& data,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices
) {
    auto result = std::vector<double>( data.edge_masses.rows(), 0.0 );

    #pragma omp parallel for
    for( size_t r = 0; r < data.edge_masses.rows(); ++r ) {
        result[r] = mass_balance( data, numerator_edge_indices, denominator_edge_indices, r );
    }

    return result;
}

double mass_balance(
    BalanceData const& data,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices,
    size_t tree_index
) {
    // Consistency and input checks.
    if( data.tree.empty() ) {
        throw std::runtime_error(
            "Invalid BalanceData: Cannot calculate balance of an empty tree."
        );
    }
    if( ! data.taxon_weights.empty() && data.taxon_weights.size() != data.edge_masses.cols() ) {
        throw std::runtime_error(
            "Invalid BalanceData: Taxon weights need to have same size as edge masses."
        );
    }
    if( data.edge_masses.cols() != data.tree.edge_count() ) {
        throw std::runtime_error(
            "Invalid BalanceData: Edge masses need to have same size as the Tree has edges."
        );
    }
    if( numerator_edge_indices.empty() || denominator_edge_indices.empty() ) {
        throw std::invalid_argument( "Cannot calculate mass balance of empty edge sets." );
    }
    if( tree_index >= data.edge_masses.rows() ) {
        throw std::invalid_argument(
            "Cannot calculate mass balance with tree index greater than number of trees."
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
    auto calc_mass_mean_and_scaling_ = [ &data, &tree_index ](
        std::unordered_set<size_t> const& indices
    ){
        // Prepare temporary vectors that store copies of the masses and weights.
        std::vector<double> sub_masses;
        std::vector<double> sub_weights;
        sub_masses.reserve( indices.size() );
        sub_weights.reserve( indices.size() );

        // Add the mases and weights to the vectors.
        for( auto idx : indices ) {

            // Collect masses at the edge indices.
            if( idx >= data.edge_masses.cols() ) {
                throw std::runtime_error( "Invalid edge index in mass balance calculation." );
            }
            assert( tree_index < data.edge_masses.rows() );
            assert( std::isfinite( data.edge_masses( tree_index, idx )));
            assert( data.edge_masses( tree_index, idx ) >= 0.0 );
            sub_masses.push_back( data.edge_masses( tree_index, idx ));

            // Collect weights at the edge indices. If we do not have weights, use 1.0 instead.
            if( data.taxon_weights.empty() ) {
                sub_weights.push_back( 1.0 );
            } else {
                assert( idx < data.taxon_weights.size() );
                assert( std::isfinite( data.taxon_weights[idx] ));
                sub_weights.push_back( data.taxon_weights[idx] );
            }
        }
        assert( sub_masses.size()  == indices.size() );
        assert( sub_weights.size() == indices.size() );

        // Calculate the mean and the n for scaling.
        // Check if all weights are zero, which can happen for subtrees with no placements at all.
        // Set them to nan, so that subsequent calculations simply ignore these values.
        double scaling_n = std::accumulate( sub_weights.begin(), sub_weights.end(), 0.0 );
        assert( std::isfinite( scaling_n ));
        double geom_mean;
        if( scaling_n > 0.0 ) {
            geom_mean = utils::weighted_geometric_mean( sub_masses, sub_weights );
            assert( std::isfinite( geom_mean ));
        } else {
            geom_mean = std::numeric_limits<double>::quiet_NaN();
        }

        // If we have no edge weights, the scaling terms should be the same as the number of edges.
        assert(
            ! data.taxon_weights.empty() ||
            utils::almost_equal_relative( scaling_n, indices.size(), 0.1 )
        );

        return BalanceTerms{ geom_mean, scaling_n };
    };

    // Get geometric means of edge subset masses, and the weighted scaling terms.
    auto const num = calc_mass_mean_and_scaling_( numerator_edge_indices );
    auto const den = calc_mass_mean_and_scaling_( denominator_edge_indices );
    assert( std::isnan( num.mean ) || ( num.mean > 0.0 ));
    assert( std::isnan( den.mean ) || ( den.mean > 0.0 ));

    // Calculate the balance.
    double const scaling = std::sqrt(( num.scaling * den.scaling ) / ( num.scaling + den.scaling ));
    double const balance = scaling * std::log( num.mean / den.mean );
    assert( std::isnan( balance ) || std::isfinite( balance ));

    return balance;
}

} // namespace tree
} // namespace genesis
