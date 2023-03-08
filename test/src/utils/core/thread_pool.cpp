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

#include <numeric>
#include <vector>

#include "src/common.hpp"

#include "genesis/utils/core/thread_pool.hpp"
#include "genesis/utils/core/options.hpp"

using namespace genesis::utils;

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

void test_thread_pool_parallel_block_( size_t num_tasks, size_t num_blocks )
{
    // LOG_DBG << "--- num_tasks " << num_tasks << " num_blocks " << num_blocks;

    // Make a list of numbers for testing.
    std::vector<int> numbers( num_tasks );
    std::iota( numbers.begin(), numbers.end(), 1 );
    auto const exp = std::accumulate( numbers.begin(), numbers.end(), 0 );

    // Prepare the pool
    auto pool = Options::get().global_thread_pool();

    // Do some parallel computation.
    auto mult_fut = pool->parallel_block(
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
    // Test the border cases
    test_thread_pool_parallel_block_( 0, 0 );
    test_thread_pool_parallel_block_( 0, 1 );
    test_thread_pool_parallel_block_( 0, 2 );
    test_thread_pool_parallel_block_( 0, 3 );
    test_thread_pool_parallel_block_( 1, 0 );
    test_thread_pool_parallel_block_( 1, 1 );
    test_thread_pool_parallel_block_( 1, 2 );
    test_thread_pool_parallel_block_( 1, 3 );
    test_thread_pool_parallel_block_( 2, 0 );
    test_thread_pool_parallel_block_( 2, 1 );
    test_thread_pool_parallel_block_( 2, 2 );
    test_thread_pool_parallel_block_( 2, 3 );
    test_thread_pool_parallel_block_( 3, 0 );
    test_thread_pool_parallel_block_( 3, 1 );
    test_thread_pool_parallel_block_( 3, 2 );
    test_thread_pool_parallel_block_( 3, 3 );

    // Test some extreme cases
    test_thread_pool_parallel_block_( 0, 100 );
    test_thread_pool_parallel_block_( 1, 100 );
    test_thread_pool_parallel_block_( 2, 100 );
    test_thread_pool_parallel_block_( 3, 100 );
    test_thread_pool_parallel_block_( 100, 0 );
    test_thread_pool_parallel_block_( 100, 1 );
    test_thread_pool_parallel_block_( 100, 2 );
    test_thread_pool_parallel_block_( 100, 3 );

    // Test for a good division of labor
    test_thread_pool_parallel_block_( 100, 10 );
    test_thread_pool_parallel_block_( 100, 11 );
    test_thread_pool_parallel_block_( 100, 12 );
    test_thread_pool_parallel_block_( 100, 13 );
    test_thread_pool_parallel_block_( 100, 14 );
    test_thread_pool_parallel_block_( 100, 15 );
    test_thread_pool_parallel_block_( 100, 16 );
    test_thread_pool_parallel_block_( 100, 17 );
    test_thread_pool_parallel_block_( 100, 18 );
    test_thread_pool_parallel_block_( 100, 19 );
    test_thread_pool_parallel_block_( 100, 20 );
}

void test_thread_pool_parallel_for_( size_t num_tasks, size_t num_blocks )
{
    // LOG_DBG << "--- num_tasks " << num_tasks << " num_blocks " << num_blocks;

    // Make a list of numbers for testing.
    std::vector<int> numbers( num_tasks );
    std::iota( numbers.begin(), numbers.end(), 1 );
    auto const exp = 2 * std::accumulate( numbers.begin(), numbers.end(), 0 );

    // Prepare the pool
    auto pool = Options::get().global_thread_pool();

    // Do some parallel computation.
    auto mult_fut = pool->parallel_for(
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
    // Test the border cases
    test_thread_pool_parallel_for_( 0, 0 );
    test_thread_pool_parallel_for_( 0, 1 );
    test_thread_pool_parallel_for_( 0, 2 );
    test_thread_pool_parallel_for_( 0, 3 );
    test_thread_pool_parallel_for_( 1, 0 );
    test_thread_pool_parallel_for_( 1, 1 );
    test_thread_pool_parallel_for_( 1, 2 );
    test_thread_pool_parallel_for_( 1, 3 );
    test_thread_pool_parallel_for_( 2, 0 );
    test_thread_pool_parallel_for_( 2, 1 );
    test_thread_pool_parallel_for_( 2, 2 );
    test_thread_pool_parallel_for_( 2, 3 );
    test_thread_pool_parallel_for_( 3, 0 );
    test_thread_pool_parallel_for_( 3, 1 );
    test_thread_pool_parallel_for_( 3, 2 );
    test_thread_pool_parallel_for_( 3, 3 );

    // Test some extreme cases
    test_thread_pool_parallel_for_( 0, 100 );
    test_thread_pool_parallel_for_( 1, 100 );
    test_thread_pool_parallel_for_( 2, 100 );
    test_thread_pool_parallel_for_( 3, 100 );
    test_thread_pool_parallel_for_( 100, 0 );
    test_thread_pool_parallel_for_( 100, 1 );
    test_thread_pool_parallel_for_( 100, 2 );
    test_thread_pool_parallel_for_( 100, 3 );

    // Test for a good division of labor
    test_thread_pool_parallel_for_( 100, 10 );
    test_thread_pool_parallel_for_( 100, 11 );
    test_thread_pool_parallel_for_( 100, 12 );
    test_thread_pool_parallel_for_( 100, 13 );
    test_thread_pool_parallel_for_( 100, 14 );
    test_thread_pool_parallel_for_( 100, 15 );
    test_thread_pool_parallel_for_( 100, 16 );
    test_thread_pool_parallel_for_( 100, 17 );
    test_thread_pool_parallel_for_( 100, 18 );
    test_thread_pool_parallel_for_( 100, 19 );
    test_thread_pool_parallel_for_( 100, 20 );
}

void test_thread_pool_parallel_for_each_( size_t num_tasks, size_t num_blocks )
{
    // LOG_DBG << "--- num_tasks " << num_tasks << " num_blocks " << num_blocks;

    // Make a list of numbers for testing.
    std::vector<int> numbers( num_tasks );
    std::iota( numbers.begin(), numbers.end(), 1 );
    auto const exp = 2 * std::accumulate( numbers.begin(), numbers.end(), 0 );

    // Prepare the pool
    auto pool = Options::get().global_thread_pool();

    // Do some parallel computation.
    auto mult_fut = pool->parallel_for_each(
        numbers.begin(), numbers.end(),
        []( int& elem )
        {
            // LOG_DBG1 << "elem " << elem;
            elem *= 2;
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

TEST( ThreadPool, ParallelForEach )
{
    // Test the border cases
    test_thread_pool_parallel_for_each_( 0, 0 );
    test_thread_pool_parallel_for_each_( 0, 1 );
    test_thread_pool_parallel_for_each_( 0, 2 );
    test_thread_pool_parallel_for_each_( 0, 3 );
    test_thread_pool_parallel_for_each_( 1, 0 );
    test_thread_pool_parallel_for_each_( 1, 1 );
    test_thread_pool_parallel_for_each_( 1, 2 );
    test_thread_pool_parallel_for_each_( 1, 3 );
    test_thread_pool_parallel_for_each_( 2, 0 );
    test_thread_pool_parallel_for_each_( 2, 1 );
    test_thread_pool_parallel_for_each_( 2, 2 );
    test_thread_pool_parallel_for_each_( 2, 3 );
    test_thread_pool_parallel_for_each_( 3, 0 );
    test_thread_pool_parallel_for_each_( 3, 1 );
    test_thread_pool_parallel_for_each_( 3, 2 );
    test_thread_pool_parallel_for_each_( 3, 3 );

    // Test some extreme cases
    test_thread_pool_parallel_for_each_( 0, 100 );
    test_thread_pool_parallel_for_each_( 1, 100 );
    test_thread_pool_parallel_for_each_( 2, 100 );
    test_thread_pool_parallel_for_each_( 3, 100 );
    test_thread_pool_parallel_for_each_( 100, 0 );
    test_thread_pool_parallel_for_each_( 100, 1 );
    test_thread_pool_parallel_for_each_( 100, 2 );
    test_thread_pool_parallel_for_each_( 100, 3 );

    // Test for a good division of labor
    test_thread_pool_parallel_for_each_( 100, 10 );
    test_thread_pool_parallel_for_each_( 100, 11 );
    test_thread_pool_parallel_for_each_( 100, 12 );
    test_thread_pool_parallel_for_each_( 100, 13 );
    test_thread_pool_parallel_for_each_( 100, 14 );
    test_thread_pool_parallel_for_each_( 100, 15 );
    test_thread_pool_parallel_for_each_( 100, 16 );
    test_thread_pool_parallel_for_each_( 100, 17 );
    test_thread_pool_parallel_for_each_( 100, 18 );
    test_thread_pool_parallel_for_each_( 100, 19 );
    test_thread_pool_parallel_for_each_( 100, 20 );
}
