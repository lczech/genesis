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

int PqueryPlacement::edge_num() const
{
    return edge_num_;
}

void PqueryPlacement::reset_edge_num( int val )
{
    edge_num_ = val;
}

const PlacementTreeEdge& PqueryPlacement::edge() const
{
    return *edge_;
}

PlacementTreeEdge& PqueryPlacement::edge()
{
    return *edge_;
}

void PqueryPlacement::reset_edge( PlacementTreeEdge* edge )
{
    edge_ = edge;
}

} // namespace placement
} // namespace genesis
