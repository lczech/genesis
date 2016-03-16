/**
 * @brief Implementation of Placement Simulator class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/simulator/twostep.hpp"

#include "placement/function/functions.hpp"
#include "tree/default/distances.hpp"
#include "tree/distances.hpp"
#include "tree/operators.hpp"

#include <assert.h>

#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Two Step
// =================================================================================================

/**
 * @brief Generates `n` many Pqueries and places them in the Sample.
 */
void SimulatorTwostep::generate (size_t n)
{
    // Prepare distributions.
    edge_distribution_.prepare();
    proximal_length_distribution_.prepare();
    pendant_length_distribution_.prepare();

    for (size_t i = 0; i < n; ++i) {
        // Generate one Pquery.
        Pquery* pqry = sample_.add_pquery();
        pqry->emplace_name("pquery_" + std::to_string(i));

        // Get a random edge.
        size_t edge_idx = edge_distribution_.generate();
        auto   edge     = sample_.tree().edge_at(edge_idx);

        // Add a placement at the edge.
        PqueryPlacement* place = pqry->emplace_placement(edge);
        place->proximal_length = proximal_length_distribution_.generate(edge);
        place->pendant_length  = pendant_length_distribution_.generate(edge);
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
void SimulatorTwostep::EdgeDistribution::set_uniform_weights ()
{
    size_t num_edges = sample_.tree().edge_count();
    weights = std::vector<double>(num_edges, 1.0);
}

/**
 * @brief Set the weights randomly to 0.0 and 1.0.
 */
void SimulatorTwostep::EdgeDistribution::set_random_weights ()
{
    size_t num_edges = sample_.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    std::bernoulli_distribution distrib;
    for (size_t i = 0; i < num_edges; ++i) {
        if( distrib( utils::Options::get().random_engine() )) {
            weights[i] = 1.0;
        }
    }
}

/**
 * @brief Sets the weights of a randomly chosen subtree to 1.0, all others to 0.0.
 */
void SimulatorTwostep::EdgeDistribution::set_random_subtree_weights ()
{
    size_t num_edges = sample_.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    std::uniform_int_distribution<int> edge_distrib(0, num_edges);
    size_t edge_idx = edge_distrib( utils::Options::get().random_engine() );

    PlacementTree::LinkType* start_link;
    // std::bernoulli_distribution dir_distrib;
    // if (dir_distrib (Options::get().random_engine())) {
    //     // Primary direction
    //     start_link = sample_.tree().edge_at(edge_idx)->primary_link();
    // } else {
        // Secondary direction
        start_link = sample_.tree().edge_at(edge_idx)->secondary_link();
    // }

    for (
        auto cur_link = start_link->next();
        cur_link != start_link;
        cur_link = cur_link->outer()->next()
    ) {
        weights[cur_link->edge()->index()] = 1.0;
    }

    // size_t num_edges = sample_.tree().edge_count();
    // weights = std::vector<double>(num_edges, 0.0);
    //
    // size_t num_nodes = sample_.tree().node_count();
    // std::uniform_int_distribution<int> distrib(0, num_nodes);
    //
    // size_t node_idx = distrib (Options::get().random_engine());
    // auto   node = sample_.tree().node_at(node_idx);
    //
    // for (
    //     auto it = node->link()->next()->outer();
    //     it != node->link()->next();
    //     it = it->next()->outer()
    // ) {
    //     weights[it->edge()->index()] = 1.0;
    // }
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
 * This method can conveniently be used with the output of Sample::closest_leaf_depth_histogram()
 * called on some other Sample (or the same, for that matter). This way, it will mimic this
 * smp in terms of the depths distribution of the placements: E.g., if the original smp (the one
 * where the histrogram results were taken from and used as input for this method) has many
 * placements near the leaves, so will the simulated one.
 */
void SimulatorTwostep::EdgeDistribution::set_depths_distributed_weights (
    const std::vector<int>& depth_weights
) {
    // TODO Some of the code is copied from Sample::closest_leaf_depth_histogram(). Maybe
    //      it is worth putting this into a method which returns the closest leaf for edges instead
    //      of nodes.

    // Prepare weights vector.
    size_t num_edges = sample_.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    // Get a vector telling us the depth from each node to its closest leaf node.
    auto depths = closest_leaf_depth_vector(sample_.tree());

    // Set the weight of each edge according to its depth in the tree.
    for (auto it = sample_.tree().begin_edges(); it != sample_.tree().end_edges(); ++it) {
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
 * given Sample.
 *
 * This method "learns" how the placements on the given smp are distributed by counting them and
 * using those counts as weights. This way, the given distribution can be imitated by randomly
 * generated placements.
 *
 * The method is intended to be used on a Tree that has the same topology as the one that is given
 * with the Sample, otherwise the Edge indices will not fit. It does not need to be the same
 * Sample or Tree -- usually, an empty copy is used.
 *
 * @param  smp Use the counts of placements of this smp to set the weights.
 *
 * @return Returns true iff the `smp` has the same topology as the one used for simulation. If false,
 * no weights were set.
 */
bool SimulatorTwostep::EdgeDistribution::transfer_weights (const Sample& smp)
{
    if (!identical_topology(smp.tree(), sample_.tree())) {
        return false;
    }

    size_t num_edges = smp.tree().edge_count();
    weights = std::vector<double>(num_edges, 0.0);

    auto place_smp = placements_per_edge( smp );

    for (auto it = smp.tree().begin_edges(); it != smp.tree().end_edges(); ++it) {
        weights[(*it)->index()] = place_smp[ (*it)->index() ].size();
    }

    return true;
}

// ------------------------------------------------------------------
//     Generate Random Edges
// ------------------------------------------------------------------

/**
 * @brief Prepares the distribution for usage.
 */
void SimulatorTwostep::EdgeDistribution::prepare()
{
    distrib_ = std::discrete_distribution<size_t>(weights.begin(), weights.end());
}

/**
 * @brief Returns a randomly chosen edge index.
 */
size_t SimulatorTwostep::EdgeDistribution::generate()
{
    return distrib_( utils::Options::get().random_engine() );
}

// =================================================================================================
//     Placement Simulator Two Step :: Proximal Length Distribution
// =================================================================================================

/**
 * @brief Prepares the distribution for usage.
 */
void SimulatorTwostep::ProximalLengthDistribution::prepare()
{
    distrib_ = std::uniform_real_distribution<double>(0.0,1.0);
}

/**
 * @brief Returns a randomly chosen position on an edge.
 */
double SimulatorTwostep::ProximalLengthDistribution::generate(
    typename PlacementTree::EdgeType* edge
) {
    // We do a multiplication with the branch length here, because this allows for a single
    // distribution instance instead of one per different length.
    return distrib_( utils::Options::get().random_engine() ) * edge->data.branch_length;
}

// =================================================================================================
//     Placement Simulator Two Step :: Pendant Length Distribution
// =================================================================================================

/**
 * @brief Prepares the distribution for usage.
 */
void SimulatorTwostep::PendantLengthDistribution::prepare()
{
    distrib_ = std::uniform_real_distribution<double>(min, max);
}

/**
 * @brief Returns a randomly chosen position on an edge.
 */
double SimulatorTwostep::PendantLengthDistribution::generate(
    typename PlacementTree::EdgeType* edge
) {
    // We don't use the edge in the default distribution.
    (void) edge;

    return distrib_( utils::Options::get().random_engine() );
}

} // namespace placement
} // namespace genesis
