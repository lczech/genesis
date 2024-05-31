#ifndef GENESIS_UTILS_CORE_THREAD_POOL_H_
#define GENESIS_UTILS_CORE_THREAD_POOL_H_

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
 * @ingroup utils
 */

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iterator>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class ThreadPool;

// =================================================================================================
//     Proactive Future
// =================================================================================================

/**
 * @brief Wrapper around `std::future` that implements busy waiting.
 *
 * This has the same interface and functionality as `std::future`, with the key difference that
 * when calling wait(), tasks from the ThreadPool queue are processed while waiting. This avoids
 * the pool to deadlock should tasks submit tasks of their own that they are then waiting for.
 * In such a scenario, all threads in the pool could be waiting for their submitted tasks, but none
 * of them can run, because all the threads are already processing a task (the one that is stuck
 * waiting).
 *
 * The technique is inspired by "C++ Concurrency in Action" * book by Anthony Williams, second
 * edition, chapter 9, where this idea is mentioned as a way to avoid starving tasks. We here wrap
 * this idea into a class, so that users of the ThreadPool have to use this feature, and hence
 * avoid the deadlock.
 */
template<class T>
class ProactiveFuture
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Public default constructor, so that for instance a `std::vector` of ProactiveFuture can
     * be created.
     */
    ProactiveFuture() noexcept = default;

private:

    /**
     * @brief Private constructur, so that only a ThreadPool can create instances.
     */
    ProactiveFuture( std::future<T> future_result, ThreadPool& thread_pool )
        : future_( std::move( future_result ))
        , thread_pool_( &thread_pool )
    {}

    friend ThreadPool;

public:

    ~ProactiveFuture() noexcept = default;

    ProactiveFuture( ProactiveFuture&& ) noexcept = default;
    ProactiveFuture( const ProactiveFuture& ) = delete;

    ProactiveFuture& operator=( ProactiveFuture&& ) noexcept = default;
    ProactiveFuture& operator=( const ProactiveFuture& ) = delete;

    // -------------------------------------------------------------
    //     Forwarded members
    // -------------------------------------------------------------

    /**
     * @brief Return the result, after calling wait().
     */
    T get()
    {
        // Use our busy waiting first, until we are ready.
        wait();
        assert( ready() );
        return future_.get();
    }

    /**
     * @brief Return the result, after calling wait().
     */
    template<typename U = T>
    typename std::enable_if<!std::is_void<U>::value, U&>::type get()
    {
        // Enable this method only if T is not void (non-void types).
        static_assert( ! std::is_same<T, void>::value, "ProactiveFuture::get() intended for T != void" );

        // Use our busy waiting first, until we are ready.
        wait();
        assert( ready() );
        return future_.get();
    }

    /**
     * @brief Return the result, after calling wait().
     */
    template<typename U = T>
    typename std::enable_if<std::is_void<U>::value>::type get()
    {
        // Enable this method only if T is void
        static_assert( std::is_same<T, void>::value, "ProactiveFuture::get() intended for T == void" );

        // Use our busy waiting first, until we are ready.
        wait();
        assert( ready() );
        return future_.get();
    }

    /**
     * @brief Check if the future has a shared state.
     */
    bool valid() const noexcept
    {
        return future_.valid();
    }

    /**
     * @brief Wait for the result to become available.
     *
     * This is the main function that differs from `std::future::wait()`, in that it processes
     * other tasks from the pool while waiting, until the underlying future is ready.
     */
    void wait() const;

    /**
     * @brief Wait for the result, return if it is not available for the specified timeout duration.
     *
     * This simply forwards to the `wait_for` function of the future. Note that this does _not_ do
     * the busy waiting that this wrapper is intended for. Hence, calling this function in a loop
     * until the future is ready might never happen, in case that the ThreadPool dead locks due to
     * the task waiting for a (then) starving other task. The whole idea of this class is to avoid
     * this scenario, by processing these potentially starving tasks. We hence recommend to not use
     * this function, or at least not in a loop, unless you are sure that none of your tasks submit
     * any tasks of their own to the same thread pool.
     */
    template< class Rep, class Period >
    std::future_status wait_for( std::chrono::duration<Rep,Period> const& timeout_duration ) const
    {
        // If the user species a time to wait for, we just forward that to the future.
        return future_.wait_for( timeout_duration );
    }

    /**
     * @brief Wait for the result, return if it is not available until specified time point has been
     * reached.
     *
     * This simply forwards to the `wait_until` function of the future. The same caveat as explained
     * in wait_for() applies here as well. We hence recommend to not use this function.
     */
    template< class Clock, class Duration >
    std::future_status wait_until( std::chrono::time_point<Clock,Duration> const& timeout_time ) const
    {
        // If the user species a time to wait until, we just forward that to the future.
        return future_.wait_until( timeout_time );
    }

    // -------------------------------------------------------------
    //     Additional members
    // -------------------------------------------------------------

    /**
     * @brief Check if the future is ready.
     */
    bool ready() const
    {
        throw_if_invalid_();
        return future_.wait_for( std::chrono::seconds(0) ) == std::future_status::ready;
    }

    /**
     * @brief Check if the future is deferred, i.e., the result will be computed only when
     * explicitly requested.
     *
     * This should always return `false`, as we never create a deferred future ourselves.
     * This would indicate some misuse of the class via `std::async` or some other mechanism.
     */
    bool deferred() const
    {
        throw_if_invalid_();
        return future_.wait_for( std::chrono::seconds(0) ) == std::future_status::deferred;
    }

    // -------------------------------------------------------------
    //     Internal members
    // -------------------------------------------------------------

private:

    void throw_if_invalid_() const
    {
        // From: https://en.cppreference.com/w/cpp/thread/future/wait
        // The implementations are encouraged to detect the case when valid() == false before the
        // call and throw a std::future_error with an error condition of std::future_errc::no_state.
        if( !future_.valid() ) {
            throw std::future_error( std::future_errc::no_state );
        }
    }

    // -------------------------------------------------------------
    //     Data members
    // -------------------------------------------------------------

private:

    std::future<T> future_;
    ThreadPool* thread_pool_;

};

// =================================================================================================
//     Thread Pool
// =================================================================================================

/**
 * @brief Thread pool for distributed work.
 *
 * This simple implementation offer a standing pool of worker threads that pick up tasks.
 *
 * For reasons explained below, it is recommended to initialize a global thread pool via
 * Options::get().global_thread_pool(), with one fewer threads than intended to keep busy,
 * as the main thread will also be able to do busy work while waiting for tasks. Use
 * guess_number_of_threads() for obtaining the adequate number of total threads to run,
 * and subtract one to get the number to use this class with.
 *
 * Example
 *
 *     // Create a thread pool with 2 worker threads
 *     ThreadPool thread_pool( 2 );
 *
 *     // Enqueue a new task by providing a function and its arguments, and store its future result.
 *     // This is a ProactiveFuture, so that calling wait() or get() on it will process other tasks.
 *     auto result = thread_pool.enqueue(
 *         []( int some_param ) {
 *             // do computations
 *             int some_result = 42;
 *             return some_result;
 *         },
 *         0 // value for `some_param`
 *     );
 *
 *     // Get the value of `some_result` from the future.
 *     // As this is a future, the function call to get() blocks until a thread has finished the
 *     // work, but also processes other tasks from the queue in the meantime, see ProactiveFuture.
 *     std::cout << result.get() << "\n";
 *
 * As the workers are stored in the ThreadPool object itself, it does not allow to be copied.
 *
 * The pool implements a technique similar to the one described in the "C++ Concurrency in Action"
 * book by Anthony Williams, second edition, chapter 9, in order to avoid dead locking when tasks
 * submit their own tasks. In such cases, the parent task could then potentially be waiting  for the
 * child, but the child might never start, as all threads in the pool are busy waiting for children,
 * this could starve. Hence, our wrapper implementation ProactiveFuture instead processes tasks from the
 * queue while waiting for the future, so that those do not starve.
 *
 * This mechanism also allows to start a ThreadPool with 0 threads. In that case, all tasks will
 * be processed once wait() or get() is called on their returned ProactiveFuture (our thin wrapper
 * around `std::future`; see there for details) - essentially making the pool behave as
 * a lazy evaluator of the tasks. This is very convenient behavior to ensure that the number of
 * actually busy threads is exactly known - a main thread that waits for some submitted tasks will
 * also be doing work while waiting. Hence, we recommend to start thise pool with one fewer threads
 * than hardware concurrency (or whatever other upper limit you want to ensure, e.g., Slurm).
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
     * We allow for 0 tasks on construction. With no threads in the pool, every task submitted will
     * be processed instead once its future is queried via wait or get; it then hence behaves as a
     * lazy evaluating task queue.
     */
    explicit ThreadPool( size_t num_threads )
    {
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
    size_t currently_enqueued_tasks() const
    {
        std::unique_lock<std::mutex> lock( task_queue_mutex_ );
        return task_queue_.size();
    }

    /**
     * @brief Return counts of the number of tasks that have been enqueued in total.
     */
    size_t total_enqueued_tasks() const
    {
        return enqueued_tasks_.load();
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
     * be caught and trapped inside of the future, until its get() function is called.
     */
    template<class F, class... Args>
    auto enqueue( F&& f, Args&&... args )
    -> ProactiveFuture<typename std::result_of<F(Args...)>::type>
    {
        using result_type = typename std::result_of<F(Args...)>::type;

        // Prepare the task by binding the function to its arguments.
        // Using a packaged task ensures that any exception thrown in the task function
        // will be caught by the future, and re-thrown when its get() function is called,
        // see e.g., https://stackoverflow.com/a/16345305/4184258
        auto task = std::make_shared< std::packaged_task<result_type()> >(
            std::bind( std::forward<F>(f), std::forward<Args>(args)... )
        );

        // Prepare the resulting future result of the task
        auto future_result = ProactiveFuture<result_type>( task->get_future(), *this );

        // Put the task into the queue, synchronized, and in a small scope.
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
     * @brief Helper function to run a pending task from outside the pool.
     *
     * The return value indicates whether a task has been run.
     * If no tasks are enqueued, return `false` without doing anything.
     * This is the function that allows ProactiveFuture to process tasks while waiting.
     */
    bool run_pending_task()
    {
        // Similar to the worker function, but without the condition_variable to wait, as we might
        // not ever have any tasks in the queue, and would be waiting for the condition indefinitely.
        // Instead, we here just want to process a task if there is one, or return otherwise.
        std::function<void()> task;

        // Short lock to get a task if there is one.
        {
            std::unique_lock<std::mutex> lock( task_queue_mutex_ );
            if( !this->task_queue_.empty() ) {
                task = std::move( this->task_queue_.front() );
                this->task_queue_.pop();
            }
        }

        // If we got a task, run it.
        if( task ) {
            task();
            return true;
        }
        return false;
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    void init_( size_t num_threads )
    {
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
};

// =================================================================================================
//      Deferred Definitions
// =================================================================================================

// Implemented here, as it needs ThreadPool to be defined first.
template<class T>
void ProactiveFuture<T>::wait() const
{
    // Let's be thorough. The standard encourages the check for validity.
    throw_if_invalid_();

    // Also, check that we have a valid thread pool.
    assert( thread_pool_ );
    // if( !thread_pool_ ) {
    //     throw std::runtime_error( "Invalid call to ProactiveFuture::wait() without a ThreadPool" );
    // }

    // If we have a deferred future, something is off - this was not created by us.
    // We do not do any busy work while waiting, as otherwise, it won't ever get ready.
    // As this might deadlock the thread pool, and was not done by us, we throw.
    assert( !deferred() );
    // if( deferred() ) {
        // throw std::runtime_error( "Invalid call to ProactiveFuture::wait() with a deferred future" );
        // return future_.wait();
    // }

    // Otherwise, we use the waiting time to process other tasks from the thread pool
    // that created this future in the first place.
    while( !ready() ) {
        assert( thread_pool_ );

        // We attempt to run a pending task. If that returns false, there were no tasks
        // in the pool, so we can yield our thread for now - nothing to do for now, just wait more.
        // We however need to keep waiting here. It could otherwise be that the task we are waiting
        // for submits more tasks later, which might then deadlock the thread pool, if we here
        // went into an actual wait for that first task - which would defy the main purpose of
        // having this proactive waiting future in the first place.
        // We also can't do anything with condition variables to omit the busy wait here, as we do
        // not know whether there will be any tasks in the queue at all before we are done here.
        if( ! thread_pool_->run_pending_task() ) {
            std::this_thread::yield();
        }
    }

    // We call wait just in case here again, to make sure that everything is all right.
    // Probably not necessary, as it's already ready, but won't hurt either.
    // future_.wait();
}

} // namespace utils
} // namespace genesis

#endif // include guard
