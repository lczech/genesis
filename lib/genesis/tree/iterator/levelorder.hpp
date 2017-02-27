#ifndef GENESIS_TREE_ITERATOR_LEVELORDER_H_
#define GENESIS_TREE_ITERATOR_LEVELORDER_H_

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
//     Levelorder Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorLevelorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorLevelorder<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorLevelorder()
        : start_( nullptr )
        , link_(  nullptr )
        , depth_( 0       )
    {}

    explicit IteratorLevelorder( Tree& tree )
        : IteratorLevelorder( tree.root_link() )
    {}

    explicit IteratorLevelorder( Tree const& tree )
        : IteratorLevelorder( tree.root_link() )
    {}

    explicit IteratorLevelorder( NodeType& node )
        : IteratorLevelorder( node.primary_link() )
    {}

    explicit IteratorLevelorder( LinkType& link )
        : start_( &link )
        , link_(  &link )
        , depth_( 0     )
    {
        push_back_children( &link, 0 );
        stack_.push_front({ &link.outer(), 1 });
    }

    ~IteratorLevelorder() = default;

    IteratorLevelorder( IteratorLevelorder const& ) = default;
    IteratorLevelorder( IteratorLevelorder&& )      = default;

    IteratorLevelorder& operator= ( IteratorLevelorder const& ) = default;
    IteratorLevelorder& operator= ( IteratorLevelorder&& )      = default;

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
            link_  = nullptr;
            depth_ = -1;
        } else {
            StackElement se = stack_.front();
            link_  = se.link;
            depth_ = se.depth;
            stack_.pop_front();
            push_back_children(link_, depth_);
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

    int depth() const
    {
        return depth_;
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

private:

    // -----------------------------------------------------
    //     Internal Helper Functions
    // -----------------------------------------------------

    void push_back_children( LinkType* link, int link_depth )
    {
        LinkType* c = &link->next();
        while( c != link ) {
            stack_.push_back({ &c->outer(), link_depth + 1 });
            c = &c->next();
        }
    }

    // -----------------------------------------------------
    //     Internal Helper Typedefs
    // -----------------------------------------------------

    // TODO add depth information to other iterators, as well.
    typedef struct {
        LinkType*     link;
        int           depth;
    } StackElement;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    LinkType* const   start_;
    LinkType*         link_;
    int               depth_;

    std::deque<StackElement> stack_;
};

// =================================================================================================
//     Levelorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorLevelorder< TreeLink const, TreeNode const, TreeEdge const >>
levelorder( ElementType const& element )
{
    return {
        IteratorLevelorder< const TreeLink, const TreeNode, const TreeEdge >( element ),
        IteratorLevelorder< const TreeLink, const TreeNode, const TreeEdge >()
    };
}

template<typename ElementType>
utils::Range< IteratorLevelorder< TreeLink, TreeNode, TreeEdge >>
levelorder( ElementType& element )
{
    return {
        IteratorLevelorder< TreeLink, TreeNode, TreeEdge >( element ),
        IteratorLevelorder< TreeLink, TreeNode, TreeEdge >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
