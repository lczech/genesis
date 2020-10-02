#ifndef GENESIS_TREE_TREE_H_
#define GENESIS_TREE_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @brief Header of Tree class.
 *
 * For more information, see Tree class.
 *
 * This header also includes all the essential other tree elements,
 * as there is rarely a case where they are not needed anyway.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree/node.hpp"
#include "genesis/tree/tree/edge.hpp"
#include "genesis/tree/tree/link.hpp"
#include "genesis/tree/tree/node_data.hpp"
#include "genesis/tree/tree/edge_data.hpp"

#include "genesis/utils/containers/range.hpp"
#include "genesis/utils/containers/deref_iterator.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
bool validate_topology( Tree const& tree );

// =================================================================================================
//     Tree
// =================================================================================================

/**
 * @brief Class for representing phylogenetic trees.
 *
 * A tree in this implementation consists of three types of elements: Links, Nodes and Edges.
 * The topoloty of the tree is completely described by the links, while nodes and edges add the
 * capability to store data on the tree.
 *
 * Data in the Tree is not directly stored in the elements (Links, Nodes, Edges) of the Tree.
 * Instead, data belonging to nodes and edges can be stored in their data pointers. For this,
 * the data classes need to derive from BaseNodeData and BaseEdgeData, respectively.
 *
 * Additional information concerning the whole tree, as well as special algorithms working on the
 * tree and its data are also not direct members of the tree - for reasons of flexibility
 * and extensibility: Instead of adding more and more data and algorithms to the tree, we create
 * new classes that encapsulate a tree and all the other needed code to work with it.
 * See the Sample class for an example.
 *
 * Thus, the tree itself only contains the needed information to store and work with a topology.
 *
 * This class has several redundant ways of representing the same information. On the one hand, this
 * makes using a Tree easy, as one can chose the representaion of data that best fits a given task.
 * On the other hand, maintaining all those invariants when changing the tree topology is tedious.
 * Here is a (comprehensive?) list of invariants of this class:
 *
 *  *  The indices in all three arrays (nodes(), links() and edges()) have to match the index
 *     integers stored in those elements: `node_at(i).index() == i`.
 *  *  The link that is stored in a node has to be the one pointing towards the root.
 *  *  The primary link of an edge has to point towards the root, the secondary away from it.
 *
 * Those invariants are established when the Tree is constructed.
 */
class Tree
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

    /**
     * @brief Alias for the container type that is used to store TreeLink%s, TreeNode%s and
     * TreeEdge%s.
     */
    template< class T >
    using ContainerType      = std::vector< std::unique_ptr< T >>;

    /**
     * @brief Alias for the container type that is used to store TreeLink%s.
     */
    using LinkContainerType  = ContainerType< TreeLink >;

    /**
     * @brief Alias for the container type that is used to store TreeNode%s.
     */
    using NodeContainerType  = ContainerType< TreeNode >;

    /**
     * @brief Alias for the container type that is used to store TreeEdge%s.
     */
    using EdgeContainerType  = ContainerType< TreeEdge >;

    using      IteratorLinks = utils::DereferenceIterator< ContainerType<TreeLink>::iterator >;
    using ConstIteratorLinks = utils::DereferenceIterator< ContainerType<TreeLink>::const_iterator >;

    using      IteratorNodes = utils::DereferenceIterator< ContainerType<TreeNode>::iterator >;
    using ConstIteratorNodes = utils::DereferenceIterator< ContainerType<TreeNode>::const_iterator >;

    using      IteratorEdges = utils::DereferenceIterator< ContainerType<TreeEdge>::iterator >;
    using ConstIteratorEdges = utils::DereferenceIterator< ContainerType<TreeEdge>::const_iterator >;

    // -------------------------------------------------------------------------
    //     Construction and Rule of Five
    // -------------------------------------------------------------------------

    Tree()  = default;
    ~Tree() = default;

    /**
     * @brief Copy constructor.
     *
     * This function creates all links, nodes and edges new, and shapes them so that the final
     * Tree has the same topology as the input Tree.
     *
     * The data belonging to the edges and nodes is copied using the clone function of the respective
     * data classes for the nodes and edges. As this data might contain pointers and
     * other structures that need a deep copy, it is the responsibility of the clone function of those
     * data classes to make sure its own data is copied correctly.
     *
     * This function internally uses clone_topology() first, and then uses the clone functions of the
     * data for all nodes and edges.
     */
    Tree( Tree const& other );
    Tree( Tree&& ) = default;

    /**
     * @brief Assignment operator.
     *
     * See Tree copy constructor for more information.
     */
    Tree& operator= ( Tree const& other );
    Tree& operator= ( Tree&& ) = default;

    /**
     * @brief Return a Tree with the same topology, but without any data.
     *
     * All data pointers of the nodes and edges of the returned tree are `nullptr`.
     */
    Tree clone_topology() const;

    /**
     * @brief Swap.
     */
    void swap( Tree& other );

    /**
     * @brief Deletes all data of the tree, including all links, nodes and edges.
     *
     * This functions results in an empty tree.
     */
    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the Tree is empty (i.e., has no nodes, edges and links).
     */
    bool empty() const
    {
        return links_.empty() && nodes_.empty() && edges_.empty();
    }

    /**
     * @brief Return the TreeLink at a certain index.
     */
    TreeLink& link_at(size_t index)
    {
        assert( index < links_.size() );
        return *links_[ index ].get();
    }

    /**
     * @brief Return the TreeLink at a certain index.
     */
    TreeLink const& link_at(size_t index) const
    {
        assert( index < links_.size() );
        return *links_[ index ].get();
    }

    /**
     * @brief Return the TreeNode at a certain index.
     */
    TreeNode& node_at(size_t index)
    {
        assert( index < nodes_.size() );
        return *nodes_[ index ].get();
    }

    /**
     * @brief Return the TreeNode at a certain index.
     */
    TreeNode const& node_at(size_t index) const
    {
        assert( index < nodes_.size() );
        return *nodes_[ index ].get();
    }

    /**
     * @brief Return the TreeEdge at a certain index.
     */
    TreeEdge& edge_at(size_t index)
    {
        assert( index < edges_.size() );
        return *edges_[ index ].get();
    }

    /**
     * @brief Return the TreeEdge at a certain index.
     */
    TreeEdge const& edge_at(size_t index) const
    {
        assert( index < edges_.size() );
        return *edges_[ index ].get();
    }

    /**
     * @brief Return the number of TreeLink%s of the Tree.
     */
    size_t link_count() const
    {
        return links_.size();
    }

    /**
     * @brief Return the number of TreeNode%s of the Tree.
     */
    size_t node_count() const
    {
        return nodes_.size();
    }

    /**
     * @brief Return the number of TreeEdge%s of the Tree.
     */
    size_t edge_count() const
    {
        return edges_.size();
    }

    // -------------------------------------------------------------------------
    //     Root
    // -------------------------------------------------------------------------

    /**
     * @brief Return the TreeLink at the current root of the Tree.
     */
    TreeLink& root_link()
    {
        return *root_link_;
    }

    /**
     * @brief Return the TreeLink at the current root of the Tree.
     */
    TreeLink const& root_link() const
    {
        return *root_link_;
    }

    /**
     * @brief Return the TreeNode at the current root of the Tree.
     */
    TreeNode& root_node()
    {
        return root_link_->node();
    }

    /**
     * @brief Return the TreeNode at the current root of the Tree.
     */
    TreeNode const& root_node() const
    {
        return root_link_->node();
    }

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Reset the link that is considered to be the root of the Tree.
     *
     * This function is meant for tree manipulation functions. Use with care!
     *
     * Caveat: This function simply sets the link, but does not change any other properties of the tree.
     * Particularly the correct primary/secondary order of TreeEdge%s and primary links of TreeNode%s
     * needs to be maintained manually when using this function! Otherwise, we end up with an invalid
     * Tree that breaks its invariants!
     */
    Tree& reset_root_link( TreeLink* root_link );

    /**
     * @brief Get the container that stores all TreeLink%s of the Tree.
     *
     * This function gives direct access to the underlying container for the Links. It is meant to
     * be used by external functions that need to manupulate the Tree, e.g., for reading or re-rooting
     * the tree, or for adding elements to it. Use with care!
     */
    LinkContainerType& expose_link_container();

    /**
     * @brief Get the container that stores all TreeNode%s of the Tree.
     *
     * This function gives direct access to the underlying container for the Links. It is meant to
     * be used by external functions that need to manupulate the Tree, e.g., for reading or re-rooting
     * the tree, or for adding elements to it. Use with care!
     */
    NodeContainerType& expose_node_container();

    /**
     * @brief Get the container that stores all TreeEdge%s of the Tree.
     *
     * This function gives direct access to the underlying container for the Links. It is meant to
     * be used by external functions that need to manupulate the Tree, e.g., for reading or re-rooting
     * the tree, or for adding elements to it. Use with care!
     */
    EdgeContainerType& expose_edge_container();

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    // -----------------------------------------------------
    //     Links
    // -----------------------------------------------------

    IteratorLinks begin_links()
    {
        return links_.begin();
    }

    ConstIteratorLinks begin_links() const
    {
        return links_.cbegin();
    }

    IteratorLinks end_links()
    {
        return links_.end();
    }

    ConstIteratorLinks end_links() const
    {
        return links_.cend();
    }

    utils::Range<IteratorLinks> links()
    {
        return { links_ };
    }

    utils::Range<ConstIteratorLinks> links() const
    {
        return { links_ };
    }

    // -----------------------------------------------------
    //     Nodes
    // -----------------------------------------------------

    IteratorNodes begin_nodes()
    {
        return nodes_.begin();
    }

    ConstIteratorNodes begin_nodes() const
    {
        return nodes_.cbegin();
    }

    IteratorNodes end_nodes()
    {
        return nodes_.end();
    }

    ConstIteratorNodes end_nodes() const
    {
        return nodes_.cend();
    }

    utils::Range<IteratorNodes> nodes()
    {
        return { nodes_ };
    }

    utils::Range<ConstIteratorNodes> nodes() const
    {
        return { nodes_ };
    }

    // -----------------------------------------------------
    //     Edges
    // -----------------------------------------------------

    IteratorEdges begin_edges()
    {
        return edges_.begin();
    }

    ConstIteratorEdges begin_edges() const
    {
        return edges_.cbegin();
    }

    IteratorEdges end_edges()
    {
        return edges_.end();
    }

    ConstIteratorEdges end_edges() const
    {
        return edges_.cend();
    }

    utils::Range<IteratorEdges> edges()
    {
        return { edges_ };
    }

    utils::Range<ConstIteratorEdges> edges() const
    {
        return { edges_ };
    }

    // -------------------------------------------------------------------------
    //     Debug and Dump
    // -------------------------------------------------------------------------

    /**
     * @brief Validate the correctness of all Tree pointers etc.
     *
     * This function is defined in tree/functions/operators.hpp
     */
    friend bool validate_topology( Tree const& tree );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    TreeLink* root_link_ = nullptr;

    LinkContainerType links_;
    NodeContainerType nodes_;
    EdgeContainerType edges_;

};

} // namespace tree
} // namespace genesis

#endif // include guard
