#ifndef GENESIS_TREE_COMMON_TREE_OPERATORS_H_
#define GENESIS_TREE_COMMON_TREE_OPERATORS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @ingroup tree
 */

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;

class CommonNodeData;
class CommonEdgeData;

using CommonTree = Tree;

// =================================================================================================
//     Comparison and Conversion
// =================================================================================================

/**
 * @brief Compare two CommonTree%s, that is, check whether they have identical topology,
 * node names, and branch lenghts.
 */
bool equal_common_trees(
    Tree const& lhs,
    Tree const& rhs,
    bool compare_node_names = true,
    bool compare_branch_lengths = true
);

/**
 * @brief Convert a Tree to a CommonTree with CommonNodeData and CommonEdgeData.
 *
 * This works for all Tree%s that have data derived from those two data base classes.
 * Also nodes or edges without data work. However, an expection is thrown if any node or edge
 * contains data that is not derived from CommonNodeData and CommonEdgeData, respectively.
 *
 * The data itself is copied using the clone functions, so that all names and branch lengths
 * are transferred to the returned Tree.
 */
CommonTree convert_to_common_tree( Tree const& source_tree );

} // namespace tree
} // namespace genesis

#endif // include guard
