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

#include "genesis/utils/threading/concurrent_vector_guard.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

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
