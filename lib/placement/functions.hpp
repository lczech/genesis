#ifndef GENESIS_PLACEMENT_FUNCTIONS_H_
#define GENESIS_PLACEMENT_FUNCTIONS_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"

namespace genesis {

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Looks for Pqueries with the same name and merges them.
 */
void merge_duplicates (PlacementMap& map);

void merge_duplicate_placements (Pquery& pquery);
void merge_duplicate_placements (PlacementMap& map);

void merge_duplicate_names (Pquery& pquery);
void merge_duplicate_names (PlacementMap& map);

} // namespace genesis

#endif // include guard
