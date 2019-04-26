#ifndef GENESIS_TREE_FUNCTION_MANIPULATION_H_
#define GENESIS_TREE_FUNCTION_MANIPULATION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/common_tree/tree.hpp"

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
//     Minimal Tree
// =================================================================================================

/**
 * @brief Create a minimal Tree that can be used with manipulation functions such as add_new_node()
 * or add_new_leaf_node() to build a custom tree.
 *
 * A minimal tree for our purposes consists of two TreeNode%s, a TreeEdge between them,
 * and the respective TreeLink%s.
 *
 * Note that the resulting tree does not have any data types at its nodes and edges.
 * See minimal_tree() for a version of this function that also creates these data types.
 */
Tree minimal_tree_topology();

/**
 * @brief Create a minimal Tree that can be used with manipulation functions such as add_new_node()
 * or add_new_leaf_node() to build a custom tree, including default data types at nodees and edges.
 *
 * A minimal tree for our purposes consists of two TreeNode%s, a TreeEdge between them,
 * and the respective TreeLink%s.
 *
 * The resulting tree has default-constructed node and edge data types of the given template
 * arugments. See minimal_tree_topology() for a version of this function that does not create
 * any data.
 */
template< class NodeDataType = CommonNodeData, class EdgeDataType = CommonEdgeData >
Tree minimal_tree()
{
    auto tree = minimal_tree_topology();
    for( size_t i = 0; i < tree.node_count(); ++i ) {
        tree.node_at(i).reset_data( NodeDataType::create() );
    }
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        tree.edge_at(i).reset_data( EdgeDataType::create() );
    }
    return tree;
}

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

// =================================================================================================
//     Delete Nodes
// =================================================================================================

/**
 * @brief Delete a TreeNode from a Tree.
 *
 * This function is a simple wrapper for the specialized deletion functions:
 *
 *  * If the node is a leaf, delete_leaf_node() is called.
 *  * If it is a linear node, delete_linear_node() is called. Note that in this case, the edges
 *    are not adjusted.
 *  * In all other cases, delete_subtree() is called, deleting the Subtree of the given
 *    @p target_node away from the root.
 *
 * See these other functions for details.
 */
void delete_node( Tree& tree, TreeNode& target_node );

/**
 * @brief Delete a leaf TreeNode.
 *
 * If the deleted node is the root, the root is reset to the node where the leaf is attached.
 */
void delete_leaf_node( Tree& tree, TreeNode& target_node );

/**
 * @brief Delete a "linear" TreeNode from a Tree, that is, a node with two neighbours.
 *
 * Such nodes for example occur as the root node in properly rooted trees. The deletion of this
 * node leads to its two edges becoming one. The edge that is deleted is the one further away
 * from the root of the tree, while the other edge remains. If the @p target_node itself is the root,
 * the edge that is being deleted is the one that is not at the primary link of the node.
 * In that case, the root is also reset to the node adjacent to the primary link of the @p target_node.
 *
 * As one edge is deleted, it might be neccessary to update the data of the other, for example,
 * to adjust its branch length by adding up both of them. The @p adjust_edges functional
 * can be used to achivve this. By default, no adjustments are done.
 * For an example of a similar function, see
 * @link add_new_node( Tree&, TreeEdge&, std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> ) add_new_node( Tree&, TreeEdge& )@endlink.
 */
void delete_linear_node(
    Tree& tree,
    TreeNode& target_node,
    std::function<void( TreeEdge& remaining_edge, TreeEdge& deleted_edge )> adjust_edges = {}
);

/**
 * @brief Delete a complete Subtree from a Tree.
 *
 * The function deletes a Subtree, including the edge where it is attached.
 * This works for almost all possible subtrees (including leaf-only subtrees), with one notable
 * exception: A subtree that contains all of the tree but one leaf cannot be deleted.
 * In that case, the remaining tree would be just a leaf, with no link and no edge,
 * which is not a valid tree.
 *
 * If the subtree contains the root node, the root is reset to the node where the subtree is attached.
 */
void delete_subtree( Tree& tree, Subtree const& subtree );

// void delete_edge(
//     Tree& tree,
//     TreeEdge& target_edge,
//     std::function<void( TreeEdge& remaining_node, TreeEdge& deleted_node )> adjust_nodes = {}
// );

// =================================================================================================
//     Rooting, Rerooting, Unrooting
// =================================================================================================

/**
 * @brief Root a Tree at a given TreeEdge.
 *
 * The function expects an unrooted tree with a top-level tri- or multifurcation,
 * which is checked via is_rooted().
 * It then adds a new @link TreeNode Node@endlink that splits the given @p target_edge in two,
 * and roots the tree on that new Node.
 *
 * The function simply combines
 * @link add_new_node( Tree&, TreeEdge&, std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> ) add_new_node( Tree&, TreeEdge& )@endlink
 * and change_rooting( Tree&, TreeNode& ). See there for details, and for the usage of @p adjust_edges.
 *
 * @see @link make_rooted( Tree&, std::function<void(TreeEdge& target_edge, TreeEdge& new_edge)>) make_rooted( Tree&, ... )@endlink
 * @see make_unrooted()
 *
 * @return The newly created root TreeNode.
 */
TreeNode& make_rooted(
    Tree& tree,
    TreeEdge& target_edge,
    std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> adjust_edges = {}
);

/**
 * @brief Root a Tree on the first TreeEdge of its current top level TreeNode.
 *
 * The tree is expected to be unrooted. The function then roots the tree on the edge that is
 * at the primary link of the current top level trifurcation. That is, a new node is inserted on
 * that edge, see @link make_rooted( Tree&, TreeEdge&, std::function<void(TreeEdge& target_edge, TreeEdge& new_edge)>) make_rooted( Tree&, TreeEdge&, ... )@endlink
 * for details.
 *
 * @see @link make_rooted( Tree&, TreeEdge&, std::function<void(TreeEdge& target_edge, TreeEdge& new_edge)>) make_rooted( Tree&, TreeEdge&, ... )@endlink
 * @see make_unrooted()
 *
 * @return The newly created root TreeNode.
 */
TreeNode& make_rooted(
    Tree& tree,
    std::function<void( TreeEdge& target_edge, TreeEdge& new_edge )> adjust_edges = {}
);

/**
 * @brief Unroot a Tree.
 *
 * The tree is expected to be rooted, which is checked via is_rooted(). The function then removes
 * this root by calling delete_linear_node(). See there for details, and for the usage of
 * @p adjust_edges.
 *
 * @see make_rooted()
 */
void make_unrooted(
    Tree& tree,
    std::function<void( TreeEdge& remaining_edge, TreeEdge& deleted_edge )> adjust_edges = {}
);

/**
 * @brief "Reroot" the Tree at the given TreeLink.
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
 * The difference between this function and change_rooting( Tree& tree, TreeNode const& )
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
void change_rooting( Tree& tree, TreeLink& at_link );

/**
 * @brief Reroot the Tree at the given TreeNode.
 *
 * See change_rooting( Tree&, TreeLink& ) for details.
 *
 * The node needs to be part of the tree, otherwise an exception is thrown.
 */
void change_rooting( Tree& tree, TreeNode& at_node );

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
