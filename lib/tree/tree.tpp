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
 * @brief Implementation of Tree class template.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>

// #include "tree/distances.hpp"
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
 * This function internally uses convert_from(), with trivial conversion by copy assigning the data.
 * See there for converting trees with different template parameter types to each other.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>::Tree( const Tree<NDT, EDT>& other )
{
    convert_from(
        other,
        [] ( NodeDataType const& node_data ) { return node_data; },
        [] ( EdgeDataType const& edge_data ) { return edge_data; }
    );
}

/**
 * @brief Assignment operator.
 *
 * See Tree copy constructor for more information.
 */
template <class NDT, class EDT>
Tree<NDT, EDT>& Tree<NDT, EDT>::operator = ( Tree<NDT, EDT> const& other )
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
 * Therefore, the main usage of this function is to get a tree from different Tree Reader classes
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

/**
 * @brief Copy the topology of another tree to this tree, and convert its data.
 *
 * The current content of this tree is cleared. Then, this function takes the given Tree
 * (possibly with different template parameters, i.e., a different tree type) and copies its
 * topology (i.e., all links, nodes and edges, and their structure) to this tree.
 * Furthermore, the data types are converted using the two provided functions for the node data
 * type and edge data type, respectively.
 *
 * This function is also internally used by the copy constructor, with trivial conversion by
 * copy assigning the data.
 */
template <class NDT, class EDT>
template <class OtherTreeType>
void Tree<NDT, EDT>::convert_from(
    OtherTreeType const& other_tree,
    std::function<typename Tree<NDT, EDT>::NodeDataType ( typename OtherTreeType::NodeDataType const& node_data )> node_data_converter,
    std::function<typename Tree<NDT, EDT>::EdgeDataType ( typename OtherTreeType::EdgeDataType const& edge_data )> edge_data_converter
) {
    // Preparation.
    clear();
    links_.resize(other_tree.link_count());
    nodes_.resize(other_tree.node_count());
    edges_.resize(other_tree.edge_count());

    // Create all objects. We need two loops per array, because the pointers have to exist
    // in order to be linked to each other.
    for (size_t i = 0; i < links_.size(); ++i) {
        links_[i] = make_unique<LinkType>();
        // links_[i]->data = other_tree.links_[i]->data;
    }
    for (size_t i = 0; i < nodes_.size(); ++i) {
        nodes_[i] = make_unique<NodeType>();
        nodes_[i]->data = node_data_converter( other_tree.node_at(i).data );
    }
    for (size_t i = 0; i < edges_.size(); ++i) {
        edges_[i] = make_unique<EdgeType>();
        edges_[i]->data = edge_data_converter( other_tree.edge_at(i).data );
    }

    // Set all pointers for the topology in a second round of loops.
    for (size_t i = 0; i < links_.size(); ++i) {
        const auto& olink = other_tree.link_at(i);
        assert(olink.index() == i);

        links_[i]->reset_index( i );
        links_[i]->reset_next(  links_[olink.next().index()].get() );
        links_[i]->reset_outer( links_[olink.outer().index()].get() );
        links_[i]->reset_node(  nodes_[olink.node().index()].get() );
        links_[i]->reset_edge(  edges_[olink.edge().index()].get() );
    }
    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& onode = other_tree.node_at(i);
        assert(onode.index() == i);

        nodes_[i]->reset_index( i );
        nodes_[i]->reset_primary_link( links_[onode.link().index()].get() );
    }
    for (size_t i = 0; i < edges_.size(); ++i) {
        const auto& oedge = other_tree.edge_at(i);
        assert(oedge.index() == i);

        edges_[i]->reset_index( i );
        edges_[i]->reset_primary_link(   links_[oedge.primary_link().index()].get()   );
        edges_[i]->reset_secondary_link( links_[oedge.secondary_link().index()].get() );
    }
}

/**
 * @brief Deletes all data of the tree, including all links, nodes and edges.
 *
 * This functions results in an empty tree.
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

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return whether the Tree is empty (i.e., has no nodes, edges and links).
 */
template <class NDT, class EDT>
bool Tree<NDT, EDT>::empty() const
{
    return links_.empty() && nodes_.empty() && edges_.empty();
}

/**
 * @brief Return the TreeLink at the current root of the Tree.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::LinkType& Tree<NDT, EDT>::root_link()
{
    return *links_.front().get();
}

/**
 * @brief Return the TreeLink at the current root of the Tree.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::LinkType const& Tree<NDT, EDT>::root_link() const
{
    return *links_.front().get();
}

/**
 * @brief Return the TreeNode at the current root of the Tree.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeType& Tree<NDT, EDT>::root_node()
{
    return links_.front()->node();
}

/**
 * @brief Return the TreeNode at the current root of the Tree.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeType const& Tree<NDT, EDT>::root_node() const
{
    return links_.front()->node();
}

/**
 * @brief Return the TreeLink at a certain index.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::LinkType& Tree<NDT, EDT>::link_at(size_t index)
{
    return *links_[index].get();
}

/**
 * @brief Return the TreeLink at a certain index.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::LinkType const& Tree<NDT, EDT>::link_at(size_t index) const
{
    return *links_[index].get();
}

/**
 * @brief Return the TreeNode at a certain index.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeType& Tree<NDT, EDT>::node_at(size_t index)
{
    return *nodes_[index].get();
}

/**
 * @brief Return the TreeNode at a certain index.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::NodeType const& Tree<NDT, EDT>::node_at(size_t index) const
{
    return *nodes_[index].get();
}

/**
 * @brief Return the TreeEdge at a certain index.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::EdgeType& Tree<NDT, EDT>::edge_at(size_t index)
{
    return *edges_[index].get();
}

/**
 * @brief Return the TreeEdge at a certain index.
 */
template <class NDT, class EDT>
typename Tree<NDT, EDT>::EdgeType const& Tree<NDT, EDT>::edge_at(size_t index) const
{
    return *edges_[index].get();
}

/**
 * @brief Return the number of TreeLink%s of the Tree.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::link_count() const
{
    return links_.size();
}

/**
 * @brief Return the number of TreeNode%s of the Tree.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::node_count() const
{
    return nodes_.size();
}

/**
 * @brief Return the number of TreeEdge%s of the Tree.
 */
template <class NDT, class EDT>
size_t Tree<NDT, EDT>::edge_count() const
{
    return edges_.size();
}

// =================================================================================================
//     Iterators
// =================================================================================================

// -------------------------------------------------------------------------
//     Links
// -------------------------------------------------------------------------

template <class NDT, class EDT>
typename Tree<NDT, EDT>::IteratorLinks Tree<NDT, EDT>::begin_links()
{
    return links_.begin();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::IteratorLinks Tree<NDT, EDT>::end_links()
{
    return links_.end();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::ConstIteratorLinks Tree<NDT, EDT>::begin_links() const
{
    return links_.cbegin();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::ConstIteratorLinks Tree<NDT, EDT>::end_links() const
{
    return links_.cend();
}

template <class NDT, class EDT>
utils::Range<typename Tree<NDT, EDT>::IteratorLinks> Tree<NDT, EDT>::links()
{
    return { links_ };
}

template <class NDT, class EDT>
utils::Range<typename Tree<NDT, EDT>::ConstIteratorLinks> Tree<NDT, EDT>::links() const
{
    return { links_ };
}

// -------------------------------------------------------------------------
//     Nodes
// -------------------------------------------------------------------------

template <class NDT, class EDT>
typename Tree<NDT, EDT>::IteratorNodes Tree<NDT, EDT>::begin_nodes()
{
    return nodes_.begin();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::IteratorNodes Tree<NDT, EDT>::end_nodes()
{
    return nodes_.end();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::ConstIteratorNodes Tree<NDT, EDT>::begin_nodes() const
{
    return nodes_.cbegin();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::ConstIteratorNodes Tree<NDT, EDT>::end_nodes() const
{
    return nodes_.cend();
}

template <class NDT, class EDT>
utils::Range<typename Tree<NDT, EDT>::IteratorNodes> Tree<NDT, EDT>::nodes()
{
    return { nodes_ };
}

template <class NDT, class EDT>
utils::Range<typename Tree<NDT, EDT>::ConstIteratorNodes> Tree<NDT, EDT>::nodes() const
{
    return { nodes_ };
}

// -------------------------------------------------------------------------
//     Edges
// -------------------------------------------------------------------------

template <class NDT, class EDT>
typename Tree<NDT, EDT>::IteratorEdges Tree<NDT, EDT>::begin_edges()
{
    return edges_.begin();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::IteratorEdges Tree<NDT, EDT>::end_edges()
{
    return edges_.end();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::ConstIteratorEdges Tree<NDT, EDT>::begin_edges() const
{
    return edges_.cbegin();
}

template <class NDT, class EDT>
typename Tree<NDT, EDT>::ConstIteratorEdges Tree<NDT, EDT>::end_edges() const
{
    return edges_.cend();
}

template <class NDT, class EDT>
utils::Range<typename Tree<NDT, EDT>::IteratorEdges> Tree<NDT, EDT>::edges()
{
    return { edges_ };
}

template <class NDT, class EDT>
utils::Range<typename Tree<NDT, EDT>::ConstIteratorEdges> Tree<NDT, EDT>::edges() const
{
    return { edges_ };
}

} // namespace tree
} // namespace genesis
