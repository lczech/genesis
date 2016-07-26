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
 * @brief Header of TreeNode class.
 *
 * For more information, see TreeNode.
 *
 * @file
 * @ingroup tree
 */

#include "utils/core/std.hpp"

#include <memory>
#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class Tree;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Tree Node Data Base
// =================================================================================================

class BaseNodeData
{
public:

    BaseNodeData() = default;
    virtual ~BaseNodeData()
    {}

    virtual std::unique_ptr< BaseNodeData > clone() const
    {
        // return utils::make_unique< BaseNodeData >( *this );
        return std::unique_ptr< BaseNodeData >( new BaseNodeData( *this ));
    };

    // Move ctor and assignment.
    BaseNodeData( BaseNodeData&& other )             = delete;
    BaseNodeData& operator= ( BaseNodeData&& other ) = delete;


protected:

    // Copy ctor and assignment.
    BaseNodeData( BaseNodeData const& other )             = default;
    BaseNodeData& operator= ( BaseNodeData const& other ) = default;

};

// =================================================================================================
//     Tree Node
// =================================================================================================

class TreeNode
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeNode()
        : index_( 0 )
        , link_(  nullptr )
    {}

    TreeNode( size_t index, TreeLink* primary_link )
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

    TreeLink      & primary_link();
    TreeLink const& primary_link() const;

    TreeLink      & link();
    TreeLink const& link() const;

    bool has_data() const;

    BaseNodeData&       data();
    BaseNodeData const& data() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeNode& reset_index( size_t val );

    TreeNode& reset_primary_link( TreeLink* val );

    TreeNode& reset_data( std::unique_ptr< BaseNodeData > data );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    size_t rank() const;
    bool   is_leaf() const;
    bool   is_inner() const;
    bool   is_root() const;

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

private:

    size_t       index_;
    TreeLink*    link_;

    std::unique_ptr< BaseNodeData > data_;

};

// =================================================================================================
//     Casts
// =================================================================================================

template< class NodeDataType >
NodeDataType& node_data_cast( std::unique_ptr< TreeNode >& node )
{
    return dynamic_cast< NodeDataType& >( node->data() );
}

template< class NodeDataType >
NodeDataType const& node_data_cast( std::unique_ptr< TreeNode > const& node )
{
    return dynamic_cast< NodeDataType const& >( node->data() );
}

template< class NodeDataType >
NodeDataType& node_data_cast( TreeNode& node )
{
    return dynamic_cast< NodeDataType& >( node.data() );
}

template< class NodeDataType >
NodeDataType const& node_data_cast( TreeNode const& node )
{
    return dynamic_cast< NodeDataType const& >( node.data() );
}

template< class NodeDataType >
NodeDataType& node_data_cast( BaseNodeData& data )
{
    return dynamic_cast< NodeDataType& >( data );
}

template< class NodeDataType >
NodeDataType const& node_data_cast( BaseNodeData const& data )
{
    return dynamic_cast< NodeDataType const& >( data );
}

} // namespace tree
} // namespace genesis

#endif // include guard
