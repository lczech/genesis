#ifndef GENESIS_UTILS_CONTAINERS_FUNCTION_CACHE_H_
#define GENESIS_UTILS_CONTAINERS_FUNCTION_CACHE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include "genesis/utils/containers/hash_tuple.hpp"
#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Simple Cache
// =================================================================================================

/**
 * @brief Simple cache, for example for function return values.
 *
 * General usage: Provide the pure function that needs to be cached to the constructor, for example
 * via a lambda. Then use operator() to request elements by their function parameters (which are
 * combined into a key, see below). If the cache already contains the result for these parameters,
 * it is returned; if not, it is first computed using the function that was provided in the
 * constructor.
 *
 * The template parameters of this class template are:
 *
 *  - `R`, the return type (value) of the function that is stored/cached.
 *  - `A`, the list of arguments of the function that produces the return value. These arguments
 *    are combined into a key at which a return value is stored.
 *
 * The cache of course assumes pure functions with no side effects, that is, the same arguments
 * always produce the same output. This is for example useful to cache complicated mathematical
 * functions that have few different, but re-used inputs.
 *
 * The class is thread safe and blocks as needed when elements are requested from multiple threads.
 * In order to facilitate concurrent fast access from mutliple threads, we are using the following
 * strategy: The hash map that we use for storing key value pairs is split into "shards" that
 * are accessible via an index computed from the keys. Each shard has its own mutex lock, and can
 * hence be accessed without lock on the whole data structure. As long as the number of shards is
 * sufficiently larger than the number of concurrent threads, it is likely that two threads request
 * values from different shards, and hence can access them concurrently.
 */
template <typename R, typename... A>
class FunctionCache
{
    // -------------------------------------------------------------------------
    //     Public Member Types
    // -------------------------------------------------------------------------

public:

    using size_type       = size_t;
    using key_type        = std::tuple<A...>;
    using value_type      = R;

    // We need a special hash function here that takes care of tuples.
    // See genesis/utils/containers/hash_tuple.hpp for details.
    using container_type  = std::unordered_map<key_type, value_type, genesis::utils::hash<key_type>>;

    using const_iterator  = typename container_type::const_iterator;

    // -------------------------------------------------------------------------
    //     Private Member Types
    // -------------------------------------------------------------------------

private:

    /**
     * @brief A shard keeps a part of the key-value-pairs, as well as a mutex for accessing
     * that partial map.
     */
    struct Shard
    {
        container_type cache_;
        std::mutex     guard_;
    };

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Constructor, takes the function that is needed to compute values for the cache.
     *
     * Additionaly, the number of @p shards can be set here, which should always be sufficiently
     * larger than the number of concurrent threads that access this class, for best performance.
     */
    FunctionCache( std::function<R(A...)> load_function, size_t shards = 256 )
        : load_function_( load_function )
    {
        if( shards == 0 ) {
            throw std::invalid_argument( "Cannot initialize FunctionCache with shards==0" );
        }
        shards_.reserve( shards );
        for( size_t i = 0; i < shards; ++i ) {
            // Although we are in untils namespace here, we specify the namespace full,
            // in order to avoid ambiguous overload when compiled with C++17.
            shards_.push_back( genesis::utils::make_unique<Shard>() );
        }
    }

    ~FunctionCache() = default;

    FunctionCache( FunctionCache const& ) = delete;
    FunctionCache( FunctionCache&& )      = default;

    FunctionCache& operator= ( FunctionCache const& ) = delete;
    FunctionCache& operator= ( FunctionCache&& )      = default;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the current count of elements in the cache.
     */
    size_type size() const
    {
        size_t result = 0;
        for( auto const& shard : shards_ ) {
            std::lock_guard<std::mutex> lock( shard->guard_ );
            result += shard->cache_.size();
        }
        return result;
    }

    /**
     * @brief Get a tally of how many cached values are stored in each of the shards.
     *
     * Useful for debugging to test that the shards have an equal distribution of values.
     */
    std::vector<size_t> shard_sizes() const
    {
        auto result = std::vector<size_t>( shards_.size(), 0 );
        for( size_t i = 0; i < shards_.size(); ++i ) {
            auto const& shard = shards_[i];
            std::lock_guard<std::mutex> lock( shard->guard_ );
            result[i] = shard->cache_.size();
        }
        return result;
    }

    /**
     * @brief Return whether the cache is currently empty.
     */
    bool empty() const
    {
        // We need to traverse all shards to make sure all of them are empty.
        return size() == 0;
    }

    /**
     * @brief Clear the cache.
     *
     * Clears all cached key-value-pairs.
     * Does not clear the hit_count() and miss_count(). See clear_counts() for that.
     */
    void clear()
    {
        for( auto& shard : shards_ ) {
            std::lock_guard<std::mutex> lock( shard->guard_ );
            shard->cache_.clear();
        }
    }

    /**
     * @brief Enable or disable the caching.
     *
     * This is useful for speed testing to see how much speedup the cache actually yields.
     */
    void enabled( bool value )
    {
        enabled_ = value;
    }

    /**
     * @brief Return whether the cache is currently enabled or not.
     */
    bool enabled() const
    {
        return enabled_;
    }

    /**
     * @brief Return how often in total there was a cache hit, that is, a key requested that
     * was already present in the cache.
     */
    size_t hit_count() const
    {
        return hit_count_;
    }

    /**
     * @brief Return how often in total there was a cache miss, that is, a key requested that
     * was not yet present in the cache and hence lead to a computation of the load function.
     */
    size_t miss_count() const
    {
        return miss_count_;
    }

    /**
     * @brief Clear the hit_count() and miss_count() counters.
     */
    void clear_counts()
    {
        hit_count_ = 0;
        miss_count_ = 0;
    }

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    /**
     * @brief Access operator for retrieving a value given the key (arguments to the cached function).
     *
     * This returns the cached value if present, or first computes and stores it if necessary.
     */
    R const& operator()( A const&... arguments )
    {
        // Build the key. Can be done without mutex locking.
        std::tuple<A...> const key( arguments... );

        // Get the shard that this key belongs to.
        assert( shard_index_( key ) < shards_.size() );
        assert( shards_[ shard_index_( key ) ] );
        auto& shard = *shards_[ shard_index_( key ) ];

        // Lock access to the shard to look up the key. Released at the end of the scope.
        std::lock_guard<std::mutex> lock( shard.guard_ );

        // Allow to disable the caching completely, for example for speed testing.
        // We still need to store it, in order to be able to return a reference...
        if( ! enabled_ ) {
            return shard.cache_[key] = load_function_( arguments... );
        }

        // Now try to find it in the cache.
        auto search = shard.cache_.find( key );
        if( search != shard.cache_.end() ) {
            ++hit_count_;
            return search->second;
        }

        // If not present, compute, store, and return it.
        ++miss_count_;
        return shard.cache_[key] = load_function_( arguments... );
    }

    /**
     * @brief Return whether a certain key is already in the cache.
     */
    bool contains( A const&... arguments ) const
    {
        // Simply follow the steps of the above operator(), but only check the presence of the key,
        // without computing the function.
        std::tuple<A...> const key( arguments... );
        auto& shard = *shards_[ shard_index_( key ) ];
        std::lock_guard<std::mutex> lock( shard.guard_ );
        auto search = shard.cache_.find( key );
        return search != shard.cache_.end();
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    size_t shard_index_( key_type const& key ) const
    {
        return genesis::utils::hash<key_type>{}( key ) % shards_.size();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // The functor that computes/loads the value for a given key. This needs to be a pure function,
    // that is, a function without any side effects that for a given key always returns the same
    // result. In fact, it does not really need to be pure, but the cache would just return
    // its cached value instead of calling the function again, and hence, its side effects would
    // not happen... So, let's say, we only want pure functions.
    std::function<R(A...)> load_function_;

    // Set of shards to distribute the locking, in order to allow for fast parallel access
    // from many threads. Unfortunately, we need pointers here, as the mutexes in the shards
    // are not copyable.
    std::vector<std::unique_ptr<Shard>> shards_;

    // For speed testing, we offer to deactivate the cache completely.
    bool enabled_ = true;

    // We also count how often we have cache hits (key already present), and cache
    // misses (key not present and load function needs to be computed).
    size_t hit_count_ = 0;
    size_t miss_count_ = 0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
