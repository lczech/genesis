#ifndef GENESIS_TREE_ITERATOR_POSTORDER_H_
#define GENESIS_TREE_ITERATOR_POSTORDER_H_

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

#include "tree/tree.hpp"
#include "utils/core/range.hpp"

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
//     Postorder Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorPostorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorPostorder<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPostorder()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    explicit IteratorPostorder( Tree& tree )
        : IteratorPostorder( tree.root_link() )
    {}

    explicit IteratorPostorder( Tree const& tree )
        : IteratorPostorder( tree.root_link() )
    {}

    explicit IteratorPostorder( NodeType& node )
        : IteratorPostorder( node.primary_link() )
    {}

    explicit IteratorPostorder( LinkType& link )
        : start_( &link )
    {
        auto link_ptr = &link;
        stack_.push_back(link_ptr);
        stack_.push_front(&link_ptr->outer());
        link_ptr = &link_ptr->outer();
        while (link_ptr->is_inner()) {
            push_front_children(link_ptr);
            link_ptr = &link_ptr->next().outer();
        }
        assert(link_ptr == stack_.front());
        stack_.pop_front();
        link_ = link_ptr;
    }

    ~IteratorPostorder() = default;

    IteratorPostorder( IteratorPostorder const& ) = default;
    IteratorPostorder( IteratorPostorder&& )      = default;

    IteratorPostorder& operator= ( IteratorPostorder const& ) = default;
    IteratorPostorder& operator= ( IteratorPostorder&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type operator ++ ()
    {
        if( stack_.empty() ) {
            // this condition marks the end of the traversal
            link_ = nullptr;
        } else if( &link_->outer().next() == stack_.front() ) {
            // this condition is active when seeing an inner node the last time,
            // meaning that it is its turn to be traversed
            link_ = stack_.front();
            stack_.pop_front();
        } else {
            // this condition is active in all other cases: going down the tree towards the leaves
            link_ = stack_.front();
            while (link_->is_inner()) {
                push_front_children(link_);
                link_ = &link_->next().outer();
            }
            assert( link_ == stack_.front() );
            stack_.pop_front();
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

    bool is_last_iteration() const
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

private:

    // -----------------------------------------------------
    //     Internal Helper Functions
    // -----------------------------------------------------

    void push_front_children( LinkType* link )
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a postorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<LinkType*> tmp;
        LinkType* c = &link->next();
        while( c != link ) {
            tmp.push_front( &c->outer() );
            c = &c->next();
        }
        for( LinkType* l : tmp ) {
            stack_.push_front(l);
        }
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    LinkType* const       start_;
    LinkType*             link_;

    std::deque<LinkType*> stack_;
};

// =================================================================================================
//     Postorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorPostorder< TreeLink const, TreeNode const, TreeEdge const >>
postorder( ElementType const& element )
{
    return {
        IteratorPostorder< const TreeLink, const TreeNode, const TreeEdge >( element ),
        IteratorPostorder< const TreeLink, const TreeNode, const TreeEdge >()
    };
}

template<typename ElementType>
utils::Range< IteratorPostorder< TreeLink, TreeNode, TreeEdge >>
postorder( ElementType& element )
{
    return {
        IteratorPostorder< TreeLink, TreeNode, TreeEdge >( element ),
        IteratorPostorder< TreeLink, TreeNode, TreeEdge >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
