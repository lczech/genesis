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
#include "genesis/utils/core/std.hpp"

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
public:

    // -------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------

    using Task = std::function<void()>;

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
        running_ = other.running_;
        other.running_ = false;
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
        running_ = other.running_;
        other.running_ = false;
        return *this;
    }

    // -------------------------------------------------------------
    //     Pool Functionality
    // -------------------------------------------------------------

    template<typename F, typename... Args>
    inline auto enqueue_and_retrieve( F&& f, Args&&... args )
    -> ProactiveFuture<typename genesis_invoke_result<F, Args...>::type>
    {
        using result_type = typename genesis_invoke_result<F, Args...>::type;

        // Bind the function and its arguments and wrap it in a std::function,
        // using a shared pointer because otherwise macos does not capture this correctly...
        auto task_ptr = std::make_shared<std::function<result_type()>>(
            std::bind( std::forward<F>(f), std::forward<Args>(args)... )
        );

        // Create a packaged_task that calls the callable via the shared_ptr.
        auto wrapped_task = std::make_shared<std::packaged_task<result_type()>>(
            [task_ptr]() { return (*task_ptr)(); }
        );
        auto future_result = ProactiveFuture<result_type>( wrapped_task->get_future(), *pool_ );

        // Enqueue the packaged task, and return the future.
        enqueue_(
            [wrapped_task]()
            {
                (*wrapped_task)();
            }
        );
        return future_result;
    }

    template<typename F, typename... Args>
    inline void enqueue_detached( F&& f, Args&&... args )
    {
        // Bind the function and its arguments and wrap it in a std::function,
        // using a shared pointer because otherwise macos does not capture this correctly...
        auto task_ptr = std::make_shared<std::function<void()>>(
            std::bind( std::forward<F>(f), std::forward<Args>(args)... )
        );

        // Capture the shared_ptr in the lambda so that the callable’s state is maintained.
        enqueue_(
            [task_ptr]() mutable
            {
                (*task_ptr)();
            }
        );
    }

    // -------------------------------------------------------------
    //     Internals
    // -------------------------------------------------------------

private:

    void enqueue_( Task task )
    {
        {
            // Scoped lock to add the task to the queue and
            // signal that we are processing now.
            std::lock_guard<std::mutex> lock( mutex_ );
            tasks_.push( std::move( task ));
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

    void process_tasks_()
    {
        while( true ) {
            Task current_task;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if( tasks_.empty() ) {
                    running_ = false;
                    break;
                }
                current_task = std::move( tasks_.front() );
                tasks_.pop();
            }
            current_task();
        }
    }

    std::shared_ptr<ThreadPool> pool_;
    std::queue<Task> tasks_;
    std::mutex mutex_;
    bool running_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
