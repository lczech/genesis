/**
 * @brief Implementation of PlacementTree class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include <map>

#include "placement/placement_map.hpp"

namespace genesis {

// =============================================================================
//     PlacementEdgeData
// =============================================================================

/**
 * @brief Returns the number of placements on this edge.
 */
size_t PlacementEdgeData::PlacementCount() const
{
    return placements.size();
}

/**
 * @brief Returns the mass of the placements on this edge, as given by their `like_weight_ratio`.
 */
double PlacementEdgeData::PlacementMass() const
{
    double mass = 0.0;
    for (PqueryPlacement pl : placements) {
        mass += pl.like_weight_ratio;
    }
    return mass;
}

/**
 * @brief Sorts the placements on this edge by their distance from the root, ascending.
 */
void PlacementEdgeData::SortPlacements()
{
    std::multimap<double, PqueryPlacement*> sorted;
    std::vector<PqueryPlacement*> new_placements;

    for (PqueryPlacement* place : placements) {
        sorted.emplace(place->proximal_length, place);
    }
    for (auto it = sorted.begin(); it != sorted.end(); ++it) {
        new_placements.push_back(it->second);
    }

    placements.swap(new_placements);
}

} // namespace genesis
