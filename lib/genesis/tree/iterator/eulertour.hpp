#ifndef GENESIS_TREE_ITERATOR_EULERTOUR_H_
#define GENESIS_TREE_ITERATOR_EULERTOUR_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/utils/core/range.hpp"

#include <iterator>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Euler Tour Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorEulertour
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorEulertour<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorEulertour()
        : start_( nullptr )
        , link_(  nullptr )
    {}

    explicit IteratorEulertour (Tree& tree)
        : start_( &tree.root_link() )
        , link_(  &tree.root_link() )
    {}

    explicit IteratorEulertour (Tree const& tree)
        : start_( &tree.root_link() )
        , link_(  &tree.root_link() )
    {}

    explicit IteratorEulertour (NodeType& node)
        : start_( &node.primary_link() )
        , link_(  &node.primary_link() )
    {}

    explicit IteratorEulertour (LinkType& link)
        : start_( &link )
        , link_(  &link )
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
        if (link_ == start_) {
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

    LinkType* const start_;
    LinkType*       link_;
};

// =================================================================================================
//     Euler Tour Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorEulertour< TreeLink const, TreeNode const, TreeEdge const >>
eulertour( ElementType const& element )
{
    return {
        IteratorEulertour< const TreeLink, const TreeNode, const TreeEdge >( element ),
        IteratorEulertour< const TreeLink, const TreeNode, const TreeEdge >()
    };
}

template<typename ElementType>
utils::Range< IteratorEulertour< TreeLink, TreeNode, TreeEdge >>
eulertour( ElementType& element )
{
    return {
        IteratorEulertour< TreeLink, TreeNode, TreeEdge >( element ),
        IteratorEulertour< TreeLink, TreeNode, TreeEdge >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
