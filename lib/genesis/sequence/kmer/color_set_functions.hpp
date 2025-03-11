#ifndef GENESIS_SEQUENCE_KMER_COLOR_SET_FUNCTIONS_H_
#define GENESIS_SEQUENCE_KMER_COLOR_SET_FUNCTIONS_H_

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

#include "genesis/sequence/kmer/color_set.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"

#include <array>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Color Set Initialization
// =================================================================================================

void add_secondary_colors_with_binary_reduction(
    KmerColorSet& cset
);

void add_secondary_colors_from_bitvectors(
    KmerColorSet& cset,
    std::vector<utils::Bitvector> const& bitvecs,
    bool test_for_all_set_color = true
);

void add_secondary_colors_from_groups(
    KmerColorSet& cset,
    std::vector<std::vector<size_t>> const& groups,
    bool test_for_all_set_color = true
);

template <typename T>
void add_secondary_colors_from_hac(
    KmerColorSet& cset,
    utils::HierarchicalAgglomerativeClustering<T> const& hac,
    bool test_for_all_set_color = true
) {
    // The HAC produces a new merged cluster in each merging step.
    // Hence, their difference is the number of original observations in the clustering.
    auto const hac_observations = hac.clusters().size() - hac.mergers().size();

    // Starting conditions. We assume that no early deactivation via keep_active_function()
    // was used in the HAC though, such that all observations are clustered into one tree.
    if( cset.get_element_count() != hac_observations ) {
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
        cset.add_merged_color(
            1 + merger.cluster_index_a,
            1 + merger.cluster_index_b
        );
    }

    // The last merger we added should have led to an entry of all primary colors,
    // such that we have at least one hit when searching for imaginary colors.
    if( test_for_all_set_color && ! utils::all_set( cset.get_color_list().back().elements )) {
        throw std::runtime_error(
            "Invalid Hierarchical Agglomerative Clustering for initializing secondary colors "
            "of a Kmer Color Set, as the last merger does not comprise all observerations."
        );
    }
}

void add_secondary_colors_from_phylogeny()
{
    // TODO
}

void add_secondary_colors_from_taxonomy()
{
    // TODO
}

// =================================================================================================
//     Color Set Functions
// =================================================================================================

size_t count_unique_lookup_keys( KmerColorSet const& cset );
void verify_unique_colors( KmerColorSet const& cset );

std::string print_kmer_color_list( KmerColorSet const& cset );
std::string print_kmer_color_lookup( KmerColorSet const& cset );
std::string print_kmer_color_gamut( KmerColorSet const& cset );
std::string print_kmer_color_set_summary( KmerColorSet const& cset );

} // namespace sequence
} // namespace genesis

#endif // include guard
