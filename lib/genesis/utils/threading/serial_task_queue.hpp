#ifndef GENESIS_UTILS_THREADING_SERIAL_TASK_QUEUE_H_
#define GENESIS_UTILS_THREADING_SERIAL_TASK_QUEUE_H_

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
 * @ingroup utils
 */

#include "genesis/utils/threading/thread_pool.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/std.hpp"

#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Serial Task Queue
// =================================================================================================

/**
 * @brief Serial Task Queue to run a series of tasks one after another.
 *
 * The class takes a ThreadPool to submit its tasks to, and then makes sure that they are run
 * in series in the pool. Upon enqueuing a task here, a task in the thread pool is enqueued as well,
 * which then processes the tasks submitted here.
 *
 * If the caller keeps submitting tasks here while there are still some running, those get added
 * to the end of the queue, and the task in the thread pool will keep running. If at some point
 * all tasks are done, the task in the pool also finishes. Then, if new tasks are submitted here
 * later, a new task is again submitted to the pool that processed the queue.
 *
 * The class offers the same two ways of submitting tasks: retrieving a ProactiveFuture to wait
 * for the submitted task, and a fire-and-forget submission that does not return a future.
 */
class SerialTaskQueue
{
    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

private:

    /**
     * @brief Helper to wrap a std::function into an object
     *
     * This seems to be needed so that macos apple clang libc++ can properly copy/move it.
     * Apparently, there is a bug in there where a normal lambda ends up valid but empty
     * when copied or moved, so we need this workaround...
     */
    struct WrappedTask
    {
        std::function<void()> function;
    };

    // -------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------

public:

    explicit SerialTaskQueue( std::shared_ptr<ThreadPool> pool )
        : pool_( std::move( pool ))
        , running_( false )
    {}

    ~SerialTaskQueue() noexcept = default;

    // Deleted copies
    SerialTaskQueue( SerialTaskQueue const& ) = delete;
    SerialTaskQueue& operator=( SerialTaskQueue const& ) = delete;

    // Move constructor.
    SerialTaskQueue( SerialTaskQueue&& other ) noexcept
    {
        // Lock the other object to safely move its state.
        std::lock_guard<std::mutex> lock( other.mutex_ );
        pool_ = std::move( other.pool_ );
        tasks_ = std::move( other.tasks_ );
        running_ = other.running_.load( std::memory_order_relaxed );
        other.running_.store( false, std::memory_order_relaxed );
    }

    // Move assignment operator.
    SerialTaskQueue& operator=( SerialTaskQueue&& other ) noexcept
    {
        if( this == &other ) {
            return *this;
        }

        // Lock both mutexes without deadlock.
        std::lock( mutex_, other.mutex_ );
        std::lock_guard<std::mutex> lock1( mutex_, std::adopt_lock );
        std::lock_guard<std::mutex> lock2( other.mutex_, std::adopt_lock );
        pool_ = std::move( other.pool_ );
        tasks_ = std::move( other.tasks_ );
        running_ = other.running_.load( std::memory_order_relaxed );
        other.running_.store( false, std::memory_order_relaxed );
        return *this;
    }

    // -------------------------------------------------------------
    //     Pool Functionality
    // -------------------------------------------------------------

    /**
     * @brief Enqueue a task that returns a future.
     *
     * This function wraps the given function and its arguments into a packaged task,
     * enqueues it for execution, and returns a future associated with the task's result.
     *
     * @tparam F   The function type.
     * @tparam Args The argument types.
     * @param f    The function to execute.
     * @param args The arguments to pass to the function.
     * @return std::future<typename std::result_of<F(Args...)>::type> A future holding the result.
     */
    template<typename F, typename... Args>
    inline auto enqueue_and_retrieve( F&& f, Args&&... args )
    -> ProactiveFuture<typename genesis_invoke_result<F, Args...>::type>
    {
        // Deduce the return type of the function.
        using result_type = typename genesis_invoke_result<F, Args...>::type;

        // Create a shared pointer to a packaged task wrapping the function and its arguments.
        auto task = std::make_shared<std::packaged_task<result_type()>>(
            std::bind( std::forward<F>(f), std::forward<Args>(args)... )
        );

        // Retrieve the future from the task.
        auto future_result = ProactiveFuture<result_type>( task->get_future(), *pool_ );

        // Enqueue the task to be executed by the thread pool.
        // The lambda captures the task by shared pointer to ensure it stays alive.
        enqueue_( WrappedTask{ [task](){ (*task)(); } } );

        return future_result;
    }

    /**
     * @brief Enqueue a task to be executed without retrieving a future.
     *
     * This function wraps the given function and its arguments into a callable object
     * and enqueues it for processing by the thread pool.
     *
     * @tparam F   The function type.
     * @tparam Args The argument types.
     * @param f    The function to execute.
     * @param args The arguments to pass to the function.
     */
    template<typename F, typename... Args>
    inline void enqueue_detached( F&& f, Args&&... args )
    {
        // Bind the function and its arguments to create a callable object.
        auto bound_task = std::bind( std::forward<F>(f), std::forward<Args>(args)... );

        // Enqueue the callable object as a WrappedTask.
        enqueue_( WrappedTask{ [bound_task](){ bound_task(); } } );
    }

    // -------------------------------------------------------------
    //     Internals
    // -------------------------------------------------------------

private:

    /**
     * @brief Internal enqueing that takes care of managing the worker thread.
     */
    inline void enqueue_( WrappedTask&& wrapped_task )
    {
        {
            // Scoped lock to add the task to the queue and
            // signal that we are processing now.
            std::lock_guard<std::mutex> lock( mutex_ );
            tasks_.push( std::move( wrapped_task ));
            if( running_ ) {
                return;
            }
            running_ = true;
        }

        // We only reach this point if there isn't already
        // a thread processing the queue. In that case, start one.
        assert( pool_ );
        pool_->enqueue_detached(
            [this]
            {
                process_tasks_();
            }
        );
    }

    /**
     * @brief Worker function that is submitted to the Thread Pool.
     */
    void process_tasks_()
    {
        while( true ) {
            WrappedTask current_task;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if( tasks_.empty() ) {
                    running_ = false;
                    break;
                }
                current_task = std::move( tasks_.front() );
                tasks_.pop();
            }
            assert( current_task.function );
            current_task.function();
        }
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    std::shared_ptr<ThreadPool> pool_;
    std::queue<WrappedTask> tasks_;
    std::mutex mutex_;
    std::atomic<bool> running_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
