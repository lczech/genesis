#ifndef GENESIS_UTILS_THREADING_BLOCKING_CONCURRENT_QUEUE_H_
#define GENESIS_UTILS_THREADING_BLOCKING_CONCURRENT_QUEUE_H_

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

/*
    This code below is adapted from the excellent moodycamel::ConcurrentQueue
    (https://github.com/cameron314/concurrentqueue), using version v1.0.4,
    which was published under a simplified BSD license, and also dual-licensed
    under the Boost Software License. The full original license
    (https://github.com/cameron314/concurrentqueue/blob/master/LICENSE.md), is copied
    in our documentation as well, see `genesis/doc/manual/supplement/acknowledgements.md`, and
    @link supplement_acknowledgements_code_reuse_concurrent_queue Acknowledgements@endlink.

    See also ConcurrentQueue and LightweightSemaphore for the other two classes
    that we adapted from the original repository.

    We adapted the original code by (roughly) formatting it to our formatting standard, as well as
    renaming the namespace from moodycamel to be contained within our namespace, to keep our
    documentation and usage consistent. Other than that, all functionality is kept as-is.
    It feels weird to rename the namespaces, as it might seem that we are trying to hide the fact
    that this code is not ours. That is not the case, and is merely based on our compulsion
    for orderly namespaces and documentation within genesis. Please note that the original code
    is excellet work that we would like to acknowledge here! Also, if anyone wants to do the same
    with genesis code, please feel free to do so ;-)
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/threading/concurrent_queue.hpp"
#include "genesis/utils/threading/lightweight_semaphore.hpp"

// =================================================================================================
//     Blocking Concurrent Queue
// =================================================================================================

// Provides an efficient blocking version of moodycamel::ConcurrentQueue.
// ©2015-2020 Cameron Desrochers. Distributed under the terms of the simplified
// BSD license, available at the top of concurrentqueue.h.
// Also dual-licensed under the Boost Software License (see LICENSE.md)
// Uses Jeff Preshing's semaphore implementation (under the terms of its
// separate zlib license, see lightweightsemaphore.h).

#include <cerrno>
#include <chrono>
#include <ctime>
#include <memory>
#include <type_traits>

namespace genesis {
namespace utils {
// This is a blocking version of the queue. It has an almost identical interface to
// the normal non-blocking version, with the addition of various wait_dequeue() methods
// and the removal of producer-specific dequeue methods.
template <typename T, typename Traits = ConcurrentQueueDefaultTraits>
class BlockingConcurrentQueue {
private:
    typedef ::genesis::utils::ConcurrentQueue<T, Traits> ConcurrentQueue;

public:
    typedef typename ConcurrentQueue::producer_token_t producer_token_t;
    typedef typename ConcurrentQueue::consumer_token_t consumer_token_t;

    typedef typename ConcurrentQueue::index_t index_t;
    typedef typename ConcurrentQueue::size_t size_t;
    typedef typename std::make_signed<size_t>::type ssize_t;

    static const size_t BLOCK_SIZE = ConcurrentQueue::BLOCK_SIZE;
    static const size_t EXPLICIT_BLOCK_EMPTY_COUNTER_THRESHOLD = ConcurrentQueue::EXPLICIT_BLOCK_EMPTY_COUNTER_THRESHOLD;
    static const size_t EXPLICIT_INITIAL_INDEX_SIZE = ConcurrentQueue::EXPLICIT_INITIAL_INDEX_SIZE;
    static const size_t IMPLICIT_INITIAL_INDEX_SIZE = ConcurrentQueue::IMPLICIT_INITIAL_INDEX_SIZE;
    static const size_t INITIAL_IMPLICIT_PRODUCER_HASH_SIZE = ConcurrentQueue::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE;
    static const std::uint32_t EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE = ConcurrentQueue::EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE;
    static const size_t MAX_SUBQUEUE_SIZE = ConcurrentQueue::MAX_SUBQUEUE_SIZE;

public:
    // Creates a queue with at least `capacity` element slots; note that the
    // actual number of elements that can be inserted without additional memory
    // allocation depends on the number of producers and the block size (e.g. if
    // the block size is equal to `capacity`, only a single block will be allocated
    // up-front, which means only a single producer will be able to enqueue elements
    // without an extra allocation -- blocks aren't shared between producers).
    // This method is not thread safe -- it is up to the user to ensure that the
    // queue is fully constructed before it starts being used by other threads (this
    // includes making the memory effects of construction visible, possibly with a
    // memory barrier).
    explicit BlockingConcurrentQueue(size_t capacity = 6 * BLOCK_SIZE)
        : inner(capacity)
        , sema(create<LightweightSemaphore, ssize_t, int>(0, (int)Traits::MAX_SEMA_SPINS), &BlockingConcurrentQueue::template destroy<LightweightSemaphore>)
    {
        assert(reinterpret_cast<ConcurrentQueue*>((BlockingConcurrentQueue*)1) == &((BlockingConcurrentQueue*)1)->inner && "BlockingConcurrentQueue must have ConcurrentQueue as its first member");
        if (!sema) {
            MOODYCAMEL_THROW(std::bad_alloc());
        }
    }

    BlockingConcurrentQueue(size_t minCapacity, size_t maxExplicitProducers, size_t maxImplicitProducers)
        : inner(minCapacity, maxExplicitProducers, maxImplicitProducers)
        , sema(create<LightweightSemaphore, ssize_t, int>(0, (int)Traits::MAX_SEMA_SPINS), &BlockingConcurrentQueue::template destroy<LightweightSemaphore>)
    {
        assert(reinterpret_cast<ConcurrentQueue*>((BlockingConcurrentQueue*)1) == &((BlockingConcurrentQueue*)1)->inner && "BlockingConcurrentQueue must have ConcurrentQueue as its first member");
        if (!sema) {
            MOODYCAMEL_THROW(std::bad_alloc());
        }
    }

    // Disable copying and copy assignment
    BlockingConcurrentQueue(BlockingConcurrentQueue const&) MOODYCAMEL_DELETE_FUNCTION;
    BlockingConcurrentQueue& operator=(BlockingConcurrentQueue const&) MOODYCAMEL_DELETE_FUNCTION;

    // Moving is supported, but note that it is *not* a thread-safe operation.
    // Nobody can use the queue while it's being moved, and the memory effects
    // of that move must be propagated to other threads before they can use it.
    // Note: When a queue is moved, its tokens are still valid but can only be
    // used with the destination queue (i.e. semantically they are moved along
    // with the queue itself).
    BlockingConcurrentQueue(BlockingConcurrentQueue&& other) MOODYCAMEL_NOEXCEPT
        : inner(std::move(other.inner)),
          sema(std::move(other.sema))
    {
    }

    inline BlockingConcurrentQueue& operator=(BlockingConcurrentQueue&& other) MOODYCAMEL_NOEXCEPT
    {
        return swap_internal(other);
    }

    // Swaps this queue's state with the other's. Not thread-safe.
    // Swapping two queues does not invalidate their tokens, however
    // the tokens that were created for one queue must be used with
    // only the swapped queue (i.e. the tokens are tied to the
    // queue's movable state, not the object itself).
    inline void swap(BlockingConcurrentQueue& other) MOODYCAMEL_NOEXCEPT
    {
        swap_internal(other);
    }

private:
    BlockingConcurrentQueue& swap_internal(BlockingConcurrentQueue& other)
    {
        if (this == &other) {
            return *this;
        }

        inner.swap(other.inner);
        sema.swap(other.sema);
        return *this;
    }

public:
    // Enqueues a single item (by copying it).
    // Allocates memory if required. Only fails if memory allocation fails (or implicit
    // production is disabled because Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE is 0,
    // or Traits::MAX_SUBQUEUE_SIZE has been defined and would be surpassed).
    // Thread-safe.
    inline bool enqueue(T const& item)
    {
        if ((details::likely)(inner.enqueue(item))) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues a single item (by moving it, if possible).
    // Allocates memory if required. Only fails if memory allocation fails (or implicit
    // production is disabled because Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE is 0,
    // or Traits::MAX_SUBQUEUE_SIZE has been defined and would be surpassed).
    // Thread-safe.
    inline bool enqueue(T&& item)
    {
        if ((details::likely)(inner.enqueue(std::move(item)))) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues a single item (by copying it) using an explicit producer token.
    // Allocates memory if required. Only fails if memory allocation fails (or
    // Traits::MAX_SUBQUEUE_SIZE has been defined and would be surpassed).
    // Thread-safe.
    inline bool enqueue(producer_token_t const& token, T const& item)
    {
        if ((details::likely)(inner.enqueue(token, item))) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues a single item (by moving it, if possible) using an explicit producer token.
    // Allocates memory if required. Only fails if memory allocation fails (or
    // Traits::MAX_SUBQUEUE_SIZE has been defined and would be surpassed).
    // Thread-safe.
    inline bool enqueue(producer_token_t const& token, T&& item)
    {
        if ((details::likely)(inner.enqueue(token, std::move(item)))) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues several items.
    // Allocates memory if required. Only fails if memory allocation fails (or
    // implicit production is disabled because Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE
    // is 0, or Traits::MAX_SUBQUEUE_SIZE has been defined and would be surpassed).
    // Note: Use std::make_move_iterator if the elements should be moved instead of copied.
    // Thread-safe.
    template <typename It>
    inline bool enqueue_bulk(It itemFirst, size_t count)
    {
        if ((details::likely)(inner.enqueue_bulk(std::forward<It>(itemFirst), count))) {
            sema->signal((LightweightSemaphore::ssize_t)(ssize_t)count);
            return true;
        }
        return false;
    }

    // Enqueues several items using an explicit producer token.
    // Allocates memory if required. Only fails if memory allocation fails
    // (or Traits::MAX_SUBQUEUE_SIZE has been defined and would be surpassed).
    // Note: Use std::make_move_iterator if the elements should be moved
    // instead of copied.
    // Thread-safe.
    template <typename It>
    inline bool enqueue_bulk(producer_token_t const& token, It itemFirst, size_t count)
    {
        if ((details::likely)(inner.enqueue_bulk(token, std::forward<It>(itemFirst), count))) {
            sema->signal((LightweightSemaphore::ssize_t)(ssize_t)count);
            return true;
        }
        return false;
    }

    // Enqueues a single item (by copying it).
    // Does not allocate memory. Fails if not enough room to enqueue (or implicit
    // production is disabled because Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE
    // is 0).
    // Thread-safe.
    inline bool try_enqueue(T const& item)
    {
        if (inner.try_enqueue(item)) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues a single item (by moving it, if possible).
    // Does not allocate memory (except for one-time implicit producer).
    // Fails if not enough room to enqueue (or implicit production is
    // disabled because Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE is 0).
    // Thread-safe.
    inline bool try_enqueue(T&& item)
    {
        if (inner.try_enqueue(std::move(item))) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues a single item (by copying it) using an explicit producer token.
    // Does not allocate memory. Fails if not enough room to enqueue.
    // Thread-safe.
    inline bool try_enqueue(producer_token_t const& token, T const& item)
    {
        if (inner.try_enqueue(token, item)) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues a single item (by moving it, if possible) using an explicit producer token.
    // Does not allocate memory. Fails if not enough room to enqueue.
    // Thread-safe.
    inline bool try_enqueue(producer_token_t const& token, T&& item)
    {
        if (inner.try_enqueue(token, std::move(item))) {
            sema->signal();
            return true;
        }
        return false;
    }

    // Enqueues several items.
    // Does not allocate memory (except for one-time implicit producer).
    // Fails if not enough room to enqueue (or implicit production is
    // disabled because Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE is 0).
    // Note: Use std::make_move_iterator if the elements should be moved
    // instead of copied.
    // Thread-safe.
    template <typename It>
    inline bool try_enqueue_bulk(It itemFirst, size_t count)
    {
        if (inner.try_enqueue_bulk(std::forward<It>(itemFirst), count)) {
            sema->signal((LightweightSemaphore::ssize_t)(ssize_t)count);
            return true;
        }
        return false;
    }

    // Enqueues several items using an explicit producer token.
    // Does not allocate memory. Fails if not enough room to enqueue.
    // Note: Use std::make_move_iterator if the elements should be moved
    // instead of copied.
    // Thread-safe.
    template <typename It>
    inline bool try_enqueue_bulk(producer_token_t const& token, It itemFirst, size_t count)
    {
        if (inner.try_enqueue_bulk(token, std::forward<It>(itemFirst), count)) {
            sema->signal((LightweightSemaphore::ssize_t)(ssize_t)count);
            return true;
        }
        return false;
    }

    // Attempts to dequeue from the queue.
    // Returns false if all producer streams appeared empty at the time they
    // were checked (so, the queue is likely but not guaranteed to be empty).
    // Never allocates. Thread-safe.
    template <typename U>
    inline bool try_dequeue(U& item)
    {
        if (sema->tryWait()) {
            while (!inner.try_dequeue(item)) {
                continue;
            }
            return true;
        }
        return false;
    }

    // Attempts to dequeue from the queue using an explicit consumer token.
    // Returns false if all producer streams appeared empty at the time they
    // were checked (so, the queue is likely but not guaranteed to be empty).
    // Never allocates. Thread-safe.
    template <typename U>
    inline bool try_dequeue(consumer_token_t& token, U& item)
    {
        if (sema->tryWait()) {
            while (!inner.try_dequeue(token, item)) {
                continue;
            }
            return true;
        }
        return false;
    }

    // Attempts to dequeue several elements from the queue.
    // Returns the number of items actually dequeued.
    // Returns 0 if all producer streams appeared empty at the time they
    // were checked (so, the queue is likely but not guaranteed to be empty).
    // Never allocates. Thread-safe.
    template <typename It>
    inline size_t try_dequeue_bulk(It itemFirst, size_t max)
    {
        size_t count = 0;
        max = (size_t)sema->tryWaitMany((LightweightSemaphore::ssize_t)(ssize_t)max);
        while (count != max) {
            count += inner.template try_dequeue_bulk<It&>(itemFirst, max - count);
        }
        return count;
    }

    // Attempts to dequeue several elements from the queue using an explicit consumer token.
    // Returns the number of items actually dequeued.
    // Returns 0 if all producer streams appeared empty at the time they
    // were checked (so, the queue is likely but not guaranteed to be empty).
    // Never allocates. Thread-safe.
    template <typename It>
    inline size_t try_dequeue_bulk(consumer_token_t& token, It itemFirst, size_t max)
    {
        size_t count = 0;
        max = (size_t)sema->tryWaitMany((LightweightSemaphore::ssize_t)(ssize_t)max);
        while (count != max) {
            count += inner.template try_dequeue_bulk<It&>(token, itemFirst, max - count);
        }
        return count;
    }

    // Blocks the current thread until there's something to dequeue, then
    // dequeues it.
    // Never allocates. Thread-safe.
    template <typename U>
    inline void wait_dequeue(U& item)
    {
        while (!sema->wait()) {
            continue;
        }
        while (!inner.try_dequeue(item)) {
            continue;
        }
    }

    // Blocks the current thread until either there's something to dequeue
    // or the timeout (specified in microseconds) expires. Returns false
    // without setting `item` if the timeout expires, otherwise assigns
    // to `item` and returns true.
    // Using a negative timeout indicates an indefinite timeout,
    // and is thus functionally equivalent to calling wait_dequeue.
    // Never allocates. Thread-safe.
    template <typename U>
    inline bool wait_dequeue_timed(U& item, std::int64_t timeout_usecs)
    {
        if (!sema->wait(timeout_usecs)) {
            return false;
        }
        while (!inner.try_dequeue(item)) {
            continue;
        }
        return true;
    }

    // Blocks the current thread until either there's something to dequeue
    // or the timeout expires. Returns false without setting `item` if the
    // timeout expires, otherwise assigns to `item` and returns true.
    // Never allocates. Thread-safe.
    template <typename U, typename Rep, typename Period>
    inline bool wait_dequeue_timed(U& item, std::chrono::duration<Rep, Period> const& timeout)
    {
        return wait_dequeue_timed(item, std::chrono::duration_cast<std::chrono::microseconds>(timeout).count());
    }

    // Blocks the current thread until there's something to dequeue, then
    // dequeues it using an explicit consumer token.
    // Never allocates. Thread-safe.
    template <typename U>
    inline void wait_dequeue(consumer_token_t& token, U& item)
    {
        while (!sema->wait()) {
            continue;
        }
        while (!inner.try_dequeue(token, item)) {
            continue;
        }
    }

    // Blocks the current thread until either there's something to dequeue
    // or the timeout (specified in microseconds) expires. Returns false
    // without setting `item` if the timeout expires, otherwise assigns
    // to `item` and returns true.
    // Using a negative timeout indicates an indefinite timeout,
    // and is thus functionally equivalent to calling wait_dequeue.
    // Never allocates. Thread-safe.
    template <typename U>
    inline bool wait_dequeue_timed(consumer_token_t& token, U& item, std::int64_t timeout_usecs)
    {
        if (!sema->wait(timeout_usecs)) {
            return false;
        }
        while (!inner.try_dequeue(token, item)) {
            continue;
        }
        return true;
    }

    // Blocks the current thread until either there's something to dequeue
    // or the timeout expires. Returns false without setting `item` if the
    // timeout expires, otherwise assigns to `item` and returns true.
    // Never allocates. Thread-safe.
    template <typename U, typename Rep, typename Period>
    inline bool wait_dequeue_timed(consumer_token_t& token, U& item, std::chrono::duration<Rep, Period> const& timeout)
    {
        return wait_dequeue_timed(token, item, std::chrono::duration_cast<std::chrono::microseconds>(timeout).count());
    }

    // Attempts to dequeue several elements from the queue.
    // Returns the number of items actually dequeued, which will
    // always be at least one (this method blocks until the queue
    // is non-empty) and at most max.
    // Never allocates. Thread-safe.
    template <typename It>
    inline size_t wait_dequeue_bulk(It itemFirst, size_t max)
    {
        size_t count = 0;
        max = (size_t)sema->waitMany((LightweightSemaphore::ssize_t)(ssize_t)max);
        while (count != max) {
            count += inner.template try_dequeue_bulk<It&>(itemFirst, max - count);
        }
        return count;
    }

    // Attempts to dequeue several elements from the queue.
    // Returns the number of items actually dequeued, which can
    // be 0 if the timeout expires while waiting for elements,
    // and at most max.
    // Using a negative timeout indicates an indefinite timeout,
    // and is thus functionally equivalent to calling wait_dequeue_bulk.
    // Never allocates. Thread-safe.
    template <typename It>
    inline size_t wait_dequeue_bulk_timed(It itemFirst, size_t max, std::int64_t timeout_usecs)
    {
        size_t count = 0;
        max = (size_t)sema->waitMany((LightweightSemaphore::ssize_t)(ssize_t)max, timeout_usecs);
        while (count != max) {
            count += inner.template try_dequeue_bulk<It&>(itemFirst, max - count);
        }
        return count;
    }

    // Attempts to dequeue several elements from the queue.
    // Returns the number of items actually dequeued, which can
    // be 0 if the timeout expires while waiting for elements,
    // and at most max.
    // Never allocates. Thread-safe.
    template <typename It, typename Rep, typename Period>
    inline size_t wait_dequeue_bulk_timed(It itemFirst, size_t max, std::chrono::duration<Rep, Period> const& timeout)
    {
        return wait_dequeue_bulk_timed<It&>(itemFirst, max, std::chrono::duration_cast<std::chrono::microseconds>(timeout).count());
    }

    // Attempts to dequeue several elements from the queue using an explicit consumer token.
    // Returns the number of items actually dequeued, which will
    // always be at least one (this method blocks until the queue
    // is non-empty) and at most max.
    // Never allocates. Thread-safe.
    template <typename It>
    inline size_t wait_dequeue_bulk(consumer_token_t& token, It itemFirst, size_t max)
    {
        size_t count = 0;
        max = (size_t)sema->waitMany((LightweightSemaphore::ssize_t)(ssize_t)max);
        while (count != max) {
            count += inner.template try_dequeue_bulk<It&>(token, itemFirst, max - count);
        }
        return count;
    }

    // Attempts to dequeue several elements from the queue using an explicit consumer token.
    // Returns the number of items actually dequeued, which can
    // be 0 if the timeout expires while waiting for elements,
    // and at most max.
    // Using a negative timeout indicates an indefinite timeout,
    // and is thus functionally equivalent to calling wait_dequeue_bulk.
    // Never allocates. Thread-safe.
    template <typename It>
    inline size_t wait_dequeue_bulk_timed(consumer_token_t& token, It itemFirst, size_t max, std::int64_t timeout_usecs)
    {
        size_t count = 0;
        max = (size_t)sema->waitMany((LightweightSemaphore::ssize_t)(ssize_t)max, timeout_usecs);
        while (count != max) {
            count += inner.template try_dequeue_bulk<It&>(token, itemFirst, max - count);
        }
        return count;
    }

    // Attempts to dequeue several elements from the queue using an explicit consumer token.
    // Returns the number of items actually dequeued, which can
    // be 0 if the timeout expires while waiting for elements,
    // and at most max.
    // Never allocates. Thread-safe.
    template <typename It, typename Rep, typename Period>
    inline size_t wait_dequeue_bulk_timed(consumer_token_t& token, It itemFirst, size_t max, std::chrono::duration<Rep, Period> const& timeout)
    {
        return wait_dequeue_bulk_timed<It&>(token, itemFirst, max, std::chrono::duration_cast<std::chrono::microseconds>(timeout).count());
    }

    // Returns an estimate of the total number of elements currently in the queue. This
    // estimate is only accurate if the queue has completely stabilized before it is called
    // (i.e. all enqueue and dequeue operations have completed and their memory effects are
    // visible on the calling thread, and no further operations start while this method is
    // being called).
    // Thread-safe.
    inline size_t size_approx() const
    {
        return (size_t)sema->availableApprox();
    }

    // Returns true if the underlying atomic variables used by
    // the queue are lock-free (they should be on most platforms).
    // Thread-safe.
    static constexpr bool is_lock_free()
    {
        return ConcurrentQueue::is_lock_free();
    }

private:
    template <typename U, typename A1, typename A2>
    static inline U* create(A1&& a1, A2&& a2)
    {
        void* p = (Traits::malloc)(sizeof(U));
        return p != nullptr ? new (p) U(std::forward<A1>(a1), std::forward<A2>(a2)) : nullptr;
    }

    template <typename U>
    static inline void destroy(U* p)
    {
        if (p != nullptr) {
            p->~U();
        }
        (Traits::free)(p);
    }

private:
    ConcurrentQueue inner;
    std::unique_ptr<LightweightSemaphore, void (*)(LightweightSemaphore*)> sema;
};

template <typename T, typename Traits>
inline void swap(BlockingConcurrentQueue<T, Traits>& a, BlockingConcurrentQueue<T, Traits>& b) MOODYCAMEL_NOEXCEPT
{
    a.swap(b);
}

} // namespace utils
} // namespace genesis

#endif // include guard
