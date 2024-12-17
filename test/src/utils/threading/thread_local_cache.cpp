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

    // We here set up the cache with one additional cache entry, which might be needed
    // in case that the final waiting for pending tasks actually has work to do:
    // Our Thread Pool implements work stealing, and so the main thread might then execute work,
    // so that we better allocate a cache entry for its thread local use as well.
    // Use brace initialization, as some older compilers (clang 6-8) do not yet expect copy elision,
    // such that the explicitly deleted move constructor fails to compile here otherwise.
    ThreadLocalCache<size_t> local_caches{ num_threads + 1 };
    ThreadPool thread_pool{ num_threads };

    // Enqueue some tasks, here, just a simple sum of all numbers up to num_tasks.
    // LOG_DBG << "start";
    for( size_t i = 0; i < num_tasks; ++i ) {
        thread_pool.enqueue_detached( [i, &local_caches]()
        {
            thread_local auto& local_cache = local_caches.get();
            local_cache += i;
        });
    }
    thread_pool.wait_for_all_pending_tasks();
    // LOG_DBG << "finish";

    // Sum up the sums of all caches.
    size_t total = 0;
    for( auto const& local_cache : local_caches ) {
        total += local_cache;
    }
    // LOG_DBG << "Cache: " << local_caches.size() << "/" << local_caches.capacity();

    // Check that we have the correct result, i.e., the sum of all numbers smaller than num_tasks
    auto const exp_val = num_tasks * (num_tasks - 1) / 2;
    EXPECT_EQ( exp_val, total );
}
