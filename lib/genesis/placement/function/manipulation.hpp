#ifndef GENESIS_PLACEMENT_FUNCTION_MANIPULATION_H_
#define GENESIS_PLACEMENT_FUNCTION_MANIPULATION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech, Pierre Barbera and HITS gGmbH

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/sample.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sample;

// =================================================================================================
//     Rooting
// =================================================================================================

/**
 * @brief Root the underlying PlacementTree of a Sample at a specified TreeEdge.
 *
 * The function inserts a new root node on the given @p target_edge,
 * which splits the edge into two edges. The edge closer to the (old) root of the tree gets a
 * @link tree::CommonEdgeData::branch_length branch length@endlink of `0.0`, and receives no placements.
 * The edge further away from the root gets the branch length and all placements of the @p target_edge.
 * See tree::make_rooted() for further details.
 *
 * The function also recalculates the @link PlacementEdgeData::edge_num edge nums@endlink of all edges
 * using reset_edge_nums(), and updates the placement data accordingly. This is because adding
 * a node and an edge in the middle of the tree changes the traversal order, and hence the edge nums.
 * It also takes care of changing the distal/proximal length values of the PqueryPlacement%s
 * that sit in between the old (trifurcation) root and the new (proper) root.
 */
void make_rooted( Sample& sample, PlacementTreeEdge& target_edge );

} // namespace tree
} // namespace placement

#endif // include guard
