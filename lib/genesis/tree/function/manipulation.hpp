#ifndef GENESIS_TREE_FUNCTION_MANIPULATION_H_
#define GENESIS_TREE_FUNCTION_MANIPULATION_H_

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

#include <cstddef> // size_t
#include <functional>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;
class Subtree;

// =================================================================================================
//     Add Nodes
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
 * @return The function returns the newly created TreeNode.
 *
 * @see @link add_new_node( Tree&, TreeEdge&, std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> ) add_new_node( Tree&, TreeEdge& )@endlink
 * for a version that adds an inner node to an edge.
 * @see add_new_leaf_node() for a function that splits and edge and adds a new leaf node to it.
 */
TreeNode& add_new_node( Tree& tree, TreeNode& target_node );

/**
 * @brief Add a new @link TreeNode Node@endlink that splits an existing @link TreeEdge Edge@endlink.
 *
 * This function adds a new Node that splits the given `target_edge` into two edges.
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
 * Be aware that the data of `target_edge` is not changed by default.
 * Thus, in trees with CommonEdgeData, the branch lengths of the two affected edges might have
 * to be changed to the desired values after calling this function.
 * Instead, one can use the @p adjust_edges functional to adjust these edges.
 * For example, in order to split the branch length in half between the two edges, use:
 *
 *     // Some tree and edge.
 *     Tree tree;
 *     auto& target_edge = ...;
 *
 *     add_new_node( tree, target_edge, []( TreeEdge& target_edge, TreeEdge& new_edge ){
 *         auto& target_bl = target_edge.data<CommonEdgeData>().branch_length;
 *         auto& new_bl    = new_edge.data<CommonEdgeData>().branch_length;
 *
 *         new_bl    = target_bl / 2.0;
 *         target_bl = target_bl / 2.0;
 *     });
 *
 * The functor is called after all changes to the Tree have been made,
 * and all data objects have been created.
 *
 * @return The function returns the newly created TreeNode.
 *
 * @see add_new_node( Tree&, TreeNode& ) for a version that adds a leaf node to a node.
 * @see add_new_leaf_node() for a function that splits and edge and adds a new leaf node to it.
 */
TreeNode& add_new_node(
    Tree& tree,
    TreeEdge& target_edge,
    std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> adjust_edges = {}
);

/**
 * @brief Add a new @link TreeNode Node@endlink as a leaf to an existing @link TreeEdge Edge@endlink,
 * by also adding a new Node in the middle of that Edge.
 *
 * This function is a combination of add_new_node( Tree&, TreeNode& ) and
 * @link add_new_node( Tree&, TreeEdge&, std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> ) add_new_node( Tree&, TreeEdge& )@endlink.
 * Before adding the new leaf node, it first adds another Node that splits the given `target_edge`
 * into two edges, and then adds the leaf to it.
 *
 * Thus, the procedure is as shown:
 * ![The Tree before and after adding the new Nodes.](tree/add_new_leaf_node.png)
 *
 * After the function, the `target_edge` with all its data ends up on the to-root side of the
 * new inner node.
 *
 * For details of how the data pointers are handled, see add_new_node( Tree&, TreeNode& ). This
 * function behaves in a similar way. The data objects of the new nodes and edges are
 * default-constructed objects of the same type as the `target_edge` and its primary node.
 *
 * Be aware that the data of `target_edge` is not changed by default.
 * Thus, in trees with CommonEdgeData, the branch lengths of the affected edges might have
 * to be changed to the desired values after calling this function.
 * In particular, this concerns the target edge as well as the new inner edge.
 * One can use the @p adjust_edges functional to adjust these edges.
 * See @link add_new_node( Tree&, TreeEdge&, std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> ) add_new_node( Tree&, TreeEdge& )@endlink
 * for an example of this.
 *
 * @return The function returns the newly created leaf TreeNode.
 *
 * @see add_new_node( Tree&, TreeNode& ) for a function that adds a leaf node to a node.
 * @see @link add_new_node( Tree&, TreeEdge&, std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> ) add_new_node( Tree&, TreeEdge& )@endlink
 * for a function that adds an inner node to an edge.
 */
TreeNode& add_new_leaf_node(
    Tree& tree,
    TreeEdge& target_edge,
    std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> adjust_edges = {}
);

/**
 * @brief Add a new @link TreeNode Node@endlink that splits an existing @link TreeEdge Edge@endlink,
 * and root the tree on that new Node.
 *
 * The function combines add_new_node( Tree&, TreeEdge& ) and reroot( Tree&, TreeNode& ).
 */
TreeNode& add_root_node( Tree& tree, TreeEdge& target_edge );

// =================================================================================================
//     Delete Nodes
// =================================================================================================

void delete_node( Tree& tree, TreeNode& target_node );

void delete_leaf_node( Tree& tree, TreeNode& target_node );

void delete_linear_node(
    Tree& tree,
    TreeNode& target_node,
    std::function<void( TreeEdge& remaining_edge, TreeEdge& deleted_edge )> adjust_edges = {}
);

void delete_subtree( Tree& tree, Subtree const& subtree );

void delete_edge(
    Tree& tree,
    TreeEdge& target_edge,
    std::function<void( TreeEdge& remaining_node, TreeEdge& deleted_node )> adjust_nodes = {}
);

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

/**
 * @brief Ladderize a Tree, that is, order its subtrees by size.
 *
 * The function flips the TreeLink order of all internal TreeNode%s of the Tree so that always
 * the smalles/largest subtree (in number of nodes) comes first when iterating the Tree.
 * This assumes a rooting, as the direction of the subtree of a node is measured away from the root.
 */
void ladderize( Tree& tree, LadderizeOrder order = LadderizeOrder::kSmallFirst );

} // namespace tree
} // namespace genesis

#endif // include guard
