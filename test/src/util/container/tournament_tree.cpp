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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/util/container/tournament_tree.hpp"
#include "genesis/util/tool/timer.hpp"

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

using namespace genesis::util;
using namespace genesis::util::container;

// =================================================================================================
//     Helpers
// =================================================================================================

/**
 * @brief Merge a set of sorted lists using TournamentTree, returning the merged result and,
 * for each emitted value, the index of the list it came from.
 */
template<typename T>
void merge_with_tournament_tree(
    std::vector<std::vector<T>> const& lists,
    std::vector<T>& out_values,
    std::vector<size_t>& out_list_indices
) {
    out_values.clear();
    out_list_indices.clear();

    TournamentTree<T> tree( lists.size() );
    tree.build( lists );

    while( ! tree.empty() ) {
        auto const& e = tree.top();
        out_values.push_back( e.value );
        out_list_indices.push_back( e.list_index );
        tree.pop();
    }
}

/**
 * @brief Reference implementation of the same k-way merge, using std::*_heap, mirroring the
 * approach used in HitCollector::query().
 */
template<typename T>
void merge_with_heap(
    std::vector<std::vector<T>> const& lists,
    std::vector<T>& out_values,
    std::vector<size_t>& out_list_indices
) {
    out_values.clear();
    out_list_indices.clear();

    struct Entry
    {
        T value;
        T const* cur;
        T const* end;
        size_t list_index;
    };
    struct Cmp
    {
        bool operator()( Entry const& a, Entry const& b ) const noexcept
        {
            return a.value > b.value;
        }
    };

    std::vector<Entry> heap;
    for( size_t i = 0; i < lists.size(); ++i ) {
        auto const& l = lists[i];
        if( ! l.empty() ) {
            heap.push_back({ l.front(), l.data(), l.data() + l.size(), i });
        }
    }
    std::make_heap( heap.begin(), heap.end(), Cmp{} );

    while( ! heap.empty() ) {
        std::pop_heap( heap.begin(), heap.end(), Cmp{} );
        auto& e = heap.back();

        out_values.push_back( e.value );
        out_list_indices.push_back( e.list_index );

        ++e.cur;
        if( e.cur != e.end ) {
            e.value = *e.cur;
            std::push_heap( heap.begin(), heap.end(), Cmp{} );
        } else {
            heap.pop_back();
        }
    }
}

// =================================================================================================
//     Correctness Tests
// =================================================================================================

TEST( Container, TournamentTreeBasic )
{
    std::vector<std::vector<int>> lists{
        { 1, 4, 7, 10 },
        { 2, 3, 11 },
        { 0, 5, 6, 8, 9 }
    };

    std::vector<int> values;
    std::vector<size_t> list_indices;
    merge_with_tournament_tree( lists, values, list_indices );

    std::vector<int> expected{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    EXPECT_EQ( expected, values );

    // Check that the reported list indices are consistent with the input lists.
    ASSERT_EQ( values.size(), list_indices.size() );
    for( size_t i = 0; i < values.size(); ++i ) {
        auto const& l = lists[ list_indices[i] ];
        EXPECT_TRUE( std::find( l.begin(), l.end(), values[i] ) != l.end() );
    }
}

TEST( Container, TournamentTreeEmptyAndSingle )
{
    // All lists empty.
    {
        std::vector<std::vector<int>> lists{ {}, {}, {} };
        TournamentTree<int> tree( lists.size() );
        tree.build( lists );
        EXPECT_TRUE( tree.empty() );
        EXPECT_EQ( 0u, tree.size() );
        EXPECT_THROW( tree.top(), std::out_of_range );
        EXPECT_THROW( tree.pop(), std::out_of_range );
    }

    // A mix of empty and non-empty lists.
    {
        std::vector<std::vector<int>> lists{ {}, { 5 }, {}, { 1, 2 } };
        std::vector<int> values;
        std::vector<size_t> list_indices;
        merge_with_tournament_tree( lists, values, list_indices );
        EXPECT_EQ( ( std::vector<int>{ 1, 2, 5 } ), values );
    }

    // A single list.
    {
        std::vector<std::vector<int>> lists{ { 1, 2, 3 } };
        std::vector<int> values;
        std::vector<size_t> list_indices;
        merge_with_tournament_tree( lists, values, list_indices );
        EXPECT_EQ( ( std::vector<int>{ 1, 2, 3 } ), values );
    }

    // No lists at all (max_bins == 0).
    {
        std::vector<std::vector<int>> lists{};
        TournamentTree<int> tree( 0 );
        tree.build( lists );
        EXPECT_TRUE( tree.empty() );
    }
}

TEST( Container, TournamentTreeDuplicates )
{
    // Equal values across different lists; consecutive popping is expected.
    std::vector<std::vector<int>> lists{
        { 1, 2, 2, 3 },
        { 2, 2, 4 },
        { 0, 2 }
    };

    std::vector<int> values;
    std::vector<size_t> list_indices;
    merge_with_tournament_tree( lists, values, list_indices );

    std::vector<int> expected{ 0, 1, 2, 2, 2, 2, 2, 3, 4 };
    EXPECT_EQ( expected, values );
}

TEST( Container, TournamentTreeTooManyBins )
{
    std::vector<std::vector<int>> lists{ { 1 }, { 2 }, { 3 } };
    TournamentTree<int> tree( 2 );
    EXPECT_THROW( tree.build( lists ), std::invalid_argument );
}

TEST( Container, TournamentTreeReuseAcrossBuilds )
{
    TournamentTree<int> tree( 4 );

    // First build with 4 bins.
    {
        std::vector<std::vector<int>> lists{ { 1, 5 }, { 2 }, { 3 }, { 0, 4 } };
        tree.build( lists );

        std::vector<int> values;
        while( ! tree.empty() ) {
            values.push_back( tree.top().value );
            tree.pop();
        }
        EXPECT_EQ( ( std::vector<int>{ 0, 1, 2, 3, 4, 5 } ), values );
    }

    // Second build with fewer bins; leftover state from the first build must not interfere.
    {
        std::vector<std::vector<int>> lists{ { 10, 20 }, { 15 } };
        tree.build( lists );

        std::vector<int> values;
        while( ! tree.empty() ) {
            values.push_back( tree.top().value );
            tree.pop();
        }
        EXPECT_EQ( ( std::vector<int>{ 10, 15, 20 } ), values );
    }
}

/**
 * @brief Minimal non-owning view over a contiguous range, with data()/size(), used to test
 * build() with something other than std::vector<std::vector<T>>.
 */
template<typename T>
struct Span
{
    T const* data_;
    size_t   size_;

    T const* data() const { return data_; }
    size_t   size() const { return size_; }
};

TEST( Container, TournamentTreeSpanInput )
{
    std::vector<int> a{ 1, 3, 5 };
    std::vector<int> b{ 2, 4, 6 };

    std::vector<Span<int>> lists{
        Span<int>{ a.data(), a.size() },
        Span<int>{ b.data(), b.size() }
    };

    TournamentTree<int> tree( lists.size() );
    tree.build( lists );

    std::vector<int> values;
    while( ! tree.empty() ) {
        values.push_back( tree.top().value );
        tree.pop();
    }
    EXPECT_EQ( ( std::vector<int>{ 1, 2, 3, 4, 5, 6 } ), values );
}

TEST( Container, TournamentTreeCustomCompare )
{
    // Descending order via std::greater.
    std::vector<std::vector<int>> lists{
        { 9, 5, 1 },
        { 8, 4, 0 },
        { 7, 6, 2, 3 }
    };
    // Lists must be sorted according to Compare, i.e. descending here.
    std::sort( lists[2].begin(), lists[2].end(), std::greater<int>{} );

    TournamentTree<int, std::greater<int>> tree( lists.size() );
    tree.build( lists );

    std::vector<int> values;
    while( ! tree.empty() ) {
        values.push_back( tree.top().value );
        tree.pop();
    }
    EXPECT_EQ( ( std::vector<int>{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 } ), values );
}

TEST( Container, TournamentTreeNonArithmeticType )
{
    // std::string has no numeric_limits sentinel, exercising the active-flag fallback path.
    std::vector<std::vector<std::string>> lists{
        { "apple", "cherry", "fig" },
        { "banana", "date" },
        {}
    };

    TournamentTree<std::string> tree( lists.size() );
    tree.build( lists );

    std::vector<std::string> values;
    while( ! tree.empty() ) {
        values.push_back( tree.top().value );
        tree.pop();
    }
    std::vector<std::string> expected{ "apple", "banana", "cherry", "date", "fig" };
    EXPECT_EQ( expected, values );
}

TEST( Container, TournamentTreeExactAndMinimalCapacity )
{
    // Exactly a power-of-two number of bins (no padding leaves needed).
    {
        std::vector<std::vector<int>> lists{ { 1 }, { 2 }, { 3 }, { 4 } };
        TournamentTree<int> tree( 4 );
        tree.build( lists );

        std::vector<int> values;
        while( ! tree.empty() ) {
            values.push_back( tree.top().value );
            tree.pop();
        }
        EXPECT_EQ( ( std::vector<int>{ 1, 2, 3, 4 } ), values );
    }

    // Minimal capacity of a single bin (one padding leaf internally).
    {
        std::vector<std::vector<int>> lists{ { 5, 6, 7 } };
        TournamentTree<int> tree( 1 );
        tree.build( lists );

        std::vector<int> values;
        while( ! tree.empty() ) {
            values.push_back( tree.top().value );
            tree.pop();
        }
        EXPECT_EQ( ( std::vector<int>{ 5, 6, 7 } ), values );
    }
}

TEST( Container, TournamentTreeShrinkToEmpty )
{
    TournamentTree<int> tree( 3 );

    // Non-empty build first.
    std::vector<std::vector<int>> lists1{ { 1, 2 }, { 3 }, { 4, 5 } };
    tree.build( lists1 );
    ASSERT_FALSE( tree.empty() );

    // Rebuild with zero lists; leftover state must not leave the tree non-empty.
    std::vector<std::vector<int>> lists2{};
    tree.build( lists2 );
    EXPECT_TRUE( tree.empty() );
    EXPECT_THROW( tree.top(), std::out_of_range );
}

TEST( Container, TournamentTreeListIndices )
{
    std::vector<std::vector<int>> lists{
        { 10, 40, 70 },
        { 20, 50 },
        { 0, 30, 60 }
    };

    std::vector<int> values;
    std::vector<size_t> list_indices;
    merge_with_tournament_tree( lists, values, list_indices );

    std::vector<int> expected_values{ 0, 10, 20, 30, 40, 50, 60, 70 };
    std::vector<size_t> expected_indices{ 2, 0, 1, 2, 0, 1, 2, 0 };

    EXPECT_EQ( expected_values, values );
    EXPECT_EQ( expected_indices, list_indices );
}

// =================================================================================================
//     Random Stress / Cross-Check Tests
// =================================================================================================

TEST( Container, TournamentTreeRandomStress )
{
    std::mt19937 rng( 42 );

    for( int trial = 0; trial < 200; ++trial ) {
        std::uniform_int_distribution<size_t> num_lists_dist( 0, 32 );
        std::uniform_int_distribution<size_t> list_size_dist( 0, 64 );
        std::uniform_int_distribution<uint64_t> value_dist( 0, 1000 );

        size_t const num_lists = num_lists_dist( rng );
        std::vector<std::vector<uint64_t>> lists( num_lists );
        for( auto& l : lists ) {
            auto const n = list_size_dist( rng );
            l.resize( n );
            for( auto& v : l ) {
                v = value_dist( rng );
            }
            std::sort( l.begin(), l.end() );
        }

        std::vector<uint64_t> tt_values;
        std::vector<size_t> tt_indices;
        merge_with_tournament_tree( lists, tt_values, tt_indices );

        std::vector<uint64_t> heap_values;
        std::vector<size_t> heap_indices;
        merge_with_heap( lists, heap_values, heap_indices );

        // The overall sequence of values must match the heap-based reference.
        ASSERT_EQ( heap_values, tt_values );

        // The result must be sorted.
        ASSERT_TRUE( std::is_sorted( tt_values.begin(), tt_values.end() ) );

        // Every value must come from the list it claims to, at the correct position.
        std::vector<size_t> cursors( lists.size(), 0 );
        for( size_t i = 0; i < tt_values.size(); ++i ) {
            auto const li = tt_indices[i];
            ASSERT_LT( cursors[li], lists[li].size() );
            ASSERT_EQ( lists[li][ cursors[li] ], tt_values[i] );
            ++cursors[li];
        }
        for( size_t li = 0; li < lists.size(); ++li ) {
            ASSERT_EQ( cursors[li], lists[li].size() );
        }
    }
}

// =================================================================================================
//     Performance Comparison
// =================================================================================================

/*
TEST( Container, TournamentTreePerformance )
{
    std::mt19937 rng( 1337 );
    std::uniform_int_distribution<uint64_t> value_dist( 0, 1ul << 40 );

    for( size_t num_lists : { size_t{ 8 }, size_t{ 32 }, size_t{ 100 } } ) {
        size_t const list_size = 100000;

        std::vector<std::vector<uint64_t>> lists( num_lists );
        for( auto& l : lists ) {
            l.resize( list_size );
            for( auto& v : l ) {
                v = value_dist( rng );
            }
            std::sort( l.begin(), l.end() );
        }

        std::vector<uint64_t> tt_values;
        std::vector<size_t> tt_indices;
        std::vector<uint64_t> heap_values;
        std::vector<size_t> heap_indices;

        Timer timer_tt;
        timer_tt.start();
        merge_with_tournament_tree( lists, tt_values, tt_indices );
        timer_tt.stop();

        Timer timer_heap;
        timer_heap.start();
        merge_with_heap( lists, heap_values, heap_indices );
        timer_heap.stop();

        // Both implementations must agree on the result.
        ASSERT_EQ( heap_values, tt_values );

        LOG_DBG
            << "k = " << num_lists
            << ", TournamentTree: " << timer_tt.elapsed() << " s"
            << ", std::*_heap: " << timer_heap.elapsed() << " s";
    }
}
*/
