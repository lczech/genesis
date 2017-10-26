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
#include "genesis/tree/function/functions.hpp"

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
//     Local Helper Functions
// =================================================================================================

// -------------------------------------------------------------------------------------------------
//     make_empty_node_distance_histograms
// -------------------------------------------------------------------------------------------------

/**
 * @brief Create a set of Histograms without any weights for a given Tree.
 */
std::vector< utils::Histogram > make_empty_node_distance_histograms (
    tree::Tree const& tree,
    utils::Matrix<double> const& node_distances,
    utils::Matrix<signed char> const& node_sides,
    size_t const  histogram_bins
) {
    auto const node_count = tree.node_count();
    if( tree.empty() ) {
        throw std::runtime_error( "Tree is empty. Cannot use Node Histogram Distance." );
    }
    if( node_distances.rows() != node_count || node_distances.cols() != node_count ) {
        throw std::runtime_error( "Node Distance Matrix has wrong size." );
    }
    if( node_sides.rows() != node_count || node_sides.cols() != node_count ) {
        throw std::runtime_error( "Node Sides Matrix has wrong size." );
    }

    // Prepare a vector of histograms for each node of the tree.
    // We init with dummy histograms, so that we can better parallelize later...
    // Stupid, but okay for now.
    auto histograms = std::vector< utils::Histogram >();
    histograms.resize( node_count, utils::Histogram( histogram_bins ));

    // Make histograms that have enough room on both sides.
    #pragma omp parallel for
    for( size_t node_idx = 0; node_idx < node_count; ++node_idx ) {

        // Find furthest nodes on root and non-root sides.
        // For now, we use both positive values, and later reverse the sign of the min entry.
        double min = 0.0;
        double max = 0.0;
        for( size_t col_idx = 0; col_idx < node_count; ++col_idx ) {

            // No need to get a distance from the node to itself.
            if( col_idx == node_idx ) {
                // assert( node_sides( node_idx, col_idx ) == 0 );
                continue;
            }

            // Get distance between nodes.
            auto const dist = node_distances( node_idx, col_idx );

            // If we are at a node that is on the root side, optimize max.
            if( node_sides( node_idx, col_idx ) == 1 && dist > max ) {
                max = dist;
            }

            // If we are at a node that is on a non-root side, optimize min.
            if( node_sides( node_idx, col_idx ) == -1 && dist > min ) {
                min = dist;
            }
        }

        // If this fails, the tree is not usable.
        if( min == 0.0 && max == 0.0 ) {
            throw std::runtime_error(
                "Tree only has branch lengths with value 0. Cannot use Node Histogram Distance."
            );
        }

        histograms[node_idx] = utils::Histogram( histogram_bins, -min, max );
    }

    return histograms;
}

// -------------------------------------------------------------------------------------------------
//     fill_node_distance_histograms
// -------------------------------------------------------------------------------------------------

/**
 * @brief Fill the placements of a Sample into Histograms.
 */
void fill_node_distance_histograms (
    Sample const& sample,
    utils::Matrix<double> const& node_distances,
    utils::Matrix<signed char> const& node_sides,
    std::vector< utils::Histogram >& histograms
) {
    auto const node_count = sample.tree().node_count();
    if( histograms.size() != node_count ) {
        throw std::runtime_error( "Number of histograms does not equal number of tree nodes." );
    }
    if( node_distances.rows() != node_count || node_distances.cols() != node_count ) {
        throw std::runtime_error( "Node Distance Matrix has wrong size." );
    }
    if( node_sides.rows() != node_count || node_sides.cols() != node_count ) {
        throw std::runtime_error( "Node Sides Matrix has wrong size." );
    }

    // We use this order of loops, as it should be faster to touch each pquery once, and
    // instead update the histograms multiple times, instead of fill the histogram for each
    // node spearately, which would require multiple iterations of the pqueries.
    for( auto const& pquery : sample ) {
        double const mult = total_multiplicity( pquery );

        for( auto const& placement : pquery.placements() ) {
            for( size_t node_index = 0; node_index < sample.tree().node_count(); ++node_index ) {

                // Get the distance from the placement to the current histogram node.
                double const p_dist = placement.proximal_length
                    + node_distances( node_index, placement.edge().primary_node().index() );
                double const d_dist = placement.edge().data<PlacementEdgeData>().branch_length
                    - placement.proximal_length
                    + node_distances( node_index, placement.edge().secondary_node().index() );
                double const dist = std::min( p_dist, d_dist );

                // Get the side of the placement relative to the current node.
                // Value 1 means, it is on the root side. Values 0 and -1 mean a non root side.
                // Use this to determine the sign used to mark the position in the histogram.
                auto const side = node_sides( node_index, placement.edge().primary_node().index() );
                double const sign = ( side == 1 ? 1.0 : -1.0 );

                // Sanity checks. They can fail if this function is used from the outside,
                // but within our local context, they should hold.
                // Nope, not true. It fails for example with negative proximal length,
                // or proximal lengths > branch length, which can happen for numerical reasons,
                // or because of bugs in the placement algortihm.
                // So, do not check, but leave the sanity check up to the user.
                // assert( dist >= 0.0 );
                // assert( sign * dist >= histograms[ node_index ].range_min() );
                // assert( sign * dist <= histograms[ node_index ].range_max() );

                // Accumulate at the distance, using the lwr and multiplicity as weight, so that the
                // total weight of a pquery sums up to the multiplicity.
                histograms[ node_index ].accumulate( sign * dist, mult * placement.like_weight_ratio );
            }
        }
    }

    // Normalize.
    for( auto& hist : histograms ) {
        normalize( hist );
    }
}

// -------------------------------------------------------------------------------------------------
//     Sample node_distance_histograms
// -------------------------------------------------------------------------------------------------

/**
 * @brief Local helper function that calculates the Histograms for a Sample.
 */
std::vector< utils::Histogram > node_distance_histograms (
    Sample const& sample,
    size_t const  histogram_bins,
    bool use_negative_axis = true
) {
    // Calculate the pairwise distance between all pairs of nodes.
    auto const node_distances = node_branch_length_distance_matrix( sample.tree() );

    // For each node, calculate which other nodes are on the root side subtree and which are not.
    auto const node_sides = ( use_negative_axis
        ? node_root_direction_matrix( sample.tree() )
        : utils::Matrix<signed char>( sample.tree().node_count(), sample.tree().node_count(), 1 )
    );

    // Make the histograms, fill them, return them.
    auto histograms = make_empty_node_distance_histograms (
        sample.tree(), node_distances, node_sides, histogram_bins
    );
    fill_node_distance_histograms ( sample, node_distances, node_sides, histograms );
    return histograms;
}

// -------------------------------------------------------------------------------------------------
//     Sample Set node_distance_histograms
// -------------------------------------------------------------------------------------------------

/**
 * @brief Local helper function that calculates all Histograms for all Samples in a SampleSet.
 */
std::vector< std::vector< utils::Histogram >> node_distance_histograms (
    SampleSet const& sample_set,
    size_t const  histogram_bins,
    bool use_negative_axis = true
) {
    auto const set_size = sample_set.size();

    // Edge case.
    if( set_size == 0 ) {
        return {};
    }

    // Prepare lookup for the trees. This assumes identical trees for all samples.
    auto const node_distances = node_branch_length_distance_matrix( sample_set[0].sample.tree() );
    auto const node_sides = ( use_negative_axis
        ? node_root_direction_matrix( sample_set[0].sample.tree() )
        : utils::Matrix<signed char>(
            sample_set[0].sample.tree().node_count(), sample_set[0].sample.tree().node_count(), 1
        )
    );

    // Prepare histograms for all samples, by copying empty histograms for the first sample.
    auto result = std::vector< std::vector< utils::Histogram >>(
        set_size,
        make_empty_node_distance_histograms (
            sample_set[ 0 ].sample.tree(), node_distances, node_sides, histogram_bins
        )
    );

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

        // Fill the histograms for every node of the sample.
        fill_node_distance_histograms ( sample_set[ i ].sample, node_distances, node_sides, result[ i ] );
        assert( result[ i ].size() == sample_set[ i ].sample.tree().node_count() );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
//     node_histogram_emd
// -------------------------------------------------------------------------------------------------

/**
 * @brief Local helper function to calculate the sum of histogram emds between two sets of histograms.
 */
double node_histogram_emd (
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
}

// =================================================================================================
//     Sample
// =================================================================================================

double node_histogram_distance (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t const  histogram_bins,
    bool use_negative_axis
) {
    if( ! compatible_trees( sample_a, sample_b ) ) {
        throw std::invalid_argument( "Incompatible trees." );
    }

    // Get the histograms describing the distances from placements to all nodes.
    auto const hist_vec_a = node_distance_histograms( sample_a, histogram_bins, use_negative_axis );
    auto const hist_vec_b = node_distance_histograms( sample_b, histogram_bins, use_negative_axis );

    // If the trees are compatible (as ensured in the beginning of this function), they need to have
    // the same number of nodes. Thus, also there should be this number of histograms in the vectors.
    assert( hist_vec_a.size() == sample_a.tree().node_count() );
    assert( hist_vec_b.size() == sample_b.tree().node_count() );
    assert( hist_vec_a.size() == hist_vec_b.size() );

    return node_histogram_emd( hist_vec_a, hist_vec_b, sample_a.tree().node_count() );
}

// =================================================================================================
//     Sample Set
// =================================================================================================

utils::Matrix<double> node_histogram_distance (
    SampleSet const& sample_set,
    size_t const     histogram_bins,
    bool use_negative_axis
) {
    // Init distance matrix.
    auto const set_size = sample_set.size();
    auto result = utils::Matrix<double>( set_size, set_size, 0.0 );

    // Get the histograms to calculate the distance.
    auto const hist_vecs = node_distance_histograms( sample_set, histogram_bins, use_negative_axis );

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
            auto const dist = node_histogram_emd( hist_vecs[ i ], hist_vecs[ j ], node_count );
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
                auto const dist = node_histogram_emd( hist_vecs[ i ], hist_vecs[ j ], node_count );
                result(i, j) = dist;
                result(j, i) = dist;
            }
        }

    #endif

    return result;
}

// =================================================================================================
//     Previous Implementation
// =================================================================================================

/*
 * These functions are not accessible from the outside.
 * We keep them here for now, just to be able to look them up quickly.
 * Probably, they can be removed in the future.
 */

// -------------------------------------------------------------------------------------------------
//     Helper Functions
// -------------------------------------------------------------------------------------------------

/**
 * @brief Local helper function to calculate the histograms of distances from all
 * @link tree::TreeNode Nodes@endlink of the @link PlacementTree Tree@endlink of a Sample to all
 * its PqueryPlacement%s.
 */
std::vector< utils::Histogram > node_distance_histograms_old (
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

double node_histogram_distance_old (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t const  histogram_bins
) {
    if( ! compatible_trees( sample_a, sample_b ) ) {
        throw std::invalid_argument( "Incompatible trees." );
    }

    // Get the histograms describing the distances from placements to all nodes.
    auto const hist_vec_a = node_distance_histograms_old( sample_a, histogram_bins );
    auto const hist_vec_b = node_distance_histograms_old( sample_b, histogram_bins );

    // If the trees are compatible (as ensured in the beginning of this function), they need to have
    // the same number of nodes. Thus, also there should be this number of histograms in the vectors.
    assert( hist_vec_a.size() == sample_a.tree().node_count() );
    assert( hist_vec_b.size() == sample_b.tree().node_count() );
    assert( hist_vec_a.size() == hist_vec_b.size() );

    return node_histogram_emd( hist_vec_a, hist_vec_b, sample_a.tree().node_count() );
}

// -------------------------------------------------------------------------------------------------
//     Full Sample Set
// -------------------------------------------------------------------------------------------------

utils::Matrix<double> node_histogram_distance_old (
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
        hist_vecs[ i ] = node_distance_histograms_old( sample_set[ i ].sample, histogram_bins );
        assert( hist_vecs[ i ].size() == sample_set[ i ].sample.tree().node_count() );
    }

    // Init distance matrix.
    auto result = utils::Matrix<double>( set_size, set_size, 0.0 );

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
            auto const dist = node_histogram_emd( hist_vecs[ i ], hist_vecs[ j ], node_count );
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
                auto const dist = node_histogram_emd( hist_vecs[ i ], hist_vecs[ j ], node_count );
                result(i, j) = dist;
                result(j, i) = dist;
            }
        }

    #endif

    return result;
}

} // namespace placement
} // namespace genesis
