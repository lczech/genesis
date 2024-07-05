#ifndef GENESIS_UTILS_THREADING_THREAD_POOL_H_
#define GENESIS_UTILS_THREADING_THREAD_POOL_H_

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

#include "genesis/utils/threading/blocking_concurrent_queue.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <iterator>
#include <functional>
#include <future>
#include <memory>
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
 * @brief Wrapper around `std::future` that implements (pro-)active waiting, i.e., work stealing.
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
 * as the main thread will also be able to do busy work while waiting for tasks via our work
 * stealing ProactiveFuture. Use guess_number_of_threads() for obtaining the adequate number of
 * total threads to run, and subtract one to get the number to use this class with.
 *
 * Example
 *
 *     // Create a thread pool with 3 worker threads, on a 4 core system.
 *     ThreadPool thread_pool( 3 );
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
 * The pool implements a work stealing technique similar to the one described in the
 * "C++ Concurrency in Action" book by Anthony Williams, second edition, chapter 9, in order to
 * avoid dead locking when tasks submit their own tasks. In such cases, the parent task could then
 * potentially be waiting for the child, but the child might never start, as all threads in the pool
 * are busy waiting for the children they enqueued. Hence, our wrapper implementation, called
 * ProactiveFuture (a thin wrapper around `std::future`; see there for details), instead processes
 * tasks from the queue while waiting for the future, so that those do not starve.
 *
 * This mechanism also allows to start a ThreadPool with 0 threads. In that case, all tasks will
 * be processed once wait() or get() is called on their returned ProactiveFuture - essentially
 * making the pool behave as a lazy evaluator of the tasks. This is very convenient behavior to
 * ensure that the number of actually busy threads is exactly known - a main thread that waits for
 * some submitted tasks will also be doing work while waiting. Hence, we recommend to start this
 * pool with one fewer threads than hardware concurrency (or whatever other upper limit you want
 * to ensure, e.g., Slurm).
 *
 * Lastly, if upon construction a maximum queue size is provided, only that many tasks will be
 * queued at a time (with a bit of leeway, due to concurrency). If a thread calls enqueue() when
 * the queue is already filled with waiting tasks up to the maximum size, the caller instead waits
 * for the queue to be below the specified max, and while waiting, starts processing tasks of its
 * own, so that the waiting time is spend productively.
 *
 * This is meant as a mechanism to allow a main thread to just keep queueing work without capturing
 * the futures and waiting for them, while avoiding to endlessly queue new tasks, with the workers
 * not being able to catch up. The max size needs to be at least double the number of threads for
 * this to make sense. Due to concurrency, the max size can be exceeded by the number of threads,
 * in case that many threads enqueue work simultaneously. That is okay, as we usually just want
 * there to be _some_ upper limit on the number of tasks. Also, in case of just a single main thread
 * that is enqueueing new tasks, the maximum is never exceeded.
 *
 * When using this mechanism of submitting work without storing the futures, the wait() function
 * of the class can be used to wait for all current work to be done. This is intended to be called,
 * for instance, from the main thread, once there is no more work to be enqueued.
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
     * lazy evaluating task queue
     *
     * If a @p max_queue_size is set to a value other than zero, roughly that many tasks will only
     * be enqueued at the same time. See the class description for details.
     */
    explicit ThreadPool( size_t num_threads, size_t max_queue_size = 0 )
        : max_queue_size_( max_queue_size )
    {
        // We disallow a max queue size smaller than half the number of threads.
        // That would be slow and inefficient, and just not really what we want.
        if( max_queue_size_ > 0 && max_queue_size_ < num_threads * 2 ) {
            throw std::runtime_error(
                "Cannot use ThreadPool with max queue size less than "
                "half the number of threads, for efficiency"
            );
        }

        // Create the threads.
        init_( num_threads );
        assert( worker_pool_.size() == num_threads );
    }

    ThreadPool( ThreadPool const& ) = delete;
    ThreadPool( ThreadPool&& )      = delete;

    ThreadPool& operator= ( ThreadPool const& ) = delete;
    ThreadPool& operator= ( ThreadPool&& )      = delete;

    /**
     * @brief Destruct the thread pool, waiting for all unfinished tasks.
     */
    ~ThreadPool()
    {
        // Just in case, we wait for any unfinished work to be done, to avoid terminating
        // when tasks are still doing work that needs to be finished.
        wait_for_all_pending_tasks();
        assert( unfinished_tasks_.load() == 0 );

        // Send the special stop task to the pool, once for each worker.
        // As each worker stops upon receiving the task, this stops all workers.
        for( size_t i = 0; i < worker_pool_.size(); ++i ) {
            task_queue_.enqueue( WrappedTask( true ));
        }

        // Join them back into the main thread, after which there is no unfinished work.
        for( std::thread& worker : worker_pool_ ) {
            if( worker.joinable() ) {
                worker.join();
            }
        }
        assert( unfinished_tasks_.load() == 0 );
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
     * @brief Return the current number of pending tasks.
     *
     * These is the number of tasks that have been enqueued, but not yet finished running.
     * It hence includes both the number of waiting tasks and those that are currently being
     * processed by a worker thread. The count is only reduced once a task is finished
     * (or threw an exception). The counter can be used to wait for all enqueued tasks to be done,
     * which is what wait_for_all_pending_tasks() does.
     *
     * Note that there is a very small window where it can happen that this counter is reduced
     * after finishing the work of a task, but before setting the value of its associated promise.
     * Hence, this counter might exclude a finished task for which the caller is still waiting
     * for the future returned from the enqueue_and_retrieve() call. That should usually not be
     * an issue though, as the caller will typically just wait for the future anyway, instead
     * of checking this counter.
     */
    size_t pending_tasks_count() const
    {
        return unfinished_tasks_.load();
        // return task_queue_.size_approx();
    }

    // -------------------------------------------------------------
    //     Pool Functionality
    // -------------------------------------------------------------

    /**
     * @brief Enqueue a new task, using a function to call and its arguments, and returning
     * a future to receive the result of the task.
     *
     * The enqueue function returns a future that can be used to check whether the task has
     * finished, or to wait for it to be finished. This also allows the task to send its result
     * back to the caller, if needed, by simply returning it from the task function.
     *
     * We internally use a std::packaged_task, so that any exception thrown in the function will
     * be caught and trapped inside of the future, until its get() function is called.
     * See enqueue_detached() for an alternative function that does not incur the overhead of
     * creating the packaged_task and future, and hence has 50% less overhead.
     *
     * If enqueuing the task would exceed the max queue size, we instead first process existing
     * tasks until there is e space in the queue. This makes the caller do wait and work.
     */
    template<class F, class... Args>
    auto enqueue_and_retrieve( F&& f, Args&&... args )
    -> ProactiveFuture<typename std::result_of<F(Args...)>::type>
    {
        using result_type = typename std::result_of<F(Args...)>::type;

        // Make sure that we do not enqueue more tasks than the max size.
        run_tasks_until_below_max_queue_size_();

        // Prepare a promise and associated future of the task; the latter is our return value.
        // Using a promise ensures that any exception thrown in the task function
        // will be caught by the future, and re-thrown when its get() function is called,
        // see e.g., https://stackoverflow.com/a/16345305/4184258
        auto task_promise = std::make_shared<std::promise<result_type>>();
        auto future_result = ProactiveFuture<result_type>( task_promise->get_future(), *this );

        // To make our lives easier for the helper functions used below, we just wrap
        // the task in a function that can be called without arguments.
        std::function<result_type()> task_function = std::bind(
            std::forward<F>(f), std::forward<Args>(args)...
        );

        // Prepare the task that we want to submit.
        // All this wrapping should be completely transparent to the compiler, and removed.
        // The task captures the package including the promise that is needed for the future.
        WrappedTask wrapped_task;
        wrapped_task.function = make_wrapped_task_with_promise_( task_promise, task_function );

        // We first incrementi the unfinished counter, and only decrementing it once the task has
        // been fully processed. Thus, the counter always tells us if there is still work going on.
        ++unfinished_tasks_;
        task_queue_.enqueue( std::move( wrapped_task ));

        // The task is submitted. Return its future for the caller to be able to wait for it.
        return future_result;
    }

    /**
     * @brief Enqueue a new task, using a function to call and its arguments, without a std::future.
     *
     * This function simply submits the task to the pool, but does not create a std::future for the
     * caller to wait for the result. Hence, this mostly makes sense for tasks that do not return a
     * result that is needed. Thus, the task function itself needs to take care for propagating its
     * result, if needed. This has 50% less overhead compared to enqueue_and_retrieve().
     *
     * If enqueuing the task would exceed the max queue size, we instead first process existing
     * tasks until there is enough space in the queue. This makes the caller do wait and work.
     */
    template<class F, class... Args>
    void enqueue_detached( F&& f, Args&&... args )
    {
        // Make sure that we do not enqueue more tasks than the max size.
        run_tasks_until_below_max_queue_size_();

        // Prepare the task that we want to submit, by wrapping the function to be called.
        // All this wrapping should be completely transparent to the compiler, and removed.
        // The task captures the package including the promise that is needed for the future.
        WrappedTask wrapped_task;
        auto task_function = std::bind( std::forward<F>(f), std::forward<Args>(args)... );
        wrapped_task.function = [task_function, this]()
        {
            // Run the actual work task here. Once done, we can signal this to the unfinished list.
            task_function();
            assert( this->unfinished_tasks_.load() > 0 );
            --this->unfinished_tasks_;
        };

        // We add the task, incrementing the unfinished counter, and only decrementing it once the
        // task has been fully processed. That way, the counter always tells us if there is still
        // work going on. We capture a reference to `this` in the task above, which could be
        // dangerous if the threads survive the lifetime of the pool, but given that their exit
        // condition is only called from the pool destructor, this should never be able to happen.
        ++unfinished_tasks_;
        task_queue_.enqueue( std::move( wrapped_task ));
    }

    /**
     * @brief Helper function to run a pending task from outside the pool.
     *
     * The return value indicates whether a task has been run.
     * If no tasks are enqueued, return `false` without doing anything.
     * This is the function that allows ProactiveFuture to process tasks while waiting.
     */
    bool try_run_pending_task()
    {
        // Similar to the worker function, but without the blocking wait, as we might not ever
        // have any tasks in the queue, and would be waiting for the condition indefinitely.
        // Instead, we here just want to process a task if there is one, or return otherwise.
        WrappedTask task;
        if( task_queue_.try_dequeue( task )) {
            task.function();
            return true;
        }
        return false;
    }

    /**
     * @brief Wait for all current tasks to be finished processing.
     *
     * This function simply calls try_run_pending_task() until there are no more tasks to process.
     * This is an alternative mechanism for tasks whose future has not been captured when being
     * enqueued. This can be used for instance by a main thread that keeps submitting work,
     * and then later needs to wait for everything to be finished. For this use case, it might
     * make sense to set a max_queue_size when constructing the pool, to ensure that the pool does
     * not grow indefinitely. See the main class description for details.
     */
    void wait_for_all_pending_tasks()
    {
        // Wait for all pending tasks to be processed. While we wait, we can also help
        // processing tasks! The loop stops once there are not more unfinished tasks.
        while( unfinished_tasks_.load() > 0 ) {
            while( try_run_pending_task() );
            std::this_thread::yield();
        }
        assert( unfinished_tasks_.load() == 0 );
    }

    // -------------------------------------------------------------
    //     Wrapped Task
    // -------------------------------------------------------------

private:

    /**
     * @brief Wrap a task, with a special case for stopping a worker.
     *
     * The workers are using a blocking concurrent queue without any condition variables,
     * for speed. That means we cannot signal them from the outside, but instead use this
     * trick to tell them when to stop: A worker that receives a WrappedTask with stop=true
     * will finish its loop waiting for more work. This is inspired by the usage of the queue
     * in https://github.com/cameron314/concurrentqueue/issues/176#issuecomment-569801801
     */
    struct WrappedTask
    {
        explicit WrappedTask( bool stop = false )
            : stop(stop)
        {}

        ~WrappedTask() = default;

        WrappedTask( WrappedTask const& ) = delete;
        WrappedTask( WrappedTask&& )      = default;

        WrappedTask& operator= ( WrappedTask const& ) = delete;
        WrappedTask& operator= ( WrappedTask&& )      = default;

        std::function<void()> function;
        bool stop;
    };

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

    void init_( size_t num_threads )
    {
        // Create the desired number of workers.
        worker_pool_.reserve( num_threads );
        for( size_t i = 0; i < num_threads; ++i ) {
            worker_pool_.emplace_back(
                &worker_, this
            );
        }
    }

    static void worker_( ThreadPool* pool )
    {
        // Using a token for the consumer speeds it up. This is created once per worker thread
        // when the function is called from the thread constructor upon emplacing the worker
        // in the pool in init_()
        ConsumerToken consumer_token( pool->task_queue_ );

        // The worker runs an infinite loop of waiting for tasks,
        // only stopping once a special "stop" task is received.
        WrappedTask task;
        while( true ) {
            pool->task_queue_.wait_dequeue( consumer_token, task );
            if( task.stop ) {
                break;
            }
            task.function();
        }
    }

    inline void run_tasks_until_below_max_queue_size_()
    {
        // Check that we can enqueue a task at the moment, of if we need to wait and do work first.
        // In a high-contention situation, this of course could fail, so that once the loop condition
        // is checked, some other task already has finished the work. But that doesn't matter, the
        // call to try_run_pending_task will catch that and just do nothing. Also, the other way round
        // could happen, and the queue could in theory be overloaded if many threads try to enqueue
        // at exactly the same time. But we probably never have enough threads for that to be a real
        // issue - worst case, we exceed the max queue size by the number of threads, which is fine.
        // All we want to avoid is to have an infinitely growing queue.
        while( max_queue_size_ > 0 && pending_tasks_count() >= max_queue_size_ ) {
            try_run_pending_task();
        }
    }

    template<typename T>
    inline std::function<void()> make_wrapped_task_with_promise_(
        std::shared_ptr<std::promise<T>> task_promise,
        std::function<T()> task_function
    ) {
        // We capture a reference to `this` in the below lambda, which could be dangerous
        // if the threads survive the lifetime of the pool, but given that the pool destructor
        // waits for all of them to finish, this should never be able to happen.
        return [this, task_promise, task_function]()
        {
            // Run the work task, and set the value of the associated promise.
            // We need to delegate this here, as the std::promise::set_value() function
            // differs for void and non-void return types. That is unfortunate.
            // Also, as either the task function or setting the value of the promise can throw
            // an exception, but in between we need to decrement the unfiished tasks counter,
            // we need a way to figure out if we already did the decrement in case of an error.
            bool decremented_unfinished_tasks = false;
            try {
                run_task_and_fulfill_promise_<T>(
                    task_promise, task_function, decremented_unfinished_tasks
                );
            } catch (...) {
                if( !decremented_unfinished_tasks ) {
                    --unfinished_tasks_;
                    decremented_unfinished_tasks = true;
                }
                task_promise->set_exception( std::current_exception() );
            }
            assert( decremented_unfinished_tasks );
        };
    }

    template<typename T>
    typename std::enable_if<!std::is_void<T>::value>::type
    inline run_task_and_fulfill_promise_(
        std::shared_ptr<std::promise<T>> task_promise,
        std::function<T()> task_function,
        bool& decremented_unfinished_tasks
    ) {
        // Run the actual work task here. Once done, we can signal this to the unfinished list.
        // This bit is the only reason why the whole wrapping exists: We need to first decrement
        // the unfinished tasks count, before setting the promise value, as otherwise, outside
        // threads might deduce that there are more pending tasks, when in fact we are already done.
        auto result = task_function();
        assert( unfinished_tasks_.load() > 0 );
        --unfinished_tasks_;
        decremented_unfinished_tasks = true;
        task_promise->set_value( std::move( result ));
    }

    template<typename T>
    typename std::enable_if<std::is_void<T>::value>::type
    inline run_task_and_fulfill_promise_(
        std::shared_ptr<std::promise<T>> task_promise,
        std::function<void()> task_function,
        bool& decremented_unfinished_tasks
    ) {
        // Same as above, but for void functions, i.e., without setting a value for the promise.
        task_function();
        assert( unfinished_tasks_.load() > 0 );
        --unfinished_tasks_;
        decremented_unfinished_tasks = true;
        task_promise->set_value();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    // Worker threads
    std::vector<std::thread> worker_pool_;

    // WrappedTask queue and its counters
    BlockingConcurrentQueue<WrappedTask> task_queue_;
    std::atomic<size_t> unfinished_tasks_{ 0 };
    size_t max_queue_size_;
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
        if( ! thread_pool_->try_run_pending_task() ) {
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
