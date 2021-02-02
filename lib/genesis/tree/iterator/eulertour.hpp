#ifndef GENESIS_TREE_ITERATOR_EULERTOUR_H_
#define GENESIS_TREE_ITERATOR_EULERTOUR_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree/subtree.hpp"
#include "genesis/utils/containers/range.hpp"

#include <iterator>
#include <type_traits>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;
class Subtree;

// =================================================================================================
//     Euler Tour Iterator
// =================================================================================================

template< bool is_const = true >
class IteratorEulertour
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    // Make the memer types const or not, depending on iterator type.
    using TreeType = typename std::conditional< is_const, Tree const, Tree >::type;
    using LinkType = typename std::conditional< is_const, TreeLink const, TreeLink >::type;
    using NodeType = typename std::conditional< is_const, TreeNode const, TreeNode >::type;
    using EdgeType = typename std::conditional< is_const, TreeEdge const, TreeEdge >::type;

    using self_type         = IteratorEulertour< is_const >;
    using iterator_category = std::forward_iterator_tag;
    // using value_type        = NodeType;
    // using pointer           = NodeType*;
    // using reference         = NodeType&;
    // using difference_type   = std::ptrdiff_t;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorEulertour()
        : link_(  nullptr )
        , start_( nullptr )
    {}

    explicit IteratorEulertour( TreeType& tree )
        : link_(  &tree.root_link() )
        , start_( &tree.root_link() )
    {}

    explicit IteratorEulertour( NodeType& node )
        : link_(  &node.primary_link() )
        , start_( &node.primary_link() )
    {}

    explicit IteratorEulertour( LinkType& link )
        : link_(  &link )
        , start_( &link )
    {}

    explicit IteratorEulertour( Subtree const& subtree )
        // We need to consider the edge case of a subtree that is only a leaf.
        // In that case, the start_ is set to the one that is visited after,
        // so that we iterate only the leaf itself.
        : link_(  &(subtree.link().next()) )
        , start_( (link_ == &link_->next()) ? &(subtree.link().outer().next()) : &(subtree.link()) )
    {}

    ~IteratorEulertour() = default;

    IteratorEulertour( IteratorEulertour const& ) = default;
    IteratorEulertour( IteratorEulertour&& )      = default;

    IteratorEulertour& operator= ( IteratorEulertour const& ) = default;
    IteratorEulertour& operator= ( IteratorEulertour&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type& operator ++ ()
    {
        link_ = &link_->outer().next();
        if( link_ == start_ ) {
            link_ = nullptr;
        }
        return *this;
    }

    self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator == (const self_type &other) const
    {
        return other.link_ == link_;
    }

    bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    bool is_first_iteration() const
    {
        return link_ == start_;
    }

    LinkType& link() const
    {
        return *link_;
    }

    NodeType& node() const
    {
        return link_->node();
    }

    EdgeType& edge() const
    {
        return link_->edge();
    }

    LinkType& start_link() const
    {
        return *start_;
    }

    NodeType& start_node() const
    {
        return start_->node();
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    LinkType*       link_;
    LinkType* const start_;
};

// =================================================================================================
//     Euler Tour Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorEulertour< true >>
eulertour( ElementType const& element )
{
    return {
        IteratorEulertour< true >( element ),
        IteratorEulertour< true >()
    };
}

template<typename ElementType>
utils::Range< IteratorEulertour< false >>
eulertour( ElementType& element )
{
    return {
        IteratorEulertour< false >( element ),
        IteratorEulertour< false >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
