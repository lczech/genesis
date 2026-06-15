#ifndef GENESIS_UTIL_CONTAINER_TOURNAMENT_TREE_H_
#define GENESIS_UTIL_CONTAINER_TOURNAMENT_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2026 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup util
 */

#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace genesis {
namespace util {
namespace container {

// =================================================================================================
//     Tournament Tree
// =================================================================================================

/**
 * @brief Tournament Tree (Loser Tree) for k-way merging of sorted sequences.
 *
 * This class implements a loser tree, a variant of a tournament tree that is commonly used
 * for an efficient k-way merge of multiple already-sorted sequences ("bins"), as for instance
 * needed when merging posting lists, or for external sorting of large data sets.
 *
 * Compared to a binary heap (as used by `std::push_heap`, `std::pop_heap`, and
 * `std::make_heap`), a loser tree needs fewer comparisons per element, as it only needs to
 * compare the previous winner of a sub-tree with its sibling on the way up to the root,
 * instead of re-comparing against all children. This advantage tends to show for a larger
 * number of bins `k`. In our tests, this is about 20x faster than the std::*_heap-based approach.
 *
 * Usage:
 *
 *     // Construct a tree with a fixed maximal capacity for the number of bins.
 *     TournamentTree<uint64_t> tree( max_bins );
 *
 *     // For each merge, populate the tree with the input sequences. The argument can be any
 *     // range of ranges of `T`, such as a `std::vector<std::vector<T>>`, or a
 *     // `std::vector<std::span<T const>>`, etc.
 *     tree.build( lists );
 *
 *     // Repeatedly pop the globally smallest element, until all bins are exhausted.
 *     while( ! tree.empty() ) {
 *         auto const& entry = tree.top();
 *         // entry.value contains the smallest value, entry.list_index its source list.
 *         tree.pop();
 *     }
 *
 * Note the difference in convention compared to `std::priority_queue`: Here, the `Compare`
 * type follows the same semantics as for `std::sort`, that is, with the default `std::less`,
 * the tree produces elements in ascending order (the smaller element "wins" and is returned
 * first). This is the natural convention for merging ascending sorted ranges, as opposed to
 * the inverted convention of `std::priority_queue`, where `std::less` yields a max-heap.
 *
 * For arithmetic @p T compared via `std::less<T>` or `std::greater<T>`, exhausted and padding
 * bins are marked using a sentinel value (either `numeric_limits<T>::max()` or `lowest()`,
 * whichever always loses under @p Compare), so that no extra "is this bin active" branching is
 * needed during comparisons. As a consequence, actual input values must not be equal to that
 * sentinel value, i.e., @p T must not contain `numeric_limits<T>::max()` for `std::less` or
 * `lowest()` for `std::greater` as a valid element. In debug builds, this precondition is
 * checked via `assert()`.
 *
 * This sentinel optimization relies on `max()`/`lowest()` being the actual extremal elements
 * under @p Compare, which only holds for `std::less<T>` and `std::greater<T>`. For any other
 * @p Compare (or non-arithmetic @p T), an explicit "is this bin active" flag is used instead;
 * this is a well-predicted branch, so the overhead is minor, and it is always correct regardless
 * of the comparator.
 *
 * @tparam T        Type of the elements to merge. Needs to be copyable, and comparable via
 *                   @p Compare.
 * @tparam Compare  Comparison functor, following `std::less` semantics (smaller element wins).
 */
template<typename T, typename Compare = std::less<T>>
class TournamentTree
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using value_type = T;
    using size_type  = std::size_t;

    /**
     * @brief Entry pointing to the current head element of one of the input bins.
     */
    struct BinEntry
    {
        /// Cached value of *cur, kept here to avoid an indirection during comparisons.
        /// For exhausted (or padding) bins, this is set to a "loses against everything" value.
        T value;

        /// Pointer to the current element of the bin's remaining range.
        T const* cur = nullptr;

        /// Pointer to the one-past-the-last element of the bin's remaining range.
        T const* end = nullptr;

        /// Index of the bin within the range passed to build(), i.e.,
        /// the index of the input list that this entry belongs to.
        size_type list_index = 0;

        bool active() const noexcept
        {
            return cur != end;
        }
    };

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Construct a tree with a fixed maximal number of bins.
     *
     * The internal storage is allocated once, for up to @p max_bins bins, and is reused
     * across repeated calls to build(). The number of bins passed to build() must not
     * exceed @p max_bins.
     */
    explicit TournamentTree( size_type max_bins )
        : num_leaves_( next_power_of_two_( max_bins ))
        , bins_( max_bins )
        , tree_( num_leaves_ )
        , size_( 0 )
    {}

    ~TournamentTree() = default;

    TournamentTree( TournamentTree const& ) = default;
    TournamentTree( TournamentTree&& )      = default;

    TournamentTree& operator= ( TournamentTree const& ) = default;
    TournamentTree& operator= ( TournamentTree&& )      = default;

    // -------------------------------------------------------------------------
    //     Build
    // -------------------------------------------------------------------------

    /**
     * @brief Build (or rebuild) the tree from a set of sorted input ranges.
     *
     * @p lists can be any range of ranges of @p T, such as `std::vector<std::vector<T>>`,
     * `std::vector<std::span<T const>>`, or similar. Each inner range is expected to already
     * be sorted in ascending order (according to @p Compare). Empty inner ranges are skipped.
     *
     * The number of (non-skipped or skipped) inner ranges must not exceed the @p max_bins
     * passed to the constructor, or an exception is thrown.
     *
     * This fully (re-)initializes the tree, and can be called repeatedly to reuse the
     * internally allocated storage for successive merges with a varying number of bins.
     */
    template<typename ListOfLists>
    void build( ListOfLists const& lists )
    {
        size_type const num_lists = std::size( lists );
        if( num_lists > bins_.size() ) {
            throw std::invalid_argument(
                "TournamentTree::build: number of input lists exceeds max_bins"
            );
        }

        size_ = 0;
        size_type i = 0;
        for( auto const& list : lists ) {
            auto const list_size = std::size( list );
            auto& bin = bins_[i];
            bin.list_index = i;
            if( list_size > 0 ) {
                bin.cur = std::data( list );
                bin.end = bin.cur + list_size;
                bin.value = *bin.cur;
                assert_not_sentinel_( bin.value );
                ++size_;
            } else {
                set_inactive_( bin );
            }
            ++i;
        }

        // Remaining bins (up to bins_.size()) are leftover from a previous build() with more
        // bins; mark them inactive as well, so they do not interfere with the tournament.
        for( ; i < bins_.size(); ++i ) {
            bins_[i].list_index = i;
            set_inactive_( bins_[i] );
        }

        // Padding leaves beyond bins_.size() (up to num_leaves_) have no BinEntry; they are
        // always treated as inactive (always-losing) by wins_(). Run the initial tournament
        // over all leaves.
        tree_[0] = play_recursive_( 1 );
    }

    // -------------------------------------------------------------------------
    //     Access
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether all bins are exhausted.
     */
    bool empty() const noexcept
    {
        return size_ == 0;
    }

    /**
     * @brief Return the number of bins that still have elements remaining.
     */
    size_type size() const noexcept
    {
        return size_;
    }

    /**
     * @brief Return the entry holding the globally smallest (per @p Compare) remaining element.
     *
     * Throws `std::out_of_range` if empty().
     */
    BinEntry const& top() const
    {
        if( empty() ) {
            throw std::out_of_range( "TournamentTree::top: tree is empty" );
        }
        return bins_[ tree_[0] ];
    }

    /**
     * @brief Advance past the current top() element, and restore the tree invariant.
     *
     * Throws `std::out_of_range` if empty().
     */
    void pop()
    {
        if( empty() ) {
            throw std::out_of_range( "TournamentTree::pop: tree is empty" );
        }

        auto const winner = tree_[0];
        auto& bin = bins_[winner];

        ++bin.cur;
        if( bin.cur != bin.end ) {
            bin.value = *bin.cur;
            assert_not_sentinel_( bin.value );
        } else {
            set_inactive_( bin );
            --size_;
        }

        // Walk from the winner's leaf to the root, replaying matches against the cached
        // losers stored at each internal node.
        size_type node = winner + num_leaves_;
        size_type cur  = winner;
        while( node > 1 ) {
            node /= 2;
            auto const other = tree_[node];
            if( wins_( other, cur )) {
                tree_[node] = cur;
                cur = other;
            }
            // else: cur remains the winner, tree_[node] (the loser "other") stays unchanged.
        }
        tree_[0] = cur;
    }

private:

    // -------------------------------------------------------------------------
    //     Internal Helpers
    // -------------------------------------------------------------------------

    static size_type next_power_of_two_( size_type n )
    {
        size_type p = 1;
        while( p < n ) {
            p <<= 1;
        }
        // A tree needs at least two leaves to have an inner structure.
        return p < 2 ? 2 : p;
    }

    // Whether the sentinel-based fast path (see class documentation) is used to mark
    // inactive/padding bins, instead of an explicit "is this bin active" check. This is only
    // correct if numeric_limits<T>::max()/lowest() are the actual extremal elements under
    // Compare, which holds for std::less<T> and std::greater<T>, but not in general.
    static constexpr bool kSentinelOptimization =
        std::numeric_limits<T>::is_specialized && (
            std::is_same_v<Compare, std::less<T>> || std::is_same_v<Compare, std::greater<T>>
        );

    /**
     * @brief Assert that an input value does not collide with the sentinel used to mark
     * inactive/padding bins (see class documentation).
     */
    static void assert_not_sentinel_( T const& value )
    {
        (void) value;
        if constexpr( kSentinelOptimization ) {
            assert( value != sentinel_() );
        }
    }

    /**
     * @brief Set the cached value of @p bin such that it always loses comparisons.
     */
    void set_inactive_( BinEntry& bin )
    {
        bin.cur = nullptr;
        bin.end = nullptr;
        if constexpr( kSentinelOptimization ) {
            bin.value = sentinel_();
        } else {
            // No usable sentinel; active() (cur == end == nullptr) is used by wins_() instead.
            bin.value = T{};
        }
    }

    /**
     * @brief Return whether bin @p a wins (i.e., compares less than, or @p b is inactive)
     * against bin @p b. Padding indices (>= bins_.size()) are always inactive.
     */
    bool wins_( size_type a, size_type b ) const
    {
        if constexpr( kSentinelOptimization ) {
            // Inactive bins (and padding indices, which have no BinEntry) carry the sentinel
            // max() value, so a plain comparison suffices in the common case.
            T const& va = ( a < bins_.size() ) ? bins_[a].value : sentinel_();
            T const& vb = ( b < bins_.size() ) ? bins_[b].value : sentinel_();
            if( a >= bins_.size() && b >= bins_.size() ) {
                return a < b; // arbitrary but deterministic tie-break among padding indices
            }
            return Compare{}( va, vb );
        } else {
            bool const a_active = a < bins_.size() && bins_[a].active();
            bool const b_active = b < bins_.size() && bins_[b].active();
            if( !b_active ) {
                return true;
            }
            if( !a_active ) {
                return false;
            }
            return Compare{}( bins_[a].value, bins_[b].value );
        }
    }

    /**
     * @brief Value used to mark inactive/padding bins, such that they always lose comparisons.
     *
     * For the default `std::less`, this is `numeric_limits<T>::max()` (nothing compares
     * greater). For `std::greater` (descending order), it is `numeric_limits<T>::lowest()`
     * instead, since under that ordering, the lowest value always loses. More generally, of
     * `max()` and `lowest()`, we pick the one that the other compares as "winning" against
     * under @p Compare, i.e. the one for which `Compare{}(other, candidate)` holds.
     */
    static T const& sentinel_()
    {
        static T const value = Compare{}(
            std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()
        )
            ? std::numeric_limits<T>::max()
            : std::numeric_limits<T>::lowest();
        return value;
    }

    /**
     * @brief Recursively play the tournament rooted at tree node @p node, returning the index
     * of the winning bin, and storing the index of the loser at @p node (for internal nodes).
     */
    size_type play_recursive_( size_type node )
    {
        if( node >= num_leaves_ ) {
            return node - num_leaves_;
        }

        auto const left  = play_recursive_( 2 * node );
        auto const right = play_recursive_( 2 * node + 1 );

        size_type winner, loser;
        if( wins_( left, right )) {
            winner = left;
            loser  = right;
        } else {
            winner = right;
            loser  = left;
        }

        tree_[node] = loser;
        return winner;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    // Number of leaves in the tree, i.e., the next power of two >= bins_.size().
    size_type num_leaves_;

    // Flat storage of bin entries, one per input list (size == max_bins from the constructor).
    std::vector<BinEntry> bins_;

    // Flat storage of the loser tree. tree_[0] is the overall winner (root); tree_[1..] are
    // internal nodes holding the index (into bins_, or a padding index >= bins_.size()) of the
    // loser of the corresponding sub-tree match. Size == num_leaves_.
    std::vector<size_type> tree_;

    // Number of bins that still have remaining elements.
    size_type size_;
};

} // namespace container
} // namespace util
} // namespace genesis

#endif // include guard
