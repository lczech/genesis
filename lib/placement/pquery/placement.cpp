/**
 * @brief Implementation of PqueryPlacement class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/pquery/placement.hpp"

#include "placement/placement_tree.hpp"

#include <assert.h>

namespace genesis {
namespace placement {

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Get the `edge_num` where this PqueryPlacement is placed.
 *
 * This number corresponds to the `edge_num` property as described in the `jplace` standard.
 * It is not to be confused with the index of the PlacementTreeEdge.
 */
int PqueryPlacement::edge_num() const
{
    return edge_->data.edge_num();
}

/**
 * @brief Get the PlacementTreeEdge where this PqueryPlacement is placed.
 */
const PlacementTreeEdge& PqueryPlacement::edge() const
{
    return *edge_;
}

/**
 * @brief Get the PlacementTreeEdge where this PqueryPlacement is placed.
 */
PlacementTreeEdge& PqueryPlacement::edge()
{
    return *edge_;
}

/**
 * @brief Set the PlacementTreeEdge at which this PqueryPlacement is placed.
 *
 * This should be rarely needed. It is mostly intended for the Readers that populate the data.
 * When setting this value, the user is responsible to make sure that the new value is actually
 * a PlacementTreeEdge of the PlacementTree that belongs to the Sample where the Pquery of this
 * PqueryPlacement is stored.
 */
void PqueryPlacement::reset_edge( PlacementTreeEdge& edge )
{
    edge_ = &edge;
}

} // namespace placement
} // namespace genesis
