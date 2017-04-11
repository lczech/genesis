#ifndef GENESIS_TREE_FUNCTION_FUNCTIONS_H_
#define GENESIS_TREE_FUNCTION_FUNCTIONS_H_

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
 * @ingroup tree
 */

#include <cstddef> // size_t
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Node Count Properties
// =================================================================================================

// TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

/**
 * @brief Return the highest rank of the Nodes of a Tree.
 *
 * If the Tree is empty, `0` is returned.
 */
size_t max_rank( Tree const& tree );

/**
 * @brief Return whether the Tree is bifurcating.
 */
bool is_bifurcating( Tree const& tree );

/**
 * @brief Count the number of leaf Nodes of a Tree.
 */
size_t leaf_node_count(  Tree const& tree );

/**
 * @brief Count the number of inner Nodes.
 */
size_t inner_node_count( Tree const& tree );

/**
* @brief Return the number of Edges of a Tree that lead to a leaf Node.
*/
size_t leaf_edge_count(  Tree const& tree );

/**
* @brief Return the number of Edges of a Tree that do not lead to a leaf Node.
*/
size_t inner_edge_count( Tree const& tree );

// =================================================================================================
//     Subtrees
// =================================================================================================

/**
 * @brief Return the size of the subtree defined by the given TreeLink, measured in number of nodes.
 */
size_t subtree_size( Tree const& tree, TreeLink const& link );

/**
 * @brief Calculate the sizes of all subtrees as seen from the given TreeNode.
 *
 * The function returns a vector with as many elements as the Tree has nodes. The vector is indexed
 * using the TreeNode::index() values.
 *
 * Each value in the vector tells the size (in number of nodes) of the subtree of the correnspinding
 * node, as seen from the given starting node, and excluding that starting node.
 *
 * In methaphorical words, the given starting node is used as a hook where the tree is suspended
 * from, so that it hangs down. A subtree is then the part of the tree that "hangs down" from a
 * certain node. We then count the number of nodes in each of those subtrees (that is,
 * we examine the subtree starting at each node of the tree).
 * For the starting node, the count is always the number of nodes of the tree minus one (because the
 * node is not counted itself).
 */
std::vector<size_t> subtree_sizes( Tree const& tree, TreeNode const& node );

/**
 * @brief Calculate the sizes of all subtrees as seen from the root of the tree.
 *
 * See @link subtree_sizes( Tree const& tree, TreeNode const& node ) subtree_sizes(...)@endlink
 * for details.
 */
std::vector<size_t> subtree_sizes( Tree const& tree );

/**
 * @brief Calculate the height of a subtree, that is, the maximum path length to a leaf of that
 * subtree, measured in edges between the link and the leaf.
 */
size_t subtree_max_path_height( Tree const& tree, TreeLink const& link );

std::vector<size_t> subtree_max_path_heights( Tree const& tree, TreeNode const& node );
std::vector<size_t> subtree_max_path_heights( Tree const& tree );

// =================================================================================================
//     Misc
// =================================================================================================

/**
 * @brief Helper function that finds all TreeLink%s between a given TreeNode and the root of the
 * Tree.
 *
 * Both the @link TreeNode::primary_link() primary_link()@endlink of the Node and the
 * @link Tree::root_link() root_link()@endlink of the Tree are included in the list.
 * The order of the list starts at the provided node and ends at the root.
 */
std::vector< TreeLink const* > path_to_root( TreeNode const& node );

/**
 * @brief Return the lowest common ancestor of two TreeNode%s.
 */
TreeNode const& lowest_common_ancestor( TreeNode const& node_a, TreeNode const& node_b );

/**
 * @brief Return the lowest common ancestor of two TreeNode%s.
 */
TreeNode&       lowest_common_ancestor( TreeNode& node_a,       TreeNode& node_b );

} // namespace tree
} // namespace genesis

#endif // include guard
