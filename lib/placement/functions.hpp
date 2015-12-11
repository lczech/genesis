#ifndef GENESIS_PLACEMENT_FUNCTIONS_H_
#define GENESIS_PLACEMENT_FUNCTIONS_H_

/**
 * @brief Provides functions for working with Placements and Pqueries.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"

#include <string>

namespace genesis {

// =================================================================================================
//     Pquery Names
// =================================================================================================

bool has_name( Pquery const& pquery, std::string const& name );

Pquery const* find_pquery( PlacementMap const& map, std::string const& name );

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

void normalize_weight_ratios( PlacementMap& map );

void restrain_to_max_weight_placements( PlacementMap& map );

void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
void sort_placements_by_proximal_length( PlacementMap& map );

void sort_placements_by_weight_ratio( Pquery& pquery );
void sort_placements_by_weight_ratio( PlacementMap& map );

// =================================================================================================
//     Merging Duplicates
// =================================================================================================

void merge_duplicates( PlacementMap& map );

void collect_duplicate_pqueries( PlacementMap& map );

void merge_duplicate_placements( Pquery& pquery );
void merge_duplicate_placements( PlacementMap& map );

void merge_duplicate_names( Pquery& pquery );
void merge_duplicate_names( PlacementMap& map );

// =================================================================================================
//     Placement Mass
// =================================================================================================

std::pair<PlacementTreeEdge*, size_t> placement_count_max_edge( PlacementTree const& tree );

std::pair<PlacementTreeEdge*, double> placement_mass_max_edge( PlacementTree const& tree );

} // namespace genesis

#endif // include guard
