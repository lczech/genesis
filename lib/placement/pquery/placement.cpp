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

double PqueryPlacement::likelihood() const
{
    return likelihood_;
}

PqueryPlacement& PqueryPlacement::likelihood( double val )
{
    likelihood_ = val;
    return *this;
}

double PqueryPlacement::like_weight_ratio() const
{
    return like_weight_ratio_;
}

PqueryPlacement& PqueryPlacement::like_weight_ratio( double val )
{
    like_weight_ratio_ = val;
    return *this;
}

double PqueryPlacement::proximal_length() const
{
    return proximal_length_;
}

PqueryPlacement& PqueryPlacement::proximal_length( double val )
{
    proximal_length_ = val;
    return *this;
}

double PqueryPlacement::pendant_length() const
{
    return pendant_length_;
}

PqueryPlacement& PqueryPlacement::pendant_length( double val )
{
    pendant_length_ = val;
    return *this;
}

int PqueryPlacement::parsimony() const
{
    return parsimony_;
}

PqueryPlacement& PqueryPlacement::parsimony( int val )
{
    parsimony_ = val;
    return *this;
}

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
