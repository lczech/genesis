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

#include "genesis/utils/containers/threadsafe_queue.hpp"

#include <atomic>
#include <future>
#include <string>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//     Empty
// =================================================================================================

// Test cases adapted from https://github.com/anthonywilliams/ccia_code_samples/blob/main/listings/listing_11.1.cpp
// See our Optional implementation in genesis/lib/genesis/utils/containers/threadsafe_queue.hpp
// for details on the copyright and license.

void test_concurrent_push_and_pop_on_empty_queue_()
{
    ThreadsafeQueue<int> q;

    std::promise<void> go, push_ready, pop_ready;
    std::shared_future<void> ready(go.get_future());

    std::future<void> push_done;
    std::future<int> pop_done;

    try
    {
        push_done = std::async(
            std::launch::async,
            [&q,ready,&push_ready]() {
                push_ready.set_value();
                ready.wait();
                q.push(42);
            }
        );
        pop_done = std::async(
            std::launch::async,
            [&q,ready,&pop_ready]() {
                pop_ready.set_value();
                ready.wait();
                return *q.wait_and_pop();
            }
        );

        // Set up all threads to wait for the signal, then go
        push_ready.get_future().wait();
        pop_ready.get_future().wait();
        go.set_value();

        // Signal was given, now we wait for results
        push_done.get();
        EXPECT_TRUE( pop_done.get() == 42 );
        EXPECT_TRUE( q.empty() );
    }
    catch(...)
    {
        go.set_value();
        throw;
    }
}

TEST( Containers, ThreadsafeQueueEmpty )
{
    for( size_t i = 0; i < 10000; ++i ) {
        test_concurrent_push_and_pop_on_empty_queue_();
    }
}

// =================================================================================================
//     High load
// =================================================================================================

void test_concurrent_push_and_pop_on_high_load_queue_()
{
    ThreadsafeQueue<int> q;

    std::atomic<int> push_ready{0};
    std::atomic<int> pop_ready{0};
    std::promise<void> go;
    std::shared_future<void> ready(go.get_future());

    size_t const n = 1000;
    auto push_done = std::vector<std::future<void>>(n);
    auto pop_done = std::vector<std::future<int>>(n);

    try
    {
        // Create many threads for pushing and popping, all incrementing their ready variables.
        // We then wait for these ready variables to saturate, indicating that all threads are
        // done being set up and are ready to go. Then, we signal _all_ of them  at the same time,
        // creating a lot of contention on the queue.
        for( size_t i = 0; i < n; ++i ) {
            push_done[i] = std::async(
                std::launch::async,
                [&q,ready,&push_ready]() {
                    ++push_ready;
                    ready.wait();
                    q.push(42);
                }
            );

            pop_done[i] = std::async(
                std::launch::async,
                [&q,ready,&pop_ready]() {
                    ++pop_ready;
                    ready.wait();
                    return *q.wait_and_pop();
                }
            );
        }

        // Set up all threads to wait for the signal, then go!
        // Busy wait is okay here for test purposes
        while( push_ready.load() != n && pop_ready.load() != n ) {
            std::this_thread::yield();
        }
        go.set_value();

        // Signal was given, now we wait for results
        for( size_t i = 0; i < n; ++i ) {
            push_done[i].get();
            EXPECT_TRUE( pop_done[i].get() == 42 );
        }
        EXPECT_TRUE( q.empty() );
    }
    catch(...)
    {
        go.set_value();
        throw;
    }
}

TEST( Containers, ThreadsafeQueueHighLoad )
{
    for( size_t i = 0; i < 10; ++i ) {
        test_concurrent_push_and_pop_on_high_load_queue_();
    }
}
