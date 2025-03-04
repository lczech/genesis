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
#include <type_traits>
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
 * However, for best performance and to safe memory, we recommend to use a more efficient hash map
 * implementation instead. We have had the best results with `phmap::parallel_flat_hash_map`,
 * see https://github.com/greg7mdp/parallel-hashmap
 * This hash map however unfortunately has a signature that cannot be used here directly,
 * as it contains non-type template parameters. So, we need to type-wrap it:
 *
 *     template <typename K, typename V>
 *     using ParallelFlatHashMap = phmap::parallel_flat_hash_map<K, V>;
 *
 * Or for the multi-threaded variant:
 *
 *     template <typename K, typename V>
 *     using ParallelFlatHashMap = phmap::parallel_flat_hash_map_m<K, V>;
 *
 * With that, we can then instantiate:
 *
 *     AccessionLookup<ParallelFlatHashMap> acc_to_tax;
 *
 * Any hash map that has the same interface as `std::unordered_map` works here.
 * Note that for the phmap above though, we provide a more efficient parallel insertion as well,
 * which uses fine-grained locking instead. When using this, there can be multiple threads adding
 * entries, such as implemented in AccessionLookupReader. However, this does not support
 * reading via the get() function at the same time as writing via add(). If entries need to be
 * retrieved while also adding new ones from other threads, external locking is needed.
 * This does not affect multi-threaded reading once the writing is done; that is thread-safe.
 *
 * @see AccessionLookupReader for a helper to read accession table formats,
 * such as the NCBI accession2taxid
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
    *
    * Add a new entry to the hash map. If an identical entry already exists, nothing happens.
    * If an entry is added for an existing key, but a different value, we throw an exception
    * by default. If @p ignore_mismatching_duplicates is set however, we just ignore the new
    * entry, and keep the existing one. This is useful to work with the messy reality of data.
    *
    * Invalidates all references and iterators to existing elements.
    *
    * Returns `true` if an entry was added, or `false` otherwise (which can only happen due to
    * @p ignore_mismatching_duplicates being set).
    */
    inline bool add(
        std::string const& accession,
        Taxon* taxon,
        bool ignore_mismatching_duplicates = false
    ) {
        // Delegate to insert() or try_emplace() depending on C++ standard and phmap usage,
        // and delegate to a concurrent write or using an external lock.
        return add_(
            has_try_emplace_<MapType>{},
            is_concurrent_<MapType>{},
            accession,
            taxon,
            ignore_mismatching_duplicates
        );
    }

    /**
     * @brief Get the lookup value (of type T, e.g., taxid, Taxon) to the requested @p accession.
     *
     * If not found, either a `nullptr` is returned, or an exception is thrown, depending on
     * @p throw_if_not_found. The returned value is invalidated when new entries are added.
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

    inline constexpr bool has_try_emplace() const
    {
        return has_try_emplace_<MapType>::value;
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    inline bool add_(
        std::false_type /* has_try_emplace_ */,
        std::false_type /* is_concurrent_ */,
        std::string const& accession,
        Taxon* taxon,
        bool ignore_mismatching_duplicates
    ) {
        // Default for std::unordered_map under C++11, using insert() and external locking.
        assert( mtx_ );
        std::lock_guard<std::mutex> lock( *mtx_ );
        auto const result = map_.insert( std::make_pair( accession, taxon ));
        if( !result.second && result.first->second != taxon ) {
            // Only throw if the key exists and has a different taxon assigned to it.
            // If it's the same anyway, we tolerate this.
            if( ignore_mismatching_duplicates ) {
                return false;
            } else {
                throw std::runtime_error(
                    "Duplicate entry for accession \"" + accession + "\" in lookup table"
                );
            }
        }
        return true;
    }

    inline bool add_(
        std::true_type /* has_try_emplace_ */,
        std::false_type /* is_concurrent_ */,
        std::string const& accession,
        Taxon* taxon,
        bool ignore_mismatching_duplicates
    ) {
        // Default for std::unordered_map under C++ > 14, or the phmap without internal locking,
        // using try_emplace() and external locking.
        assert( mtx_ );
        std::lock_guard<std::mutex> lock( *mtx_ );
        auto const result = map_.try_emplace( accession, taxon );
        if( !result.second && result.first->second != taxon ) {
            if( ignore_mismatching_duplicates ) {
                return false;
            } else {
                throw std::runtime_error(
                    "Duplicate entry for accession \"" + accession + "\" in lookup table"
                );
            }
        }
        return true;
    }

    inline bool add_(
        std::true_type /* has_try_emplace_ */,
        std::true_type /* is_concurrent_ */,
        std::string const& accession,
        Taxon* taxon,
        bool ignore_mismatching_duplicates
    ) {
        // Specialization for phmap wit internal locking.
        bool added = true;
        map_.try_emplace_l(
            accession,
            [&]( typename MapType::value_type const& it ){
                // This function is called when the element already exist,
                // while holding the internal lock, so that we can access the element.
                if( it.second != taxon ) {
                    if( ignore_mismatching_duplicates ) {
                        added = false;
                    } else {
                        throw std::runtime_error(
                            "Duplicate entry for accession \"" + accession + "\" in lookup table"
                        );
                    }
                }
            },
            taxon
        );
        return added;
    }

    // No overload for the fourth combination needed, as we only set is_concurrent_
    // when using the phmap, which then also sets has_try_emplace_ anyway.

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

    // ---------------------------
    //     has_try_emplace_
    // ---------------------------

    #if GENESIS_CPP_STD > GENESIS_CPP_STD_11

    // Try emplace using SFINAE for maps that have try_emplace (>= C++17).
    // In particular, we want this check for C++ standards where std::unordered_map supports it.
    // This also works for any map type that supports try_emplace, but we do not activate this
    // overload for C++11, as there are some template substitution errors with clang 6-8.

    // Primary template: assumes try_emplace does not exists,
    // for instance as a fallback for maps without try_emplace (C++14)
    template <typename, typename = void>
    struct has_try_emplace_ : std::false_type {};

    // Specialization: if the expression inside decltype is valid,
    // then the type has a try_emplace method, and we can use it.
    template <typename H>
    struct has_try_emplace_<H, decltype(void(
        // We attempt to call try_emplace with dummy key and value.
        std::declval<H>().try_emplace(
            std::declval<typename H::key_type>(),
            std::declval<typename H::mapped_type>()
        )
    ))> : std::true_type {};

    #else

    // Fallback for older compilers (C++11) where the above template substitution
    // SFINAE might fail to compile; we observe this with Clang 6-8.
    // The std::unordered_map of C++11 does not offer try_emplace anyway here.
    template <typename, typename = void>
    struct has_try_emplace_ : std::false_type {};

    // Specialization for parallel_flat_hash_map, which always has try_emplace.
    template <class Key, class Value, class Hash, class Eq, class Alloc, size_t N, class Mutex>
    struct has_try_emplace_<phmap::parallel_flat_hash_map<Key, Value, Hash, Eq, Alloc, N, Mutex>>
        : std::true_type
    {};

    // Specialization for parallel_node_hash_map, which always has try_emplace.
    template <class Key, class Value, class Hash, class Eq, class Alloc, size_t N, class Mutex>
    struct has_try_emplace_<phmap::parallel_node_hash_map<Key, Value, Hash, Eq, Alloc, N, Mutex>>
        : std::true_type
    {};

    #endif

    // ---------------------------
    //     is_concurrent_
    // ---------------------------

    // Nested trait to check if a phmap type is concurrent (i.e. has internal locking via mutex).
    // By default, we assume the map is not concurrent, and use external locking.
    template <class H>
    struct is_concurrent_ : std::false_type {};

    // Specialization for parallel_flat_hash_map instantiated with std::mutex.
    template <class Key, class Value, class Hash, class Eq, class Alloc, size_t N>
    struct is_concurrent_<phmap::parallel_flat_hash_map<Key, Value, Hash, Eq, Alloc, N, std::mutex>>
        : std::true_type
    {};

    // Specialization for parallel_node_hash_map instantiated with std::mutex.
    template <class Key, class Value, class Hash, class Eq, class Alloc, size_t N>
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
