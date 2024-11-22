#ifndef GENESIS_UTILS_THREADING_CONCURRENT_VECTOR_GUARD_H_
#define GENESIS_UTILS_THREADING_CONCURRENT_VECTOR_GUARD_H_

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

#include "genesis/utils/threading/concurrent_queue.hpp"
#include "genesis/utils/threading/lightweight_semaphore.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Vector Mutex Manager
// =================================================================================================

/**
 * @brief Helper to protect concurrent access to elements of a vector or array.
 *
 * Usage:
 *
 *     // Vector of values to which we want concurrent access.
 *     std:vector<int> values;
 *     auto vector_guard = ConcurrentVectorGuard( values.size(), num_threads );
 *
 *     // Some concurrent loop - simplified here.
 *     for( size_t i = 0; i < values.size(); ++i ) {
 *         auto lock = vector_guard.get_lock_guard( i );
 *         values[i] ...
 *     }
 *
 * The class internally uses a list of mutexes, set at construction.
 */
class ConcurrentVectorGuard
{
public:

    // -------------------------------------------------------------
    //     Inner Classes
    // -------------------------------------------------------------

    /**
     * @brief RAII-style lock guard for LightweightSemaphore
     */
    class LockGuard {
    public:
        // Constructor acquires the semaphore
        LockGuard( LightweightSemaphore& semaphore )
            : semaphore(semaphore)
        {
            semaphore.wait();
        }

        // Destructor releases the semaphore
        ~LockGuard()
        {
            semaphore.signal();
        }

    private:
        LightweightSemaphore& semaphore;
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Default constructor. A default constructed instance shall not be used.
     */
    ConcurrentVectorGuard() = default;

    /**
     * @brief Create a given number of mutexes for buckets of the vector to be guarded.
     */
    ConcurrentVectorGuard( size_t num_buckets )
    {
        init_buckets_( num_buckets );
    }

    /**
     * @brief Create mutexes given the number of elements to be guarded,
     * and the number of threads to be used.
     */
    ConcurrentVectorGuard( size_t num_elements, size_t num_threads )
    {
        // Ad hoc formula to get a sufficient number of buckets without overdoing it.
        // We use the square of the number of threads, as that solves for the collision probability
        // (think of it in terms of the birthday paradox). The log of num elements is also factored
        // in for scaling - not quite sure if that part is needed, or a constant based on some
        // maximum collision probability would be better here.
        // If we were to fully use the birthday paradox collision probability here, we would get
        //
        //     num_buckets = (num_threads * (num_threads-1)) / (-2 * log(1-p))
        //
        // for some maximum collision probability p, and independent of the number of elements.
        // Can definitely be optimized more, but good enough to get going.
        size_t num_buckets = 1;
        if( num_threads > 1 ) {
            num_buckets = 2 * std::log( num_elements ) * num_threads * num_threads;
            num_buckets = std::min( num_elements, num_buckets );
        }
        init_buckets_( num_buckets );
    }

    // Copy construction deleted, as mutexes cannot be copied
    ConcurrentVectorGuard( ConcurrentVectorGuard const& ) = delete;
    ConcurrentVectorGuard( ConcurrentVectorGuard&& )      = default;

    ConcurrentVectorGuard& operator= ( ConcurrentVectorGuard const& ) = delete;
    ConcurrentVectorGuard& operator= ( ConcurrentVectorGuard&& )      = default;

    ~ConcurrentVectorGuard() = default;

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    // Unused implementation for std::mutex
    // inline std::lock_guard<std::mutex> get_lock_guard( size_t index )
    // {
    //     auto const bucket_index = index % bucket_mutexes_.size();
    //     return std::lock_guard<std::mutex>( bucket_mutexes_[bucket_index] );
    // }

    /**
     * @brief Get a RAII-style lock guard to protect concurrent access
     * to some element in a vector at @p index.
     */
    inline LockGuard get_lock_guard( size_t index )
    {
        auto const bucket_index = index % bucket_mutexes_.size();
        return LockGuard( bucket_mutexes_[bucket_index] );
    }

    /**
     * @brief Acquire the lock for a specific @p index
     *
     * This has the be used with release() to release the lock again after usage.
     * It usually is better to use get_lock_guard() instead and make use of RAII to unlock.
     */
    inline void acquire( size_t index )
    {
        auto const bucket_index = index % bucket_mutexes_.size();
        bucket_mutexes_[bucket_index].wait();
    }

    /**
     * @brief Release the lock for a specific @p index
     *
     * See acquire() for details.
     */
    inline void release( size_t index )
    {
        auto const bucket_index = index % bucket_mutexes_.size();
        bucket_mutexes_[bucket_index].signal();
    }

    /**
     * @brief Number of buckets (e.g., number of mutexes) used internally
     */
    size_t bucket_count() const
    {
        return bucket_mutexes_.size();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    void init_buckets_( size_t num_buckets )
    {
        if( num_buckets == 0 ) {
            num_buckets = 1;
        }

        // Unused implementation for std::mutex
        // bucket_mutexes_ = std::vector<std::mutex>( num_buckets );

        // Implementation with LightweightSemaphore
        bucket_mutexes_ = std::vector<LightweightSemaphore>( num_buckets );
        for( size_t i = 0; i < num_buckets; ++i ) {
            bucket_mutexes_[i].signal();
        }
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    // std::vector<std::mutex> bucket_mutexes_;
    std::vector<LightweightSemaphore> bucket_mutexes_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
