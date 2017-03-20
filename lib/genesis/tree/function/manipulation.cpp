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
#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Add single Nodes
// =================================================================================================

TreeEdge& add_new_node( Tree& tree, TreeNode& target_node )
{
    // Basic check.
    if( ! belongs_to( target_node, tree ) ) {
        throw std::runtime_error(
            "Cannot create Node at Tree on an Edge that is not part of the Tree."
        );
    }

    // Get container access shortcuts.
    auto& links = tree.expose_link_container();
    auto& nodes = tree.expose_node_container();
    auto& edges = tree.expose_edge_container();

    // Create all new elements that we need:
    //  1. A link that gets added to the given node and connects it to the new node.
    //  2. The link that belongs to the newly created leaf node.
    //  3. The newly created node itself.
    //  4. The edge between the two nodes.
    auto con_link_u = utils::make_unique< TreeLink >();
    auto end_link_u = utils::make_unique< TreeLink >();
    auto end_node_u = utils::make_unique< TreeNode >();
    auto con_edge_u = utils::make_unique< TreeEdge >();

    // Get the pointers, for ease of use.
    auto const con_link = con_link_u.get();
    auto const end_link = end_link_u.get();
    auto const end_node = end_node_u.get();
    auto const con_edge = con_edge_u.get();

    // Populate the link at the given node.
    con_link->reset_index( links.size() );
    con_link->reset_node( &target_node );
    con_link->reset_edge( con_edge );
    con_link->reset_outer( end_link );

    // Find the last link of the given node (in traversal order around the node).
    auto const up_link = &target_node.primary_link();
    auto last_link = up_link;
    while( &last_link->next() != up_link ) {
        last_link = &last_link->next();
    }

    // Now insert the new link in between up_link and last_link.
    assert( &last_link->next() == up_link );
    last_link->reset_next( con_link );
    con_link->reset_next( up_link );
    assert( &last_link->next() == con_link );
    assert( &con_link->next()  == up_link  );

    // Add the link to the tree. This changes the size of the links vector, so the next call to
    // links.size() will give a new, proper value for the other link.
    links.push_back( std::move( con_link_u ));

    // Populate the link at the new node and add it to the tree.
    end_link->reset_index( links.size() );
    end_link->reset_node( end_node );
    end_link->reset_edge( con_edge );
    end_link->reset_next( end_link );
    end_link->reset_outer( con_link );
    links.push_back( std::move( end_link_u ));

    // Populate the new node and add it to the tree.
    end_node->reset_index( nodes.size() );
    end_node->reset_primary_link( end_link );
    end_node->reset_data( target_node.data_ptr()->recreate() );
    nodes.push_back( std::move( end_node_u ));

    // Populate the new edge and add it to the tree.
    con_edge->reset_index( edges.size() );
    con_edge->reset_primary_link( con_link );
    con_edge->reset_secondary_link( end_link );
    con_edge->reset_data( target_node.primary_link().edge().data_ptr()->recreate() );
    edges.push_back( std::move( con_edge_u ));

    // Return the new edge. We just moved the edge uniq ptr, but not the pointee, so this is valid.
    return *con_edge;
}

TreeEdge& add_new_node( Tree& tree, TreeEdge& target_edge )
{
    // Basic check.
    if( ! belongs_to( target_edge, tree ) ) {
        throw std::runtime_error(
            "Cannot create Node at Tree on an Edge that is not part of the Tree."
        );
    }

    // Get container access shortcuts.
    auto& links = tree.expose_link_container();
    auto& nodes = tree.expose_node_container();
    auto& edges = tree.expose_edge_container();

    // This function works in two steps: First, we create a new node with all necessary other
    // elements in the middle of the target edge. Then, we add the new leaf node to this node
    // by calling the node version of add_new_node() on the new mid-edge node.

    // Create all new elements for the first step:
    //  * Two links that build a new node in the middle of the target edge.
    //  * The new node in the middle of the target edge.
    //  * A new edge that connects to the secondary end of the target edge.
    auto pri_link_u = utils::make_unique< TreeLink >();
    auto sec_link_u = utils::make_unique< TreeLink >();
    auto mid_node_u = utils::make_unique< TreeNode >();
    auto sec_edge_u = utils::make_unique< TreeEdge >();

    // Get the pointers, for ease of use.
    auto const pri_link = pri_link_u.get();
    auto const sec_link = sec_link_u.get();
    auto const mid_node = mid_node_u.get();
    auto const sec_edge = sec_edge_u.get();

    // Populate the link towards the primary end of the target edge and add it to the tree.
    pri_link->reset_index( links.size() );
    pri_link->reset_next( sec_link );
    pri_link->reset_outer( &target_edge.primary_link() );
    pri_link->reset_node( mid_node );
    pri_link->reset_edge( &target_edge );
    links.push_back( std::move( pri_link_u ));

    // Populate the link towards the secondary end of the target edge and add it to the tree.
    sec_link->reset_index( links.size() );
    sec_link->reset_next( pri_link );
    sec_link->reset_outer( &target_edge.secondary_link() );
    sec_link->reset_node( mid_node );
    sec_link->reset_edge( sec_edge );
    links.push_back( std::move( sec_link_u ));

    // Populate the new node in the middle of the target edge and add it to the tree.
    mid_node->reset_index( nodes.size() );
    mid_node->reset_primary_link( pri_link );
    mid_node->reset_data( target_edge.primary_node().data_ptr()->recreate() );
    nodes.push_back( std::move( mid_node_u ));

    // Populate the edge at the secondary end of the target edge and add it to the tree.
    sec_edge->reset_index( edges.size() );
    sec_edge->reset_primary_link( sec_link );
    sec_edge->reset_secondary_link( &target_edge.secondary_link() );
    sec_edge->reset_data( target_edge.data_ptr()->recreate() );
    edges.push_back( std::move( sec_edge_u ));

    // Finally adjust the existing tree elements.
    target_edge.primary_link().reset_outer( pri_link );
    target_edge.secondary_link().reset_outer( sec_link );
    target_edge.secondary_link().reset_edge( sec_edge );
    target_edge.reset_secondary_link( pri_link );

    return add_new_node( tree, *mid_node );
}

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
