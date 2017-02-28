#ifndef GENESIS_TREE_ITERATOR_INORDER_H_
#define GENESIS_TREE_ITERATOR_INORDER_H_

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

#include <assert.h>
#include <deque>
#include <iterator>

namespace genesis {
namespace tree {

// =================================================================================================
//     Inorder Iterator
// =================================================================================================

/* *
 * @brief Iterates over a Tree in inorder manner.
 *
 * This iterator usually is used on binary trees and started at the root. As we are dealing with
 * (possibly) multifurcating (non-binary) trees, and also might start this iterator on some inner
 * node, it behaves a bit different from its standard mode of operation:
 *
 *  *  In multifurcating trees, inner nodes will be visited more than once, namely their number of
 *     neighbors - 2 many times. This is simply a consequence of the multifurcation. In a binary
 *     tree, each node has three neighbors, so is visited 3-2=1 many times, as usual, with notable
 *     exceptions:
 *  *  In an unrooted binary tree, the "virtual root" (meaning, the node that is used as a starting
 *     point into the tree) usually has three children, as it actually is a normal inner node that
 *     just happened to be the first one in memory. Thus, when starting this iterator at this kind
 *     of root, the root node will be visited twice during the traversal. Again, this is a
 *     consequence of the mechanism of inorder traversal.
 *  *  The same is true when the iterator is started on some other inner node (as they are
 *     topologically the same as the "unrooted" root node).
 *  *  A special side-effect of this implementation: If the iterator is started on a leaf (unusual,
 *     but can be done), this node is visited as the very first one.
 * %
 * /
template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class IteratorInorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef IteratorInorder<LinkPointerType, NodePointerType, EdgePointerType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorInorder (LinkPointerType link) : start_(link)
    {
        // the end iterator is created by handing over a nullptr to this constructor, so we first
        // need to check for this:
        if (link) {
            link_ = link;

            // if we start the iterator on an inner node, we actually need to add ALL children
            // (which means all its neighbor nodes) of this node to the stack, not only the ones
            // that point away from the given link of the node (which is what push_front_children
            // does). this means, we also need to add the one link that is given as argument here.
            if (link->outer() != link) {
                // this loop just takes care to add the given node first, and then the other
                // children. this is more or less beaty only, as it just makes sure that this link's
                // path is taken first, instead of last.
                while (link->next() != link_) {
                    link = link->next();
                }

                // now add the needed extra child
                stack_.push_front(link->outer());
            }

            // in any case (no matter whether we start at the root or some other node), we run this
            // loop. it goes down the tree until a leaf is encountered. this leaf will be the first
            // node that the iterator points to. while going there, we add yet-to-be-visited nodes
            // to the stack.
            while (link->is_inner()) {
                push_front_children(link);
                link = link->next()->outer();
            }
        }
        link_ = link;
    }

    ~IteratorInorder() = default;

    IteratorInorder( IteratorInorder const& ) = default;
    IteratorInorder( IteratorInorder&& )      = default;

    IteratorInorder& operator= ( IteratorInorder const& ) = default;
    IteratorInorder& operator= ( IteratorInorder&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

// TODO Tree iterator inorder is NOT WORKING!!!

    self_type operator ++ ()
    {
        std::string m = "  ";
        if (link_) {
           m += "@" + link_->node()->name + "   ";
        }
        for (LinkPointerType p : stack_) {
            m += p->node()->name + " ";
        }
        LOG_DBG2 << m;

        if (link_ == stack_.front()) {
            while (!stack_.empty() && link_ == stack_.front()) {
                link_ = link_->outer()->next();
                stack_.pop_front();
            }
            while (link_->outer() == link_) {
                link_ = link_->next();
            }
            if (stack_.empty()) {
                link_ = nullptr;
            }
        } else {
            assert(link_->outer() == stack_.front());
            link_ = link_->outer();
            while (link_->is_inner()) {
                push_front_children(link_);
                link_ = link_->next()->outer();
            }
            while (link_->outer() == link_) {
                link_ = link_->next();
            }
        }

        m = "  ";
        if (link_) {
           m += "@" + link_->node()->name + "   ";
        }
        for (LinkPointerType p : stack_) {
            m += p->node()->name + " ";
        }
        LOG_DBG2 << m;

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

    LinkPointerType link() const
    {
        return link_;
    }

    NodePointerType node() const
    {
        return link_->node();
    }

    EdgePointerType edge() const
    {
        return link_->edge();
    }

    LinkPointerType start_link() const
    {
        return start_;
    }

    NodePointerType start_node() const
    {
        return start_->node();
    }

private:

    void push_front_children(LinkPointerType link)
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a preorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<LinkPointerType> tmp;
        LinkPointerType c = link->next();
        while (c != link) {
            tmp.push_front(c->outer());
            c = c->next();
        }
        for (LinkPointerType l : tmp) {
            stack_.push_front(l);
        }
    }

    LinkPointerType             link_;
    LinkPointerType const       start_;
    std::deque<LinkPointerType> stack_;
};

*/

} // namespace tree
} // namespace genesis

#endif // include guard
