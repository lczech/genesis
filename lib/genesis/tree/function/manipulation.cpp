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

#include "genesis/tree/function/manipulation.hpp"

#include "genesis/tree/tree.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/node_links.hpp"

#include "genesis/utils/core/algorithm.hpp"

#include <cassert>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Rerooting
// =================================================================================================

void reroot( Tree& tree, TreeLink& at_link )
{
    if( ! belongs_to( at_link, tree ) ) {
        throw std::runtime_error( "Cannot reroot Tree on a Link that is not part of the Tree." );
    }

    // We store the old root node, becasue we will change internals of the tree, so that
    // node().is_root() won't work while this function is running.
    TreeNode* old_root = &tree.root_node();

    // Pointer to the primary link of the target node.
    TreeLink* cur_link = &tree.link_at( at_link.index() ).node().primary_link();

    // Set new root index and node link of the new root.
    tree.reset_root_link_index( at_link.index() );
    at_link.node().reset_primary_link( &tree.link_at( at_link.index() ));

    // Walk the path from the new root to the old, and change all pointers of the edges and nodes
    // on that path so that they point towards the new root.
    while( &cur_link->node() != old_root ) {

        // Assert that the primary direction is correct: Is the current link is at the secondary
        // end of its edge?
        assert( cur_link == &cur_link->edge().secondary_link() );

        // Swap the edge's links, so that they point towards the new root.
        auto link_p_tmp = &cur_link->edge().primary_link();
        auto link_s_tmp = &cur_link->edge().secondary_link();
        cur_link->edge().reset_primary_link( link_s_tmp );
        cur_link->edge().reset_secondary_link( link_p_tmp );
        // std::swap( cur_link->edge_->link_p_, cur_link->edge_->link_s_ );

        // Assert that this worked.
        assert( cur_link           == &cur_link->edge().primary_link() );
        assert( &cur_link->outer() == &cur_link->edge().secondary_link() );

        // Store the link of the next node that points towards the root.
        // We need it, because we will change this upwards link of the next node now.
        auto to_root_link = &cur_link->outer().node().primary_link();

        // Change the main link of the next node so that it points towards the new root.
        cur_link->outer().node().reset_primary_link( &cur_link->outer() );

        // Move one node towards the root.
        cur_link = to_root_link;
    }
}

void reroot( Tree& tree, TreeNode& at_node )
{
    if( ! belongs_to( at_node, tree ) ) {
        throw std::runtime_error( "Cannot reroot Tree on a Node that is not part of the Tree." );
    }
    reroot( tree, at_node.link() );
}

void reroot_at_node( Tree& tree, size_t node_index )
{
    if( node_index >= tree.node_count() ) {
        throw std::runtime_error( "Cannot reroot Tree on a Node that is not part of the Tree." );
    }
    reroot( tree, tree.node_at( node_index ));
}

// =================================================================================================
//     Ladderize
// =================================================================================================

void ladderize( Tree& tree, LadderizeOrder order )
{
    // For each node, get how many nodes its subtree (away from the root) has.
    // We use this quantity to sort each node's links.
    auto sub_sizes = subtree_sizes( tree );

    // Ladderize all nodes
    for( auto& node_it : tree.nodes() ) {

        // No need to ladderize a leaf. It would still work, but we can use this as a speedup.
        if( node_it->is_leaf() ) {
            continue;
        }

        // Get the sizes of the children/subtrees of this node.
        std::vector<size_t>    child_sizes;
        std::vector<TreeLink*> child_links;
        for( auto const& link_it : node_links( *node_it ) ) {

            // Don't treat the link towards the root; we only want to sort the subtree.
            // Assert that the first iteration is actually this link towards the root.
            if( link_it.is_first_iteration() ) {
                assert( &link_it.link() == &node_it->primary_link() );
                continue;
            }

            child_sizes.push_back( sub_sizes[ link_it.link().outer().node().index() ] );
            child_links.push_back( &link_it.link() );
        }

        // Sort the sizes. We use stable sort in order to not change the order of equal sized subtrees.
        auto child_order = ( order == LadderizeOrder::kSmallFirst
            ? utils::stable_sort_indices( child_sizes.begin(), child_sizes.end(), std::less<size_t>() )
            : utils::stable_sort_indices( child_sizes.begin(), child_sizes.end(), std::greater<size_t>() )
        );

        // The number of indices needs to be the rank of the node (number of immediate children).
        assert( child_order.size() == child_sizes.size() );
        assert( child_order.size() == child_links.size() );
        assert( child_order.size() == node_it->rank() );

        // Change all next links of the node so that they reflect the subtree size order.
        auto cur_link = &node_it->primary_link();
        for( auto child_order_i : child_order ) {

            // We use this assertion to ensure that each link is only processed once.
            // At the end of this loop, we set it to nullptr, so a second encounter would fail.
            assert( child_links[ child_order_i ] );

            // Set the link's next link and move on.
            cur_link->reset_next( child_links[ child_order_i ] );
            cur_link = child_links[ child_order_i ];

            // Set the link in the vector to null, so that the above assert can check that we
            // never process it again.
            child_links[ child_order_i ] = nullptr;
        }

        // We now need to set the next pointer of the last link of the node so that it points
        // back to the original starting node (the one towards the root).
        cur_link->reset_next( &node_it->primary_link() );

        // Finally, assert that we processed all links. If so, all of them are null by now.
        for( auto const& cl : child_links ) {
            (void) cl;
            assert( !cl );
        }
    }
}

} // namespace tree
} // namespace genesis
