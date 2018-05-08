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
 * @ingroup placement
 */

#include "genesis/placement/function/nhd.hpp"

#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/masses.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/pquery/plain.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

// -------------------------------------------------------------------------------------------------
//     make_empty_node_distance_histogram_set
// -------------------------------------------------------------------------------------------------

/**
 * @brief Create a set of Histograms without any weights for a given Tree.
 */
NodeDistanceHistogramSet make_empty_node_distance_histogram_set (
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
    // We init with default values, so that we can better parallelize later.
    NodeDistanceHistogramSet histogram_set;
    histogram_set.histograms.resize( node_count );

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

        histogram_set.histograms[ node_idx ].min  = -min;
        histogram_set.histograms[ node_idx ].max  = max;
        histogram_set.histograms[ node_idx ].bins = std::vector<double>( histogram_bins, 0.0 );
    }

    return histogram_set;
}

// -------------------------------------------------------------------------------------------------
//     fill_node_distance_histograms
// -------------------------------------------------------------------------------------------------

/**
 * @brief Fill the placements of a Sample into Histograms.
 */
void fill_node_distance_histogram_set (
    Sample const& sample,
    utils::Matrix<double> const& node_distances,
    utils::Matrix<signed char> const& node_sides,
    NodeDistanceHistogramSet& histogram_set
) {
    // Basic checks.
    auto const node_count = sample.tree().node_count();
    if( histogram_set.histograms.size() != node_count ) {
        throw std::runtime_error( "Number of histograms does not equal number of tree nodes." );
    }
    if( node_distances.rows() != node_count || node_distances.cols() != node_count ) {
        throw std::runtime_error( "Node Distance Matrix has wrong size." );
    }
    if( node_sides.rows() != node_count || node_sides.cols() != node_count ) {
        throw std::runtime_error( "Node Sides Matrix has wrong size." );
    }

    // Convert placements to plain form. We are later going to loop over them for every node of the
    // tree, so this plain form speeds things up a lot there.
    auto placements = std::vector<PqueryPlacementPlain>( total_placement_count(sample) );
    size_t pcnt = 0;
    for( auto const& pquery : sample ) {
        double const mult = total_multiplicity( pquery );

        for( auto const& placement : pquery.placements() ) {
            auto& place = placements[pcnt];

            place.edge_index           = placement.edge().index();
            place.primary_node_index   = placement.edge().primary_node().index();
            place.secondary_node_index = placement.edge().secondary_node().index();

            auto const& placement_data = placement.edge().data<PlacementEdgeData>();
            place.branch_length        = placement_data.branch_length;
            place.pendant_length       = placement.pendant_length;
            place.proximal_length      = placement.proximal_length;

            // We cheat a bit and store the multiplicity right here with the LWR.
            // That is okay, because the data is only used within the function.
            place.like_weight_ratio    = placement.like_weight_ratio * mult;

            ++pcnt;
        }
    }

    // Fill the histogram of every node.
    for( size_t node_index = 0; node_index < sample.tree().node_count(); ++node_index ) {

        // Prepare.
        auto& histogram = histogram_set.histograms[ node_index ];
        const auto min  = histogram.min;
        const auto max  = histogram.max;
        const auto bins = histogram.bins.size();
        const double bin_width = ( max - min ) / static_cast<double>( bins );
        double sum = 0.0;

        // Add all placements to the histogram for the current node.
        for( auto const& placement : placements ) {

            // Get the distance from the placement to the current histogram node.
            double const p_dist = placement.proximal_length
                + node_distances( node_index, placement.primary_node_index )
            ;
            double const d_dist = placement.branch_length
                - placement.proximal_length
                + node_distances( node_index, placement.secondary_node_index )
            ;
            double const dist = std::min( p_dist, d_dist );

            // Get the side of the placement relative to the current node.
            // Value 1 means, it is on the root side. Values 0 and -1 mean a non root side.
            // Use this to determine the sign used to mark the position in the histogram.
            auto const side = node_sides( node_index, placement.primary_node_index );
            double const sign = ( side == 1 ? 1.0 : -1.0 );

            // Calcualte the bin index.
            auto const x = sign * dist;
            size_t bin = 0;
            if( x < min ) {
                bin = 0;
            } else if( x >= max ) {
                bin = bins - 1;
            } else {
                bin = static_cast<size_t>(( x - min ) / bin_width );
                assert( bin < bins );
            }

            // Accumulate the weight at the bin.
            histogram.bins[ bin ] += placement.like_weight_ratio;
            sum += placement.like_weight_ratio;
        }

        // Normalize.
        for( auto& val : histogram.bins ) {
            val /= sum;
        }
    }
}

// =================================================================================================
//     Basic Functions
// =================================================================================================

// -------------------------------------------------------------------------------------------------
//     Sample node_distance_histogram_set
// -------------------------------------------------------------------------------------------------

NodeDistanceHistogramSet node_distance_histogram_set (
    Sample const& sample,
    utils::Matrix<double> const& node_distances,
    utils::Matrix<signed char> const& node_sides,
    size_t const  histogram_bins
) {
    // Make the histograms, fill them, return them.
    auto histograms = make_empty_node_distance_histogram_set(
        sample.tree(), node_distances, node_sides, histogram_bins
    );
    fill_node_distance_histogram_set( sample, node_distances, node_sides, histograms );
    return histograms;
}

// -------------------------------------------------------------------------------------------------
//     node_histogram_distance
// -------------------------------------------------------------------------------------------------

double node_histogram_distance (
    NodeDistanceHistogram const& lhs,
    NodeDistanceHistogram const& rhs
) {
    if( lhs.bins.size() != rhs.bins.size() || lhs.min != rhs.min || lhs.max != rhs.max ){
        throw std::runtime_error(
            "Cannot calculate distance between NodeDistanceHistograms of different dimensions."
        );
    }

    // Prepare.
    auto entries = std::vector<double>( lhs.bins.size(), 0.0 );
    double result = 0.0;
    const double bin_width = ( lhs.max - lhs.min ) / static_cast<double>( lhs.bins.size() );

    // Loop and "move" masses.
    for( size_t i = 0; i < lhs.bins.size() - 1; ++i) {
        entries[i + 1]  = lhs.bins[i] + entries[i] - rhs.bins[i];
        result += std::abs( entries[i + 1] ) * bin_width;
    }

    return result;
}

double node_histogram_distance (
    NodeDistanceHistogramSet const& lhs,
    NodeDistanceHistogramSet const& rhs
) {
    if( lhs.histograms.size() != rhs.histograms.size() ){
        throw std::runtime_error(
            "Cannot calculate distance between NodeDistanceHistogramSets of different size."
        );
    }

    // Sum up the emd distances of the histograms for each node of the tree in the
    // two samples.
    double dist = 0.0;
    for( size_t k = 0; k < lhs.histograms.size(); ++k ) {
        dist += node_histogram_distance( lhs.histograms[ k ], rhs.histograms[ k ] );
    }
    assert( dist >= 0.0 );

    // Return normalized distance.
    dist /= static_cast< double >( lhs.histograms.size() );
    return dist;
}

utils::Matrix<double> node_histogram_distance(
    std::vector<NodeDistanceHistogramSet> const& histogram_sets
) {
    // Init distance matrix.
    auto const set_size = histogram_sets.size();
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
            auto const dist = node_histogram_distance( histogram_sets[ i ], histogram_sets[ j ] );
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
                auto const dist = node_histogram_distance( histogram_sets[ i ], histogram_sets[ j ] );
                result(i, j) = dist;
                result(j, i) = dist;
            }
        }

    #endif

    return result;
}

// =================================================================================================
//     High Level Functions
// =================================================================================================

// -------------------------------------------------------------------------------------------------
//     Sample
// -------------------------------------------------------------------------------------------------

NodeDistanceHistogramSet node_distance_histogram_set(
    Sample const& sample,
    size_t const  histogram_bins
) {
    // Calculate the pairwise distance between all pairs of nodes.
    auto const node_distances = node_branch_length_distance_matrix( sample.tree() );

    // For each node, calculate which other nodes are on the root side subtree and which are not.
    auto const node_sides = node_root_direction_matrix( sample.tree() );

    // Make the histograms, fill them, return them.
    auto histograms = make_empty_node_distance_histogram_set(
        sample.tree(), node_distances, node_sides, histogram_bins
    );
    fill_node_distance_histogram_set( sample, node_distances, node_sides, histograms );
    return histograms;
}

double node_histogram_distance (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t const  histogram_bins
) {
    if( ! compatible_trees( sample_a, sample_b ) ) {
        throw std::invalid_argument( "Incompatible trees." );
    }

    // Get the histograms describing the distances from placements to all nodes.
    auto const hist_vec_a = node_distance_histogram_set( sample_a, histogram_bins );
    auto const hist_vec_b = node_distance_histogram_set( sample_b, histogram_bins );

    // If the trees are compatible (as ensured in the beginning of this function), they need to have
    // the same number of nodes. Thus, also there should be this number of histograms in the vectors.
    assert( hist_vec_a.histograms.size() == sample_a.tree().node_count() );
    assert( hist_vec_b.histograms.size() == sample_b.tree().node_count() );
    assert( hist_vec_a.histograms.size() == hist_vec_b.histograms.size() );

    return node_histogram_distance( hist_vec_a, hist_vec_b );
}

// -------------------------------------------------------------------------------------------------
//     Sample Set
// -------------------------------------------------------------------------------------------------

/**
 * @brief Local helper function that calculates all Histograms for all Samples in a SampleSet.
 */
std::vector<NodeDistanceHistogramSet> node_distance_histogram_set(
    SampleSet const& sample_set,
    size_t const  histogram_bins
) {
    auto const set_size = sample_set.size();

    // Edge case.
    if( set_size == 0 ) {
        return {};
    }

    // Prepare lookup for the trees. This assumes identical trees for all samples.
    auto const node_distances = node_branch_length_distance_matrix( sample_set[0].sample.tree() );
    auto const node_sides = node_root_direction_matrix( sample_set[0].sample.tree() );

    // Prepare histograms for all samples, by copying empty histograms for the first sample.
    auto const empty_hist = make_empty_node_distance_histogram_set(
        sample_set[ 0 ].sample.tree(), node_distances, node_sides, histogram_bins
    );
    auto result = std::vector<NodeDistanceHistogramSet>( set_size, empty_hist );

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
        fill_node_distance_histogram_set(
            sample_set[ i ].sample, node_distances, node_sides, result[ i ]
        );
        assert( result[ i ].histograms.size() == sample_set[ i ].sample.tree().node_count() );
    }

    return result;
}

utils::Matrix<double> node_histogram_distance (
    SampleSet const& sample_set,
    size_t const     histogram_bins
) {
    // Get the histograms and calculate the distance.
    auto const hist_vecs = node_distance_histogram_set( sample_set, histogram_bins );
    return node_histogram_distance( hist_vecs );
}

} // namespace placement
} // namespace genesis
