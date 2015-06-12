/**
 * @brief Implementation of Placement Simulator class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/simulator.hpp"

//~ #include <map>

#include "placement/placement_map.hpp"
#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Placement Simulator
// =============================================================================

/**
 * @brief Generates `n` many Pqueries and places them in the PlacementMap.
 */
void PlacementSimulator::generate (PlacementMap& placements, size_t n)
{
    LOG_DBG << placements.pqueries.size() << " " << n;
}

} // namespace genesis
