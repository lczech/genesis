#ifndef GENESIS_UTILS_THREADING_THREAD_LOCAL_CACHE_H_
#define GENESIS_UTILS_THREADING_THREAD_LOCAL_CACHE_H_

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

#include <cassert>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Thread Local Cache
// =================================================================================================

/**
 * @brief Helper class to offer a thread-local caching mechanism, for instance for the ThreadPool.
 *
 * Sometimes, it makes sense to cache data between otherwise independent tasks. When these tasks
 * are submitted to our ThreadPool however, it is undetermined which thread picks up which task,
 * and in order to avoid synchronization between tasks to access the cache, we might want to use
 * independent thread-local caches instead.
 *
 * This class offers a mechanism for that, such that each thread in the pool has its own
 * thread-local cache, without interference from other threads. The implementation here also
 * avoids false sharing by aligning the cache elements to be on different CPU cache lines.
 *
 * A potential use case could be as follows: We want to aggregate or gather some data, but simply
 * doing the aggregation form each thread to one shared target data structure might easily lead
 * to contention on the data structure lock. Hence, we want each thread to aggregate on their own
 * first, and then only do one last aggregation of all thread-local results in the end. This way,
 * the intermedate aggregations do not need to use any locking.
 *
 * This is also meant to work in cases where the thread pool receives independent work packages
 * which are usually not meant to interact with each other: The caching offered here does not have
 * any notion of work tasks - it simply provides caching per thread of the pool, and leaves it to
 * the user to decide how those interact with each other.
 *
 * Usage: The cache is set up for the given number of total threads, and then each thread can request
 * its cache entry *once* and store it in a `thread_local` reference:
 *
 *     auto thread_cache = ThreadLocalCache<int>( num_threads );
 *     for( auto const& data : my_data ) {
 *         thread_pool->enqueue_detached( [&thread_cache, &data]()
 *         {
 *             thread_local auto& local_cache = thread_cache.get();
 *             // operate on data and store result in local_cache
 *         });
 *     }
 *
 * This way, each worker thread of the thread pool is guaranteed to have its own cache that it can
 * write to without the need for locking.
 *
 * Then, at the end, we typically want to run some function on all caches to make use of the data
 * that each thread has accumuated, such as flushing or accumulating all per-thread data:
 *
 *     thread_pool->wait_for_all_pending_tasks();
 *     for( auto const& cache : thread_cache ) {
 *         // accumuate the data that each thread has stored in its cache
 *     }
 *
 * With this, we have used the thread pool to gather and accumulate data, without the need to lock
 * the data for each access in during the loop over the data.
 *
 * Note: The added benefit of this class over just a simple `thread_local` cache variable that is
 * alive for the thread itself is that this cache stays alive after the tasks are done. As shown
 * above, this allows us to gather the contents of the per-thread caches afterwards, and process
 * them as needed.
 */
template<typename T, size_t CacheLineSize = 64>
class ThreadLocalCache
{
public:

    // -------------------------------------------------------------
    //     Internal Structures
    // -------------------------------------------------------------

    /**
     * @brief Wrapper type for cache-line aligned elements, to avoid false sharing between threads.
     *
     * Typically, we only set up this class for the number of threads that a program uses, which
     * rarely is more than a few dozen or potentially hundred on large machines, but likely never
     * an amount where the extra memory needed for the alignment would matter overall.
     */
    template <typename U>
    struct AlignedElement {
        // Align to CacheLineSize, typically 64 bytes
        alignas(CacheLineSize) U value;

        // Constructors for convenience
        AlignedElement() = default;

        explicit AlignedElement( U const& val )
            : value(val)
        {}

        AlignedElement( U&& val )
            : value( std::move( val ))
        {}

        // Access operators
        operator U&()
        {
            return value;
        }

        operator U const&() const
        {
            return value;
        }
    };

    /**
     * @brief Transparent iterator that returns the value of the AlignedElement.
     *
     * This allows to use the begin() and end() iterator of the main class here to work
     * for the user as if the elements were not wrapped, to keep it simple for users.
     */
    template <typename U>
    class Iterator
    {
    public:
        // The underlying std::vector iterator type
        using BaseIterator = typename std::vector<AlignedElement<U>>::iterator;

        // Constructor that wraps the base iterator
        explicit Iterator(BaseIterator it)
            : base_iterator_(it)
        {}

        // Override dereference operators to access the underlying `value`
        U& operator*()
        {
            return base_iterator_->value;
        }
        U const& operator*() const
        {
            return base_iterator_->value;
        }
        U* operator->()
        {
            return &(base_iterator_->value);
        }
        U const* operator->() const
        {
            return &(base_iterator_->value);
        }

        // Increment and decrement operators
        Iterator& operator++()
        {
            ++base_iterator_;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        Iterator& operator--()
        {
            --base_iterator_;
            return *this;
        }
        Iterator operator--(int)
        {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        // Comparison operators
        bool operator==(const Iterator& other) const
        {
            return base_iterator_ == other.base_iterator_;
        }
        bool operator!=(const Iterator& other) const
        {
            return base_iterator_ != other.base_iterator_;
        }

    private:

        // The underlying std::vector iterator
        BaseIterator base_iterator_;
    };

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    // using iterator = std::vector<AlignedElement<T>>::iterator;
    using iterator = Iterator<T>;
    using const_iterator = Iterator<T const>;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Default constructor. A default constructed instance shall not be used.
     */
    ThreadLocalCache() = default;

    /**
     * @brief Create a cache for a given @p capacity.
     *
     * Typically, the @p capacity is meant to be the number of threads that need a local cache.
     * With our ThreadPool, this usually is one more than the number of threads in the pool,
     * as we normally allocate one fewer thread than we actually want to run. This is because the
     * ThreadPool implements work stealing, so that the main thread also counts towards the total
     * number of threads that can run concurrently.
     */
    ThreadLocalCache( size_t capacity )
    {
        // Allocate cache space for each potential thread that might need it.
        // We need to do this in advance, as otherwise, the thread_local references into the cache
        // could access freed memory if a re-allocation is happening later on, leading to a segfault.
        // We store the capacity explicitly here, as the vector might allocate even more.
        capacity_ = capacity;
        cache_.reserve( capacity );
    }

    ThreadLocalCache( ThreadLocalCache const& ) = delete;
    ThreadLocalCache( ThreadLocalCache&& )      = delete;

    ThreadLocalCache& operator= ( ThreadLocalCache const& ) = delete;
    ThreadLocalCache& operator= ( ThreadLocalCache&& )      = delete;

    ~ThreadLocalCache() = default;

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Get a reference to a new cache element.
     *
     * This function is meant to be called as the initializer for a `thread_local` reference,
     * e.g., from within a worker thread of a thread pool:
     *
     *     thread_local auto& local_cache = cache.get();
     *
     * With this call, a new cache element is created for the thread. The get() function is meant
     * to be called like this, to ensure that it is called exactly once per thread, due to
     * the `thread_local` qualifier, so that the reference is stored as long as the thread exists.
     */
    inline T& get()
    {
        // Make sure that threads do not interfere with each others setup.
        // As this function is only called once per thread to obtain its local cache, the overhead
        // for the mutex is fine. The lock is freed automatically at the end of the scope.
        std::lock_guard<std::mutex> lock( mutex_ );

        // Boundary check: we cannot re-allocate the vector, as this would lead to previous
        // entries being freed, i.e., seg faults.
        // So we are limited to the inital capacity that the instance was set up with.
        if( cache_.size() == capacity_ ) {
            throw std::runtime_error(
                "Cannot access ThreadLocalCache for more threads than it was initialized with."
            );
        }

        // Assert that we are not re-allocating below when we add another element.
        assert( cache_.capacity() >= capacity_ );
        assert( cache_.size() < cache_.capacity() );

        // Create a new cache instance, and return it.
        cache_.emplace_back( T{} );
        return cache_.back().value;
    }

    /**
     * @brief Begin iterator for the elements in the cache.
     *
     * The iterator allows to traverse all local caches for threads that have accessed their cache
     * via the get() method, which is size() many.
     */
    iterator begin()
    {
        return iterator( cache_.begin() );
    }

    const_iterator begin() const
    {
        return const_iterator( cache_.cbegin() );
    }

    /**
     * @brief End iterator for the elements in the cache.
     */
    iterator end()
    {
        return iterator( cache_.end() );
    }

    const_iterator end() const
    {
        return const_iterator( cache_.cend() );
    }

    /**
     * @brief Return the size of the cache.
     *
     * This is the number of distinct threads that have accessed the get() method to obtain their
     * local cache, out of capacity() maximal threads that the cache has capacity for.
     */
    size_t size() const
    {
        return cache_.size();
    }

    /**
     * @brief Return the maximum capacity of the cache.
     *
     * This is typically the number of threads that the cache was initialized with upon construction.
     */
    size_t capacity() const
    {
        return capacity_;
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    size_t capacity_;
    std::vector<AlignedElement<T>> cache_;
    std::mutex mutex_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
