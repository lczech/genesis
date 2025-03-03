#ifndef GENESIS_TAXONOMY_ACCESSION_LOOKUP_H_
#define GENESIS_TAXONOMY_ACCESSION_LOOKUP_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

// =================================================================================================
//     Forward Declarations
// =================================================================================================

// For the parallel hashmap of https://github.com/greg7mdp/parallel-hashmap
// we offer specialized refinements of the below class that allow multi-threaded
// insertion of elements without an external lock on the data structure.
// For this, we here forward-declare the two hash maps of that repository,
// such that we can check for it without having to have the classes in our code base.

namespace phmap {

template <class K, class V, class Hash, class Eq, class Alloc, size_t N, class Mutex>
class parallel_flat_hash_map;

template <class K, class V, class Hash, class Eq, class Alloc, size_t N, class Mutex>
class parallel_node_hash_map;

}

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Accession Lookup
// =================================================================================================

/**
 * @brief Lookup table from some accession name to a Taxon in a Taxonomy.
 *
 * This is a convenience wrapper around a hash map, with some more nice error messages and
 * a simplified interface for the particular use case.
 *
 * By default, the class internally uses a `std::unordered_map` as the underlying hash map.
 * However, for best performance and to safe memory, we recommend to use a more efficient
 * hash map implementation instead. We have had the best results with `phmap::parallel_flat_hash_map`,
 * see https://github.com/greg7mdp/parallel-hashmap
 * This hash map however unfortunately has a signature that cannot be used here directly,
 * as it contains non-type template parameters. So, we need to type-wrap it:
 *
 *     template <typename K, typename V>
 *     using ParallelFlatHashMap = phmap::parallel_flat_hash_map<K, V>;
 *
 * With that, we can then instantiate:
 *
 *     AccessionLookup<ParallelFlatHashMap> acc_to_tax;
 *
 * Any hash map that has the same interface as `std::unordered_map` works here.
 *
 * @see AccessionLookupReader for a helper to read a table format.
 */
template<template<typename...> class Hashmap = std::unordered_map>
// template<template<typename, typename, typename...> class Hashmap = std::unordered_map>
class AccessionLookup
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using MapType = Hashmap<std::string, Taxon*>;
    using Iterator = typename MapType::iterator;
    using ConstIterator = typename MapType::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    AccessionLookup()
        : mtx_( ::genesis::utils::make_unique<std::mutex>() )
    {}

    ~AccessionLookup() = default;

    AccessionLookup( AccessionLookup const& ) = default;
    AccessionLookup( AccessionLookup&& )      = default;

    AccessionLookup& operator= ( AccessionLookup const& ) = default;
    AccessionLookup& operator= ( AccessionLookup&& )      = default;

    // -------------------------------------------------------------------------
    //     Add and Get
    // -------------------------------------------------------------------------

    /**
    * @brief Add an @p accession to the lookup table.
    */
    inline void add( std::string const& accession, Taxon* taxon )
    {
        // Delegate to insert() or try_emplace() depending on C++ standard.
        add_( accession, taxon );
    }

    /**
     * @brief Get the Taxon to the requested @p accession.
     *
     * If not found, either a `nullptr` is returned, or an exception is thrown, depending on
     * @p throw_if_not_found.
     */
    inline Taxon* get(
        std::string const& accession,
        bool also_look_up_first_word = true,
        bool throw_if_not_found = false
    ) const {
        auto const it = find_( accession, also_look_up_first_word );
        if( it == map_.end() ) {
            if( throw_if_not_found ) {
                throw std::invalid_argument(
                    "No entry for accession '" + accession + "' in lookup table"
                );
            } else {
                return nullptr;
            }
        }
        return it->second;
    }

    /**
     * @brief Get the Taxon for the requested @p accession.
     *
     * Same as get(), but returns the Taxon as a const pointer.
     */
    inline Taxon const* cget(
        std::string const& accession,
        bool also_look_up_first_word = true,
        bool throw_if_not_found = false
    ) const {
        return get( accession, also_look_up_first_word, throw_if_not_found );
    }

    /**
     * @brief Check if the map contains a specific key.
     */
    inline bool contains(
        std::string const& accession,
        bool also_look_up_first_word = true
    ) const {
        return find_( accession, also_look_up_first_word ) != map_.end();
    }

    // -------------------------------------------------------------------------
    //     Utilities
    // -------------------------------------------------------------------------

    inline size_t size() const
    {
        return map_.size();
    }

    inline bool empty() const
    {
        return map_.empty();
    }

    inline void clear()
    {
        map_.clear();
    }

    inline Iterator begin()
    {
        return map_.begin();
    }

    inline ConstIterator begin() const
    {
        return map_.begin();
    }

    inline Iterator end()
    {
        return map_.end();
    }

    inline ConstIterator end() const
    {
        return map_.end();
    }

    inline MapType& data()
    {
        return map_;
    }

    inline MapType const& data() const
    {
        return map_;
    }

    inline constexpr bool is_concurrent() const
    {
        return is_concurrent_<MapType>::value;
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    #if GENESIS_CPP_STD > GENESIS_CPP_STD_11

    /**
     * @brief Try emplace using SFINAE for maps that have try_emplace (>= C++17).
     *
     * This works for any map type that supports try_emplace, but we do not activate this
     * overload for C++11, as there are some template substitution errors with clang 6-8.
     */
    template<typename Map = MapType>
    typename std::enable_if< decltype(
        std::declval<Map>().try_emplace(
            std::declval<std::string>(),
            std::declval<Taxon*>()
        ),
        std::true_type()
    )::value >::type
    inline add_( std::string const& accession, Taxon* taxon )
    {
        // Add a new entry to the lookup hash map.
        if( !is_concurrent_<MapType>::value ) {
            // For non-thread-safe maps, lock externally.
            assert( mtx_ );
            auto lock = std::lock_guard<std::mutex>( *mtx_ );
            auto const result = map_.try_emplace( accession, taxon );
            if( !result.second ) {
                // The key already exist. Only throw if it has a different taxon assigned to it.
                // If it's the same anyway, we tolerate this.
                is_duplicate_or_throw_( accession, taxon );
            }
        } else {
            // For concurrent maps, no external locking is needed for the insertion.
            auto const result = map_.try_emplace( accession, taxon );
            if( !result.second ) {
                // For the duplicate check, we need to obtain the lock first,
                // as the iterator of result.first is not thread-safe. The bool of
                // result.second that we access here should be thread-safe though.
                auto lock = std::lock_guard<std::mutex>( *mtx_ );
                is_duplicate_or_throw_( accession, taxon );
            }
        }
    }

    /**
     * @brief Fallback for maps without try_emplace (C++14)
     */
    template<typename Map = MapType>
    typename std::enable_if< !decltype(
        std::declval<Map>().try_emplace(
            std::declval<std::string>(),
            std::declval<Taxon*>()
        ),
        std::true_type()
    )::value >::type
    inline add_( std::string const& accession, Taxon* taxon )
    {
        // Same logic as above, but using insert() instead of try_emplace().
        // Note: This function is unfortunately duplcated below for older compilers.
        if( !is_concurrent_<MapType>::value ) {
            assert( mtx_ );
            auto lock = std::lock_guard<std::mutex>( *mtx_ );
            auto const result = map_.insert( std::make_pair( accession, taxon ));
            if( !result.second ) {
                is_duplicate_or_throw_( accession, taxon );
            }
        } else {
            auto const result = map_.insert( std::make_pair( accession, taxon ));
            if( !result.second ) {
                auto lock = std::lock_guard<std::mutex>( *mtx_ );
                is_duplicate_or_throw_( accession, taxon );
            }
        }
    }

    #else

    /**
     * @brief Fallback for older compilers (C++11) where the above template substitution
     * SFINAE might fail to compile; we observe this with Clang 6-8.
     */
    inline void add_( std::string const& accession, Taxon* taxon )
    {

        // Code duplcation from above... Not nice, but we did not get this to work properly
        // in a way that compiles with C++11, C++14, and later with just one approach...
        if( !is_concurrent_<MapType>::value ) {
            assert( mtx_ );
            auto lock = std::lock_guard<std::mutex>( *mtx_ );
            auto const result = map_.insert( std::make_pair( accession, taxon ));
            if( !result.second ) {
                is_duplicate_or_throw_( accession, taxon );
            }
        } else {
            auto const result = map_.insert( std::make_pair( accession, taxon ));
            if( !result.second ) {
                auto lock = std::lock_guard<std::mutex>( *mtx_ );
                is_duplicate_or_throw_( accession, taxon );
            }
        }
    }

    #endif

    inline void is_duplicate_or_throw_( std::string const& accession, Taxon* taxon )
    {
        // The function assumes to be called with a lock on the mutex being held by the caller!
        // The above add_() functions are thread safe for the insert/emplace,
        // either by locking everything through mtx_, or, when using a `parallel_hashmap`,
        // through its internal locking.
        // However, in case of a key already being present, the result.first of the insertion
        // is an iterator, and hence not thread-safe to access.
        // We could overload the above to work differently for std::unordered_map
        // and parallel_hashmap, but as we do not expect this error case to happen
        // often, it is fine to just load the key/value again here.
        auto it = map_.find( accession );
        if( it == map_.end() ) {
            // This should never happen, as we only call this function when an entry
            // does already exist. Still, let's check, just in case.
            throw std::runtime_error(
                "Entry for accession \"" + accession + "\" was erased in lookup table"
            );
        } else if( it->second != taxon ) {
            throw std::runtime_error(
                "Duplicate entry for accession \"" + accession + "\" in lookup table"
            );
        }
        // else: all good, nothing to do.
    }

    inline ConstIterator find_(
        std::string const& accession,
        bool also_look_up_first_word
    ) const {
        auto it = map_.find( accession );
        if( it != map_.end() ) {
            return it;
        }
        if( also_look_up_first_word ) {
            auto const acc_first = utils::split( accession, "\t " )[0];
            return map_.find( acc_first );
        }
        return map_.end();
    }

    // -------------------------------------------------------------------------
    //     Type Helpers
    // -------------------------------------------------------------------------

private:

    // Nested trait to check if a map type is concurrent (i.e. has internal locking).
    // By default, we assume the map is not concurrent, and use external locking.
    template <typename T>
    struct is_concurrent_ : std::false_type {};

    // Specialization for parallel_flat_hash_map instantiated with std::mutex.
    template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc, size_t N>
    struct is_concurrent_<phmap::parallel_flat_hash_map<Key, Value, Hash, Eq, Alloc, N, std::mutex>>
        : std::true_type
    {};

    // Specialization for parallel_node_hash_map instantiated with std::mutex.
    template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc, size_t N>
    struct is_concurrent_<phmap::parallel_node_hash_map<Key, Value, Hash, Eq, Alloc, N, std::mutex>>
        : std::true_type
    {};

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    MapType map_;

    // For multi-threaded use, we need a mutex.
    // Use a pointer, so that this class is movable.
    std::unique_ptr<std::mutex> mtx_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
