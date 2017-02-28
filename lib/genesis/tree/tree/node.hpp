#ifndef GENESIS_TREE_TREE_NODE_H_
#define GENESIS_TREE_TREE_NODE_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree/node_data.hpp"

#include "genesis/utils/core/std.hpp"

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
//     Tree Node
// =================================================================================================

class TreeNode
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    friend class Tree;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeNode()
        : index_( 0 )
        , link_(  nullptr )
        , data_(  nullptr )
    {}

    TreeNode( size_t index, TreeLink* primary_link )
        : index_( index )
        , link_(  primary_link )
        , data_(  nullptr )
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

    template< class NodeDataType >
    NodeDataType& data()
    {
        return dynamic_cast< NodeDataType& >( *data_ );
    }

    template< class NodeDataType >
    NodeDataType const& data() const
    {
        return dynamic_cast< NodeDataType const& >( *data_ );
    }

    template< class NodeDataType >
    NodeDataType* data_cast()
    {
        return dynamic_cast< NodeDataType* >( data_.get() );
    }

    template< class NodeDataType >
    NodeDataType const* data_cast() const
    {
        return dynamic_cast< NodeDataType const* >( data_.get() );
    }

    BaseNodeData*       data_ptr();
    BaseNodeData const* data_ptr() const;

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

} // namespace tree
} // namespace genesis

#endif // include guard
