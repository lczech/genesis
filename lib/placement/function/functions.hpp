#ifndef GENESIS_PLACEMENT_FUNCTION_FUNCTIONS_H_
#define GENESIS_PLACEMENT_FUNCTION_FUNCTIONS_H_

/**
 * @brief Provides functions for working with Placements and Pqueries.
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

#include <string>
#include <utility>

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery Names
// =================================================================================================

bool has_name( Pquery const& pquery, std::string const& name );
bool has_name( Sample const& smp, std::string const& name );

Pquery const* find_pquery( Sample const& smp, std::string const& name );

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

void normalize_weight_ratios( Sample& smp );

void restrain_to_max_weight_placements( Sample& smp );

void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
void sort_placements_by_proximal_length( Sample& smp );

void sort_placements_by_like_weight_ratio( Pquery& pquery );
void sort_placements_by_like_weight_ratio( Sample& smp );

// =================================================================================================
//     Merging Duplicates
// =================================================================================================

void merge_duplicates( Sample& smp );

void collect_duplicate_pqueries( Sample& smp );

void merge_duplicate_placements( Pquery& pquery );
void merge_duplicate_placements( Sample& smp );

void merge_duplicate_names( Pquery& pquery );
void merge_duplicate_names( Sample& smp );

// =================================================================================================
//     Placement Mass
// =================================================================================================

std::pair<PlacementTreeEdge*, size_t> placement_count_max_edge( PlacementTree const& tree );

std::pair<PlacementTreeEdge*, double> placement_mass_max_edge( PlacementTree const& tree );

// =================================================================================================
//     Histograms
// =================================================================================================

std::vector<int> closest_leaf_depth_histogram( Sample const& smp );

std::vector<int> closest_leaf_distance_histogram (
    Sample const& smp, const double min, const double max, const int bins = 10
);

std::vector<int> closest_leaf_distance_histogram_auto (
    Sample const& smp, double& min, double& max, const int bins = 10
);

} // namespace placement
} // namespace genesis

#endif // include guard
