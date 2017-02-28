#ifndef GENESIS_TREE_ITERATOR_PREORDER_H_
#define GENESIS_TREE_ITERATOR_PREORDER_H_

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

#include <assert.h>
#include <deque>
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
//     Preorder Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorPreorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorPreorder<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPreorder ()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    explicit IteratorPreorder( Tree& tree )
        : IteratorPreorder( tree.root_link() )
    {}

    explicit IteratorPreorder( Tree const& tree )
        : IteratorPreorder( tree.root_link() )
    {}

    explicit IteratorPreorder( NodeType& node )
        : IteratorPreorder( node.primary_link() )
    {}

    explicit IteratorPreorder( LinkType& link )
        : start_( &link )
        , link_(  &link )
    {
        push_front_children( &link );
        stack_.push_front( &link.outer() );
    }

    ~IteratorPreorder() = default;

    IteratorPreorder( IteratorPreorder const& ) = default;
    IteratorPreorder( IteratorPreorder&& )      = default;

    IteratorPreorder& operator= ( IteratorPreorder const& ) = default;
    IteratorPreorder& operator= ( IteratorPreorder&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_ = nullptr;
        } else {
            link_ = stack_.front();
            stack_.pop_front();
            push_front_children(link_);
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
    //     Internal Helper Functions
    // -----------------------------------------------------

private:

    void push_front_children( LinkType* link )
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a preorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<LinkType*> tmp;
        LinkType* c = &link->next();
        while (c != link) {
            tmp.push_front( &c->outer() );
            c = &c->next();
        }
        for (LinkType* l : tmp) {
            stack_.push_front(l);
        }
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    // TODO take a stack or vector instead of deque here; maybe reverse pushing order

    LinkType* const       start_;
    LinkType*             link_;

    std::deque<LinkType*> stack_;
};

// =================================================================================================
//     Preorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorPreorder< TreeLink const, TreeNode const, TreeEdge const >>
preorder( ElementType const& element )
{
    return {
        IteratorPreorder< const TreeLink, const TreeNode, const TreeEdge >( element ),
        IteratorPreorder< const TreeLink, const TreeNode, const TreeEdge >()
    };
}

template<typename ElementType>
utils::Range< IteratorPreorder< TreeLink, TreeNode, TreeEdge >>
preorder( ElementType& element )
{
    return {
        IteratorPreorder< TreeLink, TreeNode, TreeEdge >( element ),
        IteratorPreorder< TreeLink, TreeNode, TreeEdge >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
