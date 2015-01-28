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

// -----------------------------------------------------
//     Iterator
// -----------------------------------------------------

PlacementEdgeData::IteratorPlacements PlacementEdgeData::BeginPlacements(int sort)
{
    return IteratorPlacements(*this, sort);
}

PlacementEdgeData::IteratorPlacements PlacementEdgeData::EndPlacements()
{
    return IteratorPlacements();
}

// =============================================================================
//     Edge Data Iterator Placements
// =============================================================================

PlacementEdgeDataIteratorPlacements::PlacementEdgeDataIteratorPlacements (
    PlacementEdgeData& edge_data, int sort
) {
    std::deque<Pquery*>::iterator             pquery_it_;
    std::deque<Pquery::Placement>::iterator   placement_it_;
    std::multimap<double, Pquery::Placement*> sorted;

    // iterate over all placements of all pqueries of the edge data
    for (
        pquery_it_  = edge_data.pqueries.begin();
        pquery_it_ != edge_data.pqueries.end();
        ++pquery_it_
    ) {
        for (
            placement_it_  = (*pquery_it_)->placements.begin();
            placement_it_ != (*pquery_it_)->placements.end();
            ++placement_it_
        ) {
            // if it is a placement on the edge...
            if (placement_it_->edge_num != edge_data.edge_num) {
                continue;
            }
            // ... add it to one of the list (sorted or not)
            if (sort == 0) {
                placements_.push_back(&*placement_it_);
            } else {
                sorted.emplace(placement_it_->distal_length, &*placement_it_);
            }
        }
    }

    // if we need sorted placements, fill the placement list with sorted placements
    if (sort > 0) {
        std::multimap<double, Pquery::Placement*>::iterator fit;
        for (fit = sorted.begin(); fit != sorted.end(); ++fit) {
            placements_.push_back(fit->second);
        }
    } else if (sort < 0) {
        std::multimap<double, Pquery::Placement*>::reverse_iterator rit;
        for (rit = sorted.rbegin(); rit != sorted.rend(); ++rit) {
            placements_.push_back(rit->second);
        }
    }
}

} // namespace genesis
