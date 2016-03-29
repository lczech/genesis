#ifndef GENESIS_TREE_TREE_EDGE_H_
#define GENESIS_TREE_TREE_EDGE_H_

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
 * For more information, see TreeEdge.
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
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class  TreeNode;

// =================================================================================================
//     TreeEdge
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class TreeEdge
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs
    // ---------------------------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeEdge()
        : index_( 0 )
        , link_p_( nullptr )
        , link_s_( nullptr )
    {}

    TreeEdge( size_t index, LinkType* primary_link, LinkType* secondary_link )
        : index_(  index )
        , link_p_( primary_link )
        , link_s_( secondary_link )
    {}

    ~TreeEdge() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeEdge( TreeEdge const& ) = delete;
    TreeEdge( TreeEdge&& )      = delete;

    TreeEdge& operator= ( TreeEdge const& ) = delete;
    TreeEdge& operator= ( TreeEdge&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t index() const;

    LinkType      & primary_link();
    LinkType const& primary_link() const;

    LinkType      & secondary_link();
    LinkType const& secondary_link() const;

    NodeType      & primary_node();
    NodeType const& primary_node() const;

    NodeType      & secondary_node();
    NodeType const& secondary_node() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeEdge& reset_index( size_t val );

    TreeEdge& reset_primary_link(   LinkType* val );
    TreeEdge& reset_secondary_link( LinkType* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

public:

    EdgeDataType data;

private:

    size_t       index_;

    LinkType*    link_p_;
    LinkType*    link_s_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the Implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_edge.tpp"

#endif // include guard
