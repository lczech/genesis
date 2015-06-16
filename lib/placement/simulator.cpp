/**
 * @brief Implementation of Placement Simulator class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/simulator.hpp"

#include <assert.h>

#include "utils/logging.hpp"
#include "utils/options.hpp"

namespace genesis {

// =================================================================================================
//     Placement Simulator Edge Distribution
// =================================================================================================

// ------------------------------------------------------------------
//     Set Weights
// ------------------------------------------------------------------

/**
 * @brief Sets the weights for `num_edges` many edges to a uniform distribution.
 */
void PlacementSimulatorEdgeDistribution::set_uniform_weights (const size_t num_edges)
{
    weights = std::vector<double>(num_edges, 1.0);
}

/**
 * @brief Sets the weights to a uniform distribution for the number of edges of a given PlacementMap.
 */
void PlacementSimulatorEdgeDistribution::set_uniform_weights (const PlacementMap& map)
{
    size_t num_edges = map.tree().edge_count();
    set_uniform_weights(num_edges);
}

/**
 * @brief Set the weights so that they follow a given distribution which depends on the depth of
 * the edges of the PlacementTree.
 *
 * The `depth_weights` vector provides weights for each level of depth for an edge in the tree.
 * This means, each edge which is adjacent to a leaf node will use the weight at position 0;
 * edges which are one level deeper in the tree will get the weight at position 1, and so on.
 *
 * Similar to learn_weights(), the Tree given via the PlacementMap needs to have the same topology
 * as the one being used for the actual generation of placements later. Otherwise, the indices
 * will not fit.
 *
 * This method can conveniently be used with the output of
 * PlacementMap::closest_leaf_depth_histogram(). This way, it will mimic the distribution of
 * of the depths of the placements in the tree.
 */
void PlacementSimulatorEdgeDistribution::set_depths_distributed_weights (
    const PlacementMap& map,
    const std::vector<int>& depth_weights
) {
    // TODO Some of the code is copied from PlacementMap::closest_leaf_depth_histogram(). Maybe
    //      it is worth putting this into a method which returns the closest leaf for edges instead
    //      of nodes.

    // Prepare weights vector.
    size_t num_edges = map.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    // Get a vector telling us the depth from each node to its closest leaf node.
    PlacementTree::NodeIntVectorType depths = map.tree().closest_leaf_depth_vector();

    // Set the weight of each edge according to its depth in the tree.
    for (auto it = map.tree().begin_edges(); it != map.tree().end_edges(); ++it) {
        // Try both nodes at the end of the edge and see which one is closer to a leaf.
        int dp = depths[(*it)->primary_node()->index()].second;
        int ds = depths[(*it)->secondary_node()->index()].second;
        unsigned int ld = std::min(dp, ds);

        // Some safty. This holds as long as the indices are correct.
        assert((*it)->index() < num_edges);

        // If the depth of the current edge is in the depth vector, use it.
        // Otherwise, the tree is deeper than the given depth vector, so use zero instead,
        // which will result in no placements being generated on this edge.
        if (ld < depth_weights.size()) {
            weights[(*it)->index()] = static_cast<double>(depth_weights[ld]);
        } else {
            weights[(*it)->index()] = 0.0;
        }
    }
}

/**
 * @brief Sets the weights so that they follow the same distribution of placements per edge as the
 * given PlacementMap.
 *
 * This method "learns" how the placements on a given map are distributed by counting them and using
 * those counts as weights. This way, the given distribution can be imitated by randomly generated
 * placements.
 *
 * The method is intended to be used on a Tree that has the same topology as the one that is given
 * with the PlacementMap, otherwise the Edge indices will not fit. It does not need to be the same
 * PlacementMap or Tree -- usually, an empty copy is used.
 */
void PlacementSimulatorEdgeDistribution::set_transferred_weights (const PlacementMap& map)
{
    size_t num_edges = map.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    for (auto it = map.tree().begin_edges(); it != map.tree().end_edges(); ++it) {
        // Some safty. This holds as long as the indices are correct.
        assert((*it)->index() < num_edges);

        weights[(*it)->index()] = (*it)->placement_count();
    }
}

// ------------------------------------------------------------------
//     Generate Random Edges
// ------------------------------------------------------------------

/**
 * @brief Prepares the distribution for usage. Returns the number of edges used.
 */
size_t PlacementSimulatorEdgeDistribution::prepare()
{
    distrib_ = std::discrete_distribution<size_t>(weights.begin(), weights.end());
    return weights.size();
}

/**
 * @brief Returns a randomly chosen edge index.
 */
size_t PlacementSimulatorEdgeDistribution::generate()
{
    return distrib_(Options::get().random_engine());
}

// =================================================================================================
//     Placement Simulator Position Distribution
// =================================================================================================

/**
 * @brief Prepares the distribution for usage.
 */
void PlacementSimulatorPositionDistribution::prepare()
{
    distrib_ = std::uniform_real_distribution<double>(0.0,1.0);
}

/**
 * @brief Returns a randomly chosen position on an edge.
 */
double PlacementSimulatorPositionDistribution::generate(typename PlacementTree::EdgeType* edge)
{
    // We do a multiplication with the branch length here, because this allows for a single
    // distribution instance instead of one per different length.
    return distrib_(Options::get().random_engine()) * edge->branch_length;
}

// =================================================================================================
//     Placement Simulator
// =================================================================================================

/**
 * @brief Generates `n` many Pqueries and places them in the PlacementMap.
 */
void PlacementSimulator::generate_two_step (PlacementMap& map, size_t n)
{
    auto edge_distrib = PlacementSimulatorEdgeDistribution();
    auto pos_distrib  = PlacementSimulatorPositionDistribution();

    edge_distrib.set_uniform_weights(map);
    edge_distrib.prepare();
    pos_distrib.prepare();

    for (size_t i = 0; i < n; ++i) {
        // map.add();

        size_t edge_idx = edge_distrib.generate();
        LOG_DBG1 << "#" << i << ", edge " << edge_idx
                 << ", pos " << pos_distrib.generate(map.tree().edge_at(edge_idx));
    }
    LOG_DBG << map.placement_count() << " " << n;
}

} // namespace genesis
