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
 * The main function is fetch(), which retrieves an element from the cache, potentially loading
 * it first, and potentially removing old elements. During a fetch, it is possible that one
 * more element is kept in memory than the capacity allows, before removing the oldest one.
 * This is done in order to recover from a load that failed (with an exception) without altering
 * the state of the cache.
 *
 * In order to use this class, the functor @link MruCache::load_function load_function@endlink
 * has to be set before calling fetch() or touch(). The functor needs to take a const reference to
 * the key type and return a value type by value.
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
 * to specify a function that is exectued before an element is removed from the cache due to being
 * too old. If this functor is not set, the element is simply removed without any prior call.
 *
 * Thread safety: The class does not guarantee any thread safety. As each fetch() changes the
 * data structure, concurrent access is not possible. Thus, when this class is intended to be used
 * in multi-threaded environments, its access needs to be guareded.
 *
 * Furthermore, as fetch() returns an element by reference, only one element should be fetched and
 * stored in a reference at a time. For cases were one element is accessed at a time, e.g.,
 * in a loop over keys, this is not an issue. In the example above, `content` however becomes a
 * dangling reference after a few more calls to fetch(). In such cases, the return value of fetch()
 * needs to be stored by copy instead.
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
     * A capacity of 0 means limitless, that is, no elements are ever removed.
     * @see capacity( size_t value )
     */
    MruCache() = default;

    /**
     * @brief Construct a cache with a given @p capacity.
     *
     * A capacity of 0 means limitless, that is, no elements are ever removed.
     * @see capacity( size_t value )
     */
    MruCache( size_t capacity )
        : capacity_( capacity )
    {}

    ~MruCache() = default;

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
     * This is done to make sure that an exception thrown when loading the new file does not lead
     * to the cache being altered.
     */
    mapped_type& fetch( key_type const& key )
    {
        // Use the lookup map to check whether the element is in the cache.
        auto const lit = lookup_.find( key );

        // Found it! Move it to the front and return its mapped element.
        if( lit != lookup_.end() ) {
            assert( cache_.size() > 0 );

            // Only move if it is not already at the front.
            if( lit->second != cache_.begin() ) {
                cache_.splice( cache_.begin(), cache_, lit->second );
            }
            return lit->second->second;
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
     * @brief Get an element if it is already in the cache, without moving it to the front.
     *
     * The element is returned, but its position in the cache not altered.
     * The element has to be in the cache, otherwise an execption is thrown.
     */
    mapped_type& peek( key_type const& key )
    {
        auto const lit = lookup_.find( key );
        if( lit == lookup_.end() ) {
            throw std::invalid_argument(
                "Trying to peek at an element in MruCache that is not there."
            );
        }
        return lit->second->second;
    }

    /**
     * @brief Return whether an element is currently in the cache.
     */
    bool check( key_type const& key )
    {
        return lookup_.count( key ) > 0;
    }

    /**
     * @brief Bring an element to the front, and load it if needed.
     *
     * This is just fetch() without returning the element ;-)
     */
    void touch( key_type const& key )
    {
        (void) fetch( key );
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
        capacity_ = value;
        shrink_();
        assert( capacity_ == 0 || size() <= capacity_ );
    }

    /**
     * @brief Clear the cache.
     */
    void clear()
    {
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

            // If we are here, size >= capacity_ > 0
            assert( cache_.size() > 0 );

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

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Function to load an element into the cache if it is being fetched but not there yet.
     *
     * Has to be set before calling fetch() or touch(), otherwise an exception is thrown.
     */
    std::function< mapped_type( key_type const& )> load_function;

    /**
     * @brief Function to be called when an element is erased from the cache due to being
     * the least recently used one.
     *
     * Can be empty. In that case, nothing is called.
     *
     * The function is not called when the destructor of the cache is called.
     */
    std::function< void( key_type const&, mapped_type& )> release_function;

private:

    /**
     * @brief Target capacity. Cache size never exceeds this value.
     *
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
};

} // namespace utils
} // namespace genesis

#endif // include guard
