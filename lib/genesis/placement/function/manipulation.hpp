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
#include "genesis/placement/placement_tree.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sample;


/**
 * @brief Roots the underlying tree of a sample according to a specified outgroup.
 *
 * Inserts a new root node at distance 0 to the trifurcation separating the outgroup from the remaining
 * tree.
 * Recalculates the edge ids and updates the placement data accordingly.
 * Also takes care of the possible change of distal/proximal length values.
 */
void root(Sample& sample, PlacementTreeEdge& edge);

} // namespace tree
} // namespace placement

#endif // include guard
