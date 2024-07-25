#ifndef GENESIS_POPULATION_FILTER_SAMPLE_COUNTS_FILTER_POSITIONAL_H_
#define GENESIS_POPULATION_FILTER_SAMPLE_COUNTS_FILTER_POSITIONAL_H_

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
 * @ingroup population
 */

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/function/genome_region.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/variant.hpp"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Filter by region, tagging
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputStream on a Variant to filter its SampleCounts
 * by genome regions, by tagging non-covered positions with the given @p tag.
 *
 * This function is similar to make_variant_filter_by_region_tagging(), but instead of setting the
 * status of the whole Variant, it applies per-sample filters instead, and sets their status flags.
 * The function expects a set of GenomeLocusSet or GenomeRegionList pointers to be given, one for
 * each sample of the Variant. The template parameter GenomeMaskType allows either of those two
 * mask types to be used.
 */
template<class GenomeMaskType>
inline std::function<void(Variant&)> make_sample_counts_filter_by_region_tagging(
    std::vector<std::shared_ptr<GenomeMaskType>> const& sample_masks,
    SampleCountsFilterTag tag,
    bool complement = false
) {
    // Only allow mask filter tags here.
    if(
        tag != SampleCountsFilterTag::kMaskedPosition &&
        tag != SampleCountsFilterTag::kMaskedRegion
    ) {
        throw std::invalid_argument(
            "Can only use SampleCountsFilterTag::kMaskedPosition or "
            "SampleCountsFilterTag::kMaskedRegion as tags for "
            "make_sample_counts_filter_by_region_tagging()."
        );
    }

    // Make a filter function that can be applied as a transformation to a VariantInputStream.
    // This makes a copy of the masks as well, so that the shared pointers stay alive.
    return [sample_masks, tag, complement]( Variant& variant )
    {
        // Make sure that there is a region list for each sample, and that the Variant is passing.
        // We need to check this in every call of this function, in case that the Variants are
        // of different sizes (if so, for some buggy reasons probably).
        if( sample_masks.size() != variant.samples.size() ) {
            throw std::invalid_argument(
                "Inconsistent number of samples, with make_sample_counts_filter_by_region_tagging() "
                "using " + std::to_string( sample_masks.size() ) + " sample masks, but Variant "
                "has " + std::to_string( variant.samples.size() ) + " samples present."
            );
        }
        if( ! variant.status.passing() ) {
            return;
        }

        // Apply all per-sample masks. There might be some nullptr masks there,
        // if particular samples do not have a mask assigned, so we check and skip then.
        for( size_t i = 0; i < sample_masks.size(); ++i ) {
            if( ! sample_masks[i] ) {
                continue;
            }
            if( ! variant.samples[i].status.passing() ) {
                continue;
            }
            auto const& regions = *sample_masks[i];
            auto const keep = complement ^ is_covered( regions, variant );
            if( ! keep ) {
                variant.samples[i].status.set( tag );
            }
        }
    };
}

} // namespace population
} // namespace genesis

#endif // include guard
