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

#include "tree/function/functions.hpp"

#include "tree/function/operators.hpp"
#include "tree/iterator/eulertour.hpp"
#include "tree/tree.hpp"

#include <algorithm>
#include <assert.h>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Count Properties
// =================================================================================================

/**
 * @brief Return the highest rank of the nodes of the Tree.
 */
int max_rank( Tree const& tree )
{
    int max = -1;
    for (size_t i = 0; i < tree.node_count(); ++i) {
        int rank = tree.node_at(i).rank();
        // if (rank == 1) {
        //     LOG_WARN << "Node with rank 1 found. This is a node without furcation, and usually "
        //              << "indicates an error.";
        // }
        max = std::max(rank, max);
    }
    return max;
}

/**
 * @brief Return whether the Tree is bifurcating.
 */
bool is_bifurcating( Tree const& tree )
{
    return max_rank( tree ) == 2;
}

/**
 * @brief Count the number of leaf nodes of a Tree.
 */
size_t leaf_node_count( Tree const& tree )
{
    size_t sum = 0;
    for (size_t i = 0; i < tree.node_count(); ++i) {
        auto const& n = tree.node_at(i);
        if (n.is_leaf()) {
            ++sum;
        }
    }
    return sum;
}

/**
 * @brief Count the number of inner nodes.
 */
size_t inner_node_count( Tree const& tree )
{
    return tree.node_count() - leaf_node_count( tree );
}

/**
 * @brief Return the size of the subtree defined by the given TreeLink, measured in number of nodes.
 */
size_t subtree_size( TreeLink const& link )
{
    // This is a quick and dirty solution. Traverse the whole subtree, add all nodes to a set
    // and simply return the size of that set.

    std::unordered_set< TreeNode const* > visited_nodes;

    auto cur_link = &link.outer();
    while( cur_link != &link ) {
        visited_nodes.insert( &cur_link->node() );
        cur_link = &cur_link->next().outer();
    }

    return visited_nodes.size();
}

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
std::vector<size_t> subtree_sizes( Tree const& tree, TreeNode const& node )
{
    if( ! belongs_to( tree, node )) {
        throw std::runtime_error(
            "Cannot calculate subtree_sizes(), as the given Node does not belong to the Tree."
        );
    }

    // Prepare result vector.
    std::vector<size_t> result;
    result.resize( tree.node_count(), 0 );

    // We use a stack to track the subtree sizes.
    // We store the entry link of the preorder traversal of the nodes. The entry link is the one
    // that is given when visiting the node first while doing a eulertour traversal of the tree.
    // This is always the next() link after the towards-the-starting-node/root link.
    std::vector< TreeLink const* > stack;
    stack.push_back( &node.link() );

    // Traverse the tree.
    for( auto const& it : eulertour( node )) {

        // If this is the last time we visit that node on our way back up the tree.
        // (The second part of the condition checks whether it is the starting node, because
        // in this case, we do not want to remove it.)
        if( &it.link().next() == stack.back() && stack.back() != &node.link() ) {

            // We finished with a subtree. Add the cummulative number of children of that subtree
            // to the parent node, and remove the parent from the stack (as we are done with it).
            auto st_size = result[ stack.back()->node().index() ];
            stack.pop_back();
            result[ stack.back()->node().index() ] += st_size;

        // If this node is already the current top stack element.
        } else if( &it.node() == &stack.back()->node() ) {

            // Do nothing.

        // If it is a leaf.
        } else if( it.link().is_leaf() ) {

            // Simply increment its parent's counter.
            ++result[ stack.back()->node().index() ];

        // If we will visit that node in the future again.
        } else {

            // Add a count for the immediate child (i.e., the current node) to the current stack
            // end (i.e., increment the counter of children of that node),
            // then add the current node itself to the stack, so that in the next iteration,
            // we will increase its counts.
            ++result[ stack.back()->node().index() ];
            stack.push_back( &it.link() );
        }
    }

    // The stack now should contain only a single node, which is the starting node itself.
    assert( stack.size() == 1 && stack.back() == &node.link() );

    // The size of the subtree of the starting node is always the number of nodes in the tree
    // minus one for that node itself (as it is not counted as part of its subtree).
    assert( result[ node.index() ] = tree.node_count() - 1 );

    return result;
}

/**
 * @brief Calculate the sizes of all subtrees as seen from the root of the tree.
 *
 * See @link subtree_sizes( Tree const& tree, TreeNode const& node ) subtree_sizes(...)@endlink
 * for details.
 */
std::vector<size_t> subtree_sizes( Tree const& tree )
{
    return subtree_sizes( tree, tree.root_node() );
}

} // namespace tree
} // namespace genesis
