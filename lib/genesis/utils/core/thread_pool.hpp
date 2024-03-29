#ifndef GENESIS_UTILS_CORE_THREAD_POOL_H_
#define GENESIS_UTILS_CORE_THREAD_POOL_H_

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

/*
    The parallel block/for functionality is inspired by BS::thread_pool,
    see https://github.com/bshoshany/thread-pool, but adapted to fit into our contex,
    and improved for a more even distribution of the workload, and usage convenience.

    We here still need to include the following original license of BS::thread_pool:

    MIT License

    Copyright (c) 2022 Barak Shoshany

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <iterator>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "genesis/utils/core/multi_future.hpp"
#include "genesis/utils/core/logging.hpp"

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
 *
 * When a submitted task itself tries to enqueue a task (which can happen in our library, where
 * parallelization is introduced at several levels of granularity), we execute that nested task
 * directly, instead of enqueueing it in the pool. Otherwise, we could end up in a deadlock situation,
 * where the submitting task is executed in the pool, and keeps waiting for the nested task, but
 * that one never gets picked up by a thread, as all threads might be in such a waiting state.
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
    explicit ThreadPool( size_t num_threads )
    {
        // Store the ID where this pool was started.
        this_id_ = std::this_thread::get_id();

        // Create the threads.
        init_( num_threads );
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
    //     Accessor Members
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
     * @brief Return counts of the number of tasks that have been enqueued in total (first) and that
     * have been nested calles that were executed directly without being enqueued (second).
     */
    std::pair<size_t, size_t> statistics() const
    {
        return std::make_pair( enqueued_tasks_.load(), nested_tasks_.load() );
    }

    // -------------------------------------------------------------
    //     Pool Functionality
    // -------------------------------------------------------------

    /**
     * @brief Enqueue a new task, using a function to call and its arguments.
     *
     * The enqueue function returns a future that can be used to check whether the task has
     * finished, or to wait for it to be finished. This also allows the task to send its result
     * back to the caller, if needed, by simply returning it from the task function.
     *
     * We internally use a std::packaged_task, so that any exception thrown in the function will
     * be caught and trapped inside of the std::future, until its get() function is called.
     */
    template<class F, class... Args>
    auto enqueue( F&& f, Args&&... args )
    -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using result_type = typename std::result_of<F(Args...)>::type;
        assert( ! worker_pool_.empty() );

        // Prepare the task by binding the function to its arguments.
        // Using a packaged task ensures that any exception thrown in the task function
        // will be caught by the future, and re-thrown when its get() function is called,
        // see e.g., https://stackoverflow.com/a/16345305/4184258
        auto task = std::make_shared< std::packaged_task<result_type()> >(
            std::bind( std::forward<F>(f), std::forward<Args>(args)... )
        );

        // Prepare the resulting future result of the task
        std::future<result_type> future_result = task->get_future();

        // If this function was called from a thread that is not the same as from where this pool
        // was started, we have to assume that it's a nested call, where a task in a pool itself
        // submits tasks to the queue. In that case, we could easily deadlock, as the initial task
        // is runnining in one of the threads of the pool, but will only finish once its nested task
        // is done, but that might starve at the end of the queue, as all running tasks might be
        // in such a waiting state...
        // In order to solve this, we just run the task function here directly, without enqueueing it.
        // This might mess up the whole purpose of the pool if tasks can be submitted from other
        // threads that are not threads in the pool itself - but that's not a use case that we
        // currently have, so we can live with that. It would still work, but not make optimal use
        // of the threads then.
        if( this_id_ != std::this_thread::get_id() ) {
            (*task)();
            ++nested_tasks_;
            return future_result;
        }

        // Here, we know that the task was enqueued from the same thread as the pool was started from,
        // which we assert, and then we can add it to the queue without dead locking.
        // Put the task into the queue, synchronized, and in a small scope.
        assert( this_id_ == std::this_thread::get_id() );
        {
            std::unique_lock<std::mutex> lock( task_queue_mutex_ );

            // Do not allow adding tasks after stopping the pool
            if( terminate_pool_ ) {
                throw std::runtime_error( "Cannot enqueue task into terminated ThreadPool." );
            }
            task_queue_.emplace([task](){ (*task)(); });
            ++enqueued_tasks_;
        }

        // Get a worker to pick up the task, and return the future result
        condition_.notify_one();
        return future_result;
    }

    /**
     * @brief Parallel block over a range of elements, breaking the range into blocks for which
     * the @p body function is executed individually.
     *
     * The function takes a @p begin index and an @p end (past-the-end) index, and executes
     * the @p body in @p num_blocks many blocks, aiming to equally distribute the work across the
     * range @p begin to @p end.
     * In other words, it is equivalent to `for( T i = begin; i < end; ++i )`.
     *
     * The @p body function to loop through is called once per block. It should take two arguments:
     * the first index in the block and the index after the last index in the block, e.g.:
     *
     * ```
     * []( size_t begin, size_t end ) {
     *     for( size_t i = begin; i < end; ++i ) {
     *         // ...
     *     }
     * }
     * ```
     *
     * The @p num_blocks determines the number of blocks to split the loop body into. Default is
     * to use the number of threads in the pool.
     *
     * The function returns a MultiFuture that can be used to wait for all the blocks to finish.
     * If the loop body returns a value, the MultiFuture can also be used to obtain the values
     * returned by each block, for example:
     *
     * ```
     * []( size_t begin, size_t end ) {
     *     size_t block_sum = 0;
     *     for( size_t i = begin; i < end; ++i ) {
     *         // ...
     *         block_sum += ...
     *     }
     *     return block_sum;
     * }
     * ```
     *
     * This is useful for example for aggregating values, such as a parallel sum.
     */
    template<
        typename F,
        typename T1, typename T2, typename T = typename std::common_type<T1, T2>::type,
        typename R = typename std::result_of<typename std::decay<F>::type(T, T)>::type
    >
    MultiFuture<R> parallel_block( T1 begin, T2 end, F&& body, size_t num_blocks = 0 )
    {
        // Get the total range and number of tasks.
        // Casting, so that we work with a common type... bit of a relic, but sure, why not.
        auto begin_t = static_cast<T>( begin );
        auto end_t = static_cast<T>( end );
        if( begin_t > end_t ) {
            std::swap( begin_t, end_t );
        }
        size_t const total_size = end_t - begin_t;

        // Edge case. Nothing to do.
        if( total_size == 0 ) {
            assert( begin_t == end_t );
            return MultiFuture<R>();
        }

        // Default block size is the number of threads, unless there are not even that many tasks,
        // in which case we can use fewer blocks.
        if( num_blocks == 0 ) {
            assert( worker_pool_.size() > 0 );
            num_blocks = worker_pool_.size();
        }
        if( num_blocks > total_size ) {
            num_blocks = total_size;
        }
        assert( num_blocks > 0 );
        assert( num_blocks <= total_size );

        // Compute the needed sizes. We do _not_ follow BS::thread_pool here, as they currently
        // do not distribute work optimally, see https://github.com/bshoshany/thread-pool/issues/96
        // Instead, we use blocks of minimal size, and add the remainder to the first few blocks,
        // so that the blocks that are one larger than the others run first, minimizing our wait
        // time at the end. See e.g., https://stackoverflow.com/a/36689107
        size_t const block_size = total_size / num_blocks;
        size_t const remainder  = total_size % num_blocks;
        assert( block_size > 0 );
        assert( remainder < num_blocks );

        // Enqueue all blocks.
        size_t current_start = 0;
        MultiFuture<R> result( num_blocks );
        for( size_t i = 0; i < num_blocks; ++i ) {
            // We get the length of the current block, and in the beginning also add one to their
            // length to distribute the remainder elements that did not fit evently into the blocks.
            // Use that length to get the begin and end points, and submit the block.
            auto const l = block_size + ( i < remainder ? 1 : 0 );
            auto const b = begin_t + static_cast<T>( current_start );
            auto const e = begin_t + static_cast<T>( current_start + l );
            assert( l > 0 );
            assert( b < e );
            result[i] = enqueue( std::forward<F>( body ), b, e );

            // Our next block will start where this one ended.
            current_start += l;
            assert( current_start <= total_size );
        }

        // Now we should have processed everything exactly.
        // Check this, then return the future.
        assert( current_start == total_size );
        assert( begin_t + static_cast<T>( current_start ) == end_t );
        return result;
    }

    /**
     * @brief Parallel `for` over a range of positions, breaking the range into blocks for which
     * the @p body function is executed individually.
     *
     * This is almost the same as parallel_block(), but intended to be used with `for` loops that
     * do not need to compute per-block return values. The function signature of `F` is hence simply
     * expected to be `void F( size_t i )`, and is called for every position in the processed range.
     *
     * ```
     * std::vector<int> numbers( 100 );
     * auto futures = pool->parallel_for(
     *     0, numbers.size(),
     *     [&numbers]( size_t i )
     *     {
     *         numbers[i] *= 2;
     *     }
     * );
     * futures.get();
     * ```
     *
     * This makes it a convenience function; see also parallel_for_each() for container-based data.
     */
    template<
        typename F,
        typename T1, typename T2, typename T = typename std::common_type<T1, T2>::type
    >
    MultiFuture<void> parallel_for( T1 begin, T2 end, F&& body, size_t num_blocks = 0 )
    {
        return parallel_block(
            begin, end,
            [body]( T b, T e ){
                for( size_t i = b; i < e; ++i ) {
                    body(i);
                }
            }, num_blocks
        );
    }

    /**
     * @brief Parallel `for each` over a container, processing it in blocks for which
     * the @p body function is executed individually.
     *
     * This is almost the same as parallel_for(), but intended to be used with containers.
     * The function signature of `F` is hence simply expected to be `void F( T& element )` or
     * `void F( T const& element )`, and is called for every element in the container.
     *
     * ```
     * std::vector<int> numbers( 100 );
     * auto futures = pool->parallel_for_each(
     *     numbers.begin(), numbers.end(),
     *     []( int& element )
     *     {
     *         element *= 2;
     *     }
     * );
     * futures.get();
     * ```
     *
     * This makes it a convenience function.
     */
    template<typename F, typename T>
    MultiFuture<void> parallel_for_each( T const begin, T const end, F&& body, size_t num_blocks = 0 )
    {
        // Boundary checks.
        auto const total = std::distance( begin, end );
        if( total < 0 ) {
            throw std::invalid_argument( "Cannot use parallel_for_each() with a reverse range." );
        }
        if( total == 0 ) {
            return MultiFuture<void>();
        }

        // Run the loop over elements in parallel blocks.
        // For some reason, we need to take `begin` by const copy in the signature above,
        // and copy it again here for the lambda. Otherwise, we run into some weird iterator
        // invalidity issues, that might come from the threading or something... it's weird.
        // The iterator itself is never advanced here, so that should not lead to this error...
        return parallel_block(
            0, total,
            [begin, body]( size_t b, size_t e ){
                for( size_t i = b; i < e; ++i ) {
                    body( *(begin + i) );
                }
            }, num_blocks
        );
    }

    /**
     * @brief Parallel `for each` over a container, processing it in blocks for which
     * the @p body function is executed individually.
     *
     * Expects a random access container that supports `size()` as well as `operator[]` to access
     * individual elements, and a @p body that takes one element of that container to process.
     *
     * ```
     * std::vector<int> numbers( 100 );
     * auto futures = pool->parallel_for_each(
     *     numbers,
     *     []( int& element )
     *     {
     *         element *= 2;
     *     }
     * );
     * futures.get();
     * ```
     *
     * This makes it a convenience function.
     */
    template<typename F, typename T>
    MultiFuture<void> parallel_for_each( T& container, F&& body, size_t num_blocks = 0 )
    {
        // Boundary checks.
        if( container.size() == 0 ) {
            return MultiFuture<void>();
        }

        // Run the loop over elements in parallel blocks.
        return parallel_block(
            0, container.size(),
            [&container, body]( size_t b, size_t e ) {
                for( size_t i = b; i < e; ++i ) {
                    body( container[ i ] );
                }
            }, num_blocks
        );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    void init_( size_t num_threads )
    {
        // Some dead end checking.
        if( num_threads == 0 ) {
            throw std::runtime_error(
                "Cannot construct an empty ThreadPool, as this would not be able to do anything."
            );
        }

        // Create the desired number of workers.
        for( size_t i = 0; i < num_threads; ++i ) {
            worker_pool_.emplace_back(
                [this]
                {
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
                                [this]
                                {
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
                }
            );
        }
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    // Store the current thread ID, needed for nested task submissions.
    std::thread::id this_id_;

    // Store all workers
    std::vector<std::thread> worker_pool_;

    // Store the task queue
    std::queue<std::function<void()>> task_queue_;

    // Synchronization
    mutable std::mutex task_queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> terminate_pool_{ false };

    // Statistics
    std::atomic<size_t> enqueued_tasks_{ 0 };
    std::atomic<size_t> nested_tasks_{ 0 };
};

} // namespace utils
} // namespace genesis

#endif // include guard
