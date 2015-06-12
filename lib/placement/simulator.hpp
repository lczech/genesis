#ifndef GENESIS_PLACEMENT_SIMULATOR_H_
#define GENESIS_PLACEMENT_SIMULATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <stddef.h>
// #include <string>
#include <vector>

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

class PlacementMap;

// class PlacementSimulatorPositionDistribution
// {
//
// };

// =============================================================================
//     Placement Simulator
// =============================================================================

/**
 * @brief Simulates Placements on a Tree.
 */
class PlacementSimulator
{
public:

    // std::vector<double> edge_distribution;

    // double  proximal_length_;
    // double  pendant_length;

    void generate            (PlacementMap& placements, size_t n);
    void generate_in_subtree (PlacementMap& placements, size_t n);

};

} // namespace genesis

#endif // include guard
