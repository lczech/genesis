#ifndef GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_NODE_H_
#define GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_NODE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

    This program is free software: you can kRedistribute it and/or modify
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

namespace genesis {
namespace utils {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Definition for Red-Black Tree coloring.
 *
 * Used in IntervalTree and IntervalTreeNode.
 */
enum class RedBackColor
{
    kInvalid,
    kRed,
    kBlack,
    kKDoubleBlack
};

// =================================================================================================
//     Interval Node
// =================================================================================================

/**
 * @brief Node in an IntervalTree.
 *
 * The implementation is based on [interval-tree](https://github.com/5cript/interval-tree),
 * which is published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal);
 * see our @link supplement_acknowledgements_code_reuse_interval_tree Acknowledgements@endlink
 * for further details, and see Interval, IntervalTreeIterator, and IntervalTree for the other
 * classes based on the original code.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
class IntervalTreeNode
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using data_type = DataType;
    using numerical_type = NumericalType;
    using interval_kind = IntervalKind;

    using interval_type = Interval<DataType, NumericalType, IntervalKind>;
    using node_type = IntervalTreeNode<DataType, NumericalType, IntervalKind>;
    using tree_type = IntervalTree<DataType, NumericalType, IntervalKind>;

    // using value_type = NumericalType;
    using self_type = node_type;

    // Make the tree and the const and non-const iterators friends.
    friend IntervalTree<DataType, NumericalType, IntervalKind>;
    friend IntervalTreeIterator<node_type, true>;
    friend IntervalTreeIterator<node_type, false>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

public:

    IntervalTreeNode( IntervalTreeNode* parent, interval_type const& interval )
        : IntervalTreeNode( parent, interval_type( interval ))
    {}

    IntervalTreeNode( IntervalTreeNode* parent, interval_type&& interval )
        : interval_{ std::move(interval) }
        , max_{ interval.high() }
        , parent_{ parent }
        , left_{}
        , right_{}
        , color_{ RedBackColor::kInvalid }
    {}

    ~IntervalTreeNode() = default;

    IntervalTreeNode( IntervalTreeNode const& ) = default;
    IntervalTreeNode( IntervalTreeNode&& )      = default;

    IntervalTreeNode& operator= ( IntervalTreeNode const& ) = default;
    IntervalTreeNode& operator= ( IntervalTreeNode&& )      = default;

    // -------------------------------------------------------------------------
    //     Operators
    // -------------------------------------------------------------------------

    interval_type const& interval() const
    {
        return interval_;
    }

    numerical_type max() const
    {
        return max_;
    }

    bool is_left() const noexcept
    {
        return this == parent_->left_;
    }

    bool is_right() const noexcept
    {
        return this == parent_->right_;
    }

    bool is_root() const noexcept
    {
        return !parent_;
    }

    /**
     *  Return the color of the node.
     */
    RedBackColor color() const
    {
        return color_;
    }

    /**
     *  Return the parent node up the tree.
     */
    IntervalTreeNode const* parent() const
    {
        return parent_;
    }

    /**
     *  Return the left node (readonly).
     */
    IntervalTreeNode const* left() const
    {
        return left_;
    }

    /**
     *  Return the right node (readonly).
     */
    IntervalTreeNode const* right() const
    {
        return right_;
    }

    /**
     * @brief Return the height of the node in the tree.
     *
     * This is computed as height = how many parents does it have.
     * The root has no parents and is therefor has height 0.
     */
    size_t height() const
    {
        size_t counter{0};
        for( auto* p = parent_; p != nullptr; p = p->parent_ ) {
            ++counter;
        }
        return counter;
    }

    /**
     * @brief Return the lower bound of the interval of this node
     */
    numerical_type low() const
    {
        return interval_.low();
    }

    /**
     * @brief Return the upper bound of the interval of this node
     */
    numerical_type high() const
    {
        return interval_.high();
    }

private:

    void set_interval( interval_type const& ival )
    {
        interval_ = ival;
    }

    void set_interval( interval_type && ival )
    {
        interval_ = std::move( ival );
    }

    // void kill() const noexcept
    // {
    //     auto* parent = parent_;
    //     if (is_left())
    //     {
    //         delete parent_->left_;
    //         parent->left_ = nullptr;
    //     }
    //     else
    //     {
    //         delete parent_->right_;
    //         parent->right_ = nullptr;
    //     }
    // }

private:

    // Store the actual interval that this node represents, as well as the max of all
    // intervals high() values, as needed by the Interval Tree data structure.
    interval_type interval_;
    numerical_type max_;

    // Our relatives.
    IntervalTreeNode* parent_;
    IntervalTreeNode* left_;
    IntervalTreeNode* right_;

    // We use a Red-Black tree as our balanced underlying data structure.
    RedBackColor color_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
