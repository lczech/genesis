/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/utils/threading/serial_task_queue.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//     Basics
// =================================================================================================

TEST( SerialTaskQueue, SequentialExecution )
{
    auto pool = std::make_shared<ThreadPool>(4);
    SerialTaskQueue queue(pool);

    std::mutex vec_mutex;
    std::vector<int> execution_order;
    const int num_tasks = 1000;

    // Enqueue tasks that push their index into a vector.
    for (int i = 0; i < num_tasks; ++i) {
        LOG_DBG << "queue.enqueue_detached A";
        queue.enqueue_detached([i, &execution_order, &vec_mutex]()
        {
            LOG_DBG << "execution_order.push_back";
            std::lock_guard<std::mutex> lock(vec_mutex);
            execution_order.push_back(i);
        });
    }
    // Enqueue a final task (via enqueue_and_retrieve) to wait until all tasks finish.
    auto future = queue.enqueue_and_retrieve([](){});
    future.get();

    // Check that tasks executed in the order they were enqueued.
    ASSERT_EQ(execution_order.size(), static_cast<size_t>(num_tasks));
    for (int i = 0; i < num_tasks; ++i) {
        EXPECT_EQ(execution_order[i], i);
    }
}

TEST( SerialTaskQueue, EnqueueAndRetrieve )
{
    auto pool = std::make_shared<ThreadPool>(4);
    SerialTaskQueue queue(pool);

    // Enqueue a simple lambda that returns a computed value.
    auto future = queue.enqueue_and_retrieve(
        [](int a, int b)
        {
            return a + b;
        },
        10, 32
    );

    int result = future.get();
    EXPECT_EQ(result, 42);
}

TEST( SerialTaskQueue, EnqueueDetached )
{
    auto pool = std::make_shared<ThreadPool>(4);
    SerialTaskQueue queue(pool);

    std::atomic<int> counter(0);
    const int num_tasks = 100;

    // Enqueue detached tasks that increment the counter.
    for (int i = 0; i < num_tasks; ++i) {
        LOG_DBG << "queue.enqueue_detached A";
        queue.enqueue_detached(
            [](std::atomic<int>& cnt, int increment)
            {
                LOG_DBG << "counter.fetch_add";
                cnt.fetch_add(increment, std::memory_order_relaxed);
            },
            std::ref(counter), 1
        );
        LOG_DBG << "queue.enqueue_detached B";
    }

    // Enqueue a final task to ensure all previous tasks have run.
    // auto future = queue.enqueue_and_retrieve([](){});
    // future.get();
    pool->wait_for_all_pending_tasks();

    EXPECT_EQ(counter.load(), num_tasks);
}

// =================================================================================================
//     StressTest
// =================================================================================================

TEST( SerialTaskQueue, StressTest )
{
    std::atomic<int> counter(0);
    const int total_tasks = 128;
    // const int total_tasks = 100000;
    const int num_threads = 8;
    int tasks_per_thread = total_tasks / num_threads;

    auto pool = std::make_shared<ThreadPool>(num_threads);
    SerialTaskQueue queue(pool);

    // Launch multiple threads that concurrently enqueue detached tasks.
    std::vector<std::thread> enqueuers;
    for (int i = 0; i < num_threads; ++i) {
        LOG_DBG << "enqueuers.emplace_back A";
        enqueuers.emplace_back(
            [&queue, &counter, tasks_per_thread]()
            {
                LOG_DBG << "enqueuers.emplace_back B";
                for (int j = 0; j < tasks_per_thread; ++j) {
                    LOG_DBG << "queue.enqueue_detached A";
                    queue.enqueue_detached([&counter]() {
                        LOG_DBG << "counter.fetch_add";
                        counter.fetch_add(1, std::memory_order_relaxed);
                    });
                    LOG_DBG << "queue.enqueue_detached B";
                }
                LOG_DBG << "enqueuers.emplace_back C";
            }
        );
        LOG_DBG << "enqueuers.emplace_back D";
    }
    for (auto &t : enqueuers) {
        t.join();
    }

    // Enqueue a final task to wait for the serial queue to drain.
    // auto future = queue.enqueue_and_retrieve([](){});
    // future.get();
    pool->wait_for_all_pending_tasks();

    EXPECT_EQ(counter.load(), total_tasks);
}
