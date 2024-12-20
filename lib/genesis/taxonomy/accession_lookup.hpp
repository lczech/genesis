#ifndef GENESIS_TAXONOMY_ACCESSION_LOOKUP_H_
#define GENESIS_TAXONOMY_ACCESSION_LOOKUP_H_

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

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

    AccessionLookup()  = default;
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
        auto const result = map_.try_emplace( accession, taxon );
        if( !result.second ) {
            // Only throw if the key exists and has a different taxon assigned to it.
            // If it's the same anyway, we tolerate this.
            if( result.first->second != taxon ) {
                throw std::runtime_error(
                    "Duplicate entry for accession '" + accession + "' in lookup table"
                );
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
        auto const result = map_.insert( std::make_pair( accession, taxon ));
        if( !result.second ) {
            if( result.first->second != taxon ) {
                throw std::runtime_error(
                    "Duplicate entry for accession '" + accession + "' in lookup table"
                );
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
        auto const result = map_.insert( std::make_pair( accession, taxon ));
        if( !result.second ) {
            if( result.first->second != taxon ) {
                throw std::runtime_error(
                    "Duplicate entry for accession '" + accession + "' in lookup table"
                );
            }
        }
    }

    #endif

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
    //     Data Members
    // -------------------------------------------------------------------------

private:

    MapType map_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
