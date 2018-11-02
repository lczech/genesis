#ifndef GENESIS_TREE_FUNCTION_FUNCTIONS_H_
#define GENESIS_TREE_FUNCTION_FUNCTIONS_H_

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

#include "genesis/utils/containers/matrix.hpp"

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
//     Node Properties
// =================================================================================================

/**
 * @brief Return true iff the node of the given link is a leaf node.
 */
bool is_leaf( TreeLink const& link );

/**
 * @brief Return whether the node is a leaf/tip.
 */
bool is_leaf( TreeNode const& node );

/**
 * @brief Return true iff the secondary node (outwards) of the given edge is a leaf node.
 */
bool is_leaf( TreeEdge const& edge );

/**
 * @brief Return true iff the node of the given link is an inner node.
 */
bool is_inner( TreeLink const& link );

/**
 * @brief Return whether the node is an inner node.
 */
bool is_inner( TreeNode const& node );

/**
 * @brief Return true iff the secondary node (outwards) of the given edge is an inner node.
 */
bool is_inner( TreeEdge const& edge );

/**
 * @brief Return whether the link belongs to the root node of its Tree.
 */
bool is_root( TreeLink const& link );

/**
 * @brief Return whether the node is the root of its Tree.
 */
bool is_root( TreeNode const& node );

/**
 * @brief Return the degree of the node for a given TreeLink,
 * i.e. how many neighbouring nodes it has.
 */
size_t degree( TreeLink const& link );

/**
 * @brief Return the degree of the node, i.e. how many neighbouring nodes it has.
 */
size_t degree( TreeNode const& node );

// =================================================================================================
//     Node Count Properties
// =================================================================================================

// TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

/**
 * @brief Return the highest degree of the Nodes of a Tree.
 *
 * If the Tree is empty, `0` is returned.
 */
size_t max_degree( Tree const& tree );

/**
 * @brief Return whether the Tree is bifurcating.
 *
 * A tree is bifurcating iff all inner nodes have exactly three neighbouring nodes.
 * The only exception is the root node, which for rooted trees only has two neighbors.
 * Thus, this node is allowed to have two (for rooted trees) or three (for trees with a top-level
 * trifurcation instead of an actual root) neighbors. In order to test which of these is the case,
 * use is_rooted().
 *
 * If @p loose is set to `true`, the definition is a bit broadened and also allows other nodes with
 * two neighbors. Such nodes do not have a furcation at all, and thus "sit" in between two other
 * nodes. This case is however rare in practice, but can happen e.g., when wrongly rerooting the
 * tree (in case that the old root is not deleted), or on trees that are created from a taxonomy.
 */
bool is_bifurcating( Tree const& tree, bool loose = false );

/**
 * @brief Alias for is_bifurcating().
 */
bool is_binary( Tree const& tree, bool loose = false );

/**
 * @brief Return whether the Tree is rooted, that is, whether the root node has two neighbors.
 */
bool is_rooted( Tree const& tree );

/**
 * @brief Count the number of leaf Nodes of a Tree.
 */
size_t leaf_node_count(  Tree const& tree );

/**
 * @brief Count the number of inner Nodes.
 */
size_t inner_node_count( Tree const& tree );

/**
 * @brief Return the number of Nodes of a Tree. Same as Tree::node_count().
 */
size_t node_count( Tree const& tree );

/**
* @brief Return the number of Edges of a Tree that lead to a leaf Node.
*/
size_t leaf_edge_count(  Tree const& tree );

/**
* @brief Return the number of Edges of a Tree that do not lead to a leaf Node.
*/
size_t inner_edge_count( Tree const& tree );

/**
* @brief Return the number of Edges of a Tree. Same as Tree::edge_count().
*/
size_t edge_count( Tree const& tree );

/**
 * @brief Get a list of the @link tree::TreeEdge::index() edge indices@endlink of all inner edges,
 * that is, all TreeEdge%s that do not lead to a leaf TreeNode.
 */
std::vector<size_t> inner_edge_indices( Tree const& tree );

/**
 * @brief Get a list of the @link tree::TreeEdge::index() edge indices@endlink of all leaf edges,
 * that is, all TreeEdge%s that lead to a leaf TreeNode.
 */
std::vector<size_t> leaf_edge_indices( Tree const& tree );

/**
 * @brief Get a list of the @link tree::TreeNode::index() node indices@endlink
 * of all inner TreeNode%s.
 */
std::vector<size_t> inner_node_indices( Tree const& tree );

/**
 * @brief Get a list of the @link tree::TreeNode::index() node indices@endlink
 * of all leaf TreeNode%s.
 */
std::vector<size_t> leaf_node_indices( Tree const& tree );

// =================================================================================================
//     Tree Sides
// =================================================================================================

/**
 * @brief Create a @link utils::Matrix Matrix@endlink that indiciaces the relative position of the
 * @link TreeEdge Edges@endlink of a Tree, i.e., whether they are on the root side or non-root side.
 *
 * For a Tree with `e` many Edges, the resulting square Matrix has dimensions `( e, e )`. The row
 * at index `i` corresponds to edge `i`, which can be accessed via Tree::edge_at(). The entries of
 * that row indicate whether the other edges (as indexed by the columns) are on the root side of the
 * edge `i`, in which case the entry is `-1`, or on the non-root side, in which case the entry is
 * `1`. The entry for the edge `i` itself (which is the diagonale) is `0`.
 *
 * In other words, all edges on the proximal side of a given edge are denoted by a `-1`, while
 * all edges on the distal side of the given edge are denoted by a `1`.
 *
 * As a consequence, the rows of edges that lead to a leaf are full of `1`s (with the exception of
 * the diagonal entry, which still is `0`). This is because for leaf edges, all other edges are on
 * the root side of the tree.
 */
utils::Matrix<signed char> edge_sides( Tree const& tree );

/**
 * @brief Calculate a Matrix that indicates the nodes on the root side of a given node.
 *
 * The row and column indices of the Matrix represent TreeNode indices.
 * Each element of the Matrix indicates whether the column node is in the subtree of the row node
 * that contains the root (value `1`), or in a subtree that does not contain the root (value `-1`),
 * while the diagonale contains `0`.
 */
utils::Matrix<signed char> node_root_direction_matrix( Tree const& tree );

// TODO the naming convention of the above two functions is really off!

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

/**
 * @brief Compute the sign matrix or Sequential Binary Partition (SBP) of a Tree.
 *
 * The Tree has to be rooted and strictly bifurcating. Then, we can compute a matrix that tells for
 * each node a relative ordering of the other nodes. Say we have the tree:
 *
 *            /----------- T3
 *      /-----| N2
 *     |      \----------- T2
 *     | N1
 *     |
 *      \----------- T1
 *
 * This yields the (compressed) sign matrix:
 *
 * Taxa | T1 | T2 | T3
 * :--- |---:|---:|---:
 * N1   | +1 | -1 | -1
 * N2   |  0 | +1 | -1
 *
 * That is, all nodes in the subtree of the first child of a node get assigned a `+1`,
 * and all nodes in the subtree of the second child get a `-1`. The remaining nodes (the rest of
 * the tree, including the node itself) get assigned a `0`.
 *
 * This was introduced as Sequential Binary Partition (SBP) in [1], and called Sign Matrix in [2].
 * See there for more details.
 *
 * By default, the matrix dimensions are `n*n` with `n` being the number of nodes in the tree.
 * That is, all node indices are used. This is easiest to work with in the context of other
 * functions that used node indices. However, the matrix contains a lot of zeros.
 *
 * If however @p compressed is `true`, the matrix is not indexed by the node indices.
 * Instead, the rows only contain inner nodes, and the columns only contain leaf nodes,
 * as shown in the table above. That means that the sign of inner nodes is not available in the matrix.
 * Use inner_node_indices() to get the node indices that correspond to the rows of the matrix,
 * and use leaf_node_indices() for the node indices that correspond to the columns.
 *
 * > [1] V. Pawlowsky-Glahn and J. J. Egozcue,
 * > "Exploring Compositional Data with the CoDa-Dendrogram,"
 * > Austrian J. Stat., vol. 40, no. 1&2, pp. 103–113, 2011.
 * > https://ajs.or.at/index.php/ajs/article/view/vol40,%20no1&2%20-%2011
 *
 * > [2] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 */
utils::Matrix<signed char> sign_matrix( Tree const& tree, bool compressed = false );

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

/**
 * @brief Return the lowest common ancestor of each pair of TreeNode%s for a given tree,
 * in form of a @link utils::Matrix Matrix@endlink of their indices.
 *
 * The entries in the resulting Matrix are the Node indices of the lowest common ancestor (LCA) of a
 * given pair of Nodes. For example, for the result Matrix `r`, the entry `r[ 3, 5 ] == 7` means
 * that the LCA of Nodes `3` and `5` is Node `7`.
 *
 * These Nodes can for example be accesses via Tree::node_at().
 */
utils::Matrix<size_t> lowest_common_ancestors( Tree const& tree );

} // namespace tree
} // namespace genesis

#endif // include guard
