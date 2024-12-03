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

#include "src/common.hpp"

#include "genesis/utils/threading/thread_local_cache.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <string>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//     Thread Local Cache
// =================================================================================================

TEST( ThreadPool, ThreadLocalCache )
{
    size_t const num_threads = 8;
    size_t const num_tasks = 10000;

    // We here set up the thread pool with exactly the number of threads we want.
    // In a real life scenario, we usually want one thread fewer, and limit the task queue
    // (second argument of the pool), such that we get a pool with work stealing,
    // so that the main thread can also do work while waiting. Not done here for simplicity.
    auto global_cache = ThreadLocalCache<size_t>( num_threads );
    auto thread_pool = ThreadPool( num_threads );

    // Enqueue some tasks, here, just a simple sum of all numbers up to num_tasks.
    for( size_t i = 0; i < num_tasks; ++i ) {
        thread_pool.enqueue_detached( [i, &global_cache]()
        {
            thread_local auto& cache = global_cache.get();
            cache += i;
        });
    }
    thread_pool.wait_for_all_pending_tasks();

    // Sum up the sums of all caches.
    size_t total = 0;
    for( auto const& cache : global_cache ) {
        total += cache;
    }

    // Check that we have the correct result, i.e., the sum of all numbers smaller than num_tasks
    auto const exp_val = num_tasks * (num_tasks - 1) / 2;
    EXPECT_EQ( exp_val, total );
}
