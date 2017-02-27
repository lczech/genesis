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
 * @brief Implementation of Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/function/operators.hpp"

#include "genesis/tree/iterator/node_links.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/utils/core/logging.hpp"

#include <ostream>

namespace genesis {
namespace tree {

// =================================================================================================
//     Conversion
// =================================================================================================

/**
 * @brief Create a tree with the same topology as the source tree, while converting its data.
 *
 * This function takes the given source Tree (possibly with different data types at the nodes and
 * edges), and copies its topology (i.e., all links, nodes and edges, and their structure) to the
 * newly created result tree.
 *
 * The data types are then converted using the two provided functions for the node data type and
 * edge data type, respectively. If a node or an edge does not have data (i.e., the data pointer
 * is a nullptr), the converter functions are not called, but the data of the new tree at that
 * node or edge is also set to a nullptr.
 */
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
//     Equality
// =================================================================================================

/**
 * @brief Compares two trees for equality given binary comparator functionals for their nodes and
 * edges.
 *
 * This function does a preorder traversal of both trees in parallel and calls the comparator
 * functionals for each position of the iterator. It returns true iff the comparators are true for
 * every position.
 *
 * The comparator functionals can be either function pointers, function objects, or lambda
 * expressions.
 *
 * As the traversal is done in parallel, the trees are also checked for equal topology:
 * their elements (links, nodes, edges) have to be equal in size and the rank of each node during
 * the traversal has to be identical in both trees. Those assumptions are made because two trees
 * that do not have identical topology are never considered equal.
 */
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
        if (it_l.node().rank() != it_r.node().rank()   ||
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

/**
 * @brief Returns true iff both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of leaves and branches, they might
 * still be not identical (with respect to this function) when the branches appear in a different
 * order or when the root sits at a different node.
 */
bool identical_topology( Tree const& lhs, Tree const& rhs)
{
    auto node_comparator = [] (
        TreeNode const& node_l,
        TreeNode const& node_r
    ) {
        (void) node_l;
        (void) node_r;
        return true;
    };

    auto edge_comparator = [] (
        TreeEdge const& edge_l,
        TreeEdge const& edge_r
    ) {
        (void) edge_l;
        (void) edge_r;
        return true;
    };

    return equal( lhs, rhs, node_comparator, edge_comparator );
}

/**
 * @brief Return whether the TreeNode belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( Tree const& tree, TreeNode const& node )
{
    return node.index() < tree.node_count() && &tree.node_at( node.index() ) == &node;
}

/**
 * @brief Return whether the TreeNode belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( TreeNode const& node, Tree const& tree )
{
    return node.index() < tree.node_count() && &tree.node_at( node.index() ) == &node;
}

/**
 * @brief Return whether the TreeEdge belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( Tree const& tree, TreeEdge const& edge )
{
    return edge.index() < tree.edge_count() && &tree.edge_at( edge.index() ) == &edge;
}

/**
 * @brief Return whether the TreeEdge belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( TreeEdge const& edge, Tree const& tree )
{
    return edge.index() < tree.edge_count() && &tree.edge_at( edge.index() ) == &edge;
}

/**
 * @brief Return whether the TreeLink belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( Tree const& tree, TreeLink const& link )
{
    return link.index() < tree.link_count() && &tree.link_at( link.index() ) == &link;
}

/**
 * @brief Return whether the TreeLink belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( TreeLink const& link, Tree const& tree )
{
    return link.index() < tree.link_count() && &tree.link_at( link.index() ) == &link;
}

/**
 * @brief Return the TreeEdge between two TreeNode&s, if they are neighbours, or `nullptr` otherwise.
 */
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

/**
 * @brief Return the TreeEdge between two TreeNode&s, if they are neighbours, or `nullptr` otherwise.
 */
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
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Tree const& tree )
{
    out << "Node Count: " << tree.node_count() << "\n";
    out << "Edge Count: " << tree.edge_count() << "\n";
    out << "Link Count: " << tree.link_count() << "\n";
    return out;
}

// =================================================================================================
//     Validate
// =================================================================================================

/**
 * @brief Validate that all internal pointers of the Tree elements (TreeLink%s, TreeNode%s,
 * TreeEdge%s) to each other are correct and that some other invariants are met.
 *
 * This check is a bit pedantic, but better safe than sorry.
 */
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

    // Check if all nodes have been hit as many times as their rank is.
    for (size_t i = 0; i < tree.nodes_.size(); ++i) {
        if (links_to_nodes[i] != tree.nodes_[i]->rank() + 1) {
            LOG_INFO << "Node at index " << i << " is not visited its rank + 1 ("
                     << tree.nodes_[i]->rank() << " + 1 = " << tree.nodes_[i]->rank() + 1
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
        if( tree.node_at(i).is_root() && i != tree.root_node().index() ) {
            LOG_INFO << "Node at index " << i << " has is_root(), but it is not tree.root_node().";
            return false;
        }

        // Except for the root, all nodes must have a primary link that is the secondary link
        // of its edge.
        if( ! tree.node_at(i).is_root() &&
            &tree.node_at(i).primary_link() != &tree.node_at(i).primary_link().edge().secondary_link()
        ) {
            LOG_INFO << "Node at " << i << " (not the root node) has a primary link which is not "
                     << "the secondary link of its edge.";
            return false;
        }
    }

    // Further check the root.
    if( ! tree.root_node().is_root() ) {
        LOG_INFO << "Root node does not have is_root().";
        return false;
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
        if( ! tree.edge_at(i).primary_node().is_root() &&
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

    // Check if all nodes have been hit as many times as their rank is.
    for (size_t i = 0; i < tree.nodes_.size(); ++i) {
        if (it_nodes[i] != tree.nodes_[i]->rank() + 1) {
            LOG_INFO << "Node at index " << i << " is not visited "
                     << tree.nodes_[i]->rank() << " + 1 = " << (tree.nodes_[i]->rank() + 1)
                     << " times, but " << it_nodes[i] << " times when iterating the "
                     << "tree.";
            return false;
        }
    }

    return true;
}

} // namespace tree
} // namespace genesis
