/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "placement/simulator/placement_distribution.hpp"

#include "tree/function/distances.hpp"
#include "utils/core/options.hpp"
#include "utils/math/matrix.hpp"
#include "utils/core/std.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Number Distribution
// =================================================================================================

/**
 * @brief Prepare the distribution for usage. Needs to be called before generate().
 */
void SimulatorPlacementDistribution::prepare( Sample const& sample )
{
    // If nothing was set, initialize to use 1 placement (with 0 path length).
    if( placement_number_weights.size() == 0 ) {
        placement_number_weights = std::vector<double>( 1, 1.0 );
    }
    if( placement_path_length_weights.size() == 0 ) {
        placement_path_length_weights = std::vector<double>();
    } else if( placement_path_length_weights.size() == 1 ) {
        throw std::runtime_error( "Cannot use placement_path_length_weights with size 1." );
    } else {
        // We never want to place additional placements on the first edge of the pquery.
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

    // If we are only ever creating one placement per pquery, we can skip the part with
    // edge candidates. Those are only used when creating more than one placements.
    //
    //
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO wrong. this would be 0 placements.
    if( placement_number_weights.size() == 1 ) {
        return;
    }

    // Init a matrix with the path lengths from all edges to each other
    // (that is, the number of nodes between them).
    auto edge_dist_matrix_ = tree::edge_path_length_matrix( sample.tree() );

    // For each edge, create a list of other edges in its proximity, sorted by their
    // distance level from that edge.
    edge_proximities_.resize( edge_dist_matrix_.rows() );
    for( size_t edge_idx = 0; edge_idx < edge_dist_matrix_.rows(); ++edge_idx ) {
        for( size_t prox_idx = 0; prox_idx < edge_dist_matrix_.cols(); ++prox_idx ) {
            size_t level = edge_dist_matrix_( edge_idx, prox_idx );

            // This list will contain all other edges of the tree in the end. We might want to reduce
            // this to only a certain level, depending on the length
            if( level > placement_path_length_weights.size() ) {
                /* code */
            }

            if( edge_proximities_[edge_idx].candidates_per_level.size() < level ) {
                edge_proximities_[edge_idx].candidates_per_level.resize( level + 1 );
            }
            edge_proximities_[edge_idx].candidates_per_level[level].push_back( prox_idx );
            edge_proximities_[edge_idx].total_candidates += 1;
        }
    }
}

/**
 * @brief
 */
std::vector<size_t> SimulatorPlacementDistribution::generate( typename PlacementTree::EdgeType const& edge )
{
    // Draw a number of placements and build a result vector of that size.
    size_t placement_num = placement_number_distrib_( utils::Options::get().random_engine() );
    assert( placement_num > 0 );

    auto result = std::vector<size_t>( placement_num );

    // The first entry in the result is the edge for which the actual position is given.
    result[0] = edge.index();

    // If we are only creating one placement for this pquery, we can skip the next steps.
    if( placement_num == 1 ) {
        return result;
    }

    // We keep track of edges that we already placed a placement on.
    // This way, we make sure that no edge gets at most one placement per pquery.
    // Fot his, get a list of candidates of neighbouring edges of the given edge.
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
    for( size_t i = 1; i < placement_num; ++i ) {

        // Loop until we found an edge where this placement can go.
        bool placed = false;
        do {
            // Draw randomly a value used to determine the distance of this placment from the
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
std::string SimulatorPlacementDistribution::show_edge_proximities() const
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

} // namespace placement
} // namespace genesis
