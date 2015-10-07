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
// TODO this function might better be called something like merge_same_name_pqueries or so.
void merge_duplicates (PlacementMap& map);

} // namespace genesis

#endif // include guard
