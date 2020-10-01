#ifndef GENESIS_UTILS_CONTAINERS_SIMPLE_CACHE_H_
#define GENESIS_UTILS_CONTAINERS_SIMPLE_CACHE_H_

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

#include "genesis/utils/containers/hash_tuple.hpp"

#include <cassert>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <tuple>
#include <unordered_map>

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
 * it is returned; if not, it is first computed using the provided pure function.
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
 */
template <typename R, typename... A>
class SimpleCache
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using size_type       = size_t;
    using key_type        = std::tuple<A...>;
    using value_type      = R;

    // We need a special hash function here that takes care of tuples.
    // See genesis/utils/containers/hash_tuple.hpp for details.
    using container_type  = std::unordered_map<key_type, value_type, genesis::utils::hash<key_type>>;

    using const_iterator  = typename container_type::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor, takes the function that is needed to compute values for the cache.
     */
    SimpleCache( std::function<R(A...)> load_function )
        : load_function_( load_function )
    {}

    ~SimpleCache() = default;

    SimpleCache( SimpleCache const& ) = default;
    SimpleCache( SimpleCache&& )      = default;

    SimpleCache& operator= ( SimpleCache const& ) = default;
    SimpleCache& operator= ( SimpleCache&& )      = default;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the current count of elements in the cache.
     */
    size_type size() const
    {
        return cache_.size();
    }

    /**
     * @brief Return whether the cache is currently empty.
     */
    bool empty() const
    {
        return cache_.empty();
    }

    /**
     * @brief Clear the cache.
     */
    void clear()
    {
        // Lock access to everything. Released automatically once we return.
        std::lock_guard<std::mutex> lock( mutex_ );

        cache_.clear();
    }

#ifdef DEBUG

    /**
     * @brief Return whether the cache is currently enabled or not.
     */
    bool enabled() const
    {
        return enabled_;
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
     * @brief Return how often a particular result for a given set of function arguments was
     * requested in total.
     */
    size_t count( A const&... arguments )
    {
        std::tuple<A...> key( arguments... );
        return count( key );
    }

    /**
     * @brief Return how often a particular result for a given set of function arguments was
     * requested in total, given its key.
     *
     * This can be used to look up counts while iterating the elements via begin() and end().
     */
    size_t count( std::tuple<A...> const& key )
    {
        std::lock_guard<std::mutex> lock( mutex_ );
        if( counts_.count( key )) {
            return counts_[key];
        } else {
            return 0;
        }
    }

    /**
     * @brief Clear the counts of how often each element was requested.
     */
    void clear_counts()
    {
        // Lock access to everything. Released automatically once we return.
        std::lock_guard<std::mutex> lock( mutex_ );

        counts_.clear();
    }

#endif

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
        // Lock access to everything. Released automatically once we return.
        std::lock_guard<std::mutex> lock( mutex_ );

        // Build the key.
        std::tuple<A...> key( arguments... );

        #ifdef DEBUG
            // In debug, count how often the element was requested.
            ++counts_[key];

            // And allow to disable the caching completely, for speed testing.
            // We still need to store it, in order to be able to return a reference...
            if( ! enabled_ ) {
                return cache_[key] = load_function_( arguments... );
            }
        #endif

        // Now try to find it in the cache.
        auto search = cache_.find( key );
        if( search != cache_.end() ) {
            return search->second;
        }

        // If not present, compute, store, and return it.
        return cache_[key] = load_function_( arguments... );
    }

    /**
     * @brief Iterate all elements currently present in the cache.
     *
     * Naturally, not thread-safe in itself. User has to ensure thread safety externally,
     * that is, not to access elements via the operator() while iterating the cache,
     * as accessing not-yet-cached elements will change the container and iterators.
     */
    const_iterator begin() const
    {
        return cache_.begin();
    }

    /**
     * @brief Iterate all elements currently present in the cache.
     */
    const_iterator end() const
    {
        return cache_.end();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::mutex mutex_;
    std::function<R(A...)> load_function_;

    container_type cache_;

#ifdef DEBUG

    // In debug, we offer a count of how often each element was requested.
    // We again need a special hash function here that takes care of tuples.
    // See genesis/utils/containers/hash_tuple.hpp for details.
    std::unordered_map<key_type, size_t, genesis::utils::hash<key_type>> counts_;

    // Furthermore, for speed testing, we offer to deactivate the cache completely.
    bool enabled_ = true;

#endif

};

} // namespace utils
} // namespace genesis

#endif // include guard
