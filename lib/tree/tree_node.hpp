#ifndef GENESIS_TREE_TREE_NODE_H_
#define GENESIS_TREE_TREE_NODE_H_

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
 * @brief Header of TreeNode class template.
 *
 * For more information, see TreeNode.
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
class TreeLink;

// =================================================================================================
//     TreeNode
// =================================================================================================

template <class node_data_type, class edge_data_type>
class TreeNode
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

    TreeNode()
        : index_( 0 )
        , link_(  nullptr )
    {}

    TreeNode( size_t index, LinkType* primary_link )
        : index_( index )
        , link_(  primary_link )
    {}

    ~TreeNode() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeNode( TreeNode const& ) = delete;
    TreeNode( TreeNode&& )      = delete;

    TreeNode& operator= ( TreeNode const& ) = delete;
    TreeNode& operator= ( TreeNode&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t index() const;

    LinkType      & primary_link();
    LinkType const& primary_link() const;

    LinkType      & link();
    LinkType const& link() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeNode& reset_index( size_t val );

    TreeNode& reset_primary_link( LinkType* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    size_t rank() const;
    bool   is_leaf() const;
    bool   is_inner() const;

    //~ /** True if the node is the root, false otherwise. */
    //~ bool IsRoot();

    //~ /** Depth of the node in the tree, i.e. its distance from root. */
    //~ int Depth();

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

public:

    NodeDataType data;

private:

    size_t       index_;
    LinkType*    link_;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

#include "tree/tree_node.tpp"

#endif // include guard
