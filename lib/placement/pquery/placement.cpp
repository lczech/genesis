/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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
    return edge_->data<PlacementEdgeData>().edge_num();
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
