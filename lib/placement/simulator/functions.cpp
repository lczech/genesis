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
#include "placement/simulator/placement_distribution.hpp"

#include "tree/function/distances.hpp"

#include "utils/math/matrix.hpp"

#include <vector>

namespace genesis {
namespace placement {

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
 */
void set_random_subtree_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    size_t num_edges = sample.tree().edge_count();
    edge_distrib.edge_weights = std::vector<double>( num_edges, 0.0 );

    std::uniform_int_distribution<int> edge_selection( 0, num_edges );
    size_t edge_idx = edge_selection( utils::Options::get().random_engine() );

    PlacementTree::LinkType const* start_link;
    // std::bernoulli_distribution dir_distrib;
    // if (dir_distrib (Options::get().random_engine())) {
    //     // Primary direction
    //     start_link = sample.tree().edge_at(edge_idx)->primary_link();
    // } else {
        // Secondary direction
        start_link = &sample.tree().edge_at(edge_idx).secondary_link();
    // }

    for (
        auto cur_link = &start_link->next();
        cur_link != start_link;
        cur_link = &cur_link->outer().next()
    ) {
        edge_distrib.edge_weights[cur_link->edge().index()] = 1.0;
    }

    // size_t num_edges = sample.tree().edge_count();
    // weights = std::vector<double>(num_edges, 0.0);
    //
    // size_t num_nodes = sample.tree().node_count();
    // std::uniform_int_distribution<int> distrib(0, num_nodes);
    //
    // size_t node_idx = distrib (Options::get().random_engine());
    // auto   node = sample.tree().node_at(node_idx);
    //
    // for (
    //     auto it = node->link()->next()->outer();
    //     it != node->link()->next();
    //     it = it->next()->outer()
    // ) {
    //     weights[it->edge()->index()] = 1.0;
    // }
}

// -----------------------------------------------------
//     set_per_edge_weights
// -----------------------------------------------------

/**
 * @brief Sets the weights of an SimulatorEdgeDistributionso that they follow the same distribution
 * of placements per edge as a given Sample.
 *
 * This method "learns" how the placements on the given smp are distributed by counting them and
 * using those counts as weights. This way, the given distribution can be imitated by randomly
 * generated placements.
 *
 * The method is intended to be used on a Tree that has the same topology as the one that is given
 * with the Sample, otherwise the Edge indices will not fit.
 */
void learn_per_edge_weights( Sample const& sample, SimulatorEdgeDistribution& edge_distrib )
{
    size_t num_edges = sample.tree().edge_count();
    edge_distrib.edge_weights = std::vector<double>(num_edges, 0.0);

    auto place_smp = placements_per_edge( sample );

    for (auto it = sample.tree().begin_edges(); it != sample.tree().end_edges(); ++it) {
        edge_distrib.edge_weights[(*it)->index()] = place_smp[ (*it)->index() ].size();
    }
}

// =================================================================================================
//     Placement Number Distribution
// =================================================================================================

void learn_placement_number_weights( Sample const& sample, SimulatorPlacementDistribution& p_distib )
{
    auto weights = std::vector<double>();
    for( auto const& pquery : sample.pqueries() ) {
        if( weights.size() < pquery.placement_size() ) {
            weights.resize( pquery.placement_size() + 1 );
        }
        ++weights[ pquery.placement_size() ];
    }
    p_distib.placement_number_weights = weights;
}

void learn_placement_path_length_weights( Sample const& sample, SimulatorPlacementDistribution& p_distib )
{
    auto edge_dist_matrix_ = tree::edge_path_length_matrix( sample.tree() );

    auto weights = std::vector<double>();
    for( auto const& pquery : sample.pqueries() ) {
        for( auto const& place_from : pquery.placements() ) {
            for( auto const& place_to : pquery.placements() ) {
                size_t dist = edge_dist_matrix_( place_from.edge().index(), place_to.edge().index() );

                if( weights.size() < dist ) {
                    weights.resize( dist + 1 );
                }
                ++weights[ dist ];
            }
        }
    }
    p_distib.placement_path_length_weights = weights;
}

} // namespace placement
} // namespace genesis
