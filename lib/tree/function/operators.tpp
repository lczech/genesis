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
 * @brief Implementation of Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/iterator/preorder.hpp"
#include "utils/core/logging.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Equality
// =================================================================================================

/**
 * @brief Compares two trees for equality given binary comparator functionals for their nodes and
 * edges.
 *
 * This function does a preorder traversal of both trees in parallel and calls the comparator
 * functionals for each position of the iterator. It returns true iff the comparator is true for
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
template <class TreeTypeL, class TreeTypeR>
bool equal(
    const TreeTypeL& lhs,
    const TreeTypeR& rhs,
    std::function<bool
        (const typename TreeTypeL::NodeType&, const typename TreeTypeR::NodeType&)
    > node_comparator,
    std::function<bool
        (const typename TreeTypeL::EdgeType&, const typename TreeTypeR::EdgeType&)
    > edge_comparator
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

/**
 * @brief Compares two trees for equality using the respective comparision operators for their nodes
 * and edges.
 *
 * This method is mainly a shortcut for the other equal function, where the comparator functionals
 * are instanciated using the default comparision operators of the tree's data.
 */
template <class TreeTypeL, class TreeTypeR>
bool equal(const TreeTypeL& lhs, const TreeTypeR& rhs)
{
    auto node_comparator = [] (
        const typename TreeTypeL::NodeType& node_l,
        const typename TreeTypeR::NodeType& node_r
    ) {
        return node_l == node_r;
    };

    auto edge_comparator = [] (
        const typename TreeTypeL::EdgeType& edge_l,
        const typename TreeTypeR::EdgeType& edge_r
    ) {
        return edge_l == edge_r;
    };

    return equal<TreeTypeL, TreeTypeR>(lhs, rhs, node_comparator, edge_comparator);
}

/**
 * @brief Returns true iff both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of leaves and branches, they might
 * still be not identical (with respect to this function) when the branches appear in a different
 * order or when the root sits at a different node.
 */
template <class TreeTypeL, class TreeTypeR>
bool identical_topology(const TreeTypeL& lhs, const TreeTypeR& rhs)
{
    auto node_comparator = [] (
        const typename TreeTypeL::NodeType& node_l,
        const typename TreeTypeR::NodeType& node_r
    ) {
        (void) node_l;
        (void) node_r;
        return true;
    };

    auto edge_comparator = [] (
        const typename TreeTypeL::EdgeType& edge_l,
        const typename TreeTypeR::EdgeType& edge_r
    ) {
        (void) edge_l;
        (void) edge_r;
        return true;
    };

    return equal<TreeTypeL, TreeTypeR>(lhs, rhs, node_comparator, edge_comparator);
}

// =================================================================================================
//     Validate
// =================================================================================================

// TODO do all node->link_ links point to the root? same for all edge->primary?

/**
 * @brief Validate that all pointers of the tree elements (links, nodes, edges) to each other
 * are correct and that some other invariants are met.
 *
 * This check is a bit pedantic, but better safe than sorry.
 */
// bool Tree<NDT, EDT>::validate() const
template <class TreeType>
bool validate( TreeType const& tree )
{
    // check that the member arrays are valid: if at least one of them is empty, the tree is not
    // fully initialized, so either it is a new tree without any data (all arrays empty, valid),
    // or some are empty, but others not (not valid).
    if (tree.links_.empty() || tree.nodes_.empty() || tree.edges_.empty()) {
        bool emp = tree.links_.empty() && tree.nodes_.empty() && tree.edges_.empty();
        if (emp) {
            LOG_INFO << "Tree is empty.";
        } else {
            LOG_INFO << "Tree is not empty, but one of its data members is.";
        }
        return emp;
    }

    if( &tree.links_.front()->node() != tree.nodes_.front().get() ) {
        LOG_INFO << "The first link does not correspond to the first node.";
        return false;
    }

    if (tree.links_.front()->index() != 0 || tree.links_.front()->node().index() != 0) {
        LOG_INFO << "Root does not have index 0.";
        return false;
    }

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
    }

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
    }

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
