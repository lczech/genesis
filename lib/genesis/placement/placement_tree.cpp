/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @brief Implementation of PlacementTree class.
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/placement_tree.hpp"

#include <map>

#include "genesis/placement/sample.hpp"
#include "genesis/utils/core/logging.hpp"

namespace genesis {
namespace placement {

// =============================================================================
//     PlacementTreeEdgeData
// =============================================================================

/**
 * @brief Return the `edge_num` of this edge. This value is defined by the `jplace` standard.
 */
int PlacementEdgeData::edge_num() const
{
    return edge_num_;
}

/**
 * @brief Force to set the edge_num to a certain value.
 *
 * The edge_num is usually considered to be constant for the tree. It rarely needs to be set,
 * except for when constructing the object. Use with care.
 */
void PlacementEdgeData::reset_edge_num( int val )
{
    edge_num_ = val;
}

} // namespace placement
} // namespace genesis
