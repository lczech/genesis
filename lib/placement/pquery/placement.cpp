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
//     Accessors
// =================================================================================================

/**
 * @brief Get the `edge_num` where this PqueryPlacement is placed.
 *
 * This number corresponds to the `edge_num` property as described in the `jplace` standard.
 * It is not to be confused with the index of the PlacementTreeEdge.
 */
int PqueryPlacement::edge_num() const
{
    return edge_num_;
}

/**
 * @brief Reset the edge num.
 *
 * This should be rarely needed. It is mostly intended for the Readers that populate the data.
 * Be aware that changing the value after the PqueryPlacement is in use may result in weird
 * behaviour.
 */
void PqueryPlacement::reset_edge_num( int val )
{
    edge_num_ = val;
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
 * @brief Reset the PlacementTreeEdge at which this PqueryPlacement is placed.
 *
 * This should be rarely needed. It is mostly intended for the Readers that populate the data.
 * Be aware that changing the value after the PqueryPlacement is in use may result in weird
 * behaviour.
 */
void PqueryPlacement::reset_edge( PlacementTreeEdge* edge )
{
    edge_ = edge;
}

} // namespace placement
} // namespace genesis
