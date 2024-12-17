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

#include "genesis/utils/math/random.hpp"
#include "genesis/utils/threading/concurrent_vector_guard.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <atomic>
#include <ctime>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//     Multi-threaded Madness
// =================================================================================================

TEST( Threading, ConcurrentVectorGuard )
{
    // Constants to be tuned for speed testing
    // size_t const num_elems = 1000000000;
    size_t const num_elems = 1000000;
    size_t const num_threads = 10;

    // Create the of data
    auto values = std::vector<uint8_t>( num_elems );

    // Run 10 threads, each incrementing each element once.
    auto thread_pool = std::make_shared<ThreadPool>( num_threads );
    auto vector_guard = ConcurrentVectorGuard( num_threads * 100 );

    // Start high-resolution timer
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Start speed test
    // LOG_DBG << "start";
    for( size_t i = 0; i < num_threads; ++i ) {
        thread_pool->enqueue_detached([&](){
            for( size_t i = 0; i < num_elems; ++i ) {
                auto lock = vector_guard.get_lock_guard(i);
                // vector_guard.acquire(i);
                ++values[i];
                // vector_guard.release(i);
            }
        });
    }
    thread_pool->wait_for_all_pending_tasks();

    // Calculate the elapsed time in seconds
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    // LOG_DBG << "time: " << elapsed_time << "s";
    (void) elapsed_time;

    // Now check that each value is what we expect.
    size_t cnt_wrong = 0;
    for( size_t i = 0; i < num_elems; ++i ) {
        if( values[i] != num_threads ) {
            ++cnt_wrong;
        }
    }
    EXPECT_EQ( 0, cnt_wrong );
}

TEST( Threading, VectorEntries )
{
    // Test siez
    size_t const num_threads = 10;
    size_t const num_vecs = 1000;
    size_t const max_length = 1000;

    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // Fill a vector with vectors of different lengths, with values that are all 1.
    // Then, we can add up those entries to get our expected number of processed elements.
    size_t exp_num_elem = 0;
    auto data = std::vector<std::vector<int>>( num_vecs );
    for( auto& vec : data ) {
        vec.resize( permuted_congruential_generator( max_length ), 1 );
        exp_num_elem += vec.size();
    }

    // Now we spin up some threads and erase elements in parallel from the vector, starting
    // at different offsets for speed. Each thread starts processing elements, and removes
    // them from their vec within data, until empty, and then moves to the next one.
    auto thread_pool = std::make_shared<ThreadPool>( num_threads );
    auto vector_guard = ConcurrentVectorGuard( num_vecs );
    std::atomic<size_t> num_elem{0};
    for( size_t t = 0; t < num_threads; ++t ) {
        thread_pool->enqueue_detached([&, t]()
        {
            // Get equally distributed starting group indices.
            size_t const start_group_idx = t * num_vecs / num_threads;
            size_t cur_group_idx = start_group_idx;

            while( true ) {
                // Lock the vector that we are currently operating on.
                auto lock = vector_guard.get_lock_guard(cur_group_idx);

                // Find the next vector that has data.
                // If the current one does not, we move to the next (in the next iteration).
                // If we looped around and arrive back where we started, we are done.
                if( data[cur_group_idx].empty() ) {
                    ++cur_group_idx;
                    cur_group_idx %= num_vecs;
                    if( cur_group_idx == start_group_idx ) {
                        break;
                    }
                    continue;
                }

                // Remove the last entry from the vector
                // LOG_DBG << "thread " << t << " popping data[" << cur_group_idx << "] at " << (data[cur_group_idx].size() - 1);
                data[cur_group_idx].pop_back();
                ++num_elem;
            }
        });
    }
    thread_pool->wait_for_all_pending_tasks();

    // Now we expect to have processed exactly the elements that we put in data,
    // and that we have left the vectors in data empty.
    EXPECT_EQ( exp_num_elem, num_elem );
    for( auto const& vec : data ) {
        EXPECT_TRUE( vec.empty() );
        EXPECT_EQ( 0, vec.size() );
    }
}
