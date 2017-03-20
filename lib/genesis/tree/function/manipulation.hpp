#ifndef GENESIS_TREE_FUNCTION_MANIPULATION_H_
#define GENESIS_TREE_FUNCTION_MANIPULATION_H_

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
//     Add single Nodes
// =================================================================================================

/**
 * @brief Add a new @link TreeNode Node@endlink as a leaf to an existing Node.
 *
 * The function adds a new leaf to the Tree by appending it to an existing TreeNode.
 * For this, four new elements are created and added to the tree:
 *
 *  1. A TreeLink that gets added to the given node and connects it to the new node.
 *  2. A TreeLink for the new TreeNode.
 *  3. The new TreeNode itself.
 *  4. A TreeEdge that connects the new and the given TreeNode with each other.
 *
 * Thus, the procedure is as shown:
 * ![The Tree before and after adding the new Node.](tree/add_new_node_node.png)
 *
 * The new Node is added at the "end" of the given Node. That is, when traversing the Tree or the
 * given Node, the new Node will be visited last. In the example, the tree is bifurcating before
 * the new node is added. This is just an example - the function also works with Nodes that are
 * already multifurcating.
 *
 * The data pointers of the new Node and Edge are initialized to default-constructed objects:
 *
 *  * The new node data type is the same as the one of the given `target_node`.
 *  * The new edge data type is the same as the one of the edge towards the root of the
 *    `target_node`.
 *
 * This means that in case of a Tree where every Node and Edge has the same node and edge data type
 * (standard case), the newly created ones will also have these data types.
 *
 * @return The function returns the newly create TreeEdge. This way, all other new elements can
 * be accessed, using TreeEdge::primary_link(), TreeEdge::secondary_link() and
 * TreeEdge::secondary_node().
 */
TreeEdge& add_new_node( Tree& tree, TreeNode& target_node );

/**
 * @brief Add a new @link TreeNode Node@endlink as a leaf to an existing @link TreeEdge Edge@endlink,
 * by also adding a new Node in the middle of that Edge.
 *
 * This function is an extension of add_new_node( Tree&, TreeNode& ). Before adding the new leaf
 * node, it first adds another Node that splits the given `target_edge` into two edges, and then
 * adds the leaf to it.
 *
 * Thus, the procedure is as shown:
 * ![The Tree before and after adding the new Node.](tree/add_new_node_edge.png)
 *
 * After the function, the `target_edge` with all its data ends up on the to-root side of the
 * new inner node.
 *
 * For details of how the data pointers are handled, see add_new_node( Tree&, TreeNode& ). This
 * function behaves in a similar way. The data objects of the new nodes and edges are
 * default-constructed objects of the same type as the `target_edge` and its primary node.
 *
 * Be aware that the data of `target_edge` is not changed. Thus, in trees with DefaultEdgeData,
 * the branch lengths of all three affected edges might have to be changed to the desired values
 * after calling this function.
 *
 * @return The function returns the newly created TreeEdge that leads to the new leaf node.
 */
TreeEdge& add_new_node( Tree& tree, TreeEdge& target_edge );

// =================================================================================================
//     Rerooting
// =================================================================================================

/**
 * @brief Reroot the Tree at the given TreeLink.
 *
 * The function sets the root of the tree to the node of the given link. This operation does not
 * change the topology of the tree, but merely adjusts some internal properties. The main changes
 * are that Tree::root_node() and Tree::root_link() will return the new root after calling this
 * function, and that tree iterators will start traversing the tree from this new root by default.
 *
 * There are three internal changes made to the tree data structure:
 *
 *   * All primary and secondary ends of the edges on the path between the new root and the old
 *     root are swapped. This is because the edges now need to point towards the new root.
 *   * Similarly, all (primary) links of the nodes on that path are changed so that they point
 *     towards the new root.
 *   * Also, the (internal) root_link_index is changed to the new root link. This is used for
 *     the functions Tree::root_node() and Tree::root_link().
 *
 * The difference between this function and reroot( Tree& tree, TreeNode const& )
 * is that when specifying a specific link, this link is used as the (primary) link of the new root
 * node. This way, algorithms and iterators (e.g., IteratorLevelorder) will start traversing the
 * tree in the direction of this link by default.
 * When specifying a node for rerooting instead, the primary link of that node is used,
 * so that iterators start traversing the tree in the direction of the old root instead.
 * For most applications, this does not make a difference. However, there might be cases where the
 * start directing makes a difference. Thus, we offer both versions of this function.
 *
 * The link needs to be part of the tree, otherwise an exception is thrown.
 */
void reroot( Tree& tree, TreeLink& at_link );

/**
 * @brief Reroot the Tree at the given TreeNode.
 *
 * See reroot( Tree&, TreeLink& ) for details.
 *
 * The node needs to be part of the tree, otherwise an exception is thrown.
 */
void reroot( Tree& tree, TreeNode& at_node );

/**
 * @brief Reroot the Tree at the TreeNode with the given index.
 *
 * See reroot( Tree&, TreeLink& ) for details.
 *
 * The node index needs to be valid for the tree, otherwise an exception is thrown.
 */
void reroot_at_node( Tree& tree, size_t node_index );

// =================================================================================================
//     Ladderize
// =================================================================================================

enum class LadderizeOrder
{
    kSmallFirst,
    kLargeFirst
};

void ladderize( Tree& tree, LadderizeOrder order = LadderizeOrder::kSmallFirst );

} // namespace tree
} // namespace genesis

#endif // include guard
