/**
 * @brief Implementation of PlacementTree class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include <map>

#include "placement/placements.hpp"

namespace genesis {

// =============================================================================
//     PlacementEdgeData
// =============================================================================

size_t PlacementEdgeData::PlacementCount()
{
    // TODO this is legacy. in fact, simply returning placements.size() should do the job!
    size_t count = 0;
    for (PqueryPlacement pl : placements) {
        assert(pl.edge_num == edge_num);
        ++count;
    }
    assert(count == placements.size());
    return count;
}

double PlacementEdgeData::PlacementMass()
{
    double mass = 0.0;
    for (PqueryPlacement pl : placements) {
        assert(pl.edge_num == edge_num);
        mass += pl.like_weight_ratio;
    }
    return mass;
}

void PlacementEdgeData::Sort()
{
    std::multimap<double, PqueryPlacement*> sorted;
    std::deque<PqueryPlacement*> new_placements;

    for (PqueryPlacement* place : placements) {
        sorted.emplace(place->distal_length, place);
    }
    std::multimap<double, PqueryPlacement*>::iterator it;
    for (it = sorted.begin(); it != sorted.end(); ++it) {
        new_placements.push_back(it->second);
    }

    placements.swap(new_placements);
}

} // namespace genesis
