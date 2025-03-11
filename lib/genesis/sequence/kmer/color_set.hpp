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
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/hac.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
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

    ~KmerColorSet() = default;

    KmerColorSet( KmerColorSet const& ) = default;
    KmerColorSet( KmerColorSet&& )      = default;

    KmerColorSet& operator= ( KmerColorSet const& ) = default;
    KmerColorSet& operator= ( KmerColorSet&& )      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    void set_on_saturation_callback(
        std::function<void()> callback
    ) {
        on_saturation_callback_ = callback;
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

    GamutStatistics const& get_gamut_statistics() const
    {
        return gamut_stats_;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void init_primary_colors_();

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
    // so that we can see when in our data processing we have saturated the colors.
    std::function<void()> on_saturation_callback_;

    // Once we have filled the list of colors up to the max, we freeze it, and only ever return
    // already existing colors upon lookup. For this, for each color, we use the minimal subset that
    // contains that color and each additional element set. This matrix serves as lookup for that.
    // Its rows are the color indices, its columns are the element indices, and its values are
    // the new color indices of the color that is the minimal superset of those two.
    // Many of them will not be exact matches for the color we would want (hence the minimal subset),
    // so in a sense, these are imaginary colors: They cannot be exactly produced.
    utils::Matrix<size_t> gamut_;

    // For debugging and performance assessment, we keep track of statis of the gamut.
    GamutStatistics gamut_stats_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
