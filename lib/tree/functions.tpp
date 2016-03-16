/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/core/logging.hpp"

#include <algorithm>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Count Properties
// =================================================================================================

/**
 * @brief Return the highest rank of the nodes of the Tree.
 */
template <class Tree>
int max_rank( Tree const& tree )
{
    int max = -1;
    for (size_t i = 0; i < tree.node_count(); ++i) {
        int rank = tree.node_at(i)->rank();
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
template <class Tree>
bool is_bifurcating( Tree const& tree )
{
    return max_rank( tree ) == 2;
}

/**
 * @brief Count the number of leaf nodes of a Tree.
 */
template <class Tree>
size_t leaf_nodes_count( Tree const& tree )
{
    size_t sum = 0;
    for (size_t i = 0; i < tree.node_count(); ++i) {
        auto const& n = tree.node_at(i);
        if (n->is_leaf()) {
            ++sum;
        }
    }
    return sum;
}

/**
 * @brief Count the number of inner nodes.
 */
template <class Tree>
size_t inner_nodes_count( Tree const& tree )
{
    return tree.node_count() - leaf_nodes_count( tree );
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

    if (tree.links_.front()->node_ != tree.nodes_.front().get()) {
        LOG_INFO << "The first link does not correspond to the first node.";
        return false;
    }

    if (tree.links_.front()->index() != 0 || tree.links_.front()->node()->index() != 0) {
        LOG_INFO << "Root does not have index 0.";
        return false;
    }

    // Check Links.
    std::vector<size_t> links_to_edges(tree.edges_.size(), 0);
    std::vector<size_t> links_to_nodes(tree.nodes_.size(), 0);
    for (size_t i = 0; i < tree.links_.size(); ++i) {
        // Check indices.
        if (i != tree.links_[i]->index_) {
            LOG_INFO << "Link at index " << i << " has wrong index (" << tree.links_[i]->index_ << ").";
            return false;
        }

        // Check next cycle and node.
        auto nl = tree.links_[i].get();
        do {
            if (nl->node() != tree.links_[i]->node()) {
                LOG_INFO << "Link at index " << nl->index_ << " points to wrong node.";
                return false;
            }
            nl = nl->next();
        } while(nl != tree.links_[i].get());
        ++links_to_nodes[tree.links_[i]->node()->index()];

        // Check outer cycle.
        if (tree.links_[i]->outer()->outer() != tree.links_[i].get()) {
            LOG_INFO << "Link at index " << i << " has wrong outer link.";
            return false;
        }

        // Check edge.
        auto edge = tree.links_[i]->edge();
        if (edge->primary_link() != tree.links_[i].get() && edge->secondary_link() != tree.links_[i].get()) {
            LOG_INFO << "Link at index " << i << " has wrong edge pointer.";
            return false;
        }
        ++links_to_edges[tree.links_[i]->edge()->index()];
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
        if (i != tree.nodes_[i]->index_) {
            LOG_INFO << "Node at index " << i << " has wrong index (" << tree.nodes_[i]->index_ << ").";
            return false;
        }

        // Check link.
        if (tree.nodes_[i]->link()->node() != tree.nodes_[i].get()) {
            LOG_INFO << "Node at index " << i << " has wrong link.";
            return false;
        }
    }

    // Check Edges.
    for (size_t i = 0; i < tree.edges_.size(); ++i) {
        // Check indices.
        if (i != tree.edges_[i]->index_) {
            LOG_INFO << "Edge at index " << i << " has wrong index (" << tree.edges_[i]->index_ << ").";
            return false;
        }

        // Check links.
        if (tree.edges_[i]->primary_link()->edge() != tree.edges_[i].get()) {
            LOG_INFO << "Edge at index " << i << " has wrong primary link.";
            return false;
        }
        if (tree.edges_[i]->secondary_link()->edge() != tree.edges_[i].get()) {
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
        ++it_links[link->index()];
        ++it_edges[link->edge()->index()];
        ++it_nodes[link->node()->index()];
        link = link->next_->outer_;
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
