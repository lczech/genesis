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
//     Placement Simulator Two Step
// =================================================================================================

/**
 * @brief Generates `n` many Pqueries and places them in the PlacementMap.
 */
void PlacementSimulatorTwostep::generate (size_t n)
{
    // Prepare distributions.
    edge_distribution_.prepare();
    proximal_length_distribution_.prepare();

    for (size_t i = 0; i < n; ++i) {
        // Generate one Pquery.
        Pquery* pqry = placements_.add_pquery();
        pqry->add_name("pquery_" + std::to_string(i));

        // Get a random edge.
        size_t edge_idx = edge_distribution_.generate();
        auto   edge     = placements_.tree().edge_at(edge_idx);

        // Add a placement at the edge.
        PqueryPlacement* place = pqry->add_placement(edge);
        place->proximal_length = proximal_length_distribution_.generate(edge);
    }
}

// =================================================================================================
//     Placement Simulator Two Step :: Edge Distribution
// =================================================================================================

// ------------------------------------------------------------------
//     Set Weights
// ------------------------------------------------------------------

/**
 * @brief Sets the weights to a uniform distribution for all edges.
 */
void PlacementSimulatorTwostep::EdgeDistribution::set_uniform_weights ()
{
    size_t num_edges = placements_.tree().edge_count();
    weights = std::vector<double>(num_edges, 1.0);
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
 * This method can conveniently be used with the output of PlacementMap::closest_leaf_depth_histogram()
 * called on some other PlacementMap (or the same, for that matter). This way, it will mimic this
 * map in terms of the depths distribution of the placements: E.g., if the original map (the one
 * where the histrogram results were taken from and used as input for this method) has many
 * placements near the leaves, so will the simulated one.
 */
void PlacementSimulatorTwostep::EdgeDistribution::set_depths_distributed_weights (
    const std::vector<int>& depth_weights
) {
    // TODO Some of the code is copied from PlacementMap::closest_leaf_depth_histogram(). Maybe
    //      it is worth putting this into a method which returns the closest leaf for edges instead
    //      of nodes.

    // Prepare weights vector.
    size_t num_edges = placements_.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    // Get a vector telling us the depth from each node to its closest leaf node.
    PlacementTree::NodeIntVectorType depths = placements_.tree().closest_leaf_depth_vector();

    // Set the weight of each edge according to its depth in the tree.
    for (auto it = placements_.tree().begin_edges(); it != placements_.tree().end_edges(); ++it) {
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
 * @brief Sets the weights so that they follow the same distribution of placements per edge as a
 * given PlacementMap.
 *
 * This method "learns" how the placements on the given map are distributed by counting them and
 * using those counts as weights. This way, the given distribution can be imitated by randomly
 * generated placements.
 *
 * The method is intended to be used on a Tree that has the same topology as the one that is given
 * with the PlacementMap, otherwise the Edge indices will not fit. It does not need to be the same
 * PlacementMap or Tree -- usually, an empty copy is used.
 *
 * @param  map Use the counts of placements of this map to set the weights.
 *
 * @return Returns true iff the `map` has the same topology as the one used for simulation. If false,
 * no weights were set.
 */
bool PlacementSimulatorTwostep::EdgeDistribution::transfer_weights (const PlacementMap& map)
{
    if (!map.tree().identical_topology(placements_.tree())) {
        return false;
    }

    size_t num_edges = map.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    for (auto it = map.tree().begin_edges(); it != map.tree().end_edges(); ++it) {
        weights[(*it)->index()] = (*it)->placement_count();
    }

    return true;
}

// ------------------------------------------------------------------
//     Generate Random Edges
// ------------------------------------------------------------------

/**
 * @brief Prepares the distribution for usage.
 */
void PlacementSimulatorTwostep::EdgeDistribution::prepare()
{
    distrib_ = std::discrete_distribution<size_t>(weights.begin(), weights.end());
}

/**
 * @brief Returns a randomly chosen edge index.
 */
size_t PlacementSimulatorTwostep::EdgeDistribution::generate()
{
    return distrib_(Options::get().random_engine());
}

// =================================================================================================
//     Placement Simulator Two Step :: Proximal Length Distribution
// =================================================================================================

/**
 * @brief Prepares the distribution for usage.
 */
void PlacementSimulatorTwostep::ProximalLengthDistribution::prepare()
{
    distrib_ = std::uniform_real_distribution<double>(0.0,1.0);
}

/**
 * @brief Returns a randomly chosen position on an edge.
 */
double PlacementSimulatorTwostep::ProximalLengthDistribution::generate(typename PlacementTree::EdgeType* edge)
{
    // We do a multiplication with the branch length here, because this allows for a single
    // distribution instance instead of one per different length.
    return distrib_(Options::get().random_engine()) * edge->branch_length;
}

} // namespace genesis
