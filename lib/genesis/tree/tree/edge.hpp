#ifndef GENESIS_TREE_TREE_EDGE_H_
#define GENESIS_TREE_TREE_EDGE_H_

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

#include "genesis/tree/tree/edge_data.hpp"
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
class TreeLink;
class TreeNode;

class BaseEdgeData;

// =================================================================================================
//     Tree Edge
// =================================================================================================

class TreeEdge
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    friend class Tree;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeEdge()
        : index_( 0 )
        , link_p_( nullptr )
        , link_s_( nullptr )
        , data_( nullptr )
    {}

    TreeEdge( size_t index, TreeLink* primary_link, TreeLink* secondary_link )
        : index_(  index )
        , link_p_( primary_link )
        , link_s_( secondary_link )
        , data_( nullptr )
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

    /**
     * @brief Return the index of this Edge.
     */
    size_t index() const
    {
        return index_;
    }

    /**
     * @brief Return the TreeLink of this TreeEdge that points towards the root.
     */
    TreeLink& primary_link()
    {
        return *link_p_;
    }

    /**
     * @brief Return the TreeLink of this TreeEdge that points towards the root.
     */
    TreeLink const& primary_link() const
    {
        return *link_p_;
    }

    /**
     * @brief Return the TreeLink of this TreeEdge that points away from the root.
     */
    TreeLink& secondary_link()
    {
        return *link_s_;
    }

    /**
     * @brief Return the TreeLink of this TreeEdge that points away from the root.
     */
    TreeLink const& secondary_link() const
    {
        return *link_s_;
    }

    /**
     * @brief Return the TreeNode of this TreeEdge that points towards the root.
     */
    TreeNode& primary_node()
    {
        return link_p_->node();
    }

    /**
     * @brief Return the TreeNode of this TreeEdge that points towards the root.
     */
    TreeNode const& primary_node() const
    {
        return link_p_->node();
    }

    /**
     * @brief Return the TreeNode of this TreeEdge that points away from the root.
     */
    TreeNode& secondary_node()
    {
        return link_s_->node();
    }

    /**
     * @brief Return the TreeNode of this TreeEdge that points away from the root.
     */
    TreeNode const& secondary_node() const
    {
        return link_s_->node();
    }

    // ---------------------------------------------------------------------
    //     Data Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Return `true` if the TreeEdge has a data object assigned to it.
     */
    bool has_data() const
    {
        return data_.get() != nullptr;
    }

    /**
     * @brief Return `true` iff the TreeEdge has a data object assigned to it that is of a particular
     * given data type.
     */
    template< class EdgeDataType >
    bool data_is( bool allow_null = false ) const
    {
        if( data_.get() == nullptr ) {
            return allow_null;
        }
        assert( data_.get() );
        auto const& ref = *data_.get();
        return ( typeid( ref ) == typeid( EdgeDataType ));
    }

    /**
     * @brief Return `true` iff the TreeEdge has a data object assigned to it that is derived from
     * a particular given data type.
     */
    template< class EdgeDataType >
    bool data_is_derived_from( bool allow_null = false ) const
    {
        if( data_.get() == nullptr ) {
            return allow_null;
        }
        assert( data_.get() );
        return ( dynamic_cast< EdgeDataType const* >( data_.get() ) != nullptr );
    }

    template< class EdgeDataType >
    EdgeDataType& data()
    {
        return dynamic_cast< EdgeDataType& >( *data_ );
    }

    template< class EdgeDataType >
    EdgeDataType const& data() const
    {
        return dynamic_cast< EdgeDataType const& >( *data_ );
    }

    template< class EdgeDataType >
    EdgeDataType* data_cast()
    {
        return dynamic_cast< EdgeDataType* >( data_.get() );
    }

    template< class EdgeDataType >
    EdgeDataType const* data_cast() const
    {
        return dynamic_cast< EdgeDataType const* >( data_.get() );
    }

    /**
     * @brief Return a pointer to the data.
     *
     * In most cases, using data<>() is more convenient. However, in some cases, this function
     * might be necessary.
     */
    BaseEdgeData* data_ptr()
    {
        return data_.get();
    }

    /**
     * @brief Return a const pointer to the data.
     *
     * In most cases, using data<>() is more convenient. However, in some cases, this function
     * might be necessary.
     */
    BaseEdgeData const* data_ptr() const
    {
        return data_.get();
    }

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    /**
     * @brief Reset the internal index of this TreeEdge.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     * The index is an invariant that needs to be kept, as it needs to match the index in the Tree
     * container.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeEdge& reset_index( size_t val )
    {
        index_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the primary TreeLink of this TreeEdge.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeEdge& reset_primary_link(   TreeLink* val )
    {
        link_p_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the secondary TreeLink of this TreeEdge.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeEdge& reset_secondary_link( TreeLink* val )
    {
        link_s_ = val;
        return *this;
    }

    /**
     * @brief Reset the data pointer of this TreeEdge.
     *
     * Using this function, a TreeEdge can be assigend new data. It is also possible to change the
     * data type completely (as long as it derives from BaseEdgeData). Be however aware that many
     * functions that work with Tree%s expect a certain data type. Thus, changing it might break those
     * functions and lead to exceptions and other errors.
     */
    TreeEdge& reset_data( std::unique_ptr< BaseEdgeData > data )
    {
        data_ = std::move( data );
        return *this;
    }

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

private:

    size_t       index_;

    TreeLink*    link_p_;
    TreeLink*    link_s_;

    std::unique_ptr< BaseEdgeData > data_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
