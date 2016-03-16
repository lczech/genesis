/**
 * @brief Implementation of PlacementTree class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include <map>

#include "placement/sample.hpp"
#include "utils/core/logging.hpp"

namespace genesis {
namespace placement {

// =============================================================================
//     PlacementTreeEdgeData
// =============================================================================

int PlacementTreeEdgeData::edge_num() const
{
    return edge_num_;
}

/**
 * @brief Force to set the edge_num to a certain value.
 *
 * The edge_num is usually considered to be constant for the tree. It rarely needs to be set,
 * except for when constructing the object. Use with care.
 */
void PlacementTreeEdgeData::reset_edge_num( int val )
{
    edge_num_ = val;
}

// /**
//  * @brief Destructor.
//  */
// PlacementTreeEdgeData::~PlacementTreeEdgeData()
// {
//     for( auto& place : placements ) {
//         place->edge = nullptr;
//     }
// }
//
// /**
//  * @brief Returns the number of placements on this edge.
//  */
// size_t PlacementTreeEdgeData::placement_count() const
// {
//     return placements.size();
// }
//
// /**
//  * @brief Returns the mass of the placements on this edge, as given by their `like_weight_ratio`.
//  */
// double PlacementTreeEdgeData::placement_mass() const
// {
//     double mass = 0.0;
//     for (PqueryPlacement* pl : placements) {
//         mass += pl->like_weight_ratio;
//     }
//     return mass;
// }
//
// /**
//  * @brief Sorts the placements on this edge by their distance from the root, ascending.
//  */
// void PlacementTreeEdgeData::sort_placements()
// {
//     std::multimap<double, PqueryPlacement*> sorted;
//     std::vector<PqueryPlacement*> new_placements;
//
//     for (PqueryPlacement* place : placements) {
//         sorted.emplace(place->proximal_length, place);
//     }
//     for (auto it = sorted.begin(); it != sorted.end(); ++it) {
//         new_placements.push_back(it->second);
//     }
//
//     placements.swap(new_placements);
// }

} // namespace placement
} // namespace genesis
