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

#include "placement/simulator/functions.hpp"

#include "placement/function/functions.hpp"
#include "placement/function/helper.hpp"

#include "placement/simulator/edge_distribution.hpp"
#include "placement/simulator/extra_placement_distribution.hpp"
#include "placement/simulator/like_weight_ratio_distribution.hpp"
#include "placement/simulator/pendant_length_distribution.hpp"
#include "placement/simulator/proximal_length_distribution.hpp"

#include "tree/function/distances.hpp"

#include "utils/core/std.hpp"
#include "utils/math/matrix.hpp"
#include "utils/text/string.hpp"

#include <assert.h>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Stream Output Operators
// =================================================================================================

std::ostream& operator << ( std::ostream& out, SimulatorEdgeDistribution const& distrib )
{
    out << "Weight of each edge: ";
    out << utils::join( distrib.edge_weights, ", ") << "\n";
    return out;
}

std::ostream& operator << ( std::ostream& out, SimulatorExtraPlacementDistribution const& distrib )
{
    out << "Extra placement weights:\n";
    for( size_t i = 0; i < distrib.placement_number_weights.size(); ++i ) {
        out << i << ": " << distrib.placement_number_weights[i] << "\n";
    }
    out << "Path length weights:\n";
    for( size_t i = 0; i < distrib.placement_path_length_weights.size(); ++i ) {
        out << i << ": " << distrib.placement_path_length_weights[i] << "\n";
    }
    return out;
}

std::ostream& operator << ( std::ostream& out, SimulatorLikeWeightRatioDistribution const& distrib )
{
    if( distrib.intervals.size() != distrib.weights.size() ) {
        throw std::logic_error( "Invalid SimulatorLikeWeightRatioDistribution." );
    }

    out << "Like weight ratio intervals and weights:\n";
    for( size_t i = 0; i < distrib.intervals.size(); ++i ) {
        out << distrib.intervals[i] << ": " << distrib.weights[i] << "\n";
    }
    return out;
}

// =================================================================================================
//     Edge Distribution : Set Weights
// =================================================================================================

// -----------------------------------------------------
//     set_uniform_weights
// -----------------------------------------------------

/**
 * @brief Sets the weights of an SimulatorEdgeDistribution to 1.0 for all edges, so that each edge
 * has the same probability of being chosen.
 *
 * The number of edges is taken from the provided Sample.
 */
void set_uniform_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    set_uniform_weights( sample.tree().edge_count(), edge_distrib );
}

/**
 * @brief Sets the weights of an SimulatorEdgeDistribution to 1.0 for all edges, so that each edge
 * has the same probability of being chosen.
 */
void set_uniform_weights( size_t edge_count, SimulatorEdgeDistribution& edge_distrib )
{
    edge_distrib.edge_weights = std::vector<double>( edge_count, 1.0 );
}

// -----------------------------------------------------
//     set_random_weights
// -----------------------------------------------------

/**
 * @brief Set the weights of an SimulatorEdgeDistribution for the edges randomly to a value
 * between 0.0 and 1.0.
 *
 * The number of edges is taken from the provided Sample.
 */
void set_random_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    set_random_weights( sample.tree().edge_count(), edge_distrib );
}

/**
 * @brief Set the weights of an SimulatorEdgeDistribution for the edges randomly to a value
 * between 0.0 and 1.0.
 */
void set_random_weights( size_t edge_count, SimulatorEdgeDistribution& edge_distrib )
{
    edge_distrib.edge_weights = std::vector<double>( edge_count, 0.0 );

    std::uniform_real_distribution<double> distrib( 0.0, 1.0 );
    for( size_t i = 0; i < edge_count; ++i ) {
        edge_distrib.edge_weights[i] = distrib( utils::Options::get().random_engine() );
    }
}

// -----------------------------------------------------
//     set_random_edges
// -----------------------------------------------------

/**
 * @brief Set the weights of a SimulatorEdgeDistribution randomly to either 0.0 or 1.0, so that a
 * random subset of edges is selected (with the same probability for each selected edge).
 *
 * The number of edges is taken from the provided Sample.
 */
void set_random_edges( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    set_random_edges( sample.tree().edge_count(), edge_distrib );
}

/**
 * @brief Set the weights of an SimulatorEdgeDistribution randomly to either 0.0 or 1.0, so that a
 * random subset of edges is selected (with the same probability for each selected edge).
 */
void set_random_edges( size_t edge_count, SimulatorEdgeDistribution& edge_distrib )
{
    edge_distrib.edge_weights = std::vector<double>( edge_count, 0.0 );

    std::bernoulli_distribution distrib;
    for (size_t i = 0; i < edge_count; ++i) {
        if( distrib( utils::Options::get().random_engine() )) {
            edge_distrib.edge_weights[i] = 1.0;
        }
    }
}

// -----------------------------------------------------
//     set_depths_distributed_weights
// -----------------------------------------------------

/**
 * @brief Set the weights of an SimulatorEdgeDistribution so that they follow the depth distribution
 * of the edges in the provided Sample.
 *
 * This function is similar to
 * set_depths_distributed_weights(
 * Sample const& sample, std::vector<int> const& depth_weights, SimulatorEdgeDistribution&
 * edge_distrib ), but instead of using a given depth_weight vector, this vector is also
 * estimated from the given Sample. This is done by using closest_leaf_weight_distribution(), which
 * counts the number of placements at a given depth in the tree.
 */
void set_depths_distributed_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    auto depth_weights = closest_leaf_weight_distribution( sample );
    set_depths_distributed_weights( sample, depth_weights, edge_distrib );
}

/**
 * @brief Set the weights so that they follow a given depth distribution of the edges in the
 * PlacementTree.
 *
 * The `depth_weights` vector provides weights for each level of depth for an edge in the tree.
 * This means, each edge which is adjacent to a leaf node (speak: it has depth 0) will use the weight
 * at position 0; edges which are one level deeper in the tree will get the weight at position 1,
 * and so on.
 *
 * This method can conveniently be used with the output of closest_leaf_weight_distribution()
 * called on some exemplary Sample. This way, it will mimic this sample in terms of the depths
 * distribution of the placements: E.g., if the original sample (the one where the histrogram
 * results were taken from and used as input for this method) has many placements near the leaves,
 * so will the simulated one.
 * See set_depths_distributed_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
 * for a version of this function which does exaclty that.
 */
void set_depths_distributed_weights(
    Sample const&              sample,
    std::vector<double> const& depth_weights,
    SimulatorEdgeDistribution& edge_distrib
) {
    // TODO Some of the code is copied from Sample::closest_leaf_weight_distribution(). Maybe
    //      it is worth putting this into a method which returns the closest leaf for edges instead
    //      of nodes.

    // Prepare weights vector.
    size_t num_edges = sample.tree().edge_count();
    edge_distrib.edge_weights = std::vector<double>(num_edges, 0.0);

    // Get a vector telling us the depth from each node to its closest leaf node.
    auto depths = closest_leaf_depth_vector(sample.tree());

    // Set the weight of each edge according to its depth in the tree.
    for (auto it = sample.tree().begin_edges(); it != sample.tree().end_edges(); ++it) {
        // Try both nodes at the end of the edge and see which one is closer to a leaf.
        int dp = depths[(*it)->primary_node().index()].second;
        int ds = depths[(*it)->secondary_node().index()].second;
        unsigned int ld = std::min(dp, ds);

        // Some safty. This holds as long as the indices are correct.
        assert((*it)->index() < num_edges);

        // If the depth of the current edge is in the depth vector, use it.
        // Otherwise, the tree is deeper than the given depth vector, so use zero instead,
        // which will result in no placements being generated on this edge.
        if (ld < depth_weights.size()) {
            edge_distrib.edge_weights[ (*it)->index() ] = depth_weights[ld];
        } else {
            edge_distrib.edge_weights[ (*it)->index() ] = 0.0;
        }
    }
}

// -----------------------------------------------------
//     set_random_subtree_weights
// -----------------------------------------------------

/**
 * @brief Sets the weights of an SimulatorEdgeDistribution to 1.0 for a randomly chosen subtree,
 * all others to 0.0.
 *
 * Returns the index of the chosen edge.
 */
size_t set_random_subtree_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    // Reset all edge weights.
    size_t edge_count = sample.tree().edge_count();
    edge_distrib.edge_weights = std::vector<double>( edge_count, 0.0 );

    std::uniform_int_distribution<int> edge_selection( 0, edge_count - 1 );
    size_t edge_idx = edge_selection( utils::Options::get().random_engine() );

    // Randomly select a subtree, either starting at the end of the edge in direction of the root,
    // or away from it.
    PlacementTreeLink const* start_link;
    std::bernoulli_distribution dir_distrib;
    if( dir_distrib( utils::Options::get().random_engine() )) {
        // Primary direction
        start_link = &sample.tree().edge_at(edge_idx).primary_link();
    } else {
        // Secondary direction
        start_link = &sample.tree().edge_at(edge_idx).secondary_link();
    }

    for (
        auto cur_link = &start_link->next();
        cur_link != start_link;
        cur_link = &cur_link->outer().next()
    ) {
        edge_distrib.edge_weights[ cur_link->edge().index() ] = 1.0;
    }

    return edge_idx;
}

// -----------------------------------------------------
//     set_subtree_weights
// -----------------------------------------------------

/**
 * @brief Set the weights of a subtree to 1.0 and all other weights to 0.0.
 *
 * The subtree is selected via the index of the link that leads away from it. As leaf nodes do not
 * count as subtrees, the link has to belong to an inner node.
 */
void set_subtree_weights(
    Sample const& sample,
    size_t link_index,
    SimulatorEdgeDistribution& edge_distrib
) {
    // Validity checks.
    if( link_index >= sample.tree().link_count() ) {
        throw std::runtime_error( "Invalid link index for subtree." );
    }
    if( sample.tree().link_at( link_index ).is_leaf() ) {
        throw std::runtime_error( "Cannot use a leaf node as subtree." );
    }

    // Reset all edge weights.
    auto edge_count = sample.tree().edge_count();
    edge_distrib.edge_weights = std::vector<double>( edge_count, 0.0 );

    // Iterate the subtree and set edge weights.
    PlacementTreeLink const* start_link = &sample.tree().link_at( link_index );
    for (
        auto cur_link = &start_link->next();
        cur_link != start_link;
        cur_link = &cur_link->outer().next()
    ) {
        edge_distrib.edge_weights[ cur_link->edge().index() ] = 1.0;
    }
}

// -----------------------------------------------------
//     learn_per_edge_weights
// -----------------------------------------------------

/**
 * @brief Sets the weights of an SimulatorEdgeDistributionso that they follow the same distribution
 * of placement weight per edge as a given Sample.
 *
 * This method "learns" how the placements on the given Sample are distributed by summing up their
 * weight per edge and using this as weights. This way, the given distribution can be imitated by
 * randomly generated placements.
 *
 * The method is intended to be used on a Tree that has the same topology as the one that is given
 * with the Sample, otherwise the Edge indices will not fit.
 */
void learn_per_edge_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    edge_distrib.edge_weights = placement_weight_per_edge( sample );
}

// =================================================================================================
//     Placement Number Distribution
// =================================================================================================

void learn_placement_number_weights(
    Sample const& sample,
    SimulatorExtraPlacementDistribution& p_distib
) {
    auto weights = std::vector<double>();
    for( auto const& pquery : sample.pqueries() ) {
        size_t extra_placements = pquery.placement_size() - 1;
        if( weights.size() < extra_placements + 1 ) {
            weights.resize( extra_placements + 1 );
        }
        ++weights[ extra_placements ];
    }
    p_distib.placement_number_weights = weights;
}

void learn_placement_path_length_weights(
    Sample const& sample,
    SimulatorExtraPlacementDistribution& p_distib
) {
    // The distance (path length) between two placements is the number of nodes between them.
    // Get a matrix that gives us this number for each pair of edges of the tree.
    auto edge_dist_matrix = tree::edge_path_length_matrix( sample.tree() );

    // Iterate all pqueries and collect the distances between all of their placements, where
    // the distance is the number of nodes between them.
    auto weights = std::vector<double>();
    for( auto const& pquery : sample.pqueries() ) {
        for( auto const& place_from : pquery.placements() ) {
            for( auto const& place_to : pquery.placements() ) {
                size_t dist = edge_dist_matrix(
                    place_from.edge().index(), place_to.edge().index()
                );

                // The following assertion holds as long as our dist matrix is correct.
                // If assertions are disabled (release), the whole condition should be eliminated
                // from the binary, so there is no speed penalty.
                if( place_from.edge().index() == place_to.edge().index() ) {
                    assert( dist == 0 );
                }

                // We don't need paths of length 0 currently. They will be eliminated in the
                // distrubution class anyway, when calling prepare().
                if( dist == 0 ) {
                    continue;
                }
                if( weights.size() < dist + 1 ) {
                    weights.resize( dist + 1 );
                }
                weights[ dist ] += 1.0;
            }
        }
    }
    p_distib.placement_path_length_weights = weights;
}

// =================================================================================================
//     Like Weight Ratio Distribution
// =================================================================================================

void learn_like_weight_ratio_distribution(
    Sample const& sample,
    SimulatorLikeWeightRatioDistribution& lwr_distib,
    size_t number_of_intervals
) {
    // Init the result vectors (we need one more element than number of intervals).
    auto intervals = std::vector<double>( number_of_intervals + 1, 0.0 );
    auto weights   = std::vector<double>( number_of_intervals + 1, 0.0 );
    for( size_t i = 0; i < number_of_intervals + 1; ++i ) {
        intervals[i] = static_cast<double>( i ) / static_cast<double>( number_of_intervals );
    }

    // Iterate all placements and use their rounded lwr value to increase the weights of the
    // distribution.
    for( auto const& pquery : sample.pqueries() ) {
        for( auto const& placement : pquery.placements() ) {

            double rounded_lwr = std::round( placement.like_weight_ratio * number_of_intervals )
                               / number_of_intervals;

            if( 0.0 > rounded_lwr || rounded_lwr > 1.0 ) {
                throw std::runtime_error( "Invalid like_weight_ratio in Sample." );
            }
            size_t pos = static_cast<size_t>( rounded_lwr * number_of_intervals );
            assert( pos < weights.size() );
            weights[ pos ] += 1.0;
        }
    }

    // Set the result.
    lwr_distib.intervals = intervals;
    lwr_distib.weights   = weights;
}

} // namespace placement
} // namespace genesis
