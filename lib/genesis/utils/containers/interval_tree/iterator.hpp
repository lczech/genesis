#ifndef GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/*
    The code below is adapted from https://github.com/5cript/interval-tree
    which was published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal).
    We modified the code to fit our needs and formatting, but the functionality is the same.
 */

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <cassert>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "genesis/utils/containers/interval_tree/fwd.hpp"
#include "genesis/utils/containers/interval_tree/interval.hpp"
#include "genesis/utils/containers/interval_tree/functions.hpp"
#include "genesis/utils/containers/interval_tree/node.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Interval Iterator
// =================================================================================================

/**
 * @brief Iterate the Interval%s stored in an IntervalTree.
 *
 * The implementation is based on [interval-tree](https://github.com/5cript/interval-tree),
 * which is published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal);
 * see our @link supplement_acknowledgements_code_reuse_interval_tree Acknowledgements@endlink
 * for further details, and see Interval, IntervalTreeNode, and IntervalTree for the other
 * classes based on the original code.
 */
template< typename NodeType, bool is_const = true >
class IntervalTreeIterator
    : public std::forward_iterator_tag
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

    using data_type      = typename NodeType::data_type;
    using numerical_type = typename NodeType::numerical_type;
    using interval_kind  = typename NodeType::interval_kind;
    using interval_type  = typename NodeType::interval_type;

    using node_type  = NodeType;
    using tree_type  = IntervalTree<data_type, numerical_type, interval_kind>;

    using self_type  = IntervalTreeIterator<NodeType, is_const>;
    using value_type = NodeType;

    using node_ptr_t = typename std::conditional< is_const, node_type const*, node_type* >::type;
    using owner_type = typename std::conditional< is_const, tree_type const*, tree_type* >::type;

    friend IntervalTree<data_type, numerical_type, interval_kind>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

private:

    IntervalTreeIterator( node_ptr_t node, owner_type owner )
        : node_{node}
        , owner_{owner}
    {}

public:

    ~IntervalTreeIterator() = default;

    IntervalTreeIterator( IntervalTreeIterator const& ) = default;
    IntervalTreeIterator( IntervalTreeIterator&& )      = default;

    IntervalTreeIterator& operator= ( IntervalTreeIterator const& ) = default;
    IntervalTreeIterator& operator= ( IntervalTreeIterator&& )      = default;

    // -------------------------------------------------------------------------
    //     Operators
    // -------------------------------------------------------------------------

public:

    bool operator!=(IntervalTreeIterator const& other) const
    {
        return node_ != other.node_;
    }

    bool operator==(IntervalTreeIterator const& other) const
    {
        return node_ == other.node_;
    }

    self_type& operator++()
    {
        if( !node_ ) {
            node_ = owner_->root_;
            if( !node_ ) {
                return *this;
            }
            while( node_->left_ ){
                node_ = node_->left_;
            }
        }

        if( node_->right_ ) {
            node_ = node_->right_;
            while( node_->left_ ){
                node_ = node_->left_;
            }
        } else {
            auto* parent = node_->parent_;
            while( parent != nullptr && node_ == parent->right_ )  {
                node_ = parent;
                parent = parent->parent_;
            }
            node_ = parent;
        }

        return *this;
    }

    self_type operator++(int)
    {
        auto cpy = *this;
        operator++();
        return cpy;
    }

    interval_type const& operator*() const
    {
        if( node_ ) {
            return node_->interval();
        } else {
            throw std::out_of_range("Dereferencing IntervalTreeIterator out of bounds.");
        }
    }

    value_type const* operator->() const
    {
        return node_;
    }

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return an iterator to the parent of this node.
     *
     * Will equal std::end(tree) if there is no parent node.
     */
    self_type parent() const
    {
        if (node_)
            return {node_->parent_, owner_};
        else
            throw std::out_of_range("interval_tree_iterator out of bounds");
    }

    /**
     * @brief Continue down the left side of this node.
     *
     * Will equal std::end(tree) if there is no left node.
     */
    self_type left() const
    {
        if (node_)
            return {node_->left_, owner_};
        else
            throw std::out_of_range("interval_tree_iterator out of bounds");
    }

    /**
     * @brief Continue down the right side of this node.
     *
     * Will equal std::end(tree) if there is no right node.
     */
    self_type right() const
    {
        if (node_)
            return {node_->right_, owner_};
        else
            throw std::out_of_range("interval_tree_iterator out of bounds");
    }

    /**
     * @brief Return the max property of the node.
     */
    numerical_type max() const
    {
        return node_->max();
    }

    /**
     * @brief Return the color of the node.
     */
    RedBackColor color() const
    {
        return node_->color();
    }

    interval_type interval() const
    {
        return node_->interval();
    }

    // -------------------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------------------

private:

    node_ptr_t node_;
    owner_type owner_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
