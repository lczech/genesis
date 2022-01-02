#ifndef GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_H_
#define GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_H_

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

#include <algorithm>
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
#include "genesis/utils/containers/interval_tree/iterator.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Interval Tree
// =================================================================================================

/**
 * @brief Interval tree that enables storing and querying intervals, each containing some data.
 *
 * The implementation takes a red black tree as its base to inhibit degeneration to linked lists.
 * It is based on [interval-tree](https://github.com/5cript/interval-tree),
 * which is published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal);
 * see our @link supplement_acknowledgements_code_reuse_interval_tree Acknowledgements@endlink
 * for further details, and see Interval, IntervalTreeNode, and IntervalTreeIterator for the other
 * classes based on the original code.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
class IntervalTree
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

    using self_type = tree_type;

    using iterator       = IntervalTreeIterator<node_type, false>;
    using const_iterator = IntervalTreeIterator<node_type, true>;

    friend IntervalTreeIterator<node_type, false>;
    friend IntervalTreeIterator<node_type, true>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    IntervalTree()
        : root_{ nullptr }
        , size_{ 0 }
    {}

    ~IntervalTree()
    {
        clear();
    }

    IntervalTree(IntervalTree const& other)
        : root_{nullptr}
        , size_{0}
    {
        operator=(other);
    }

    IntervalTree( IntervalTree&& ) = default;

    IntervalTree& operator=(IntervalTree const& other)
    {
        if( !empty() ) {
            clear();
        }

        if (other.root_ != nullptr) {
            root_ = copy_tree_( other.root_, nullptr );
        }

        size_ = other.size_;
        return *this;
    }

    IntervalTree& operator= ( IntervalTree&& ) = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the size of the object, that is, the number of Intervals contained.
     */
    size_t size() const
    {
        return size_;
    }

    /**
     * @brief Return wether or not the tree is empty.
     */
    bool empty() const noexcept
    {
        return root_ == nullptr;
    }

    /**
     * @brief Return the root node from this tree.
     */
    iterator root()
    {
        return {root_, this};
    }

    /**
     * @brief Return the root node from this tree.
     */
    const_iterator root() const
    {
        return {root_, this};
    }

    /**
     * @brief Return the lowest low value of the contained Interval%s.
     *
     * This is the leftmost position of all intervals.
     */
    numerical_type lowest() const
    {
        if( ! root_ ) {
            throw std::runtime_error( "Cannot call lowest() on empty IntervalTree" );
        }
        return minimum_( root_ )->low();
    }

    /**
     * @brief Return the highest high value of the contained Interval%s.
     *
     * This is the rightmost position of all intervals.
     */
    numerical_type highest() const
    {
        if( ! root_ ) {
            throw std::runtime_error( "Cannot call highest() on empty IntervalTree" );
        }
        return root_->max();
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Helper function to get the node for the begin() iterator.
     */
    node_type* get_begin_node_() const
    {
        // Empty tree.
        if( !root_ ) {
            return nullptr;
        }

        // Find leftmost leaf.
        auto* iter = root_;
        while (iter->left_) {
            iter = iter->left_;
        }
        return iter;
    }

public:

    iterator begin()
    {
        return iterator{ get_begin_node_(), this };
    }

    iterator end()
    {
        return {nullptr, this};
    }

    const_iterator cbegin() const
    {
        return const_iterator{ get_begin_node_(), this };
    }

    const_iterator cend() const
    {
        return const_iterator{nullptr, this};
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

    // -------------------------------------------------------------------------
    //     Find
    // -------------------------------------------------------------------------

    /**
     * @brief Find the first exact match.
     *
     * @param ival The interval to find an exact match for within the tree.
     * @param compare A comparison function to use.
     */
    template <typename CompareFunctionT>
    iterator find(interval_type const& ival, CompareFunctionT const& compare)
    {
        if (root_ == nullptr) {
            return end();
        }
        return iterator{ find_(root_, ival, compare), this };
    }

    /**
     * @brief Find the first exact match.
     *
     * @param ival The interval to find an exact match for within the tree.
     * @param compare A comparison function to use.
     */
    template <typename CompareFunctionT>
    const_iterator find(interval_type const& ival, CompareFunctionT const& compare) const
    {
        if (root_ == nullptr) {
            return end();
        }
        return const_iterator{find_(root_, ival, compare), this};
    }

    /**
     * @brief Find the first exact match.
     *
     * @param ival The interval to find an exact match for within the tree.
     */
    iterator find(interval_type const& ival)
    {
        return find(
            ival,
            []( interval_type const& lhs, interval_type const& rhs ) {
                return lhs == rhs;
            }
        );
    }

    /**
     * @brief Find the first exact match.
     *
     * @param ival The interval to find an exact match for within the tree.
     */
    const_iterator find(interval_type const& ival) const
    {
        return find(
            ival,
            []( interval_type const& lhs, interval_type const& rhs ) {
                return lhs == rhs;
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Find All
    // -------------------------------------------------------------------------

    /**
     * @brief Find all exact matches and execute a function for each of them.
     */
    template <typename FunctionT, typename CompareFunctionT>
    void find_all(
        interval_type const& ival,
        FunctionT const& on_find,
        CompareFunctionT const& compare
    ) {
        if (root_ == nullptr) {
            return;
        }
        find_all_<self_type, iterator>(this, root_, ival, on_find, compare);
    }

    /**
     * @brief Find all exact matches and execute a function for each of them.
     */
    template <typename FunctionT, typename CompareFunctionT>
    void find_all(
        interval_type const& ival,
        FunctionT const& on_find,
        CompareFunctionT const& compare
    ) const {
        if (root_ == nullptr) {
            return;
        }
        find_all_<self_type, const_iterator>(this, root_, ival, on_find, compare);
    }

    /**
     * @brief Find all exact matches and execute a function for each of them.
     */
    template <typename FunctionT>
    void find_all(interval_type const& ival, FunctionT const& on_find)
    {
        find_all(
            ival,
            on_find,
            []( interval_type const& lhs, interval_type const& rhs ) {
                return lhs == rhs;
            }
        );
    }

    /**
     * @brief Find all exact matches and execute a function for each of them.
     */
    template <typename FunctionT>
    void find_all(interval_type const& ival, FunctionT const& on_find) const
    {
        find_all(
            ival,
            on_find,
            []( interval_type const& lhs, interval_type const& rhs ) {
                return lhs == rhs;
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Find In Subtree
    // -------------------------------------------------------------------------

    /**
     * @brief Find the next exact match EXCLUDING from.
     *
     * @param from The iterator to search from EXCLUSIVE!
     * @param ival The interval to find an exact match for within the tree.
     * @param compare A comparison function to use.
     */
    template <typename CompareFunctionT>
    iterator find_next_in_subtree(
        iterator from,
        interval_type const& ival,
        CompareFunctionT const& compare
    ) {
        if (root_ == nullptr) {
            return end();
        }
        return iterator{find_ex_(from.node_, ival, compare), this};
    }

    template <typename CompareFunctionT>
    const_iterator find_next_in_subtree(
        iterator from,
        interval_type const& ival,
        CompareFunctionT const& compare
    ) const {
        if (root_ == nullptr) {
            return end();
        }
        return iterator{find_ex_(from.node_, ival, compare), this};
    }

    /**
     * @brief Find the next exact match EXCLUDING from.
     *
     * @param from The iterator to search from, EXCLUSIVE!
     * @param ival The interval to find an exact match for within the tree.
     * @param compare A comparison function to use.
     */
    iterator find_next_in_subtree(iterator from, interval_type const& ival)
    {
        return find_next_in_subtree(
            from,
            ival,
            []( interval_type const& lhs, interval_type const& rhs ) {
                return lhs == rhs;
            }
        );
    }

    const_iterator find_next_in_subtree(iterator from, interval_type const& ival) const
    {
        return find_next_in_subtree(
            from,
            ival,
            []( interval_type const& lhs, interval_type const& rhs ) {
                return lhs == rhs;
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Overlap Find
    // -------------------------------------------------------------------------

    /**
     * @brief Find the first interval that overlaps with ival.
     *
     * @param ival The interval to find an overlap for within the tree.
     * @param exclusive Exclude edges?
     */
    iterator overlap_find(interval_type const& ival, bool exclusive = false)
    {
        if (root_ == nullptr) {
            return end();
        }
        if (exclusive) {
            return iterator{overlap_find_<true>(root_, ival), this};
        } else {
            return iterator{overlap_find_<false>(root_, ival), this};
        }
    }

    const_iterator overlap_find(interval_type const& ival, bool exclusive = false) const
    {
        if (root_ == nullptr) {
            return end();
        }
        if (exclusive) {
            return const_iterator{overlap_find_<true>(root_, ival), this};
        } else {
            return const_iterator{overlap_find_<false>(root_, ival), this};
        }
    }

    /**
     * @brief Find the first interval that overlaps with the given position.
     */
    iterator overlap_find( numerical_type position )
    {
        interval_type ival{ position, position };
        return overlap_find( ival, false );
    }

    /**
     * @brief Find the first interval that overlaps with the given position.
     */
    const_iterator overlap_find( numerical_type position ) const
    {
        interval_type ival{ position, position };
        return overlap_find( ival, false );
    }

    // -------------------------------------------------------------------------
    //     Overlap Find All
    // -------------------------------------------------------------------------

    /**
     * @brief Find all intervals that overlap with a given interval.
     *
     * @param ival The interval to find an overlap for within the tree.
     * @param exclusive Exclude edges?
     */
    template <typename FunctionT>
    void overlap_find_all(
        interval_type const& ival,
        FunctionT const& on_find,
        bool exclusive = false
    ) {
        if (root_ == nullptr) {
            return;
        }
        if (exclusive) {
            overlap_find_all_<self_type, true, iterator, FunctionT>(
                this, root_, ival, on_find
            );
        } else {
            overlap_find_all_<self_type, false, iterator, FunctionT>(
                this, root_, ival, on_find
            );
        }
    }

    template <typename FunctionT>
    void overlap_find_all(
        interval_type const& ival,
        FunctionT const& on_find,
        bool exclusive = false
    ) const {
        if (root_ == nullptr) {
            return;
        }
        if (exclusive) {
            overlap_find_all_<self_type, true, const_iterator, FunctionT>(
                this, root_, ival, on_find
            );
        } else {
            overlap_find_all_<self_type, false, const_iterator, FunctionT>(
                this, root_, ival, on_find
            );
        }
    }

    /**
     * @brief Find all intervals that overlap with a given position.
     */
    template <typename FunctionT>
    void overlap_find_all(
        numerical_type position,
        FunctionT const& on_find
    ) {
        interval_type ival{ position, position };
        overlap_find_all( ival, on_find, false );
    }

    /**
     * @brief Find all intervals that overlap with a given position.
     */
    template <typename FunctionT>
    void overlap_find_all(
        numerical_type position,
        FunctionT const& on_find
    ) const {
        interval_type ival{ position, position };
        overlap_find_all( ival, on_find, false );
    }

    // -------------------------------------------------------------------------
    //     Overlap Find In Subtree
    // -------------------------------------------------------------------------

    /**
     * @brief Find the next interval that overlaps with ival
     *
     * @param from The iterator to start from, EXCLUSIVE!
     * @param ival The interval to find an overlap for within the tree.
     * @param exclusive Exclude edges?
     */
    iterator overlap_find_next_in_subtree(
        iterator from,
        interval_type const& ival,
        bool exclusive = false
    ) {
        if (root_ == nullptr) {
            return end();
        }
        return iterator{overlap_find_ex_(from.node_, ival, exclusive), this};
    }

    const_iterator overlap_find_next_in_subtree(
        const_iterator from,
        interval_type const& ival,
        bool exclusive = false
    ) const {
        if (root_ == nullptr) {
            return end();
        }
        return const_iterator {overlap_find_ex_(from.node_, ival, exclusive), this};
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Remove all nodes from this tree.
     */
    void clear()
    {
        if( !root_ ) {
            return;
        }

        clear_( root_ );
        root_ = nullptr;
        size_ = 0;

        // This is the original implementation, which does a full rebalancing for every
        // deleted node... quite wasteful.
        // for( auto i = std::begin(*this); i != std::end(*this); ) {
        //     i = erase(i);
        // }
    }

    /**
     * @brief Insert an Interval into the tree.
     *
     * Copies the interval.
     */
    iterator insert( interval_type&& ival )
    {
        // Prepare nodes
        node_type* z = new node_type(nullptr, std::forward <interval_type&&> (ival));
        node_type* y = nullptr;
        node_type* x = root_;

        // Find the leaf that we want to attach the node to.
        while (x) {
            y = x;
            if (z->interval_.low() < x->interval_.low()) {
                x = x->left_;
            } else {
                x = x->right_;
            }
        }

        // Attach!
        z->parent_ = y;
        if (!y){
            root_ = z;
        } else if (z->interval_.low() < y->interval_.low()) {
            y->left_ = z;
        } else {
            y->right_ = z;
        }
        z->color_ = RedBackColor::kRed;

        // Fix red-black properties, get the interval tree max value, and return the new node.
        insert_fixup_(z);
        recalculate_max_(z);
        ++size_;
        return {z, this};
    }

    /**
     * @brief Insert an Interval into the tree.
     *
     * Moves the interval.
     */
    iterator insert( interval_type const& ival )
    {
        return insert( interval_type{ ival } );
    }

    /**
     * @brief Insert an interval into the tree if no other interval overlaps it.
     * Otherwise merge the interval with the being overlapped.
     *
     * @param ival The interval
     * @param exclusive Exclude borders.
     */
    iterator insert_overlap(interval_type const& ival, bool exclusive = false)
    {
        return insert_overlap( ival, data_type{}, exclusive );
    }

    /**
     * @brief Insert an interval into the tree if no other interval overlaps it.
     * Otherwise merge the interval with the being overlapped, and use @p data for this interval.
     *
     * @param ival The interval
     * @param data The data to be used if a new interval is created.
     * @param exclusive Exclude borders.
     */
    iterator insert_overlap(interval_type const& ival, data_type const& data, bool exclusive = false)
    {
        return insert_overlap( ival, data_type{ data }, exclusive );
    }

    /**
     * @brief Insert an interval into the tree if no other interval overlaps it.
     * Otherwise merge the interval with the being overlapped, and use @p data for this interval.
     *
     * @param ival The interval
     * @param data The data to be used if a new interval is created.
     * @param exclusive Exclude borders.
     */
    iterator insert_overlap(interval_type const& ival, data_type&& data, bool exclusive = false)
    {
        auto iter = overlap_find(ival, exclusive);
        if (iter == end()) {
            return insert(ival);
        } else {
            auto merged = join( iter->interval(), ival, std::move( data ));
            erase(iter);
            return insert(merged);
        }
    }

    /**
     * @brief Erase the element pointed to be iter.
     */
    iterator erase( iterator iter )
    {
        if (!iter.node_){
            throw std::out_of_range("Cannot erase end iterator");
        }

        auto next = iter;
        ++next;

        node_type* y;
        if (!iter.node_->left_ || !iter.node_->right_) {
            y = iter.node_;
        } else {
            y = successor_(iter.node_);
        }

        node_type* x;
        if (y->left_) {
            x = y->left_;
        } else {
            x = y->right_;
        }

        if (x) {
            x->parent_ = y->parent_;
        }

        auto* x_parent = y->parent_;
        if (!y->parent_) {
            root_ = x;
        } else if (y->is_left()) {
            y->parent_->left_ = x;
        } else {
            y->parent_->right_ = x;
        }

        if (y != iter.node_) {
            iter.node_->interval_ = y->interval_;
            iter.node_->max_ = y->max_;
            recalculate_max_(iter.node_);
        }

        if (x && x->color_ == RedBackColor::kRed) {
            if (x_parent)
                erase_fixup_(x, x_parent, y->is_left());
            else
                x->color_ = RedBackColor::kBlack;
        }

        delete y;

        --size_;
        return next;
    }

    // -------------------------------------------------------------------------
    //     Flatten and Punch
    // -------------------------------------------------------------------------

    /**
     * @brief Flatten the tree.
     *
     *  Merges all overlapping intervals by erasing overlapping intervals and reinserting
     *  the merged interval. All resulting interval data is default constructed, and all data
     *  in the current intervals is lost.
     */
    IntervalTree& flatten()
    {
        *this = flatten_copy();
        return *this;
    }

    /**
     * @brief Flatten the tree, but returns it as a copy.
     */
    IntervalTree flatten_copy()
    {
        IntervalTree fresh;
        for (auto i = begin(), e = end(); i != e; ++i) {
            fresh.insert_overlap(*i);
        }
        return fresh;
    }

    /**
     * @brief Create an interval tree that contains all gaps between the intervals as intervals.
     *
     * Only works with flattened trees, that is, trees without overlapping intervals.
     * This is equivalent to the other punch overload with `ival = [tree_lowest, tree_highest]`
     */
    IntervalTree punch() const
    {
        if (empty()) {
            return {};
        }
        auto min = std::begin(*this)->interval().low();
        auto max = root_->max_;
        return punch({min, max});
    }

    /**
     * @brief Create an interval tree that contains all gaps between the intervals as intervals.
     *
     * Only works with flattened trees, that is, trees without overlapping intervals.
     * Removes all intervals from the given interval and produces a tree that contains the
     * remaining intervals.
     */
    IntervalTree punch(interval_type const& ival) const
    {
        if (empty()) {
            return {};
        }

        IntervalTree result;
        auto i = std::begin(*this);
        if (ival.low() < i->interval().low()) {
            result.insert({ival.low(), i->interval().low()});
        }

        for( auto e = end(); i != e; ++i ){
            auto next = i; ++next;
            if (next != e) {
                result.insert({i->interval().high(), next->interval().low()});
            } else {
                break;
            }
        }

        if( i != end() && i->interval().high() < ival.high()) {
            result.insert({i->interval().high(), ival.high()});
        }
        return result;
    }

    // -------------------------------------------------------------------------
    //     Private Functions
    // -------------------------------------------------------------------------

private:

    void clear_( node_type* node )
    {
        if( node->left_ ) {
            clear_( node->left_ );
        }
        if( node->right_ ) {
            clear_( node->right_ );
        }
        delete node;
    }

    node_type* copy_tree_(node_type* root, node_type* parent)
    {
        if( !root ) {
            return nullptr;
        }

        auto* cpy = new node_type(parent, root->interval());
        cpy->color_ = root->color_;
        cpy->max_ = root->max_;
        cpy->left_ = copy_tree_(root->left_, cpy);
        cpy->right_ = copy_tree_(root->right_, cpy);
        return cpy;
    };

    // -------------------------------------------------------------------------
    //     Find Implementations
    // -------------------------------------------------------------------------

    template <typename ComparatorFunctionT>
    node_type* find_(
        node_type* ptr, interval_type const& ival, ComparatorFunctionT const& compare
    ) const {
        if( compare(ptr->interval(), ival)) {
            return ptr;
        } else {
            return find_ex_(ptr, ival, compare);
        }
    }

    // excludes ptr
    template <typename ComparatorFunctionT>
    node_type* find_ex_(
        node_type* ptr, interval_type const& ival, ComparatorFunctionT const& compare
    ) const {
        if (ptr->left_ && ival.high() <= ptr->left_->max()) {
            // no right? can only continue left
            if (!ptr->right_ || ival.low() > ptr->right_->max()) {
                return find_(ptr->left_, ival, compare);
            }

            auto* res = find_(ptr->left_, ival, compare);
            if (res != nullptr) {
                return res;
            }
        }

        if (ptr->right_ && ival.high() <= ptr->right_->max()) {
            if (!ptr->left_ || ival.low() > ptr->left_->max()) {
                return find_(ptr->right_, ival, compare);
            }

            auto* res = find_(ptr->right_, ival, compare);
            if (res != nullptr) {
                return res;
            }
        }
        return nullptr;
    }

    template <typename ThisType, typename IteratorT, typename FunctionT, typename ComparatorFunctionT>
    static bool find_all_(
        typename std::conditional<std::is_same<IteratorT, iterator>::value, ThisType, ThisType const>::type* self,
        node_type* ptr,
        interval_type const& ival,
        FunctionT const& on_find,
        ComparatorFunctionT const& compare
    ) {
        if (compare(ptr->interval(), ival)) {
            if (!on_find(IteratorT{ptr, self})) {
                return false;
            }
        }
        if (ptr->left_ && ival.high() <= ptr->left_->max()) {
            // no right? can only continue left
            if (!ptr->right_ || ival.low() > ptr->right_->max()) {
                return find_all_<ThisType, IteratorT>(self, ptr->left_, ival, on_find, compare);
            }

            if (!find_all_<ThisType, IteratorT>(self, ptr->left_, ival, on_find, compare)) {
                return false;
            }
        }
        if (ptr->right_ && ival.high() <= ptr->right_->max()) {
            if (!ptr->left_ || ival.low() > ptr->left_->max()) {
                return find_all_<ThisType, IteratorT>(self, ptr->right_, ival, on_find, compare);
            }

            if (!find_all_<ThisType, IteratorT>(self, ptr->right_, ival, on_find, compare)) {
                return false;
            }
        }
        return true;
    }

    // -------------------------------------------------------------------------
    //     Overlap Implementations
    // -------------------------------------------------------------------------

    template <bool Exclusive>
    node_type* overlap_find_(node_type* ptr, interval_type const& ival) const
    {
        if (Exclusive) {
            if (ptr->interval().overlaps_exclusive(ival)) {
                return ptr;
            }
        } else {
            if (ptr->interval().overlaps(ival)){
                return ptr;
            }
        }
        return overlap_find_ex_<Exclusive>(ptr, ival);
    }

    // excludes ptr
    template <bool Exclusive>
    node_type* overlap_find_ex_(node_type* ptr, interval_type const& ival) const
    {
        if (ptr->left_ && ptr->left_->max() >= ival.low()) {
            // no right? can only continue left
            // or upper bounds higher than what is contained right? continue left.
            if (!ptr->right_ || ival.low() > ptr->right_->max()) {
                return overlap_find_<Exclusive>(ptr->left_, ival);
            }

            auto* res = overlap_find_<Exclusive>(ptr->left_, ival);
            if (res != nullptr) {
                return res;
            }
        }

        if (ptr->right_ && ptr->right_->max() >= ival.low()) {
            if (!ptr->left_ || ival.low() > ptr->left_->max()) {
                return overlap_find_<Exclusive>(ptr->right_, ival);
            }

            auto* res = overlap_find_<Exclusive>(ptr->right_, ival);
            if (res != nullptr) {
                return res;
            }
        }
        return nullptr;
    }

    template <typename ThisType, bool Exclusive, typename IteratorT, typename FunctionT>
    static bool overlap_find_all_(
        typename std::conditional<
            std::is_same<IteratorT, iterator>::value, ThisType, ThisType const
        >::type* self,
        node_type* ptr,
        interval_type const& ival,
        FunctionT const& on_find
    ) {
        if (Exclusive) {
            if (ptr->interval().overlaps_exclusive(ival)) {
                if (!on_find(IteratorT{ptr, self})) {
                    return false;
                }
            }
        } else {
            if (ptr->interval().overlaps(ival)) {
                if (!on_find(IteratorT{ptr, self})) {
                    return false;
                }
            }
        }
        if (ptr->left_ && ptr->left_->max() >= ival.low()) {
            // no right? can only continue left
            // or interval low is bigger than max of right branch.
            if (!ptr->right_ || ival.low() > ptr->right_->max()) {
                return overlap_find_all_<ThisType, Exclusive, IteratorT, FunctionT>(
                    self, ptr->left_, ival, on_find
                );
            }

            if( !overlap_find_all_<ThisType, Exclusive, IteratorT, FunctionT>(
                    self, ptr->left_, ival, on_find
                )
            ) {
                return false;
            }
        }
        if (ptr->right_ && ptr->right_->max() >= ival.low()) {
            if (!ptr->left_ || ival.low() > ptr->right_->max()) {
                return overlap_find_all_<ThisType, Exclusive, IteratorT, FunctionT>(
                    self, ptr->right_, ival, on_find
                );
            }

            if( !overlap_find_all_<ThisType, Exclusive, IteratorT, FunctionT>(
                    self, ptr->right_, ival, on_find
                )
            ) {
                return false;
            }
        }
        return true;
    }

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

    node_type* successor_(node_type* node)
    {
        if (node->right_) {
            return minimum_(node->right_);
        }
        auto* y = node->parent_;
        while (y && node == y->right_) {
            node = y;
            y = y->parent_;
        }
        return y;
    }

    bool is_descendant_(iterator par, iterator desc)
    {
        auto p = desc->parent_;
        for (; p && p != par.node_; p = p->parent_) {}
        return p != nullptr;
    }

    /**
     *  Get leftest of x.
     */
    node_type* minimum_(node_type* x) const
    {
        while (x->left_) {
            x = x->left_;
        }
        return x;
    }

    void recalculate_max_(node_type* reacalculation_root)
    {
        auto* p = reacalculation_root;
        while (p && p->max_ <= reacalculation_root->max_) {
            if (p->left_ && p->left_->max_ > p->max_) {
                p->max_ = p->left_->max_;
            }
            if (p->right_ && p->right_->max_ > p->max_) {
                p->max_ = p->right_->max_;
            }
            p = p->parent_;
        }
    }

    // /**
    //  *  Set v inplace of u. Does not delete u.
    //  *  Creates orphaned nodes. A transplant call must be succeeded by delete calls.
    //  */
    // void transplant(node_type* u, node_type* v)
    // {
    //     if (u->is_root())
    //         root_ = v;
    //     else if (u->is_left())
    //         u->parent_->left_ = v;
    //     else
    //         u->parent_->right_ = v;
    //     if (v)
    //         v->parent_ = u->parent_;
    // }

    // -------------------------------------------------------------------------
    //     Modifiers Implementations
    // -------------------------------------------------------------------------

    void left_rotate_(node_type* x)
    {
        auto* y = x->right_;
        x->right_ = y->left_;
        if (y->left_) {
            y->left_->parent_ = x;
        }

        y->parent_ = x->parent_;
        if (!x->parent_) {
            root_ = y;
        } else if (x->is_left()) {
            x->parent_->left_ = y;
        } else {
            x->parent_->right_ = y;
        }

        y->left_ = x;
        x->parent_ = y;

        // max fixup
        if (x->left_ && x->right_) {
            x->max_ = std::max(x->interval_.high(), std::max(x->left_->max_, x->right_->max_));
        } else if (x->left_) {
            x->max_ = std::max(x->interval_.high(), x->left_->max_);
        } else if (x->right_) {
            x->max_ = std::max(x->interval_.high(), x->right_->max_);
        } else {
            x->max_ = x->interval_.high();
        }

        if (y->right_) {
            y->max_ = std::max(y->interval_.high(), std::max(y->right_->max_, x->max_));
        } else {
            y->max_ = std::max(y->interval_.high(), x->max_);
        }
    }

    void right_rotate_(node_type* y)
    {
        auto* x = y->left_;
        y->left_ = x->right_;

        if (x->right_) {
            x->right_->parent_ = y;
        }

        x->parent_= y->parent_;
        if (!y->parent_) {
            root_ = x;
        } else if (y->is_left()) {
            y->parent_->left_ = x;
        } else {
            y->parent_->right_ = x;
        }

        x->right_ = y;
        y->parent_ = x;

        // max fixup
        if (y->left_ && y->right_) {
            y->max_ = std::max(y->interval_.high(), std::max(y->left_->max_, y->right_->max_));
        } else if (y->left_) {
            y->max_ = std::max(y->interval_.high(), y->left_->max_);
        } else if (y->right_) {
            y->max_ = std::max(y->interval_.high(), y->right_->max_);
        } else {
            y->max_ = y->interval_.high();
        }

        if (x->left_) {
            x->max_ = std::max(x->interval_.high(), std::max(x->left_->max_, y->max_));
        } else {
            x->max_ = std::max(x->interval_.high(), y->max_);
        }
    }

    void insert_fixup_(node_type* z)
    {
        while (z->parent_ && z->parent_->color_ == RedBackColor::kRed) {
            if (!z->parent_->parent_) {
                break;
            }
            if (z->parent_ == z->parent_->parent_->left_) {
                node_type* y = z->parent_->parent_->right_;
                if (y && y->color_ == RedBackColor::kRed) {
                    z->parent_->color_ = RedBackColor::kBlack;
                    y->color_ = RedBackColor::kBlack;
                    z->parent_->parent_->color_ = RedBackColor::kRed;
                    z = z->parent_->parent_;
                } else {
                    if (z == z->parent_->right_) {
                        z = z->parent_;
                        left_rotate_(z);
                    }
                    z->parent_->color_ = RedBackColor::kBlack;
                    z->parent_->parent_->color_ = RedBackColor::kRed;
                    right_rotate_(z->parent_->parent_);
                }
            } else {
                node_type* y = z->parent_->parent_->left_;
                if (y && y->color_ == RedBackColor::kRed) {
                    z->parent_->color_ = RedBackColor::kBlack;
                    y->color_ = RedBackColor::kBlack;
                    z->parent_->parent_->color_ = RedBackColor::kRed;
                    z = z->parent_->parent_;
                } else {
                    if (z->is_left()) {
                        z = z->parent_;
                        right_rotate_(z);
                    }
                    z->parent_->color_ = RedBackColor::kBlack;
                    z->parent_->parent_->color_ = RedBackColor::kRed;
                    left_rotate_(z->parent_->parent_);
                }
            }
        }
        root_->color_ = RedBackColor::kBlack;
    }

    void erase_fixup_(node_type* x, node_type* x_parent, bool y_is_left)
    {
        while (x != root_ && x->color_ == RedBackColor::kBlack) {
            node_type* w;
            if (y_is_left) {
                w = x_parent->right_;
                if (w->color_ == RedBackColor::kRed) {
                    w->color_ = RedBackColor::kBlack;
                    x_parent->color_ = RedBackColor::kRed;
                    left_rotate_(x_parent);
                    w = x_parent->right_;
                }

                if (
                    w->left_->color_ == RedBackColor::kBlack &&
                    w->right_->color_ == RedBackColor::kBlack
                ) {
                    w->color_ = RedBackColor::kRed;
                    x = x_parent;
                    x_parent = x->parent_;
                    y_is_left = (x == x_parent->left_);
                } else {
                    if (w->right_->color_ == RedBackColor::kBlack) {
                        w->left_->color_ = RedBackColor::kBlack;
                        w->color_ = RedBackColor::kRed;
                        right_rotate_(w);
                        w = x_parent->right_;
                    }

                    w->color_ = x_parent->color_;
                    x_parent->color_ = RedBackColor::kBlack;
                    if (w->right_) {
                        w->right_->color_ = RedBackColor::kBlack;
                    }

                    left_rotate_(x_parent);
                    x = root_;
                    x_parent = nullptr;
                }
            } else {
                w = x_parent->left_;
                if (w->color_ == RedBackColor::kRed) {
                    w->color_ = RedBackColor::kBlack;
                    x_parent->color_ = RedBackColor::kRed;
                    right_rotate_(x_parent);
                    w = x_parent->left_;
                }

                if (
                    w->right_->color_ == RedBackColor::kBlack &&
                    w->left_->color_ == RedBackColor::kBlack
                ) {
                    w->color_ = RedBackColor::kRed;
                    x = x_parent;
                    x_parent = x->parent_;
                    y_is_left = (x == x_parent->left_);
                } else {
                    if (w->left_->color_ == RedBackColor::kBlack) {
                        w->right_->color_ = RedBackColor::kBlack;
                        w->color_ = RedBackColor::kRed;
                        left_rotate_(w);
                        w = x_parent->left_;
                    }

                    w->color_ = x_parent->color_;
                    x_parent->color_ = RedBackColor::kBlack;
                    if (w->left_) {
                        w->left_->color_ = RedBackColor::kBlack;
                    }

                    right_rotate_(x_parent);
                    x = root_;
                    x_parent = nullptr;
                }
            }
        }

        x->color_ = RedBackColor::kBlack;
    }

private:

    node_type* root_;
    size_t size_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
