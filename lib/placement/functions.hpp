#ifndef GENESIS_PLACEMENT_FUNCTIONS_H_
#define GENESIS_PLACEMENT_FUNCTIONS_H_

/**
 * @brief Provides functions for working with Placements and Pqueries.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"

namespace genesis {

// =================================================================================================
//     Merging Duplicates
// =================================================================================================

/**
 * @brief Looks for Pqueries with the same name and merges them.
 *
 * This function is a wrapper that simply calls three other functions on the provided PlacementMap:
 *
 *     * collect_duplicate_pqueries()
 *     * merge_duplicate_names()
 *     * merge_duplicate_placements()
 *
 * See there for more information on what they do.
 */
void merge_duplicates (PlacementMap& map);

/**
 * @brief Finds all Pqueries that share a common name and combines them into a single Pquery
 * containing all their collective Placements and Names.
 *
 * The function collects all Pqueries that share at least one name. This is transitive, so that for
 * example three Pqueries with two names each like `(a,b) (b,c) (c,d)` will be combined into one
 * Pquery. Thus, the transitive closure of shared names is collected.
 *
 * All those Pqueries with shared names are combined by simply moving all their Placements and
 * Names into one Pquery and deleting the others. This means that at least the shared names will
 * be doubled after this function. Also, Placements on the same edge can occur.
 * Thus, usually `merge_duplicate_names()` and `merge_duplicate_placements()` are called after
 * this function. The function merge_duplicates() does exaclty this, for convenience.
 */
void collect_duplicate_pqueries (PlacementMap& map);

/**
 * @brief Merges all Placements of a Pquery that are on the same Edge into one averaged Placement.
 *
 * The merging is done via averaging all values of the Placement: `likelihood`, `like_weight_ratio`,
 * `proximal_length`, `pendant_length` and `parsimony`.
 */
void merge_duplicate_placements (Pquery& pquery);

/**
 * @brief Calls `merge_duplicate_placements()` for each Pquery of the PlacementMap.
 */
void merge_duplicate_placements (PlacementMap& map);

/**
 * @brief Merges all Names that are the same into one, while adding up their `multiplicity`.
 */
void merge_duplicate_names (Pquery& pquery);

/**
 * @brief Calls `merge_duplicate_names()` for each Pquery of the PlacementMap.
 */
void merge_duplicate_names (PlacementMap& map);

} // namespace genesis

#endif // include guard
