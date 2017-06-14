#ifndef GENESIS_PLACEMENT_FUNCTION_OPERATORS_H_
#define GENESIS_PLACEMENT_FUNCTION_OPERATORS_H_

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/placement_tree.hpp"

#include <iosfwd>
#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class Tree;

    class DefaultNodeData;
    class DefaultEdgeData;

    using DefaultTree = Tree;
}

namespace placement {
    class PlacementEdgeData;
    class PlacementNodeData;

    class Sample;
}

namespace placement {

// =================================================================================================
//     Comparision and Equality
// =================================================================================================

/**
 * @brief Return whether two PlacementTree%s are compatible.
 *
 * This is the case iff:
 *
 *   * they have the same topology,
 *   * they have the same internal structure (e.g., node indices),
 *   * they have the same node names at corresponding nodes,
 *   * they have the same edge nums at corresponding edges,
 *   * the data types of all nodes and edges are those of a ::PlacementTree
 *
 * In all other cases, `false` is returned.
 */
bool compatible_trees( PlacementTree const& lhs, PlacementTree const& rhs );

/**
 * @brief Return whether the PlacementTree%s of two Sample%s are compatible.
 *
 * See
 * @link compatible_trees( PlacementTree const& lhs, PlacementTree const& rhs ) this version @endlink
 * of the function for details.
 */
bool compatible_trees( Sample        const& lhs, Sample        const& rhs );

// =================================================================================================
//     Conversion
// =================================================================================================

/**
 * @brief Convert a @link tree::DefaultTree DefaultTree @endlink into a ::PlacementTree.
 *
 * This function returns a new tree with the same topology as the source tree, and the same
 * node names and branch lengths. In addition, the `edge_num` property of the PlacementTree is
 * established, as it is not part of the DefaultTree data.
 */
PlacementTree convert_default_tree_to_placement_tree( tree::DefaultTree const& source_tree );

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Print a table of all Pqueries with their Placements and Names to the stream.
 */
std::ostream& operator << ( std::ostream& out, Sample const& smp );

/**
 * @brief Return a simple view of the Tree of a Sample with information about the
 * @link Pquery Pqueries @endlink on it.
 */
std::string print_tree( Sample const& smp );

} // namespace placement
} // namespace genesis

#endif // include guard
