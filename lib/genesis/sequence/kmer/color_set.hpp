#ifndef GENESIS_SEQUENCE_KMER_COLOR_SET_H_
#define GENESIS_SEQUENCE_KMER_COLOR_SET_H_

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
 * @ingroup sequence
 */

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/hac.hpp"
#include "genesis/utils/threading/concurrent_vector_guard.hpp"

// This whole class is only available from C++17 onwards,
// as we are using std::shared_mutex and other features from there.
#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Kmer Color Set
// =================================================================================================

class KmerColorSet
{
public:

    // -------------------------------------------------------------------------
    //     Types and Enums
    // -------------------------------------------------------------------------

    using Bitvector = genesis::utils::Bitvector;

    struct Color
    {
        // For each primary element that we want to color, store a bit indicating
        // if this color comprises the element. Primary colors have exactly one bit set,
        // while secondary colors have all bits of their primary colors set.
        Bitvector elements;

        // In the future, we might want to keep track of how often this color is referenced,
        // such that we can decommission it if it is not longer used.
        // size_t occurrence = 0;
    };

    struct GamutStatistics
    {
        size_t real_color_count = 0;
        size_t imag_color_count = 0;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    // KmerColorSet() = default;

    KmerColorSet( size_t element_count, size_t max_color_count = 0 )
        : element_count_( element_count )
        , max_color_count_( max_color_count )
    {
        if( element_count_ == 0 ) {
            throw std::runtime_error( "Element count has to be greater than zero" );
        }
        init_primary_colors_();
    }

    KmerColorSet( size_t element_count, std::vector<Bitvector>&& secondary_colors )
        : KmerColorSet( element_count, 1 + element_count + secondary_colors.size() )
    {
        // We call the other constructor first, which sets up the primary colors,
        // and initializes the max colors to exactly what we need here:
        // One empty, all primary colors, and the list of secondary colors.
        // That is, with this constructor, we declare to only use the given colors,
        // and immedately use the gamut without ever adding any additional colors.
        for( auto& sec_col : secondary_colors ) {
            add_color( std::move( sec_col ));
        }
    }

    ~KmerColorSet() = default;

    KmerColorSet( KmerColorSet const& ) = delete;
    KmerColorSet( KmerColorSet&& )      = delete;

    KmerColorSet& operator= ( KmerColorSet const& ) = delete;
    KmerColorSet& operator= ( KmerColorSet&& )      = delete;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    void set_on_gamut_start_callback(
        std::function<void()> callback
    ) {
        on_gamut_start_callback_ = callback;
    }

    void set_on_gamut_filled_callback(
        std::function<void()> callback
    ) {
        on_gamut_filled_callback_ = callback;
    }

    // -------------------------------------------------------------------------
    //     Lookup & Modification
    // -------------------------------------------------------------------------

    size_t add_color( Bitvector&& elements );
    size_t add_merged_color( size_t color_index_1, size_t color_index_2 );

    size_t find_existing_color(
        Bitvector const& target_elements
    ) const;

    size_t find_minimal_superset(
        Bitvector const& target_elements
    ) const;

    size_t get_joined_color_index(
        size_t existing_color_index,
        size_t additive_element_index
    );

    // -------------------------------------------------------------------------
    //     Data Access
    // -------------------------------------------------------------------------

    size_t get_element_count() const
    {
        return element_count_;
    }

    size_t get_max_color_count() const
    {
        return max_color_count_;
    }

    Color const& get_color_at( size_t index ) const
    {
        if( index >= colors_.size() ) {
            throw std::invalid_argument(
                "Invalid color index " + std::to_string( index ) +
                " in color list of size " + std::to_string( colors_.size() )
            );
        }
        return colors_[ index ];
    }

    size_t get_color_list_size() const
    {
        // Read access to the colors. We only need the lock if we have not started with the gamut,
        // as after that, the colors are not changing any more.
        // This function does not really need to be optimized that much for realistic use cases,
        // but we use it in the internal benchmarking, and do not want to bias those with this.
        std::shared_lock read_lock( color_mutex_, std::defer_lock );
        if( ! gamut_started_.load( std::memory_order_acquire )) {
            read_lock.lock();
        }
        return colors_.size();
    }

    std::vector<Color> const& get_color_list() const
    {
        return colors_;
    }

    std::unordered_multimap<size_t, size_t> const& get_color_lookup() const
    {
        return lookup_;
    }

    utils::Matrix<size_t> const& get_gamut() const
    {
        return gamut_;
    }

    GamutStatistics get_gamut_statistics() const
    {
        // Copy over the state, so that the caller does not have to deal with atomics.
        GamutStatistics stats;
        stats.real_color_count = gamut_stats_.real_color_count.load();
        stats.imag_color_count = gamut_stats_.imag_color_count.load();
        return stats;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void init_primary_colors_();

    size_t get_joined_color_index_read_(
        size_t     existing_color_index,
        size_t     additive_element_index,
        Bitvector& target_elements,
        size_t&    target_hash
    );

    size_t get_joined_color_index_write_(
        size_t     existing_color_index,
        size_t     additive_element_index,
        Bitvector& target_elements,
        size_t&    target_hash
    );

    size_t find_matching_color_(
        size_t     existing_color_index,
        size_t     additive_element_index,
        Bitvector& target_elements,
        size_t&    target_hash
    ) const;

    void populate_target_color_(
        size_t     existing_color_index,
        size_t     additive_element_index,
        Bitvector& target_elements,
        size_t&    target_hash
    ) const;

    size_t find_existing_color_(
        Bitvector const& target_elements,
        size_t           hash
    ) const;

    size_t add_color_(
        Bitvector&& elements,
        size_t hash
    );

    void init_gamut_();

    size_t get_gamut_entry_(
        size_t existing_color_index,
        size_t additive_element_index
    );

    size_t set_gamut_entry_(
        size_t existing_color_index,
        size_t additive_element_index,
        size_t target_color_index,
        std::atomic<size_t>& stat_counter
    );

    size_t find_minimal_superset_(
        Bitvector const& target_elements
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Keep track of our set sizes
    size_t element_count_;
    size_t max_color_count_;

    // List of all colors (empty, primary, secondary).
    std::vector<Color> colors_;

    // Lookup from the hash of a color's bitvector to its index in the list.
    // Instead of mapping from bitvectors to their color index, we map from their hashes to the index.
    // This avoids having to keep another copy of each bitvector of colors as keys in the lookup.
    // Note that we are using a multimap here, as different colors can have the same hash.
    // Hence, when using this lookup, we need an additional step to identify the correct color,
    // by comparing the pointed-to bitvector with the one we are looking up.
    // This should not be significantly slower than using the bitvectors as hash keys directly,
    // as the hash map would also need to do a final key comparison on top anyway.
    // The only overhead is that the hashmap also needs to do that double check on the key
    // (our color hash) itself, but that's cheap compared to the bitvector comparisons.
    std::unordered_multimap<size_t, size_t> lookup_;

    // For user reporting purposes, we have a callback when the gamut was initialized,
    // so that we can see when in our data processing we have saturated the colors,
    // and one when it is completely filled.
    std::function<void()> on_gamut_start_callback_;
    std::function<void()> on_gamut_filled_callback_;

    // Once we have filled the list of colors up to the max, we freeze it, and only ever return
    // already existing colors upon lookup. For this, for each color, we use the minimal subset that
    // contains that color and each additional element set. This matrix serves as lookup for that.
    // Its rows are the color indices, its columns are the element indices, and its values are
    // the new color indices of the color that is the minimal superset of those two.
    // Many of them will not be exact matches for the color we would want (hence the minimal subset),
    // so in a sense, these are imaginary colors: They cannot be exactly produced.
    utils::Matrix<size_t> gamut_;

    // For debugging and performance assessment, we keep track of statis of the gamut.
    // We use a private atomic variant of this, as we are writing the gamut concurrently.
    struct GamutStatisticsAtomic
    {
        std::atomic<size_t> real_color_count = 0;
        std::atomic<size_t> imag_color_count = 0;
    };
    GamutStatisticsAtomic gamut_stats_;

    // We have a bit of a tricky concurrency situation here. Not only do we want reader/writer
    // shared/exclusive access for the color list and lookup, but also differenciate between
    // the color accumualtion phase and the gamut phase...
    // The color_mutex_ is used for shared/exclusive locking during the color collection phase,
    // such that the color list and lookup can grow under concurrent access.
    // Then, once we have saturated the colors and switch to the gamut, we instead use a more
    // fine-grained locking of the gamut matrix cells instead, using the vector guard.
    mutable std::shared_timed_mutex color_mutex_;
    mutable std::atomic<bool> gamut_started_ = false;
    mutable std::atomic<bool> gamut_filled_ = false;
    mutable utils::ConcurrentVectorGuard gamut_guard_;

};

} // namespace sequence
} // namespace genesis

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17
#endif // include guard
