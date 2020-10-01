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
 * The template parameters of this class template are:
 *
 *  - `R`, the return type (value) that is stored.
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

    void clear()
    {
        // Lock access to everything. Released automatically once we return.
        std::lock_guard<std::mutex> lock( mutex_ );

        cache_.clear();
    }

#ifdef DEBUG

    size_t count( A const&... arguments )
    {
        std::tuple<A...> key( arguments... );
        return count( key );
    }

    size_t count( std::tuple<A...> const& key )
    {
        std::lock_guard<std::mutex> lock( mutex_ );
        if( count_.count( key )) {
            return count_[key];
        } else {
            return 0;
        }
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

        // Build the key. If we are in debug, we also count how often that key has been called.
        std::tuple<A...> key( arguments... );
        #ifdef DEBUG
            ++count_[key];
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
    std::unordered_map<key_type, size_t, genesis::utils::hash<key_type>> count_;
#endif

};

} // namespace utils
} // namespace genesis

#endif // include guard
