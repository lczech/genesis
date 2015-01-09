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

double PlacementEdgeData::PlacementWeight()
{
    double weight = 0.0;
    for (Pquery* pqry : pqueries) {
        for (Pquery::Placement pl : pqry->placements) {
            if (pl.edge_num != edge_num) {
                continue;
            }

            weight += (pl.distal_length + pl.pendant_length) * pl.like_weight_ratio;
        }
    }
    return weight;
}

} // namespace genesis
