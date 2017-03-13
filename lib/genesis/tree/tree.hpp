#ifndef GENESIS_TREE_TREE_H_
#define GENESIS_TREE_TREE_H_

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

#include "genesis/utils/core/range.hpp"

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

    using      IteratorLinks = typename ContainerType< TreeLink >::iterator;
    using ConstIteratorLinks = typename ContainerType< TreeLink >::const_iterator;

    using      IteratorNodes = typename ContainerType< TreeNode >::iterator;
    using ConstIteratorNodes = typename ContainerType< TreeNode >::const_iterator;

    using      IteratorEdges = typename ContainerType< TreeEdge >::iterator;
    using ConstIteratorEdges = typename ContainerType< TreeEdge >::const_iterator;

    // -------------------------------------------------------------------------
    //     Construction and Rule of Five
    // -------------------------------------------------------------------------

    Tree()  = default;
    ~Tree() = default;

    Tree( Tree const& other );
    Tree( Tree&& ) = default;

    Tree& operator= ( Tree const& other );
    Tree& operator= ( Tree&& ) = default;

    Tree clone_topology() const;
    void swap( Tree& other );

    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    bool empty() const;

    TreeLink      & root_link();
    TreeLink const& root_link() const;

    TreeNode      & root_node();
    TreeNode const& root_node() const;

    TreeLink      & link_at(size_t index);
    TreeLink const& link_at(size_t index) const;

    TreeNode      & node_at(size_t index);
    TreeNode const& node_at(size_t index) const;

    TreeEdge      & edge_at(size_t index);
    TreeEdge const& edge_at(size_t index) const;

    size_t link_count() const;
    size_t node_count() const;
    size_t edge_count() const;

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    Tree& reset_root_link_index( size_t val );

    LinkContainerType& expose_link_container();
    NodeContainerType& expose_node_container();
    EdgeContainerType& expose_edge_container();

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    // -----------------------------------------------------
    //     Links
    // -----------------------------------------------------

    IteratorLinks      begin_links();
    ConstIteratorLinks begin_links() const;

    IteratorLinks      end_links();
    ConstIteratorLinks end_links() const;

    utils::Range<     IteratorLinks> links();
    utils::Range<ConstIteratorLinks> links() const;

    // -----------------------------------------------------
    //     Nodes
    // -----------------------------------------------------

    IteratorNodes      begin_nodes();
    ConstIteratorNodes begin_nodes() const;

    IteratorNodes      end_nodes();
    ConstIteratorNodes end_nodes() const;

    utils::Range<     IteratorNodes> nodes();
    utils::Range<ConstIteratorNodes> nodes() const;

    // -----------------------------------------------------
    //     Edges
    // -----------------------------------------------------

    IteratorEdges      begin_edges();
    ConstIteratorEdges begin_edges() const;

    IteratorEdges      end_edges();
    ConstIteratorEdges end_edges() const;

    utils::Range<     IteratorEdges> edges();
    utils::Range<ConstIteratorEdges> edges() const;

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

    size_t root_link_index_ = 0;

    LinkContainerType links_;
    NodeContainerType nodes_;
    EdgeContainerType edges_;

};

} // namespace tree
} // namespace genesis

#endif // include guard
