/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include <functional>
#include <numeric>
#include <vector>

#include "src/common.hpp"

#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/thread_pool.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/random.hpp"

using namespace genesis::utils;

// =================================================================================================
//     Nested Tests
// =================================================================================================

void thread_pool_sleep_( size_t milliseconds = 10 )
{
    // We add a sleep in each task, so that multiple tasks get submitted first,
    // before the pool starts running them, so that they have a chance to submit their nested tasks.
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void thread_pool_work_(size_t i)
{
    (void) i;

    // LOG_DBG << "sub B " << i;
    auto pool = Options::get().global_thread_pool();
    auto res = pool->enqueue([=](){
        // LOG_DBG << "beg B " << i;
        thread_pool_sleep_();
        // LOG_DBG << "end B " << i;
        return true;
    });//.get();
    // LOG_DBG << "sup B " << i;
    res.get();
    // LOG_DBG << "fin B " << i;
}

TEST( ThreadPool, Nested )
{
    // LOG_DBG << "---";
    std::vector<std::future<bool>> tasks;

    for (size_t i = 0; i < 4; ++i) {
        // LOG_DBG << "sub A " << i;
        auto pool = Options::get().global_thread_pool();
        tasks.emplace_back( pool->enqueue( [=](){
            // LOG_DBG << "beg A " << i;
            thread_pool_sleep_();
            thread_pool_work_(i);
            thread_pool_sleep_();
            // LOG_DBG << "end A " << i;
            return true;
        }));
        // LOG_DBG << "sup A " << i;
    }

    for( auto & task : tasks ) {
        EXPECT_TRUE( task.get() );
    }

    // tp_pool.wait_for_tasks();
    // LOG_DBG << "---";
}

// =================================================================================================
//     Parallel Block
// =================================================================================================

void test_thread_pool_parallel_block_( size_t num_threads, size_t num_tasks, size_t num_blocks )
{
    // LOG_DBG << "--- num_tasks " << num_tasks << " num_blocks " << num_blocks;

    // Make a list of numbers for testing.
    std::vector<int> numbers( num_tasks );
    std::iota( numbers.begin(), numbers.end(), 1 );
    auto const exp = std::accumulate( numbers.begin(), numbers.end(), 0 );

    // Prepare the pool
    ThreadPool pool( num_threads );

    // Do some parallel computation.
    auto mult_fut = pool.parallel_block(
        0, num_tasks,
        [&numbers]( size_t b, size_t e )
        {
            // LOG_DBG1 << "b " << b << " e " << e << " s " << numbers.size();
            int sum = 0;
            for( size_t i = b; i < e; ++i ) {
                sum += numbers[i];
            }
            return sum;
        },
        num_blocks
    );

    // Aggregate the result per block.
    auto const res = mult_fut.get();
    auto total = std::accumulate( res.begin(), res.end(), 0 );
    EXPECT_EQ( exp, total );

    // EXPECT_EQ( num_blocks, mult_fut.size() );
    // EXPECT_EQ( std::min( num_tasks, num_blocks ), mult_fut.size() );
    // LOG_DBG << ">>> " << mult_fut.size();

    // For debugging, to avoid interleaved output, we add a wait here...
    // not sure if that helps, but at the moment, things seem to be out of order.
    // thread_pool_sleep_(100);
}

TEST( ThreadPool, ParallelBlock )
{
    for( size_t num_threads = 1; num_threads < 10; ++num_threads ) {
        // Test the border cases
        test_thread_pool_parallel_block_( num_threads, 0, 0 );
        test_thread_pool_parallel_block_( num_threads, 0, 1 );
        test_thread_pool_parallel_block_( num_threads, 0, 2 );
        test_thread_pool_parallel_block_( num_threads, 0, 3 );
        test_thread_pool_parallel_block_( num_threads, 1, 0 );
        test_thread_pool_parallel_block_( num_threads, 1, 1 );
        test_thread_pool_parallel_block_( num_threads, 1, 2 );
        test_thread_pool_parallel_block_( num_threads, 1, 3 );
        test_thread_pool_parallel_block_( num_threads, 2, 0 );
        test_thread_pool_parallel_block_( num_threads, 2, 1 );
        test_thread_pool_parallel_block_( num_threads, 2, 2 );
        test_thread_pool_parallel_block_( num_threads, 2, 3 );
        test_thread_pool_parallel_block_( num_threads, 3, 0 );
        test_thread_pool_parallel_block_( num_threads, 3, 1 );
        test_thread_pool_parallel_block_( num_threads, 3, 2 );
        test_thread_pool_parallel_block_( num_threads, 3, 3 );

        // Test some extreme cases
        test_thread_pool_parallel_block_( num_threads, 0, 100 );
        test_thread_pool_parallel_block_( num_threads, 1, 100 );
        test_thread_pool_parallel_block_( num_threads, 2, 100 );
        test_thread_pool_parallel_block_( num_threads, 3, 100 );
        test_thread_pool_parallel_block_( num_threads, 100, 0 );
        test_thread_pool_parallel_block_( num_threads, 100, 1 );
        test_thread_pool_parallel_block_( num_threads, 100, 2 );
        test_thread_pool_parallel_block_( num_threads, 100, 3 );

        // Test for a good division of labor
        test_thread_pool_parallel_block_( num_threads, 100, 10 );
        test_thread_pool_parallel_block_( num_threads, 100, 11 );
        test_thread_pool_parallel_block_( num_threads, 100, 12 );
        test_thread_pool_parallel_block_( num_threads, 100, 13 );
        test_thread_pool_parallel_block_( num_threads, 100, 14 );
        test_thread_pool_parallel_block_( num_threads, 100, 15 );
        test_thread_pool_parallel_block_( num_threads, 100, 16 );
        test_thread_pool_parallel_block_( num_threads, 100, 17 );
        test_thread_pool_parallel_block_( num_threads, 100, 18 );
        test_thread_pool_parallel_block_( num_threads, 100, 19 );
        test_thread_pool_parallel_block_( num_threads, 100, 20 );
    }
}

// =================================================================================================
//     Parallel For
// =================================================================================================

void test_thread_pool_parallel_for_( size_t num_threads, size_t num_tasks, size_t num_blocks )
{
    // LOG_DBG << "--- num_tasks " << num_tasks << " num_blocks " << num_blocks;

    // Make a list of numbers for testing.
    std::vector<int> numbers( num_tasks );
    std::iota( numbers.begin(), numbers.end(), 1 );
    auto const exp = 2 * std::accumulate( numbers.begin(), numbers.end(), 0 );

    // Prepare the pool
    ThreadPool pool( num_threads );

    // Do some parallel computation.
    auto mult_fut = pool.parallel_for(
        0, num_tasks,
        [&numbers]( size_t i )
        {
            // LOG_DBG1 << "i " << i << " s " << numbers.size();
            numbers[i] *= 2;
        },
        num_blocks
    );

    // Aggregate the result per block.
    mult_fut.get();
    auto const total = std::accumulate( numbers.begin(), numbers.end(), 0 );
    EXPECT_EQ( exp, total );

    // EXPECT_EQ( num_blocks, mult_fut.size() );

    // Same as above, for debugging...
    // thread_pool_sleep_(100);
}

TEST( ThreadPool, ParallelFor )
{
    for( size_t num_threads = 1; num_threads < 10; ++num_threads ) {
        // Test the border cases
        test_thread_pool_parallel_for_( num_threads, 0, 0 );
        test_thread_pool_parallel_for_( num_threads, 0, 1 );
        test_thread_pool_parallel_for_( num_threads, 0, 2 );
        test_thread_pool_parallel_for_( num_threads, 0, 3 );
        test_thread_pool_parallel_for_( num_threads, 1, 0 );
        test_thread_pool_parallel_for_( num_threads, 1, 1 );
        test_thread_pool_parallel_for_( num_threads, 1, 2 );
        test_thread_pool_parallel_for_( num_threads, 1, 3 );
        test_thread_pool_parallel_for_( num_threads, 2, 0 );
        test_thread_pool_parallel_for_( num_threads, 2, 1 );
        test_thread_pool_parallel_for_( num_threads, 2, 2 );
        test_thread_pool_parallel_for_( num_threads, 2, 3 );
        test_thread_pool_parallel_for_( num_threads, 3, 0 );
        test_thread_pool_parallel_for_( num_threads, 3, 1 );
        test_thread_pool_parallel_for_( num_threads, 3, 2 );
        test_thread_pool_parallel_for_( num_threads, 3, 3 );

        // Test some extreme cases
        test_thread_pool_parallel_for_( num_threads, 0, 100 );
        test_thread_pool_parallel_for_( num_threads, 1, 100 );
        test_thread_pool_parallel_for_( num_threads, 2, 100 );
        test_thread_pool_parallel_for_( num_threads, 3, 100 );
        test_thread_pool_parallel_for_( num_threads, 100, 0 );
        test_thread_pool_parallel_for_( num_threads, 100, 1 );
        test_thread_pool_parallel_for_( num_threads, 100, 2 );
        test_thread_pool_parallel_for_( num_threads, 100, 3 );

        // Test for a good division of labor
        test_thread_pool_parallel_for_( num_threads, 100, 10 );
        test_thread_pool_parallel_for_( num_threads, 100, 11 );
        test_thread_pool_parallel_for_( num_threads, 100, 12 );
        test_thread_pool_parallel_for_( num_threads, 100, 13 );
        test_thread_pool_parallel_for_( num_threads, 100, 14 );
        test_thread_pool_parallel_for_( num_threads, 100, 15 );
        test_thread_pool_parallel_for_( num_threads, 100, 16 );
        test_thread_pool_parallel_for_( num_threads, 100, 17 );
        test_thread_pool_parallel_for_( num_threads, 100, 18 );
        test_thread_pool_parallel_for_( num_threads, 100, 19 );
        test_thread_pool_parallel_for_( num_threads, 100, 20 );
    }
}

// =================================================================================================
//     Parallel For Each
// =================================================================================================

void test_thread_pool_parallel_for_each_(
    size_t num_threads, size_t num_tasks, size_t num_blocks, bool range
) {
    // LOG_DBG << "--- num_tasks " << num_tasks << " num_blocks " << num_blocks;

    // Make a list of numbers for testing.
    std::vector<int> numbers( num_tasks );
    std::iota( numbers.begin(), numbers.end(), 1 );
    auto const exp = 2 * std::accumulate( numbers.begin(), numbers.end(), 0 );

    // Prepare the pool
    ThreadPool pool( num_threads );

    // Do some parallel computation.
    // We offer to use both version, the range and the container overload of the loop.
    MultiFuture<void> mult_fut;
    if( range ) {
        mult_fut = pool.parallel_for_each(
            numbers.begin(), numbers.end(),
            []( int& elem )
            {
                // LOG_DBG1 << "elem " << elem;
                elem *= 2;
            },
            num_blocks
        );
    } else {
        mult_fut = pool.parallel_for_each(
            numbers,
            []( int& elem )
            {
                // LOG_DBG1 << "elem " << elem;
                elem *= 2;
            },
            num_blocks
        );
    }

    // Aggregate the result per block.
    mult_fut.get();
    auto const total = std::accumulate( numbers.begin(), numbers.end(), 0 );
    EXPECT_EQ( exp, total );

    // EXPECT_EQ( num_blocks, mult_fut.size() );

    // Same as above, for debugging...
    // thread_pool_sleep_(100);
}

void test_thread_pool_parallel_for_each_( bool range )
{
    for( size_t num_threads = 1; num_threads < 10; ++num_threads ) {
        // Test the border cases
        test_thread_pool_parallel_for_each_( num_threads, 0, 0, range );
        test_thread_pool_parallel_for_each_( num_threads, 0, 1, range );
        test_thread_pool_parallel_for_each_( num_threads, 0, 2, range );
        test_thread_pool_parallel_for_each_( num_threads, 0, 3, range );
        test_thread_pool_parallel_for_each_( num_threads, 1, 0, range );
        test_thread_pool_parallel_for_each_( num_threads, 1, 1, range );
        test_thread_pool_parallel_for_each_( num_threads, 1, 2, range );
        test_thread_pool_parallel_for_each_( num_threads, 1, 3, range );
        test_thread_pool_parallel_for_each_( num_threads, 2, 0, range );
        test_thread_pool_parallel_for_each_( num_threads, 2, 1, range );
        test_thread_pool_parallel_for_each_( num_threads, 2, 2, range );
        test_thread_pool_parallel_for_each_( num_threads, 2, 3, range );
        test_thread_pool_parallel_for_each_( num_threads, 3, 0, range );
        test_thread_pool_parallel_for_each_( num_threads, 3, 1, range );
        test_thread_pool_parallel_for_each_( num_threads, 3, 2, range );
        test_thread_pool_parallel_for_each_( num_threads, 3, 3, range );

        // Test some extreme cases
        test_thread_pool_parallel_for_each_( num_threads, 0, 100, range );
        test_thread_pool_parallel_for_each_( num_threads, 1, 100, range );
        test_thread_pool_parallel_for_each_( num_threads, 2, 100, range );
        test_thread_pool_parallel_for_each_( num_threads, 3, 100, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 0, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 1, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 2, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 3, range );

        // Test for a good division of labor
        test_thread_pool_parallel_for_each_( num_threads, 100, 10, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 11, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 12, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 13, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 14, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 15, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 16, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 17, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 18, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 19, range );
        test_thread_pool_parallel_for_each_( num_threads, 100, 20, range );
    }
}

TEST( ThreadPool, ParallelForEachRange )
{
    test_thread_pool_parallel_for_each_( true );
}

TEST( ThreadPool, ParallelForEachContainer )
{
    test_thread_pool_parallel_for_each_( false );
}

// =================================================================================================
//     Randomized For Loop
// =================================================================================================

void thread_pool_for_loop_fuzzy_work_()
{
    // We simply test that all elements of a for-each loop are processed.
    // For this, we create a vector, initialized to 0, then set each element in a thread,
    // and later check that we get the correct sum.

    // Generate a random length of data that we want to process,
    // and create a vector to fill it.
    auto const num_tasks = permuted_congruential_generator() % 100;
    auto numbers = std::vector<int>( num_tasks, -1 );

    // We randomize the number of blocks. This can also be greater than the number
    // of elements, which the pool should handle.
    auto const num_blocks = permuted_congruential_generator() % 100;

    // We do not use the global thread pool here, but instead create one
    // with a random number of threads, to test that it works for all of them.
    auto const num_threads = 1 + permuted_congruential_generator() % 100;
    ThreadPool pool( num_threads );

    // Debug output
    LOG_DBG
        << "num_tasks=" << num_tasks
        << " num_blocks=" << num_blocks
        << " num_threads=" << num_threads
    ;

    // Do the parallel computation.
    auto mult_fut = pool.parallel_for(
        0, num_tasks,
        [&numbers]( size_t i )
        {
            // Test that no element is being processed twice.
            EXPECT_EQ( -1, numbers[i] );
            numbers[i] = i;
        },
        num_blocks
    );
    mult_fut.get();

    // Aggregate the result and check that we got the correct sum.
    auto const total = std::accumulate( numbers.begin(), numbers.end(), 0 );
    EXPECT_EQ( num_tasks * (num_tasks-1) / 2, total );
}

TEST( ThreadPool, ParallelForFuzzy )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    // 0.5s runtime, our default for normal tests.
    size_t const max_tests = 300;

    // Run tests while we have time.
    for( size_t test_num = 0; test_num < max_tests; ++test_num ) {
        // LOG_DBG << "Test " << test_num;
        thread_pool_for_loop_fuzzy_work_();
    }
}

// =================================================================================================
//     Randomized Nested
// =================================================================================================

void thread_pool_compute_nested_fuzzy_work_(
    ThreadPool& pool, std::vector<int>& numbers, size_t begin, size_t end
) {
    ASSERT_LE( begin, end );
    ASSERT_LE( begin, numbers.size() );
    ASSERT_LE( end, numbers.size() );

    if( begin == end ) {
        LOG_DBG1 << "begin==end";
        return;
    }

    // We randomize the number of blocks into which we split the interval.
    auto const dist = 1 + end - begin;
    auto const num_blocks = permuted_congruential_generator() % dist;
    LOG_DBG1 << "begin=" << begin << " end=" << end << " num_blocks=" << num_blocks;

    // Submit tasks.
    auto mult_fut = pool.parallel_block(
        begin, end,
        [&]( size_t begin, size_t end )
        {
            // We split half the blocks further.
            // For the other half, we compute the values here.
            // That gives us some nesting, and nested nesting, etc,
            // without degrading into computing each element individually.
            auto const split = permuted_congruential_generator() % 2;
            if( split ) {
                LOG_DBG2 << "split begin=" << begin << " end=" << end;
                thread_pool_compute_nested_fuzzy_work_( pool, numbers, begin, end );
            } else {
                LOG_DBG2 << "comp begin=" << begin << " end=" << end;
                for( size_t i = begin; i < end; ++i ) {
                    // Test that no element is being processed twice.
                    EXPECT_EQ( -1, numbers[i] );
                    numbers[i] = i;
                }
            }
        },
        num_blocks
    );
    mult_fut.get();
}

void thread_pool_nested_fuzzy_work_()
{
    // Generate a random length of data that we want to process,
    // and create a vector to fill it.
    auto const num_tasks = permuted_congruential_generator() % 1000;
    auto numbers = std::vector<int>( num_tasks, -1 );

    // We do not use the global thread pool here, but instead create one
    // with a random number of threads, to test that it works for all of them.
    auto const num_threads = 1 + permuted_congruential_generator() % 10;
    ThreadPool pool( num_threads );

    // Debug output
    LOG_DBG << "num_tasks=" << num_tasks << " num_threads=" << num_threads;

    // Run the function that recursively splits the tasks into blocks.
    thread_pool_compute_nested_fuzzy_work_( pool, numbers, 0, num_tasks );

    // Aggregate the result and check that we got the correct sum.
    auto const total = std::accumulate( numbers.begin(), numbers.end(), 0 );
    EXPECT_EQ( num_tasks * (num_tasks-1) / 2, total );
}

TEST( ThreadPool, NestedFuzzy )
{
    // Same as above.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t const max_tests = 300;
    for( size_t test_num = 0; test_num < max_tests; ++test_num ) {
        thread_pool_nested_fuzzy_work_();
    }
}
