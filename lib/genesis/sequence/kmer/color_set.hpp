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
    // -------------------------------------------------------------------------
    //     Types and Enums
    // -------------------------------------------------------------------------

public:

    using Bitvector = genesis::utils::Bitvector;

    enum class ColorType
    {
        kEmpty,
        kPrimary,
        kSecondary,
        kImaginary
    };

    struct Color
    {
        // We keep all colors we have observed in the list. For primary and secondary colors,
        // they are indexing themselves, i.e., the index here is exactly their position
        // in the color list. However, an imaginary color references another color that is
        // a superset of itself (in terms of the bit set), and so we need to store that index
        // here instead.
        // An alternative design would be to store the imaginary colors separately to avoid that,
        // but then we'd need to search both lists when looking up if a color already exists.
        size_t index = 0;

        // For each primary element that we want to color, store a bit indicating
        // if this color comprises the element. Primary colors have exactly one bit set,
        // while secondary colors have all bits of their primary colors set.
        Bitvector elements;

        // Type of the color.
        ColorType type = ColorType::kEmpty;

        // We keep track of how often this color is referenced in total.
        // size_t occurrence = 0;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

public:

    // KmerColorSet() = default;

    KmerColorSet( size_t primary_color_count, size_t max_real_color_count = 0 )
        : primary_color_count_( primary_color_count )
        , max_real_color_count_( max_real_color_count )
    {
        if( primary_color_count_ == 0 ) {
            throw std::runtime_error( "Primary color count has to be greater than zero" );
        }
        init_primary_colors_();
    }

    ~KmerColorSet() = default;

    KmerColorSet( KmerColorSet const& ) = default;
    KmerColorSet( KmerColorSet&& )      = default;

    KmerColorSet& operator= ( KmerColorSet const& ) = default;
    KmerColorSet& operator= ( KmerColorSet&& )      = default;

    // -------------------------------------------------------------------------
    //     Init
    // -------------------------------------------------------------------------

private:

    void init_primary_colors_();

public:

    void init_secondary_colors_with_binary_reduction();
    void init_secondary_colors_from_bitvectors( std::vector<Bitvector> const& bitvecs );
    void init_secondary_colors_from_indices( std::vector<std::vector<size_t>> const& indices );

    template <typename T>
    void init_secondary_colors_from_hac(
        utils::HierarchicalAgglomerativeClustering<T> const& hac
    ) {
        // The HAC produces a new merged cluster in each merging step.
        // Hence, their difference is the number of original observations in the clustering.
        auto const hac_observations = hac.clusters().size() - hac.mergers().size();

        // Starting conditions.
        // We assume that no early deactivation via keep_active_function() was used
        // in the clustering though.
        check_uninitialized_secondary_colors_();
        if( primary_color_count_ != hac_observations ) {
            throw std::invalid_argument(
                "Primary color count in Kmer Color Set does not match "
                "the number of observations in the Hierarchical Agglomerative Clustering."
            );
        }
        if( hac_observations != hac.mergers().size() + 1 ) {
            throw std::invalid_argument(
                "Invalid Hierarchical Agglomerative Clustering for initializing secondary colors "
                "of a Kmer Color Set, as the merging is incomplete with unmerged clusters."
            );
        }

        // We build a reduction of our data by combining primary color bitvectors
        // in the same way that the HAC has merged the observeration clusters.
        for( size_t i = 0; i < hac.mergers().size(); ++i ) {
            auto const& merger = hac.mergers()[i];

            // The i-th merger created a new cluster that is located at the i-th position
            // after the initial clusters of the observations.
            assert( hac_observations + i == merger.cluster_index_p );

            // We simply use the two cluster indices that were merged, and merge
            // our corresponding colors. Due to the empty color, we need an offset of one here.
            merge_colors_and_push_back(
                1 + merger.cluster_index_a,
                1 + merger.cluster_index_b
            );
        }

        // The last merger we added should have led to an entry of all primary colors,
        // such that we have at least one hit when searching for imaginary colors.
        // Nope: Deactivated again - we leave this up to the user, and fail later instead.
        // if( ! all_set( colors_.back().elements )) {
        //     throw std::runtime_error(
        //         "Invalid Hierarchical Agglomerative Clustering for initializing secondary colors "
        //         "of a Kmer Color Set, as the last merger does not comprise all observerations."
        //     );
        // }
        check_max_real_color_count_();
    }

    void init_secondary_colors_from_phylogeny()
    {
        // TODO
    }

    void init_secondary_colors_from_taxonomy()
    {
        // TODO
    }

private:

    void check_uninitialized_secondary_colors_() const;
    void check_max_real_color_count_() const;

    // -------------------------------------------------------------------------
    //     Lookup & Modification
    // -------------------------------------------------------------------------

public:

    size_t lookup_and_update(
        size_t existing_color_index,
        size_t target_element_index
    );

    size_t find_color( Bitvector const& target ) const;

    size_t find_minimal_superset( Bitvector const& target ) const;

    size_t push_back_color( Color&& color );

    size_t merge_colors_and_push_back( size_t color_index_1, size_t color_index_2 );

    // -------------------------------------------------------------------------
    //     Data Access
    // -------------------------------------------------------------------------

public:

    Color const& get_color_at( size_t index ) const
    {
        return colors_.at( index );
    }

    std::vector<Color> const& get_color_list() const
    {
        return colors_;
    }

    std::unordered_multimap<size_t, size_t> const& get_lookup_map() const
    {
        return lookup_;
    }

    size_t get_primary_color_count() const
    {
        return primary_color_count_;
    }

    size_t get_max_real_color_count() const
    {
        return max_real_color_count_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // List of all colors
    std::vector<Color> colors_;

    // Lookup from the hash of a color's bitvector to its index in the list.
    // Instead of mapping from bitvectors to their color index, we map from their hashes to the index.
    // This avoids having to keep another copy of each bitvector as keys in the lookup.
    // Note that we are using a multimap here, as different colors can have the same hash.
    // Hence, when using this lookup, we need an additional step to identify the correct color,
    // by comparing the pointed-to bitvector with the one we are looking up.
    // This should not be significantly slower than using the bitvectors as hash keys directly,
    // as the hash map would also need to do a final key comparison on top anyway.
    // The only overhead is that the hashmap also needs to do that double check on the key
    // (our color hash) itself, but that's cheap compared to the bitvector comparisons.
    std::unordered_multimap<size_t, size_t> lookup_;

    // Keep track of our set sizes
    size_t primary_color_count_;
    size_t max_real_color_count_;

    // For convenience, we also keep track of the ranges of each type of color.
    // The second element is the past-the-end index.
    // std::pair<size_t, size_t> primary_color_range_;
    // std::pair<size_t, size_t> initial_secondary_color_range_;
    // std::pair<size_t, size_t> secondary_color_range_;
    // std::pair<size_t, size_t> imaginary_color_range_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
