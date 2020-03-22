#ifndef GENESIS_TREE_TREE_NODE_H_
#define GENESIS_TREE_TREE_NODE_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree/node_data.hpp"
#include "genesis/tree/tree/link.hpp"

#include "genesis/utils/core/std.hpp"

#include <memory>
#include <string>
#include <typeinfo>

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

    /**
     * @brief Return the index of this Node.
     */
    size_t index() const
    {
        return index_;
    }

    /**
     * @brief Return the TreeLink that points towards the root.
     */
    TreeLink& primary_link()
    {
        return *link_;
    }

    /**
     * @brief Return the TreeLink that points towards the root.
     */
    TreeLink const& primary_link() const
    {
        return *link_;
    }

    /**
     * @brief Return the TreeLink that points towards the root.
     *
     * This is just an alias for primary_link(), that is shorter to use when needed
     * frequently in an algorithm.
     */
    TreeLink& link()
    {
        return *link_;
    }

    /**
     * @brief Return the TreeLink that points towards the root.
     *
     * This is just an alias for primary_link(), that is shorter to use when needed
     * frequently in an algorithm.
     */
    TreeLink const& link() const
    {
        return *link_;
    }

    /**
     * @brief Return the TreeEdge that points towards the root.
     */
    TreeEdge& primary_edge()
    {
        return link_->edge();
    }

    /**
     * @brief Return the TreeEdge that points towards the root.
     */
    TreeEdge const& primary_edge() const
    {
        return link_->edge();
    }

    // ---------------------------------------------------------------------
    //     Data Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Return `true` if the TreeNode has a data object assigned to it.
     */
    bool has_data() const
    {
        return data_.get() != nullptr;
    }

    /**
     * @brief Return `true` iff the TreeNode has a data object assigned to it that is of a particular
     * given data type.
     */
    template< class NodeDataType >
    bool data_is( bool allow_null = false ) const
    {
        if( data_.get() == nullptr ) {
            return allow_null;
        }
        assert( data_.get() );
        auto const& ref = *data_.get();
        return ( typeid( ref ) == typeid( NodeDataType ));
    }

    /**
     * @brief Return `true` iff the TreeNode has a data object assigned to it that is derived from
     * a particular given data type.
     */
    template< class NodeDataType >
    bool data_is_derived_from( bool allow_null = false ) const
    {
        if( data_.get() == nullptr ) {
            return allow_null;
        }
        assert( data_.get() );
        return ( dynamic_cast< NodeDataType const* >( data_.get() ) != nullptr );
    }

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

    /**
     * @brief Return a pointer to the data.
     *
     * In most cases, using data<>() is more convenient. However, in some cases, this function
     * might be necessary.
     */
    BaseNodeData* data_ptr()
    {
        return data_.get();
    }

    /**
     * @brief Return a const pointer to the data.
     *
     * In most cases, using data<>() is more convenient. However, in some cases, this function
     * might be necessary.
     */
    BaseNodeData const* data_ptr() const
    {
        return data_.get();
    }

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    /**
     * @brief Reset the internal index of this TreeNode.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     * The index is an invariant that needs to be kept, as it needs to match the index in the Tree
     * container.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeNode& reset_index( size_t val )
    {
        index_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the TreeLink of this TreeNode.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeNode& reset_primary_link( TreeLink* val )
    {
        link_ = val;
        return *this;
    }

    /**
     * @brief Reset the data pointer of this TreeNode.
     *
     * Using this function, a TreeNode can be assigend new data. It is also possible to change the
     * data type completely (as long as it derives from BaseNodeData). Be however aware that many
     * functions that work with trees expcet a certain data type. Thus, changing it might break those
     * functions and lead to exceptions and other errors.
     */
    TreeNode& reset_data( std::unique_ptr< BaseNodeData > data )
    {
        data_ = std::move( data );
        return *this;
    }

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
