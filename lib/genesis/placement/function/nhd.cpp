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
 * @ingroup placement
 */

#include "genesis/placement/function/emd.hpp"

#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/function/distances.hpp"

#include "genesis/utils/math/histogram.hpp"
#include "genesis/utils/math/histogram/accumulator.hpp"
#include "genesis/utils/math/histogram/distances.hpp"
#include "genesis/utils/math/histogram/operations.hpp"
#include "genesis/utils/math/histogram/operators.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"

#include <cassert>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Node Histogram Distance
// =================================================================================================

// -------------------------------------------------------------------------------------------------
//     Helper Functions
// -------------------------------------------------------------------------------------------------

/**
 * @brief Local helper function to calculate the histograms of distances from all
 * @link tree::TreeNode Nodes@endlink of the @link PlacementTree Tree@endlink of a Sample to all
 * its PqueryPlacement%s.
 */
std::vector< utils::Histogram > node_distance_histograms (
    Sample const& sample,
    size_t const  histogram_bins
) {
    // Prepare a vector of histograms for each node of the tree.
    auto hist_vec = std::vector< utils::Histogram >();
    hist_vec.reserve( sample.tree().node_count() );

    // Calculate the pairwise distance between all pairs of nodes.
    auto const node_dists = node_branch_length_distance_matrix( sample.tree() );

    // Calculate the longest distance from any node. This is used as upper bound for the histograms.
    auto const diameters = furthest_leaf_distance_vector( sample.tree(), node_dists );

    // Init the histograms, using the diameter at each node as max values.
    for( size_t node_index = 0; node_index < sample.tree().node_count(); ++node_index ) {
        hist_vec.push_back( utils::Histogram( histogram_bins, 0.0, diameters[ node_index ].second ));
    }

    // We use this order of loops, as it should be faster to touch each pquery once, and
    // instead update the histograms multiple times, instead of calculating a histogram for each
    // node spearately, which would require multiple iterations of the pqueries.
    for( auto const& pquery : sample ) {
        double const mult = total_multiplicity( pquery );

        for( auto const& placement : pquery.placements() ) {
            for( size_t node_index = 0; node_index < sample.tree().node_count(); ++node_index ) {

                double const p_dist = placement.proximal_length
                + node_dists( node_index, placement.edge().primary_node().index() );

                double const d_dist = placement.edge().data<PlacementEdgeData>().branch_length
                - placement.proximal_length
                + node_dists( node_index, placement.edge().secondary_node().index() );

                double const dist = std::min( p_dist, d_dist );
                assert( dist >= 0.0 && dist <= diameters[ node_index ].second );

                // Accumulate at the distance, using the lwr and multiplicity as weight, so that the
                // total weight of a pquery sums up to the multiplicity.
                hist_vec[ node_index ].accumulate( dist, placement.like_weight_ratio * mult );
            }
        }
    }

    // Normalize.
    for( auto& hist : hist_vec ) {
        normalize( hist );
    }

    return hist_vec;
}

// -------------------------------------------------------------------------------------------------
//     Between Samples
// -------------------------------------------------------------------------------------------------

double node_histogram_distance (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t const  histogram_bins
) {
    if( ! compatible_trees( sample_a, sample_b ) ) {
        throw std::invalid_argument( "Incompatible trees." );
    }

    // Get the histograms describing the distances from placements to all nodes.
    auto const hist_vec_a = node_distance_histograms( sample_a, histogram_bins );
    auto const hist_vec_b = node_distance_histograms( sample_b, histogram_bins );

    // If the trees are compatible (as ensured in the beginning of this function), they need to have
    // the same number of nodes. Thus, also there should be this number of histograms in the vectors.
    assert( hist_vec_a.size() == sample_a.tree().node_count() );
    assert( hist_vec_b.size() == sample_b.tree().node_count() );
    assert( hist_vec_a.size() == hist_vec_b.size() );

    double dist = 0.0;
    for( size_t i = 0; i < hist_vec_a.size(); ++i ) {
        dist += earth_movers_distance( hist_vec_a[i], hist_vec_b[i], false );
    }
    assert( dist >= 0.0 );

    // Return normalized distance.
    return dist / static_cast< double >( sample_a.tree().node_count() );
}

// -------------------------------------------------------------------------------------------------
//     Sample Set
// -------------------------------------------------------------------------------------------------

utils::Matrix<double> node_histogram_distance (
    SampleSet const& sample_set,
    size_t const     histogram_bins
) {
    auto const set_size = sample_set.size();

    // Prepare histograms for all samples.
    auto hist_vecs = std::vector< std::vector< utils::Histogram >>( set_size );

    // Calculate the histograms for all samples.
    #pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < set_size; ++i ) {

        // Check compatibility.
        // It suffices to check adjacent pairs of samples, as compatibility is transitive.
        if( i > 0 ) {
            if( ! compatible_trees( sample_set[ i - 1 ].sample, sample_set[ i ].sample )) {
                throw std::invalid_argument(
                    "Trees in SampleSet not compatible for calculating Node Histogram Distance."
                );
            }
        }

        // Calculate the histograms for every node of the sample.
        hist_vecs[ i ] = node_distance_histograms( sample_set[ i ].sample, histogram_bins );
        assert( hist_vecs[ i ].size() == sample_set[ i ].sample.tree().node_count() );
    }

    // Init distance matrix.
    auto result = utils::Matrix<double>( set_size, set_size, 0.0 );

    // Local helper function to calculate the sum of histogram emds between two sets of histograms.
    auto histogram_emd = [] (
        std::vector< utils::Histogram > const& lhs,
        std::vector< utils::Histogram > const& rhs,
        size_t const                           node_count
    ) {
        // Sum up the emd distances of the histograms for each node of the tree in the
        // two samples.
        double dist = 0.0;
        assert( lhs.size() == rhs.size() );
        for( size_t k = 0; k < lhs.size(); ++k ) {
            dist += earth_movers_distance( lhs[ k ], rhs[ k ], false );
        }
        assert( dist >= 0.0 );

        // Return normalized distance.
        dist /= static_cast< double >( node_count );
        return dist;
    };

    // Parallel specialized code.
    #ifdef GENESIS_OPENMP

        // We only need to calculate the upper triangle. Get the number of indices needed
        // to describe this triangle.
        size_t const max_k = utils::triangular_size( set_size );

        // Calculate distance matrix for every pair of samples.
        #pragma omp parallel for
        for( size_t k = 0; k < max_k; ++k ) {

            // For the given linear index, get the actual position in the Matrix.
            auto const ij = utils::triangular_indices( k, set_size );
            auto const i = ij.first;
            auto const j = ij.second;

            // Calculate and store distance.
            auto const node_count = sample_set[ i ].sample.tree().node_count();
            auto const dist = histogram_emd( hist_vecs[ i ], hist_vecs[ j ], node_count );
            result(i, j) = dist;
            result(j, i) = dist;
        }

    // If no threads are available at all, use serial version.
    #else

        // Calculate distance matrix for every pair of samples.
        for( size_t i = 0; i < set_size; ++i ) {

            // The result is symmetric - we only calculate the upper triangle.
            for( size_t j = i + 1; j < set_size; ++j ) {

                // Calculate and store distance.
                auto const node_count = sample_set[ i ].sample.tree().node_count();
                auto const dist = histogram_emd( hist_vecs[ i ], hist_vecs[ j ], node_count );
                result(i, j) = dist;
                result(j, i) = dist;
            }
        }

    #endif

    return result;
}

} // namespace placement
} // namespace genesis
