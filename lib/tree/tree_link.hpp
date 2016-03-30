#ifndef GENESIS_TREE_TREE_LINK_H_
#define GENESIS_TREE_TREE_LINK_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class Tree;

template <class NodeDataType, class EdgeDataType>
class TreeEdge;

template <class NodeDataType, class EdgeDataType>
class TreeNode;

// =================================================================================================
//     TreeLink
// =================================================================================================

template <class node_data_type, class edge_data_type>
class TreeLink
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs
    // ---------------------------------------------------------------------

    using NodeDataType       = node_data_type;
    using EdgeDataType       = edge_data_type;

    using TreeType           = Tree    <NodeDataType, EdgeDataType>;
    using LinkType           = TreeLink<NodeDataType, EdgeDataType>;
    using NodeType           = TreeNode<NodeDataType, EdgeDataType>;
    using EdgeType           = TreeEdge<NodeDataType, EdgeDataType>;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeLink()
        : index_( 0       )
        , next_(  nullptr )
        , outer_( nullptr )
        , node_(  nullptr )
        , edge_(  nullptr )
    {}

    TreeLink( size_t index, LinkType* next, LinkType* outer, NodeType* node, EdgeType* edge )
        : index_( index )
        , next_(  next  )
        , outer_( outer )
        , node_(  node  )
        , edge_(  edge  )
    {}

    ~TreeLink() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeLink( TreeLink const& ) = delete;
    TreeLink( TreeLink&& )      = delete;

    TreeLink& operator= ( TreeLink const& ) = delete;
    TreeLink& operator= ( TreeLink&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t index() const;

    LinkType      & next();
    LinkType const& next() const;

    LinkType      & prev();
    LinkType const& prev() const;

    LinkType      & outer();
    LinkType const& outer() const;

    EdgeType      & edge();
    EdgeType const& edge() const;

    NodeType      & node();
    NodeType const& node() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeLink& reset_index( size_t val );

    TreeLink& reset_next(  LinkType* val );
    TreeLink& reset_outer( LinkType* val );

    TreeLink& reset_node(  NodeType* val );
    TreeLink& reset_edge(  EdgeType* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    bool is_leaf() const;
    bool is_inner() const;

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

private:

    size_t index_;

    LinkType* next_;
    LinkType* outer_;

    NodeType* node_;
    EdgeType* edge_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the Implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_link.tpp"

#endif // include guard
