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

#include "tree/function/distances.hpp"
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

// =================================================================================================
//     Subtrees
// =================================================================================================

/**
 * @brief Return the size of the subtree defined by the given TreeLink, measured in number of nodes.
 */
size_t subtree_size( Tree const& tree, TreeLink const& link )
{
    if( ! belongs_to( tree, link )) {
        throw std::runtime_error(
            "Cannot caluclate subtree_size, as the given Link does not belong to the Tree."
        );
    }

    // TODO This is a quick and dirty solution. Traverse the whole subtree, add all nodes to a set
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

    // TODO this is an overly complex and thus error prone solution, maybe there is a better way?!

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

/**
 * @brief Calculate the height of a subtree, that is, the maximum path length to a leaf of that
 * subtree, measured in edges between the link and the leaf.
 */
size_t subtree_max_path_height( Tree const& tree, TreeLink const& link )
{
    // TODO make more efficient. no need for full dist vector.
    auto dists = node_path_length_vector( tree, link.outer().node() );
    size_t max = 0;

    auto cur_link = &link.outer();
    while( cur_link != &link ) {
        max = std::max( max, dists[ cur_link->node().index() ] );
        cur_link = &cur_link->next().outer();
    }
    return max;
}

// std::vector<size_t> subtree_max_path_heights( Tree const& tree, TreeNode const& node )
// {
//     (void) tree;
//     (void) node;
//     throw std::domain_error("Not implemented.");
// }
//
// std::vector<size_t> subtree_max_path_heights( Tree const& tree )
// {
//     return subtree_heights( tree, tree.root_node() );
// }

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
std::vector< TreeLink const* > path_to_root( TreeNode const& node )
{
    std::vector< TreeLink const* > path;

    // Move towards the root and record all links in between.
    TreeLink const* cur_link = &node.primary_link();
    while( &cur_link->edge().secondary_link() == cur_link ) {

        // The above while condition means: is it the root?! Assert, that the default way of
        // checking for the root by using the node gives the same result.
        assert( ! cur_link->node().is_root() );

        // Assert that the primary direction is correct.
        assert( cur_link == &cur_link->edge().secondary_link() );

        // Add the primary link of the current node to the list.
        path.push_back( cur_link );

        // Move one node towards the root.
        // Assert that the default way of finding the next node towards the root (by using
        // the edge) gives the same result as simply using the link's outer node.
        // This is the case because the cur link is the one that points towards the root
        // (which was asserted above).
        assert( &cur_link->edge().primary_link() == &cur_link->outer() );
        cur_link = &cur_link->outer().node().primary_link();
    }

    // Now finally add the root itself and return the list.
    assert( cur_link->node().is_root() );
    path.push_back( cur_link );
    return path;
}

/**
 * @brief Return the lowest common ancestor of two TreeNode%s.
 */
TreeNode const& lowest_common_ancestor( TreeNode const& node_a, TreeNode const& node_b )
{
    // Speedup and simplification.
    if( &node_a == &node_b ) {
        return node_a;
    }

    auto path_a = path_to_root( node_a );
    auto path_b = path_to_root( node_b );

    // We must have at least the two original links in the front and the root in the back.
    assert( path_a.size() > 0 && path_b.size() > 0 );
    assert( path_a.front()    == &node_a.link() );
    assert( path_b.front()    == &node_b.link() );
    assert( path_a.back()     == path_b.back() );

    // Remove from back as long as the last two elements are the same.
    // At the end of this, the remaining links are the ones on the path between
    // the two original links.
    while(
        path_a.size() > 1 &&
        path_b.size() > 1 &&
        path_a.at( path_a.size() - 1 ) == path_b.at( path_b.size() - 1 ) &&
        path_a.at( path_a.size() - 2 ) == path_b.at( path_b.size() - 2 )
    ) {
        path_a.pop_back();
        path_b.pop_back();
    }

    // Now, the last elements need to be the same (the LCA of the start and finish node).
    assert( path_a.size() > 0 && path_b.size() > 0 );
    assert( path_a.back()     == path_b.back() );

    return path_a.back()->node();
}

/**
 * @brief Return the lowest common ancestor of two TreeNode%s.
 */
TreeNode&       lowest_common_ancestor( TreeNode& node_a,       TreeNode& node_b )
{
    auto const& c_node_a = static_cast< TreeNode const& >( node_a );
    auto const& c_node_b = static_cast< TreeNode const& >( node_b );
    return const_cast< TreeNode& >( lowest_common_ancestor( c_node_a, c_node_b ));
}

} // namespace tree
} // namespace genesis
