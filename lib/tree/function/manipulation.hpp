#ifndef GENESIS_TREE_FUNCTION_MANIPULATION_H_
#define GENESIS_TREE_FUNCTION_MANIPULATION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * The difference between this function and @link reroot( TreeNode const& ) its node version@endlink
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
 * See @link reroot( TreeLink const& ) this version of the function@endlink for details.
 *
 * The node needs to be part of the tree, otherwise an exception is thrown.
 */
void reroot( Tree& tree, TreeNode& at_node );

/**
 * @brief Reroot the Tree at the TreeNode with the given index.
 *
 * See @link reroot( TreeLink const& ) reroot(...)@endlink for details.
 *
 * The node index needs to be valid for the tree, otherwise an exception is thrown.
 */
void reroot_at_node( Tree& tree, size_t node_index );

} // namespace tree
} // namespace genesis

#endif // include guard
