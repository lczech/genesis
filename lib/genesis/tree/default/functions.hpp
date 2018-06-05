#ifndef GENESIS_TREE_DEFAULT_FUNCTIONS_H_
#define GENESIS_TREE_DEFAULT_FUNCTIONS_H_

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
 * @brief Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/utils/containers/sorted_vector.hpp"

#include <set>
#include <string>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeSet;

// =================================================================================================
//     Node Names
// =================================================================================================

/**
 * @brief Returns an unordered set of all TreeNode names of a Tree.
 *
 * If `leaves_only` is set to true, nodes names of inner nodes are not included.
 * Unnamed nodes (`node.data.name == ""`) are always excluded.
 * The only difference to node_names_sorted() is the type of container used for storing the result.
 *
 * The provided Tree needs to have TreeNode%s with data types deriveed from DefaultNodeData.
 */
std::unordered_set<std::string> node_names(
    Tree const& tree,
    bool leaves_only = false
);

/**
 * @brief Returns a set of all TreeNode names of a Tree.
 *
 * If `leaves_only` is set to true, nodes names of inner nodes are not included.
 * Unnamed nodes (`node.data.name == ""`) are always excluded.
 * The only difference to node_names() is the type of container used for storing the result.
 *
 * The provided Tree needs to have TreeNode%s with data types deriveed from DefaultNodeData.
 */
utils::SortedVector<std::string> node_names_sorted(
    Tree const& tree,
    bool leaves_only = false
);

/**
 * @brief Returns a set of all TreeNode names of a TreeSet.
 *
 * The function returns the set of all names of all Tree%s in the set. See
 * @link node_names( Tree const&, bool ) node_names(...)@endlink this version of the
 * function for details.
 */
std::unordered_set<std::string> node_names(
    TreeSet const& tree_set,
    bool leaves_only = false
);

/**
 * @brief Returns a set of all TreeNode names of a TreeSet.
 *
 * The function returns the set of all names of all Tree%s in the set. See
 * @link node_names_sorted( Tree const&, bool ) node_names_sorted(...)@endlink this version of the
 * function for details.
 */
utils::SortedVector<std::string> node_names_sorted(
    TreeSet const& tree_set,
    bool leaves_only = false
);

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
TreeNode const* find_node(
    Tree const& tree,
    std::string const& name,
    bool replace_underscores = false
);

/**
 * @brief Finds a Node, given its name. If not found, nullptr is returned.
 */
TreeNode* find_node(
    Tree& tree,
    std::string const& name,
    bool replace_underscores = false
);

// =================================================================================================
//     Branch Length
// =================================================================================================

/**
 * @brief Get the length of the tree, i.e., the sum of all branch lengths.
 */
double length(   Tree const& tree );

/**
 * @brief Get the height of the tree, i.e., the longest distance from the root to a leaf,
 * measured using the branch_length.
 */
double height(   Tree const& tree );

/**
 * @brief Get the diameter of the tree, i.e., the longest distance between any two nodes,
 * measured using the branch_length.
 */
double diameter( Tree const& tree );

/**
 * @brief Get a vector of all branch lengths of a Tree, index by the
 * @link TreeEdge::index() edge index@endlink.
 */
std::vector<double> branch_lengths( Tree const& tree );

/**
 * @brief Set all branch lengths of a Tree to a given value.
 *
 * See also scale_all_branch_lengths() for a scaling function.
 */
void set_all_branch_lengths(
    Tree&  tree,
    double length = 1.0
);

/**
 * @brief Scale all branch lengths of a Tree by a given factor.
 *
 * This function simply multiplies all branch lengths with the given factor.
 * See also set_all_branch_lengths() for setting the branch lengths to a value.
 */
void scale_all_branch_lengths(
    Tree&  tree,
    double factor = 1.0
);

/**
 * @brief Return a Tree where the branch lengths are the average of the Tree%s in the TreeSet,
 * given that they all have the same topology.
 *
 * The function works only under the following conditions:
 *
 *     * All trees must have the same topology.
 *     * The TreeType must provide a data member `branch_length` for the edges.
 *
 * Otherwise, the function throws an `std::runtime_error`. It does not check for node names,
 * but the returned tree will contain the names of the first tree in the set.
 */
Tree average_branch_length_tree( TreeSet const& tset );

} // namespace tree
} // namespace genesis

#endif // include guard
