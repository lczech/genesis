#ifndef GENESIS_TREE_ITERATOR_PREORDER_H_
#define GENESIS_TREE_ITERATOR_PREORDER_H_

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

#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>

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
//     Preorder Iterator
// =================================================================================================

template< bool is_const = true >
class IteratorPreorder
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
    using SubtreeType = typename std::conditional< is_const, Subtree const, Subtree >::type;

    using self_type         = IteratorPreorder< is_const >;
    using iterator_category = std::forward_iterator_tag;
    // using value_type        = NodeType;
    // using pointer           = NodeType*;
    // using reference         = NodeType&;
    // using difference_type   = std::ptrdiff_t;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPreorder ()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    /**
     * @brief Start a preorder traversal at the root of the given Tree.
     */
    explicit IteratorPreorder( TreeType& tree )
        : IteratorPreorder( tree.root_link() )
    {}

    /**
     * @brief Start a preorder traversal at the given TreeNode, moving in the root direction first.
     */
    explicit IteratorPreorder( NodeType& node )
        : IteratorPreorder( node.primary_link() )
    {}

    /**
     * @brief Start a preorder traversal at a given TreeLink, moving in the direction of the link first.
     */
    explicit IteratorPreorder( LinkType& link )
        // Set the starting link and the one where we currently are.
        // In preorder traversal, we start at the actual node, so this is easy.
        : start_( &link )
        , link_(  &link )
    {
        // Add all neighbouring nodes of the starting one to the stack.
        // As push_back_children_() does not add the outer() of the given link,
        // we need to do this extra (this is done in order to keep this function simple,
        // without haven to consider the "edge case" that occurs here in the constructor).
        // All these neighbours will then be visisted later.
        push_back_children_( &link );
        stack_.push_back( &link.outer() );
    }

    /**
     * @brief Start a preorder traversal at the top TreeNode of a Subtree,
     * only traversing the nodes in the subtree
     */
    explicit IteratorPreorder( SubtreeType& subtree )
        : start_( &(subtree.link()) )
        , link_(  &(subtree.link()) )
    {
        // Compared to the normal constructor above, we do the same, but leave out
        // the outer() link, as this is the part of the tree that we want to skip.
        push_back_children_( link_ );
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
        if( stack_.empty() ) {

            // We reached the end of the stack.
            // This is the signal to stop traversing.
            link_ = nullptr;

        } else {

            // While the stack is not empty, it gives us the next link to move to.
            // Go there, and add its children to the stack for the next iterations.
            link_ = stack_.back();
            stack_.pop_back();
            push_back_children_(link_);
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

    void push_back_children_( LinkType* link )
    {
        // we need to push to a tmp first, in order to get the order right.
        // otherwise, we would still do a preorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::vector<LinkType*> tmp;
        LinkType* c = &link->next();
        while (c != link) {
            tmp.push_back( &c->outer() );
            c = &c->next();
        }
        for( auto lit = tmp.rbegin(); lit != tmp.rend(); ++lit ) {
            stack_.push_back( *lit );
        }
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    LinkType* const       start_;
    LinkType*             link_;

    std::vector<LinkType*> stack_;
};

// =================================================================================================
//     Preorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorPreorder< true >>
preorder( ElementType const& element )
{
    return {
        IteratorPreorder< true >( element ),
        IteratorPreorder< true >()
    };
}

template<typename ElementType>
utils::Range< IteratorPreorder< false >>
preorder( ElementType& element )
{
    return {
        IteratorPreorder< false >( element ),
        IteratorPreorder< false >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
