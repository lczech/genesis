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
 * @file
 * @ingroup tree
 */

#include "tree/tree/edge_data.hpp"

#include "utils/core/std.hpp"

#include <memory>
#include <string>

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

    size_t index() const;

    TreeLink      & primary_link();
    TreeLink const& primary_link() const;

    TreeLink      & secondary_link();
    TreeLink const& secondary_link() const;

    TreeNode      & primary_node();
    TreeNode const& primary_node() const;

    TreeNode      & secondary_node();
    TreeNode const& secondary_node() const;

    bool has_data() const;

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

    BaseEdgeData*       data_ptr();
    BaseEdgeData const* data_ptr() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeEdge& reset_index( size_t val );

    TreeEdge& reset_primary_link(   TreeLink* val );
    TreeEdge& reset_secondary_link( TreeLink* val );

    TreeEdge& reset_data( std::unique_ptr< BaseEdgeData > data );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    std::string dump() const;

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
