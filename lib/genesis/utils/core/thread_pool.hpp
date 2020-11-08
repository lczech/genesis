#ifndef GENESIS_UTILS_CORE_THREAD_POOL_H_
#define GENESIS_UTILS_CORE_THREAD_POOL_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Thread Pool
// =================================================================================================

/**
 * @brief Thread pool for distributed work.
 *
 * This simple implementation offer a standing pool of worker threads that pick up tasks.
 *
 * Example
 *
 *     // Create a thread pool with 2 worker threads
 *     ThreadPool thread_pool( 2 );
 *
 *     // Enqueue a new task by providing a function and its arguments, and store its future result
 *     auto result = thread_pool.enqueue(
 *         []( int some_param ) {
 *             // do computations
 *             int some_result = 42;
 *             return some_result;
 *         },
 *         0 // value for `some_param`
 *     );
 *
 *     // Get the value of `some_result` from the future
 *     // As this is a future, the function call to get() blocks until a thread has finished the work
 *     std::cout << result.get() << "\n";
 *
 * As the workers are stored in the ThreadPool object itself, it does not allow to be copied.
 */
class ThreadPool
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct a thread pool with a given number of workers.
     *
     * We allow for 0 tasks on construction, so that default construction is possible.
     * However, the enqueue() throws if used with such an instance.
     */
    explicit ThreadPool( size_t num_threads = 0 )
    {
        // Create the desired number of workers
        for( size_t i = 0; i < num_threads; ++i ) {
            worker_pool_.emplace_back( [this]{

                // Each worker runs an infinite loop until requested to stop,
                // using conditional waits to not waste compute power.
                while( true ) {
                    std::function<void()> task;

                    // Synchronized access to the task list: see if there is a task to be done,
                    // and if so, pick it up and remove it from the queue
                    {
                        std::unique_lock<std::mutex> lock( task_queue_mutex_ );
                        this->condition_.wait(
                            lock,
                            [this]{
                                return !this->task_queue_.empty() || this->terminate_pool_;
                            }
                        );
                        if( this->terminate_pool_ && this->task_queue_.empty() ) {
                            return;
                        }
                        task = std::move( this->task_queue_.front() );
                        this->task_queue_.pop();
                    }

                    // Run the task
                    task();
                }
            });
        }
    }

    ThreadPool( ThreadPool const& ) = delete;
    ThreadPool( ThreadPool&& )      = delete;

    ThreadPool& operator= ( ThreadPool const& ) = delete;
    ThreadPool& operator= ( ThreadPool&& )      = delete;

    /**
     * @brief Destruct the thread pool,
     * stopping and joining any workers that are potentially still running or waiting.
     */
    ~ThreadPool()
    {
        // Set synchronized signal to all workers to terminate
        {
            std::unique_lock<std::mutex> lock( task_queue_mutex_ );
            terminate_pool_ = true;
        }

        // Wake up all workers, and join them back into the main thread
        condition_.notify_all();
        for( std::thread& worker : worker_pool_ ) {
            if( worker.joinable() ) {
                worker.join();
            }
        }
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Return the number of workers of the thread pool.
     */
    size_t size() const
    {
        return worker_pool_.size();
    }

    /**
     * @brief Return the current number of queued tasks.
     */
    size_t load() const
    {
        std::unique_lock<std::mutex> lock( task_queue_mutex_ );
        return task_queue_.size();
    }

    /**
     * @brief Enqueue a new task, using a function to call and its arguments.
     *
     * The enqueue function returns a future that can be used to check whether the task has
     * finished, or to wait for it to be finished. This also allows the task to send its result
     * back to the caller, if needed, by simply returning it from the task function.
     */
    template<class F, class... Args>
    auto enqueue( F&& f, Args&&... args )
    -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using result_type = typename std::result_of<F(Args...)>::type;

        // Some dead end checking
        if( worker_pool_.empty() ) {
            throw std::runtime_error( "Cannot enqueue task into empty ThreadPool." );
        }

        // Prepare the task by binding the function to its arguments
        auto task = std::make_shared< std::packaged_task<result_type()> >(
            std::bind( std::forward<F>(f), std::forward<Args>(args)... )
        );

        // Prepare the resulting future result of the task
        std::future<result_type> future_result = task->get_future();

        // Put the task into the queue, synchronized
        {
            std::unique_lock<std::mutex> lock( task_queue_mutex_ );

            // Do not allow adding tasks after stopping the pool
            if( terminate_pool_ ) {
                throw std::runtime_error( "Cannot enqueue task into terminated ThreadPool." );
            }
            task_queue_.emplace([task](){ (*task)(); });
        }

        // Get a worker to pick up the task, and return the future result
        condition_.notify_one();
        return future_result;
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    // Store all workers
    std::vector<std::thread> worker_pool_;

    // Store the task queue
    std::queue<std::function<void()>> task_queue_;

    // Synchronization
    mutable std::mutex task_queue_mutex_;
    std::condition_variable condition_;
    bool terminate_pool_ = false;
};

} // namespace utils
} // namespace genesis

#endif // include guard
