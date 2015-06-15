#ifndef GENESIS_PLACEMENT_SIMULATOR_H_
#define GENESIS_PLACEMENT_SIMULATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"

#include <random>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class PlacementMap;

// =================================================================================================
//     Placement Simulator Edge Distribution
// =================================================================================================

class PlacementSimulatorEdgeDistribution
{
public:

    // -----------------------------------------------------
    //     Set Weights
    // -----------------------------------------------------

    void set_uniform_weights (const size_t num_edges);
    void set_uniform_weights (const PlacementMap& map);

    void set_depths_distributed_weights (
        const PlacementMap& map,
        const std::vector<int>& depth_weights
    );
    void set_transferred_weights (const PlacementMap& from_map);

    // -----------------------------------------------------
    //     Generate Random Edges
    // -----------------------------------------------------

    size_t prepare();
    size_t generate();

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    std::vector<double> weights;

protected:
    std::discrete_distribution<size_t> distrib_;
};

// =================================================================================================
//     Placement Simulator Position Distribution
// =================================================================================================

class PlacementSimulatorPositionDistribution
{
public:

    // -----------------------------------------------------
    //     Generate Random Positions
    // -----------------------------------------------------

    void prepare();
    double generate(typename PlacementTree::EdgeType* edge);

protected:
    std::uniform_real_distribution<double> distrib_;
};

// =================================================================================================
//     Placement Simulator
// =================================================================================================

/**
 * @brief Simulates Placements on a Tree.
 */
class PlacementSimulator
{
public:

    // std::vector<double> edge_distribution;

    // double  proximal_length_;
    // double  pendant_length;

    void generate_two_step   (PlacementMap& placements, size_t n);
    void generate_in_subtree (PlacementMap& placements, size_t n);

};

} // namespace genesis

#endif // include guard
