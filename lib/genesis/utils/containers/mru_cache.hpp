#ifndef GENESIS_UTILS_CONTAINERS_MRU_CACHE_H_
#define GENESIS_UTILS_CONTAINERS_MRU_CACHE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include <cassert>
#include <functional>
#include <list>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     MRU Cache
// =================================================================================================

/**
 * @brief Most Recently Used Cache.
 *
 * The class offers a cache that maps from unique keys of type `Key` to values of type `T`,
 * automatically loading elements as needed using the
 * @link MruCache::load_function load_function@endlink.
 * The cache only keeps a certain number of elements. That is, the elements that
 * were least recently used are removed to avoid exceeding the capacity().
 *
 * The main functions are fetch() and fetch_copy(), which retrieve an element from the cache,
 * potentially loading it first, and potentially removing old elements.
 * During a fetch, it is possible that one more element is kept in memory than the capacity allows,
 * before removing the oldest one. This is done in order to recover from a load that failed
 * (with an exception) without altering the state of the cache.
 *
 * In order to use this class, the functor @link MruCache::load_function load_function@endlink
 * has to be set before calling fetch(), fetch_copy() or touch(). The functor needs to take a const
 * reference to the key type `Key` and return a mapped type `T` by value.
 *
 * Example:
 *
 * ~~~cpp
 * // Path to some data.
 * std::string dir = "/path/to/data";
 *
 * // Create a cache from file names to file contents, with a capacity of 5.
 * MruCache<std::string, std::string> cache{ 5 };
 *
 * // Our load function is to read the file.
 * cache.load_function = [ &dir ]( std::string const& file ){
 *     return file_read( dir + "/" + file );
 * };
 *
 * // Access an element, that is, load a file into the cache.
 * auto const& content = cache.fetch( "test.txt" );
 * ~~~
 *
 * Lastly, a second functor @link MruCache::release_function release_function@endlink can be used
 * to specify a function that is exectued before an element is removed from the cache.
 * If this functor is not set, the element is simply removed without any prior call.
 *
 * Thread safety: As fetch() returns by reference, it is not thread safe. A fetched element can be
 * removed at any time when other threads invoke fetch() again, leaving to a dangling reference.
 * Thus, for multi-threaded use, fetch_copy() can be used, which is guarded and returns a copy of
 * the elment instead. See there for details.
 */
template< typename Key, typename T >
class MruCache
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using key_type        = Key;
    using mapped_type     = T;
    using value_type      = std::pair<const key_type, mapped_type>;

    using       iterator  = typename std::list< value_type >::iterator;
    using const_iterator  = typename std::list< value_type >::const_iterator;

    using size_type       = size_t;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor. Uses a capacity() of 0, that is, limitless.
     *
     * A capacity of 0 means limitless, meaning that no elements are ever removed.
     * @see capacity( size_t value )
     */
    MruCache() = default;

    /**
     * @brief Construct a cache with a given @p capacity.
     *
     * A capacity of 0 means limitless, meaning that no elements are ever removed.
     * @see capacity( size_t value )
     */
    MruCache( size_t capacity )
        : capacity_( capacity )
    {}

    ~MruCache()
    {
        // Need to call the release_function. Make it easy, just call clear.
        clear();
    }

    MruCache( MruCache const& ) = default;
    MruCache( MruCache&& )      = default;

    MruCache& operator= ( MruCache const& ) = default;
    MruCache& operator= ( MruCache&& )      = default;

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    /**
     * @brief Get an iterator to the begin of the cache, that is, the most recently used element.
     */
    iterator begin()
    {
        return cache_.begin();
    }

    /**
     * @brief Get a const iterator to the begin of the cache, that is, the most recently used element.
     */
    const_iterator begin() const
    {
        return cache_.cbegin();
    }

    /**
     * @brief Get an iterator to past-the-end of the cache, that is, one element after the
     * least recently used one.
     */
    iterator end()
    {
        return cache_.end();
    }

    /**
     * @brief Get a const iterator to past-the-end of the cache, that is, one element after the
     * least recently used one.
     */
    const_iterator end() const
    {
        return cache_.cend();
    }

    /**
     * @copydoc begin() const
     */
    const_iterator cbegin()
    {
        return cache_.cbegin();
    }

    /**
     * @copydoc end() const
     */
    const_iterator cend()
    {
        return cache_.cend();
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the current count of elements being loaded in the cache.
     */
    size_type size() const
    {
        assert( lookup_.size() == cache_.size() );
        return lookup_.size();
    }

    /**
     * @brief Return the currently set capacity of the cache, i.e., its maximal size.
     *
     * A capacity of 0 means limitless, that is, no elements are ever removed.
     */
    size_type capacity() const
    {
        assert( capacity_ == 0 || lookup_.size() <= capacity_ );
        return capacity_;
    }

    /**
     * @brief Return whether the cache is currently empty, that is, has no elements loaded.
     */
    bool empty() const
    {
        return lookup_.empty();
    }

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    /**
     * @brief Get an element.
     *
     * This is the main function of the class. It gets an element given its @p key, either by
     * retrieving it from the cache, or loading it into the cache first, if needed.
     *
     * If loading an element leads to the capacity of the cache begin exceeded,
     * the least recently used element is removed. The removal is done after loading the new
     * element. This means that the memory usage can be one more element than the capacity() allows.
     * This is done to make sure that an exception thrown when loading the new element does not lead
     * to the cache being altered.
     *
     * Thread safety: Not thread safe, because it does not use a guard, and because it returns a
     * reference, which can become dangling if other threads fetch new elements, leading to the
     * referenced one being removed. For multi-threaded use, see fetch_copy().
     *
     * Caveat: Even in single-thread use, a variable storing a reference obtained from fetch()
     * can become dangling, if more new elements are fetched or touched than the capacity allows.
     * Thus, the variable needs to go out of scope before this happens. For example, a loop
     * over keys, fetching an element in the beginning of the loop body and keeping the reference
     * only within the loop body without calling fetch() again, is fine.
     */
    mapped_type& fetch( key_type const& key )
    {
        // Use the lookup map to check whether the element is in the cache.
        auto const lit = lookup_.find( key );

        // Found it! Move it to the front and return its mapped element.
        if( lit != lookup_.end() ) {
            return promote_and_get_( lit->second );
        }

        // If we are here, the element was not found.
        // Add the element to the cache and lookup. Throws if the load function is empty.
        // Also, if loading fails with an exception, nothing happens to the container.
        assert( lookup_.count( key ) == 0 );
        cache_.push_front({ key, load_function( key ) });
        lookup_[ key ] = cache_.begin();
        assert( cache_.size() == lookup_.size() );

        // Make sure that we stay within capacity.
        shrink_();

        // Finally, return the element.
        assert( cache_.size() > 0 );
        return cache_.begin()->second;
    }

    /**
     * @brief Get an element by copy.
     *
     * This works exactly the same as fetch(), but is thread safe and returns a copy.
     * See fetch() for details.
     *
     * Because the loading is not part of the mutex that makes this function thread safe,
     * it is possible to parallely load elements in different threads.
     * However, when two threads need to load an element at the same time, the loading
     * may happen twice. Then, only the first thread that finishes loading stores the element
     * in the cache, while the other one is discarded. This is done in order to allow
     * parallel loading without the hassle of per-element locks.
     *
     * If the cache is used in a multi-threaded environment and holds large elements,
     * making actual copies might be too expensive. In that case, a neat trick is to store
     * shared pointers to the elements instead:
     *
     * ~~~cpp
     * // Path to some data.
     * std::string dir = "/path/to/data";
     *
     * // Create a cache from file names to shared pointers of file contents.
     * MruCache<std::string, std::shared_ptr<std::string>> cache{ 5 };
     *
     * // Load elements from file.
     * cache.load_function = [ &dir ]( std::string const& file ){
     *     return std::make_shared<std::string>( file_read( dir + "/" + file ));
     * };
     *
     * // Fetch an element, that is, load a file into the cache.
     * // Store it by copy, which just copies the shared pointer.
     * auto content = cache.fetch_copy( "fail2.jtest" );
     * ~~~
     *
     * As the control block of `std::shared_ptr` is thread safe, these shared pointer copies can
     * stay alive in a thread that still needs the element, even if the element was removed from
     * the cache by other threads in the meantime.
     */
    mapped_type fetch_copy( key_type const& key )
    {
        // First, check if the elemt is there. If so, simply return it.
        {
            // Lock access to everything.
            std::lock_guard<std::mutex> lock( mutex_ );

            // Use the lookup map to check whether the element is in the cache.
            auto const lit = lookup_.find( key );

            // Found it! Move it to the front and return its mapped element.
            if( lit != lookup_.end() ) {
                // LOG_DBG << "Found.";
                return promote_and_get_( lit->second );
            }
        }

        // If we are here, the element is not in the cache. Load it into a dummy list,
        // without locking, so that loading can happen in parallel.
        std::list< value_type > tmp_list;
        tmp_list.push_front({ key, load_function( key ) });

        // Now, store it if needed.
        {
            // Lock access to everything.
            std::lock_guard<std::mutex> lock( mutex_ );

            // Check whether the element was put into the cache in the meantime (by another thread).
            auto const lit = lookup_.find( key );

            // Found it! Move it to the front and return its mapped element.
            // In that case, we do not use tmp_list, but discard the element in there.
            if( lit != lookup_.end() ) {
                // LOG_DBG << "Wasted.";
                return promote_and_get_( lit->second );
            }
            // LOG_DBG << "Loaded.";

            // If we are here, the element was not found.
            // Add the element to the cache and lookup.
            assert( lookup_.count( key ) == 0 );
            assert( tmp_list.size() == 1 );
            cache_.splice( cache_.begin(), tmp_list, tmp_list.begin() );
            lookup_[ key ] = cache_.begin();
            assert( cache_.size() == lookup_.size() );

            // Make sure that we stay within capacity.
            shrink_();

            // Finally, return the element.
            assert( cache_.size() > 0 );
            return cache_.begin()->second;
        }
    }

    /**
     * @brief Return whether an element is currently in the cache.
     *
     * Thread safety: Safe. But the element might be removed by other threads soon.
     */
    bool contains( key_type const& key )
    {
        return lookup_.count( key ) > 0;
    }

    /**
     * @brief Bring an element to the front, and load it if needed.
     *
     * The function behaves just like fetch_copy(), but without returning the element.
     * Useful to pre-load the cache.
     *
     * Be aware however that having touched an element in multi threaded used does not guarantee
     * that it stays in the cache for long. Other threads might have fetched other elements,
     * leading to the removal of the touched one. In that case, it has to be loaded again when
     * fetched later.
     */
    void touch( key_type const& key )
    {
        (void) fetch_copy( key );
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Set the capacity of the cache, i.e., its maximal size.
     *
     * Setting the capacity to 0 means limitless, that is, no elements are ever removed.
     */
    void capacity( size_t value )
    {
        // Lock access to everything. It would be a weird use case where this function is called
        // while also fetching copies of elements in a different thread, but still,
        // we need do protect this.
        std::lock_guard<std::mutex> lock( mutex_ );

        capacity_ = value;
        shrink_();
        assert( capacity_ == 0 || size() <= capacity_ );
    }

    /**
     * @brief Clear the cache.
     */
    void clear()
    {
        // Lock access to everything.
        std::lock_guard<std::mutex> lock( mutex_ );

        if( release_function ) {
            for( auto& elem : cache_ ) {
                release_function( elem.first, elem.second );
            }
        }
        cache_.clear();
        lookup_.clear();
    }

    // -------------------------------------------------------------------------
    //     Implementation Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief If there are more elements than capacity, remove elements from the back of the cache.
     */
    void shrink_()
    {
        // Capacity/target size of 0 means limitless. No shrinking.
        if( capacity_ == 0 ) {
            return;
        }

        // Remove last elements from cache and lookup if needed.
        while( size() > capacity_ ) {

            // This needs to be maintained.
            assert( lookup_.size() == cache_.size() );

            // If we are here, size > capacity_ > 0
            assert( cache_.size() > 1 );

            auto& last = cache_.back();
            if( release_function ) {
                release_function( last.first, last.second );
            }
            lookup_.erase( last.first );
            cache_.pop_back();
        }

        // Check invariant and result.
        assert( lookup_.size() == cache_.size() );
        assert( lookup_.size() <= capacity() );
    }

    /**
     * @brief Helper function that moves an element which is being cached to the front
     * if needed and returns it.
     */
    mapped_type& promote_and_get_( iterator const& cache_it )
    {
        assert( cache_.size() > 0 );

        // Only move if it is not already at the front.
        if( cache_it != cache_.begin() ) {
            cache_.splice( cache_.begin(), cache_, cache_it );
        }

        // Return the element.
        return cache_it->second;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Function to load an element into the cache if it is being fetched but not there yet.
     *
     * Has to be set before calling fetch(), fetch_copy() or touch(),
     * otherwise an exception is thrown.
     */
    std::function< mapped_type( key_type const& )> load_function;

    /**
     * @brief Function to be called when an element is removed from the cache.
     *
     * This function is called whenever elements are removed from the cache, e.g., due to being
     * the least recently used one, due to a call to clear(), or when the destructor is called.
     *
     * Can be empty. In that case, nothing is called when elements are removed.
     */
    std::function< void( key_type const&, mapped_type& )> release_function;

private:

    /**
     * @brief Target capacity of the cache.
     *
     * Cache size never exceeds this value, except by one when fetching a new element.
     * Special case: Capacity == 0 means limitless. No elements are ever removed.
     */
    size_type capacity_ = 0;

    /**
     * @brief List of key value pairs. The actual data.
     *
     * The first element is the most recently used one.
     */
    std::list< value_type > cache_;

    /**
     * @brief Lookup to find elemetns in the list quickly.
     *
     * Introduces space overhead for storing the keys again, but allows to find an iterator
     * to an element in the list quickly.
     */
    std::unordered_map< key_type, iterator > lookup_;

    /**
     * @brief Protect concurrent access.
     */
    std::mutex mutex_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
