/**
 * @brief Implementation of basic tree functions.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <sstream>

#include "tree/distances.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Construction and Destruction
// =================================================================================================

/**
 * @brief Copy constructor.
 *
 * This function creates all links, nodes and edges new, and shapes them so that the final
 * Tree has the same topology as the input Tree.
 *
 * The data belonging to the edges and nodes is copied using the copy assignment of the respective
 * template parameter classes EdgeDataType and NodeDataType. As this data might contain pointers and
 * other structures that need a deep copy, it is the responsibility of those data classes to make
 * sure its own data is copied correctly.
 *
 * In case of pointers, the class that is using the tree might need to do further copying after
 * calling this copy constructor. See Placements for an example.
 *
 * TODO Idea for a nice feature (not yet implemented):
 * The advantage of copying the topology only is that we are able to make this function completely
 * independend of the data, hence the `other` Tree does not need to share the same data types.
 * Some potential function declarations can be found in the header file tree.hpp.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>::Tree (const Tree<NDT, EDT>& other)
{
    // preparation.
    clear();
    links_.resize(other.links_.size());
    nodes_.resize(other.nodes_.size());
    edges_.resize(other.edges_.size());

    // create all objects. we need two loops per array, because the pointers have to exist
    // in order to be used with each other.
    for (size_t i = 0; i < links_.size(); ++i) {
        links_[i] = make_unique<LinkType>();
        // links_[i]->data = other.links_[i]->data;
    }
    for (size_t i = 0; i < nodes_.size(); ++i) {
        nodes_[i] = make_unique<NodeType>();
        nodes_[i]->data = other.nodes_[i]->data;
    }
    for (size_t i = 0; i < edges_.size(); ++i) {
        edges_[i] = make_unique<EdgeType>();
        edges_[i]->data = other.edges_[i]->data;
    }

    // set all pointers for the topology in a second round of loops.
    for (size_t i = 0; i < links_.size(); ++i) {
        const auto& olink = other.links_[i];
        assert(olink->index_ == i);

        links_[i]->index_  = i;
        links_[i]->next_   = links_[olink->next_->index_].get();
        links_[i]->outer_  = links_[olink->outer_->index_].get();
        links_[i]->node_   = nodes_[olink->node_->index_].get();
        links_[i]->edge_   = edges_[olink->edge_->index_].get();
    }
    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& onode = other.nodes_[i];
        assert(onode->index_ == i);

        nodes_[i]->index_  = i;
        nodes_[i]->link_   = links_[onode->link_->index_].get();
    }
    for (size_t i = 0; i < edges_.size(); ++i) {
        const auto& oedge = other.edges_[i];
        assert(oedge->index_ == i);

        edges_[i]->index_  = i;
        edges_[i]->link_p_ = links_[oedge->link_p_->index_].get();
        edges_[i]->link_s_ = links_[oedge->link_s_->index_].get();
    }
}

/**
 * @brief Assignment operator. Copies the topology, but not the data of a tree.
 *
 * See Tree copy constructor for more information.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>& Tree<NDT, EDT>::operator = (const Tree<NDT, EDT>& other)
{
    // check for self-assignment. we want this explicitly, in order to avoid unnecessary copies of
    // the tree, which would mean loosing the data in process.
    if (&other == this) {
        return *this;
    }

    // the Tree tmp is a copy of the right hand side object (automatically created using the
    // copy constructor). we can thus simply swap the arrays, and upon leaving the function,
    // tmp is automatically destroyed, so that its arrays are cleared and the data freed.
    Tree<NDT, EDT> tmp(other);
    std::swap(links_, tmp.links_);
    std::swap(nodes_, tmp.nodes_);
    std::swap(edges_, tmp.edges_);
    return *this;
}

/**
 * @brief Deletes all data of the tree, including all links, nodes and edges.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::clear()
{
    // std::vector<std::unique_ptr<LinkType>>().swap(links_);
    // std::vector<std::unique_ptr<NodeType>>().swap(nodes_);
    // std::vector<std::unique_ptr<EdgeType>>().swap(edges_);

    links_.clear();
    nodes_.clear();
    edges_.clear();
}

/**
 * @brief Swap.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::swap (Tree<NDT, EDT>& other)
{
    using std::swap;
    swap(links_, other.links_);
    swap(nodes_, other.nodes_);
    swap(edges_, other.edges_);
}

/**
 * @brief Imports all elements of a tree.
 *
 * This function overwrites the topology and data of this tree with a given set of links, nodes
 * and edges. Use with care! No checks are done concerning the validity of the passed input.
 *
 * Caveat: Only the pointers to the tree elements are copied, not the elements themselves. Thus,
 * this function is not intended for creating a deep copy. It merely is a fast way to pass pointers
 * to tree elements.
 *
 * Therefore, the main usage of this function is to get a tree from different TreeProcessor objects
 * for reading trees from files.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::import_content (
    LinkContainer& links,
    NodeContainer& nodes,
    EdgeContainer& edges
) {
    using std::swap;

    clear();
    swap(links_, links);
    swap(nodes_, nodes);
    swap(edges_, edges);
}

/**
 * @brief Exports all elements of a tree.
 *
 * Caveat: Only the pointers to the tree elements are copied, not the elements themselves. Thus,
 * this function is not intended for creating a deep copy. It merely is a fast way to pass pointers
 * to tree elements.
 */
template <class NDT, class EDT>
void Tree<NDT, EDT>::export_content (
    LinkContainer& links,
    NodeContainer& nodes,
    EdgeContainer& edges
) {
    links = links_;
    nodes = nodes_;
    edges = edges_;
}

// =================================================================================================
//     Accessors
// =================================================================================================

template <class NDT, class EDT>
bool Tree<NDT, EDT>::empty() const
{
    return links_.empty() && nodes_.empty() && edges_.empty();
}

// =================================================================================================
//     Dump and Debug Functions
// =================================================================================================

} // namespace tree
} // namespace genesis
