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
 * @brief Implementation of Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/function/operators.hpp"

#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/node_links.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/printer/compact.hpp"
#include "genesis/tree/tree/subtree.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"

#include <ostream>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Conversion
// =================================================================================================

Tree convert(
    Tree const& source,
    std::function< std::unique_ptr<BaseNodeData>( BaseNodeData const& node_data )> node_data_converter,
    std::function< std::unique_ptr<BaseEdgeData>( BaseEdgeData const& edge_data )> edge_data_converter
) {
    // Copy the topology. All data pointers of the new tree are nullptrs.
    auto res = source.clone_topology();

    // Convert data where there is data.
    for( size_t i = 0; i < res.node_count(); ++i ) {
        if( source.node_at(i).has_data() ) {
            res.node_at(i).reset_data( node_data_converter( *source.node_at(i).data_ptr() ));
        }
    }
    for( size_t i = 0; i < res.edge_count(); ++i ) {
        if( source.edge_at(i).has_data() ) {
            res.edge_at(i).reset_data( edge_data_converter( *source.edge_at(i).data_ptr() ));
        }
    }

    return res;
}

// =================================================================================================
//     Equality and Identity
// =================================================================================================

bool equal(
    Tree const& lhs,
    Tree const& rhs,
    std::function<bool ( TreeNode const&, TreeNode const&) > node_comparator,
    std::function<bool ( TreeEdge const&, TreeEdge const&) > edge_comparator
) {
    // Check array sizes.
    if (lhs.link_count() != rhs.link_count() ||
        lhs.node_count() != rhs.node_count() ||
        lhs.edge_count() != rhs.edge_count()
    ) {
        return false;
    }

    // do a preorder traversal on both trees in parallel
    auto it_l = preorder(lhs).begin();
    auto it_r = preorder(rhs).begin();
    for (
        ;
        it_l != preorder(lhs).end() && it_r != preorder(rhs).end();
        ++it_l, ++it_r
    ) {
        if( degree( it_l.node() ) != degree( it_r.node() ) ||
            !node_comparator( it_l.node(), it_r.node() ) ||
            !edge_comparator( it_l.edge(), it_r.edge() )
        ) {
            return false;
        }
    }

    // check whether we are done with both trees
    if( it_l != preorder(lhs).end() || it_r != preorder(rhs).end() ) {
        return false;
    }

    return true;
}

bool equal(
    std::vector<Tree> const& trees,
    std::function<bool ( TreeNode const&, TreeNode const&) > node_comparator,
    std::function<bool ( TreeEdge const&, TreeEdge const&) > edge_comparator
) {
    // If all pairs of two adjacent trees are equal, all of them are.
    // Thus, we do not need a complete pairwise comparision.
    // In order to also accelarate via OpenMP, we need a flag instead of immediately
    // returning on finding a false. We cannot break in OpenMP, but we can skip (which is fast),
    // if we already know the result.

    bool result = true;
    #pragma omp parallel for
    for (size_t i = 1; i < trees.size(); i++) {
        if( ! result ) {
            continue;
        }

        if( ! equal( trees[i-1], trees[i], node_comparator, edge_comparator )) {
            result = false;
        }
    }
    return result;
}

/* *
 * @brief Compares two trees for equality using the respective comparision operators for their nodes
 * and edges.
 *
 * This method is mainly a shortcut for the other equal function, where the comparator functionals
 * are instanciated using the default comparision operators of the tree's data.
 */
// bool equal( Tree const& lhs, Tree const& rhs)
// {
//     auto node_comparator = [] (
//         TreeNode const& node_l,
//         TreeNode const& node_r
//     ) {
//         return node_l == node_r;
//     };
//
//     auto edge_comparator = [] (
//         TreeEdge const& edge_l,
//         TreeEdge const& edge_r
//     ) {
//         return edge_l == edge_r;
//     };
//
//     return equal<TreeTypeL, TreeTypeR>(lhs, rhs, node_comparator, edge_comparator);
// }

bool identical_topology( Tree const& lhs, Tree const& rhs, bool identical_indices )
{
    auto node_comparator = [ &identical_indices ] (
        TreeNode const& node_l,
        TreeNode const& node_r
    ) {
        if( identical_indices && node_l.index() != node_r.index() ) {
            return false;
        }
        return true;
    };

    auto edge_comparator = [ &identical_indices ] (
        TreeEdge const& edge_l,
        TreeEdge const& edge_r
    ) {
        if( identical_indices && edge_l.index() != edge_r.index() ) {
            return false;
        }
        return true;
    };

    return equal( lhs, rhs, node_comparator, edge_comparator );
}

bool identical_topology( std::vector<Tree> const& trees, bool identical_indices )
{
    // If all pairs of two adjacent trees have same the topology, all of them have.
    // Thus, we do not need a complete pairwise comparision.
    // In order to also accelarate via OpenMP, we need a flag instead of immediately
    // returning on finding a false. We cannot break in OpenMP, but we can skip (which is fast),
    // if we already know the result.

    bool result = true;
    #pragma omp parallel for
    for (size_t i = 1; i < trees.size(); i++) {
        if( ! result ) {
            continue;
        }

        if( ! identical_topology( trees[i-1], trees[i], identical_indices )) {
            result = false;
        }
    }
    return result;
}

// =================================================================================================
//     Element Ownership Checks
// =================================================================================================

bool belongs_to( Tree const& tree, TreeNode const& node )
{
    return node.index() < tree.node_count() && &tree.node_at( node.index() ) == &node;
}

bool belongs_to( TreeNode const& node, Tree const& tree )
{
    return node.index() < tree.node_count() && &tree.node_at( node.index() ) == &node;
}

bool belongs_to( Tree const& tree, TreeEdge const& edge )
{
    return edge.index() < tree.edge_count() && &tree.edge_at( edge.index() ) == &edge;
}

bool belongs_to( TreeEdge const& edge, Tree const& tree )
{
    return edge.index() < tree.edge_count() && &tree.edge_at( edge.index() ) == &edge;
}

bool belongs_to( Tree const& tree, TreeLink const& link )
{
    return link.index() < tree.link_count() && &tree.link_at( link.index() ) == &link;
}

bool belongs_to( TreeLink const& link, Tree const& tree )
{
    return link.index() < tree.link_count() && &tree.link_at( link.index() ) == &link;
}

bool belongs_to( Tree     const& tree, Subtree  const& subt )
{
    return belongs_to( subt.link(), tree );
}

bool belongs_to( Subtree  const& subt, Tree     const& tree )
{
    return belongs_to( subt.link(), tree );
}

TreeEdge* edge_between( TreeNode& lhs, TreeNode& rhs )
{
    // No need to check whether the two nodes belong to the same tree.
    // If they don't, this loop will simply exit without success, so that a nullptr is returend.
    for( auto it : node_links( lhs ) ) {
        if( &it.link().outer().node() == &rhs ) {
            return &it.link().edge();
        }
    }
    return nullptr;
}

TreeEdge const* edge_between( TreeNode const& lhs, TreeNode const& rhs )
{
    // No need to check whether the two nodes belong to the same tree.
    // If they don't, this loop will simply exit without success, so that a nullptr is returend.
    for( auto it : node_links( lhs ) ) {
        if( &it.link().outer().node() == &rhs ) {
            return &it.link().edge();
        }
    }
    return nullptr;
}

// =================================================================================================
//     Output Printing
// =================================================================================================

std::string print_info( Tree const& tree )
{
    return "<genesis::tree::Tree"
           " node_count=" + std::to_string( tree.node_count() ) +
           " edge_count=" + std::to_string( tree.edge_count() ) +
           " link_count=" + std::to_string( tree.link_count() ) +
           ">";
}

std::string print_info( TreeEdge const& edge )
{
    return "<genesis::tree::TreeEdge"
           " index=" + std::to_string( edge.index() ) +
           " has_data=" + ( edge.has_data() ? "true" : "false" ) +
           ">";
}

std::string print_info( TreeLink const& link )
{
    return "<genesis::tree::TreeLink"
           " index=" + std::to_string( link.index() ) +
           // " has_data=" + ( edge.has_data() ? "true" : "false" ) +
           ">";
}

std::string print_info( TreeNode const& node )
{
    return "<genesis::tree::TreeNode"
           " index=" + std::to_string( node.index() ) +
           " has_data=" + ( node.has_data() ? "true" : "false" ) +
           ">";
}

std::string print_gist( Tree const& tree, long items )
{
    auto pc = PrinterCompact();
    pc.limit( items );
    return pc.print( tree );
}

std::ostream& operator << ( std::ostream& out, Tree const& tree )
{
    if( utils::Options::get().print_object_infos() ) {
        out << print_info( tree );
    }
    out << print_gist( tree, utils::Options::get().print_object_gists() );
    return out;
}

std::ostream& operator << ( std::ostream& out, TreeEdge const& edge )
{
    if( utils::Options::get().print_object_infos() ) {
        out << print_info( edge );
    }
    return out;
}

std::ostream& operator << ( std::ostream& out, TreeLink const& link )
{
    if( utils::Options::get().print_object_infos() ) {
        out << print_info( link );
    }
    return out;
}

std::ostream& operator << ( std::ostream& out, TreeNode const& node )
{
    if( utils::Options::get().print_object_infos() ) {
        out << print_info( node );
    }
    return out;
}

// =================================================================================================
//     Validate
// =================================================================================================

bool validate_topology( Tree const& tree )
{
    // -----------------------------------------------------
    //     Empty Tree
    // -----------------------------------------------------

    // check that the member arrays are valid: if at least one of them is empty, the tree is not
    // fully initialized, so either it is a new tree without any data (all arrays empty, valid),
    // or some are empty, but others not (not valid).
    if (tree.links_.empty() || tree.nodes_.empty() || tree.edges_.empty()) {
        bool empty = tree.links_.empty() && tree.nodes_.empty() && tree.edges_.empty();
        if( !empty ) {
            LOG_INFO << "Tree is not empty, but one of its data members is.";
        }
        return empty;
    }

    // -----------------------------------------------------
    //     Links
    // -----------------------------------------------------

    // Check Links.
    std::vector<size_t> links_to_edges(tree.edges_.size(), 0);
    std::vector<size_t> links_to_nodes(tree.nodes_.size(), 0);
    for (size_t i = 0; i < tree.links_.size(); ++i) {
        // Check indices.
        if( i != tree.link_at(i).index() ) {
            LOG_INFO << "Link at index " << i << " has wrong index ("
                     << tree.link_at(i).index() << ").";
            return false;
        }

        // Check next cycle and node.
        auto nl = tree.links_[i].get();
        do {
            if( &nl->node() != &tree.links_[i]->node() ) {
                LOG_INFO << "Link at index " << nl->index() << " points to wrong node.";
                return false;
            }
            nl = &nl->next();
        } while(nl != tree.links_[i].get());
        ++links_to_nodes[tree.links_[i]->node().index()];

        // Check outer cycle.
        if( &tree.links_[i]->outer().outer() != tree.links_[i].get() ) {
            LOG_INFO << "Link at index " << i << " has wrong outer link.";
            return false;
        }

        // Check edge.
        auto edge = &tree.links_[i]->edge();
        if(
            &edge->primary_link()   != tree.links_[i].get() &&
            &edge->secondary_link() != tree.links_[i].get()
        ) {
            LOG_INFO << "Link at index " << i << " has wrong edge pointer.";
            return false;
        }
        ++links_to_edges[tree.links_[i]->edge().index()];
    }

    // Check if all edges have been hit twice.
    for (size_t i = 0; i < tree.edges_.size(); ++i) {
        if (links_to_edges[i] != 2) {
            LOG_INFO << "Edge at index " << i << " is not visited twice but " << links_to_edges[i]
                     << " times when traversing the links.";
            return false;
        }
    }

    // Check if all nodes have been hit as many times as their degree is.
    for (size_t i = 0; i < tree.nodes_.size(); ++i) {
        if (links_to_nodes[i] != degree( *tree.nodes_[i] ) ) {
            LOG_INFO << "Node at index " << i << " is not visited its degree ("
                     << degree( *tree.nodes_[i] )
                     << ") times, but " << links_to_nodes[i] << " times when "
                     << "traversing the links.";
            return false;
        }
    }

    // -----------------------------------------------------
    //     Nodes
    // -----------------------------------------------------

    // Check Nodes.
    for (size_t i = 0; i < tree.nodes_.size(); ++i) {
        // Check indices.
        if( i != tree.node_at(i).index() ) {
            LOG_INFO << "Node at index " << i << " has wrong index ("
                     << tree.node_at(i).index() << ").";
            return false;
        }

        // Check link.
        if( &tree.nodes_[i]->link().node() != tree.nodes_[i].get() ) {
            LOG_INFO << "Node at index " << i << " has wrong link.";
            return false;
        }

        // If a node claims to be the root, it better be the root.
        if( is_root( tree.node_at(i) ) && i != tree.root_node().index() ) {
            LOG_INFO << "Node at index " << i << " has is_root(), but it is not tree.root_node().";
            return false;
        }

        // Except for the root, all nodes must have a primary link that is the secondary link
        // of its edge.
        if( ! is_root( tree.node_at(i) ) &&
            &tree.node_at(i).primary_link() != &tree.node_at(i).primary_link().edge().secondary_link()
        ) {
            LOG_INFO << "Node at " << i << " (not the root node) has a primary link which is not "
                     << "the secondary link of its edge.";
            return false;
        }

        // All (primary) links must point towards the root.
        size_t root_c = 0;
        auto root_l = &( tree.node_at(i).primary_link() );
        while( root_l != &( tree.root_node().link() )) {
            root_l = &( root_l->outer().node().primary_link() );
            ++root_c;

            // We need to avoid infinite loops in case of wrong trees, so that this function
            // correctly termines. We cannot need more hops than nodes in the tree!
            if( root_c > tree.node_count() ) {
                LOG_INFO << "Node at " << i << " and the nodes towards the root contain "
                         << "a primary link which is not pointing towards the root.";
                return false;
            }
        }
    }

    // -----------------------------------------------------
    //     Edges
    // -----------------------------------------------------

    // Check Edges.
    for (size_t i = 0; i < tree.edges_.size(); ++i) {
        // Check indices.
        if( i != tree.edge_at(i).index() ) {
            LOG_INFO << "Edge at index " << i << " has wrong index ("
                     << tree.edge_at(i).index() << ").";
            return false;
        }

        // Check links.
        if( &tree.edges_[i]->primary_link().edge() != tree.edges_[i].get() ) {
            LOG_INFO << "Edge at index " << i << " has wrong primary link.";
            return false;
        }
        if( &tree.edges_[i]->secondary_link().edge() != tree.edges_[i].get() ) {
            LOG_INFO << "Edge at index " << i << " has wrong secondary link.";
            return false;
        }

        // Check outer links.
        if( &tree.edges_[i]->primary_link().outer() != &tree.edges_[i]->secondary_link() ) {
            LOG_INFO << "Edge at index " << i << " has a primary link that does not "
                     << "connect to its secondary link.";
            return false;
        }
        if( &tree.edges_[i]->secondary_link().outer() != &tree.edges_[i]->primary_link() ) {
            LOG_INFO << "Edge at index " << i << " has a secondary link that does not "
            << "connect to its primary link.";
            return false;
        }

        // Check primary node, except for root.
        if( ! is_root( tree.edge_at(i).primary_node() ) &&
            &tree.edge_at(i).primary_node().primary_link() == &tree.edge_at(i).primary_link()
        ) {
            LOG_INFO << "Edge at " << i << " has a primary node that does not "
                     << "point towards the root.";
            return false;
        }

        // Check secondary node.
        if( &tree.edge_at(i).secondary_node().primary_link() != &tree.edge_at(i).secondary_link() ) {
            LOG_INFO << "Edge at " << i << " has a secondary node that does not "
                     << "point towards the root.";
            return false;
        }

        // All primary links must point towards the root.
        size_t root_c = 0;
        auto root_l = &( tree.edge_at(i).primary_link() );
        while( root_l != &( tree.root_node().link() )) {
            root_l = &( root_l->node().primary_link().edge().primary_link() );
            ++root_c;

            // We need to avoid infinite loops in case of wrong trees, so that this function
            // correctly termines. We cannot need more hops than nodes in the tree!
            if( root_c > tree.node_count() ) {
                LOG_INFO << "Edge at " << i << " and the nodes towards the root contain "
                         << "a primary link which is not pointing towards the root.";
                return false;
            }
        }
    }

    // -----------------------------------------------------
    //     Eulertour
    // -----------------------------------------------------

    // If we are here, all three arrays (links, nodes, edges) contain data, so we can start a full
    // traversal along all links.

    // Count, how many times the elements are hit while traversing.
    std::vector<size_t> it_links(tree.links_.size(), 0);
    std::vector<size_t> it_edges(tree.edges_.size(), 0);
    std::vector<size_t> it_nodes(tree.nodes_.size(), 0);

    // Do the traversal. We do not use the iterator here, to keep it simple when testing.
    // (We want to validate the tree here, not the iterator.)
    auto link = tree.links_.front().get();
    do {
        ++it_links[ link->index() ];
        ++it_edges[ link->edge().index() ];
        ++it_nodes[ link->node().index() ];
        link = &link->next().outer();

        // Check if we have a loop. Baaad.
        if( it_links[ link->index() ] > 1 ) {
            LOG_INFO << "Loop or other kind of wrong link chain in Tree.";
            return false;
        }
    } while (link != tree.links_.front().get());

    // Check if all links have been hit once.
    for (size_t i = 0; i < tree.links_.size(); i++) {
        if (it_links[i] != 1) {
            LOG_INFO << "Link at index " << i << " is not visited 1 but " << it_links[i]
                     << " times when iterating the tree.";
            return false;
        }
    }

    // Check if all edges have been hit twice.
    for (size_t i = 0; i < tree.edges_.size(); ++i) {
        if (it_edges[i] != 2) {
            LOG_INFO << "Edge at index " << i << " is not visited 2 but " << it_edges[i]
                     << " times when iterating the tree.";
            return false;
        }
    }

    // Check if all nodes have been hit as many times as their degree is.
    for (size_t i = 0; i < tree.nodes_.size(); ++i) {
        if (it_nodes[i] != degree( *tree.nodes_[i] ) ) {
            LOG_INFO << "Node at index " << i << " is not visited "
                     << degree( *tree.nodes_[i] )
                     << " times, but " << it_nodes[i] << " times when iterating the "
                     << "tree.";
            return false;
        }
    }

    // -----------------------------------------------------
    //     Root
    // -----------------------------------------------------

    // All edges of the root node need to have this node as their primary node.
    auto rl = &( tree.root_link().next());
    while( rl != &( tree.root_link()) ) {
        if( &( rl->edge().primary_link() ) != rl ) {
            LOG_INFO << "Root node of the tree is not root in the topology.";
            return false;
        }
        rl = &( rl->next() );
    }

    // Check root link and node.
    if( &tree.root_link() != &tree.root_link().node().primary_link() ) {
        LOG_INFO << "Tree root link is not the primary link of its node.";
        return false;
    }

    // Further check the root.
    if( ! is_root( tree.root_node() ) ) {
        LOG_INFO << "Root node is not true in is_root().";
        return false;
    }

    return true;
}

} // namespace tree
} // namespace genesis
