#ifndef GENESIS_TREE_ITERATOR_POSTORDER_H_
#define GENESIS_TREE_ITERATOR_POSTORDER_H_

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
#include <deque>
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
//     Postorder Iterator
// =================================================================================================

template< bool is_const = true >
class IteratorPostorder
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

    using self_type         = IteratorPostorder< is_const >;
    using iterator_category = std::forward_iterator_tag;
    // using value_type        = NodeType;
    // using pointer           = NodeType*;
    // using reference         = NodeType&;
    // using difference_type   = std::ptrdiff_t;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPostorder()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    /**
     * @brief Start a postorder traversal at the root of the given Tree.
     */
    explicit IteratorPostorder( TreeType& tree )
        : IteratorPostorder( tree.root_link() )
    {}

    /**
     * @brief Start a postorder traversal at the given TreeNode, moving in the root direction first.
     *
     * The iterator "starts" that the given TreeNode (for a postorder traversal, starting means
     * that this will be the last node visited).
     * It first traverses in the direction towards the root, until a leaf is found.
     * This leaf is the first node being visited. From there on, the traversal continues as usual.
     */
    explicit IteratorPostorder( NodeType& node )
        : IteratorPostorder( node.primary_link() )
    {}

    /**
     * @brief Start a postorder traversal at a given TreeLink, moving in the direction of the link first.
     *
     * The iterator "starts" that the TreeNode of the given link (for a postorder traversal,
     * starting means that this will be the last node visited).
     * It first traverses in the direction of the outer() link of the given link,
     * until a leaf is found. This leaf is the first node being visited.
     * From there on, the traversal continues as usual.
     */
    explicit IteratorPostorder( LinkType& link )
        : start_( &link )
    {
        // The stack keeps links to nodes that yet need to be visited.
        // These are the links that point towards the start link.

        // Fill in the start link. This will be the last node visited.
        auto link_ptr = &link;
        stack_.push_back( link_ptr );

        // Now start traversing in the direction of this link.
        stack_.push_front( &link_ptr->outer() );
        link_ptr = &link_ptr->outer();

        // Add all inner nodes along the first path, until we reach a leaf.
        while( is_inner_( *link_ptr )) {
            push_front_children_( link_ptr );
            link_ptr = &link_ptr->next().outer();
        }

        // The fist leaf that we found is the last link that was added.
        // Remove it from the stack again, because this is where we stop - this is the first
        // node being visited, and hence does not need to be on the stack any more
        // (it was added to keep the push_front_children fct simple and without edge case).
        assert( link_ptr == stack_.front() );
        stack_.pop_front();
        link_ = link_ptr;
    }

    /**
     * @brief Start a postorder traversal at the top TreeNode of a Subtree,
     * only traversing the nodes in the subtree.
     */
    explicit IteratorPostorder( Subtree const& subtree )
        : start_( &(subtree.link()) )
    {
        // Add the starting/subtree link as the final one to the stack.
        auto link_ptr = &(subtree.link());
        stack_.push_back( link_ptr );

        // Compared to the normal constructor above, we simply leave out the part
        // where we move in the outer() direction of the link, and instead just continue
        // in the direction of the subtree itself.

        // Find the first leaf.
        while( is_inner_( *link_ptr )) {
            push_front_children_( link_ptr );
            link_ptr = &link_ptr->next().outer();
        }

        // The leaf that was last added is the one that we visit now.
        // Remove it again, as we are here now.
        assert( link_ptr == stack_.front() );
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

            // This condition marks the end of the traversal:
            // There are no more links on the stack to be visited.
            link_ = nullptr;

        } else if( &link_->outer().next() == stack_.front() ) {

            // This condition is active when seeing an inner node the last time,
            // meaning that it is its turn to be visited.
            link_ = stack_.front();
            stack_.pop_front();

        } else {

            // This condition is active in all other cases: going down the tree towards the leaves.
            // That means, we are in a part that is not yet on the stack, so we need to add it.
            link_ = stack_.front();
            while( is_inner_( *link_ )) {
                push_front_children_(link_);
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

    void push_front_children_( LinkType* link )
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

    bool is_inner_( TreeLink const& link )
    {
        // Duplication of the free function. But this avoids pulling in the header.
        return &( link.next() ) != &link;
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
utils::Range< IteratorPostorder< true >>
postorder( ElementType const& element )
{
    return {
        IteratorPostorder< true >( element ),
        IteratorPostorder< true >()
    };
}

template<typename ElementType>
utils::Range< IteratorPostorder< false >>
postorder( ElementType& element )
{
    return {
        IteratorPostorder< false >( element ),
        IteratorPostorder< false >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
