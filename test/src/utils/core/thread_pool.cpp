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

#include "src/common.hpp"

#include "genesis/utils/core/thread_pool.hpp"
#include "genesis/utils/core/options.hpp"

using namespace genesis::utils;

void thread_pool_sleep()
{
    // We add a sleep in each task, so that multiple tasks get submitted first,
    // before the pool starts running them, so that they have a chance to submit their nested tasks.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void thread_pool_work(size_t i)
{
    (void) i;

    // LOG_DBG << "sub B " << i;
    auto pool = Options::get().global_thread_pool();
    auto res = pool->enqueue([=](){
        // LOG_DBG << "beg B " << i;
        thread_pool_sleep();
        // LOG_DBG << "end B " << i;
        return true;
    });//.get();
    // LOG_DBG << "sup B " << i;
    res.get();
    // LOG_DBG << "fin B " << i;
}

TEST( ThreadPool, Nested )
{
    Options::get().init_global_thread_pool( 2 );

    // LOG_DBG << "---";
    std::vector<std::future<bool>> tasks;

    for (size_t i = 0; i < 4; ++i) {
        // LOG_DBG << "sub A " << i;
        auto pool = Options::get().global_thread_pool();
        tasks.emplace_back( pool->enqueue( [=](){
            // LOG_DBG << "beg A " << i;
            thread_pool_sleep();
            thread_pool_work(i);
            thread_pool_sleep();
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
