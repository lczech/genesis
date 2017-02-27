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

#include "genesis/placement/simulator/distributions.hpp"

#include "genesis/tree/function/distances.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/core/std.hpp"

#include <algorithm>
#include <assert.h>
#include <numeric>
#include <stdexcept>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Number Distribution
// =================================================================================================

/**
 * @brief Prepare the distribution for usage. Needs to be called before generate().
 *
 * It expects to be called with the Sample into which the generated (simulated) placements are
 * inserted.
 */
void SimulatorExtraPlacementDistribution::prepare( Sample const& sample )
{
    // Make sure to never produce a placement on the central edge. We only want to produce
    // additional placements, as we assume that the simulator already placed on the central edge.
    if( placement_path_length_weights.size() > 0 ) {
        placement_path_length_weights[0] = 0.0;
    }

    // Init the distribs.
    placement_number_distrib_ = std::discrete_distribution<size_t>(
        placement_number_weights.begin(),
        placement_number_weights.end()
    );
    placement_path_length_distrib_ = std::discrete_distribution<size_t>(
        placement_path_length_weights.begin(),
        placement_path_length_weights.end()
    );

    // If we are never ever creating additional placements (so, either we have 0 or 1 weights for
    // the distribution), we can skip the part with edge candidates.
    // Those are only used when creating additional placements.
    if( placement_number_weights.size() < 2 ) {
        return;
    }

    // Init a matrix with the path lengths from all edges to each other
    // (that is, the number of nodes between them).
    auto edge_dist_matrix = tree::edge_path_length_matrix( sample.tree() );

    // For each edge, create a list of other edges in its proximity, sorted by their
    // distance level (number of nodes in between) from that edge.
    edge_proximities_.clear();
    edge_proximities_.resize( edge_dist_matrix.rows() );
    for( size_t edge_idx = 0; edge_idx < edge_dist_matrix.rows(); ++edge_idx ) {
        for( size_t prox_idx = 0; prox_idx < edge_dist_matrix.cols(); ++prox_idx ) {
            size_t level = edge_dist_matrix( edge_idx, prox_idx );

            // We do not want to add the edge itself. This would mean to add more than one
            // placement on that edge.
            if( level == 0 ) {
                assert( edge_idx == prox_idx );
                continue;
            }

            // This list will contain all other edges of the tree in the end. We want to
            // reduce this to only a certain level, depending on the maximal length of a path that
            // we can ever generate according to the given distribution.
            if( level >= placement_path_length_weights.size() ) {
                continue;
            }

            if( edge_proximities_[ edge_idx ].candidates_per_level.size() < level + 1 ) {
                edge_proximities_[ edge_idx ].candidates_per_level.resize( level + 1 );
            }
            edge_proximities_[ edge_idx ].candidates_per_level[ level ].push_back( prox_idx );
            edge_proximities_[ edge_idx ].total_candidates += 1;
        }
    }
}

/**
 * @brief
 */
std::vector<size_t> SimulatorExtraPlacementDistribution::generate( PlacementTreeEdge const& edge )
{
    // Draw a number of placements and build a result vector of that size.
    size_t placement_num = placement_number_distrib_( utils::Options::get().random_engine() );
    auto result = std::vector<size_t>( placement_num );

    // If we are not creating any additional placements, we can skip the next steps.
    if( placement_num == 0 ) {
        return result;
    }

    // We make sure that an edge gets at most one placement per pquery by maintaining a list of
    // possible candidate edges that do not have a placement (for this pquery) yet.
    // For this, get a list of all possible candidates of neighbouring edges of the given edge.
    // We shuffle them so that we take different edges for every pquery.
    auto edge_prox = edge_proximities_[ edge.index() ];
    for( auto& candidates : edge_prox.candidates_per_level ) {
        std::random_shuffle( candidates.begin() , candidates.end() );
    }

    // We can only place as many placements as there are neighbouring edges.
    // Only happens in very small trees, but we need this to avoid an endless loop later.
    if( placement_num > edge_prox.total_candidates ) {
        placement_num = edge_prox.total_candidates;
    }

    // Now create as many more placement positions as needed.
    for( size_t i = 0; i < placement_num; ++i ) {

        // Loop until we found an edge where this placement can go.
        bool placed = false;
        do {
            // Draw randomly a value used to determine the distance of this placement from the
            // central one. As we set the weight for path length 0 to 0.0, there should never
            // be a path of 0 length, so assert it.
            size_t path_len = placement_path_length_distrib_(
                utils::Options::get().random_engine()
            );
            assert( path_len > 0 );

            // If we drew a path for which all edges of that distance are already used, we cannot
            // use it, so skip it.
            if( edge_prox.candidates_per_level[ path_len ].size() == 0 ) {
                continue;
            }

            auto place_edge_num = edge_prox.candidates_per_level[ path_len ].back();
            edge_prox.candidates_per_level[ path_len ].pop_back();

            result[i] = place_edge_num;
            placed = true;
        } while( ! placed );
    }

    return result;
}

/**
 * @brief
 */
std::string SimulatorExtraPlacementDistribution::dump_edge_proximities() const
{
    std::string result;

    for( size_t edge_idx = 0; edge_idx < edge_proximities_.size(); ++edge_idx ) {
        auto& prox = edge_proximities_[ edge_idx ];
        result += "Edge at index " + std::to_string( edge_idx ) + ":\n";

        for( size_t cand_idx = 0; cand_idx < prox.candidates_per_level.size(); ++cand_idx ) {
            auto& cand = prox.candidates_per_level[ cand_idx ];

            result += "    Level " + std::to_string( cand_idx ) + ": ";
            result += std::to_string( cand.size() ) + " candidates\n";
        }
    }

    return result;
}

/**
 * @brief
 */
std::vector<size_t> SimulatorExtraPlacementDistribution::edge_proximity_maxima() const
{
    auto result = std::vector<size_t>();

    for( auto const& prox : edge_proximities_ ) {
        for( size_t cand_idx = 0; cand_idx < prox.candidates_per_level.size(); ++cand_idx ) {
            auto& cand = prox.candidates_per_level[ cand_idx ];

            if( result.size() < cand_idx + 1 ) {
                result.resize( cand_idx + 1 );
            }
            result[ cand_idx ] = std::max( result[ cand_idx ], cand.size() );
        }
    }

    return result;
}

// =================================================================================================
//     Like Weight Ratio Distribution
// =================================================================================================

void SimulatorLikeWeightRatioDistribution::prepare( Sample const& sample )
{
    (void) sample;

    // Check condition.
    if( intervals.size() != weights.size() ) {
        throw std::logic_error(
            "The number of intervals and weights has to identical for "
            "SimulatorLikeWeightRatioDistribution."
        );
    }
    if( ! std::is_sorted( intervals.begin(), intervals.end() )) {
        throw std::logic_error(
            "Intervals need to be sorted in SimulatorLikeWeightRatioDistribution."
        );
    }
    if( std::any_of( weights.begin(), weights.end(), [] ( double v ) { return v < 0.0; } )) {
        throw std::logic_error(
            "Weights need to be non-negative in SimulatorLikeWeightRatioDistribution."
        );
    }

    // Set distribution.
    distrib_ = std::piecewise_linear_distribution<double>(
        intervals.begin(),
        intervals.end(),
        weights.begin()
    );
}

} // namespace placement
} // namespace genesis
