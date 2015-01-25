/**
 * @brief Implementation of Placement Tree
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include "placement/placements.hpp"

namespace genesis {

// =============================================================================
//     PlacementEdgeData
// =============================================================================

double PlacementEdgeData::PlacementMass()
{
    double mass = 0.0;
    for (Pquery* pqry : pqueries) {
        for (Pquery::Placement pl : pqry->placements) {
            if (pl.edge_num != edge_num) {
                continue;
            }

            //~ mass += pl.like_mass_ratio;
            mass += 1.0;
        }
    }
    return mass;
}

} // namespace genesis
