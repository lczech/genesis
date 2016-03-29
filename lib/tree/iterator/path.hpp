#ifndef GENESIS_TREE_ITERATOR_PATH_H_
#define GENESIS_TREE_ITERATOR_PATH_H_

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

#include <assert.h>
#include <deque>
#include <iterator>

namespace genesis {
namespace tree {

// =================================================================================================
//     Path Iterator
// =================================================================================================

/*

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class IteratorPath
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef IteratorPath<LinkPointerType, NodePointerType, EdgePointerType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPath (LinkPointerType from, LinkPointerType to) : start_(from), to_(to);
    {
        if (link) {
            stack_.push_back(link);
            if (link->outer() != link) {
                stack_.push_front(link->outer());
                link = link->outer();
            }
            while (link->is_inner()) {
                push_front_children(link);
                link = link->next()->outer();
            }
            assert(link == stack_.front());
            stack_.pop_front();
        }
        link_ = link;
    }

    ~IteratorPath() = default;

    IteratorPath( IteratorPath const& ) = default;
    IteratorPath( IteratorPath&& )      = default;

    IteratorPath& operator= ( IteratorPath const& ) = default;
    IteratorPath& operator= ( IteratorPath&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_ = nullptr;
        } else if (link_->outer()->next() == stack_.front()) {
            link_ = stack_.front();
            stack_.pop_front();
        } else {
            link_ = stack_.front();
            while (link_->is_inner()) {
                push_front_children(link_);
                link_ = link_->next()->outer();
            }
            assert(link_ == stack_.front());
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

    LinkPointerType from_link() const
    {
        return from_;
    }

    NodePointerType from_node() const
    {
        return from_->node();
    }

    LinkPointerType to_link() const
    {
        return to_;
    }

    NodePointerType to_node() const
    {
        return to_->node();
    }

private:

    LinkPointerType             link_;
    LinkPointerType             from_;
    LinkPointerType             to_;
    std::deque<LinkPointerType> stack_;
};

*/

} // namespace tree
} // namespace genesis

#endif // include guard
