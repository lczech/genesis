/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/utils/containers/filter_iterator.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"
#include "genesis/utils/containers/transform_iterator.hpp"

#include <algorithm>
#include <atomic>
#include <numeric>
#include <utility>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//      Transform Iterator
// =================================================================================================

TEST( Containers, TransformIteratorCopy )
{
    // Prepare a list of consequtive numbers.
    size_t size = 10;
    std::vector<size_t> list( size );
    std::iota( list.begin(), list.end(), 0 );

    // We double each of them, returning a copy of the result.
    auto twice = []( size_t x ){
        return 2*x;
    };
    std::vector<size_t> expected_result{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

    // Version 1: Two independent iterators to begin and end.
    std::vector<size_t> res_it;
    auto begin = make_transform_iterator( twice, list.begin() );
    auto end   = make_transform_iterator( twice, list.end() );
    for( auto it = begin; it != end; ++it ) {
        res_it.push_back( *it );
    }
    EXPECT_EQ( expected_result, res_it );

    // Version 2: A range with specified begin end end.
    std::vector<size_t> res_range_be;
    for( auto e : make_transform_range( twice, list.begin(), list.end() )) {
        res_range_be.push_back( e );
    }
    EXPECT_EQ( expected_result, res_range_be );

    // Version 3: A range based on a container.
    std::vector<size_t> res_range_cont;
    for( auto e : make_transform_range( twice, list )) {
        res_range_cont.push_back( e );
    }
    EXPECT_EQ( expected_result, res_range_cont );
}

TEST( Containers, TransformIteratorConstReference )
{
    // Prepare a list of consequtive numbers, in a pair, where only one entry is useful for us here.
    size_t size = 10;
    std::vector<std::pair<size_t, size_t>> list;
    for( size_t i = 0; i < size; ++i ) {
        list.push_back( std::pair<size_t, size_t>( i, 0 ));
    }

    // We return the first element as our test transform iterator.
    auto first_entry = []( std::pair<size_t, size_t> const& e ) -> size_t const& {
        return e.first;
    };

    // Version 1: Two independent iterators to begin and end.
    auto begin = make_transform_iterator( first_entry, list.begin() );
    auto end   = make_transform_iterator( first_entry, list.end() );
    size_t i = 0;
    for( auto it = begin; it != end; ++it ) {
        EXPECT_EQ( i, *it );
        ++i;
    }

    // Version 2: A range with specified begin end end.
    i = 0;
    for( auto e : make_transform_range( first_entry, list.begin(), list.end() )) {
        EXPECT_EQ( i, e );
        ++i;
    }

    // Version 3: A range based on a container.
    i = 0;
    for( auto e : make_transform_range( first_entry, list )) {
        EXPECT_EQ( i, e );
        ++i;
    }
}

TEST( Containers, TransformIteratorReference )
{
    // Prepare a list of consequtive numbers, in a pair, where only one entry is useful for us here.
    size_t size = 10;
    std::vector<std::pair<size_t, size_t>> list;
    for( size_t i = 0; i < size; ++i ) {
        list.push_back( std::pair<size_t, size_t>( i, 0 ));
    }

    // We return the first element as our test transform iterator.
    auto first_entry = []( std::pair<size_t, size_t>& e ) -> size_t& {
        return e.first;
    };

    // In the following, we iterator three times, each time using a different method of creating
    // the iterator (to test them all), and each time doubling the values in the original list
    // (as everything is handled by non-const reference here). After that, each entry is 8 times
    // its original value, which we also test at the end.

    // Version 1: Two independent iterators to begin and end.
    auto begin = make_transform_iterator( first_entry, list.begin() );
    auto end   = make_transform_iterator( first_entry, list.end() );
    size_t i = 0;
    for( auto it = begin; it != end; ++it ) {
        *it *= 2;
        EXPECT_EQ( 2 * i, *it );
        ++i;
    }

    // Version 2: A range with specified begin end end.
    i = 0;
    for( auto& e : make_transform_range( first_entry, list.begin(), list.end() )) {
        e *= 2;
        EXPECT_EQ( 4 * i, e );
        ++i;
    }

    // Version 3: A range based on a container.
    i = 0;
    for( auto& e : make_transform_range( first_entry, list )) {
        e *= 2;
        EXPECT_EQ( 8 * i, e );
        ++i;
    }

    // Finally, test that the vector has been modifed now.
    std::vector<std::pair<size_t, size_t>> expected_result;
    for( size_t i = 0; i < size; ++i ) {
        expected_result.push_back( std::pair<size_t, size_t>( 8 * i, 0 ));
    }
    EXPECT_EQ( expected_result, list );
}

// =================================================================================================
//      Filter Iterator
// =================================================================================================

TEST( Containers, FilterIterator )
{
    // Prepare a list of consequtive numbers.
    size_t size = 20;
    std::vector<size_t> list( size );
    std::iota( list.begin(), list.end(), 0 );

    // We only filter even numbers. Incidentally. the result is the same as above.
    auto even = []( size_t x ){
        return x % 2 == 0;
    };
    std::vector<size_t> result{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

    // Version 1: Two independent iterators to begin and end.
    std::vector<size_t> res_it;
    auto begin = make_filter_iterator( even, list.begin(), list.end() );
    auto end   = make_filter_iterator( even, list.end(), list.end() );
    for( auto it = begin; it != end; ++it ) {
        res_it.push_back( *it );
    }
    EXPECT_EQ( result, res_it );

    // Version 2: A range with specified begin end end.
    std::vector<size_t> res_range_be;
    for( auto e : make_filter_range( even, list.begin(), list.end() )) {
        res_range_be.push_back( e );
    }
    EXPECT_EQ( result, res_range_be );

    // Version 3: A range based on a container.
    std::vector<size_t> res_range_cont;
    for( auto e : make_filter_range( even, list )) {
        res_range_cont.push_back( e );
    }
    EXPECT_EQ( result, res_range_cont );
}

// =================================================================================================
//      Generic Input Stream
// =================================================================================================

void test_generic_input_stream_( size_t num_elements, size_t block_size )
{
    LOG_DBG << "====================================";
    LOG_DBG << "num_elements " << num_elements << ", block_size " << block_size;

    // Create data as sequence of numbers, and get their sum.
    // By using a sequence of numbers, we make sure to not accidentally test things
    // that would otherwise be constant. Here, we have a different number for each
    // visited item. We then use their sum as the expected test result, and use different methods
    // to arrive at that sum, which all need to come to the same conclusion. Ground truth
    // is just the sum computed here. Could be done with Gauss. Too lazy to look it up now.
    std::vector<size_t> data( num_elements );
    std::iota( data.begin(), data.end(), 0 );
    auto const expected_sum = std::accumulate( data.begin(), data.end(), size_t{0} );

    // The input is a sequence of numbers. We use a counter while looping to check every element.
    std::atomic<size_t> called_counter{0};
    std::atomic<size_t> value_counter{0};

    // Set up the GenericInputStream.
    using NumberGenericInputStream = GenericInputStream<size_t>;
    auto beg = data.begin();
    auto end = data.end();
    auto generator = NumberGenericInputStream(
        [beg, end, &called_counter, &value_counter]( size_t& value ) mutable {
            ++called_counter;
            if( beg != end ) {
                value = *beg;

                // Check that the series is complete.
                // We do this by checking that we have consequtive numbers.
                auto const lc = value_counter.load();
                EXPECT_EQ( lc, value );
                ++value_counter;

                ++beg;
                return true;
            } else {
                return false;
            }
        }, nullptr, block_size
    );

    // Result variables.
    size_t loop_sum = 0;
    size_t on_enter_sum = 0;
    size_t on_leave_sum = 0;

    // We add observers, which also add up the numbers, to test their behaviour as well.
    generator.add_on_enter_observer(
        [&on_enter_sum]( size_t elem ){
            on_enter_sum += elem;
        }
    );
    generator.add_on_leave_observer(
        [&on_leave_sum]( size_t elem ){
            on_leave_sum += elem;
        }
    );

    // Add begin and end callbacks, in which we test that the calcuated sums are 0 in the beginning,
    // and the correct sum in the end.
    generator.add_begin_callback(
        [&]( NumberGenericInputStream const& ){
            LOG_DBG << "begin";
            EXPECT_EQ( 0, loop_sum );
            EXPECT_EQ( 0, on_enter_sum );
            EXPECT_EQ( 0, on_leave_sum );
        }
    );
        generator.add_end_callback(
        [&]( NumberGenericInputStream const& ){
            LOG_DBG << "end";
            EXPECT_EQ( expected_sum, loop_sum );
            EXPECT_EQ( expected_sum, on_enter_sum );
            EXPECT_EQ( expected_sum, on_leave_sum );
        }
    );

    // Run the iteration and check that it matches our expectation.
    for( auto const& it : generator ) {
        LOG_DBG << "at " << it;
        loop_sum += it;
    }

    // Regression test.
    // We had a bug where the generic stream would not check for the end of the input correctly.
    // This test makes sure that after the loop is done, there is nothing in the thread pool
    // any more - the iterator should have waited for the end of everything before finishing.
    // We are only using the global thread pool sequentially in the tests here, so there
    // cannot be anything left from other places once we are done with the iteration.
    EXPECT_EQ( 0, Options::get().global_thread_pool()->pending_tasks_count() );

    // We called the get element function in the lambda exactly one per data item,
    // and one last time at the end to indicate that ther is no more data.
    EXPECT_EQ( data.size() + 1, called_counter.load() );

    // Check the numerical outputs as well
    EXPECT_EQ( expected_sum, loop_sum );
    EXPECT_EQ( expected_sum, on_enter_sum );
    EXPECT_EQ( expected_sum, on_leave_sum );
}

TEST( Containers, GenericInputStream )
{
    // We test several cases here, for example where the block size is exactly the number of
    // elements, or a divisor of it, or off by one, or more than the number, etc...

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    // Loop a few times, to have a higher chance of finding race conditions etc in the threading.
    for( size_t i = 0; i < 500; ++i ) {

        // No elements
        test_generic_input_stream_( 0, 0 );
        test_generic_input_stream_( 0, 1 );
        test_generic_input_stream_( 0, 2 );
        test_generic_input_stream_( 0, 3 );

        // Single element
        test_generic_input_stream_( 1, 0 );
        test_generic_input_stream_( 1, 1 );
        test_generic_input_stream_( 1, 2 );
        test_generic_input_stream_( 1, 3 );

        // Two elements
        test_generic_input_stream_( 2, 0 );
        test_generic_input_stream_( 2, 1 );
        test_generic_input_stream_( 2, 2 );
        test_generic_input_stream_( 2, 3 );

        // Three elements
        test_generic_input_stream_( 3, 0 );
        test_generic_input_stream_( 3, 1 );
        test_generic_input_stream_( 3, 2 );
        test_generic_input_stream_( 3, 3 );

        // Four elements
        test_generic_input_stream_( 4, 0 );
        test_generic_input_stream_( 4, 1 );
        test_generic_input_stream_( 4, 2 );
        test_generic_input_stream_( 4, 3 );

        // Many elements
        test_generic_input_stream_( 100, 0 );
        test_generic_input_stream_( 100, 1 );
        test_generic_input_stream_( 100, 2 );
        test_generic_input_stream_( 100, 3 );

        // Long buffer block
        test_generic_input_stream_( 0, 100 );
        test_generic_input_stream_( 1, 100 );
        test_generic_input_stream_( 2, 100 );
        test_generic_input_stream_( 3, 100 );
    }
}
