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
    using MassTree = Tree;
}

namespace placement {
    class PlacementEdgeData;
    class PlacementNodeData;

    class Sample;
    class SampleSet;
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

/**
* @brief Helper function to copy masses from a Sample to a
* @link tree::MassTree MassTree@endlink.
*
* The function copies the masses from a Sample to a MassTree.
* It returns the amount of work needed to move the masses from their pendant
* position to the branch (this result is only used if `with_pendant_length` is `true` in the
* calculation functions).
*/
double add_sample_to_mass_tree(
    Sample const& smp, double const sign, double const scaler, tree::MassTree& target
);

/**
 * @brief Convert a Sample to a tree::MassTree.
 *
 * The function takes all PqueryPlacement%s of the Sample and adds their masses in form of the
 * `like_weight_ratio` as mass points on a tree::MassTree.
 */
std::pair< tree::MassTree, double > convert_sample_to_mass_tree( Sample const& sample );

/**
 * @brief Convert all Sample%s in a SampleSet to tree::MassTree%s.
 */
std::pair< std::vector<tree::MassTree>, std::vector<double> >
convert_sample_set_to_mass_trees( SampleSet const& sample_set );

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
