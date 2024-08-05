#ifndef GENESIS_POPULATION_FILTER_VARIANT_FILTER_POSITIONAL_H_
#define GENESIS_POPULATION_FILTER_VARIANT_FILTER_POSITIONAL_H_

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/genome_region.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/variant.hpp"

#include <functional>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Filter by region, excluding
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputStream to filter by a genome region,
 * by excluding non-covered positions from the stream.
 *
 * This function can be used as a filter with VariantInputStream::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p region (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p region (if @p complement is `true`).
 *
 * @see make_variant_filter_by_region_tagging()
 */
inline std::function<bool(Variant const&)> make_variant_filter_by_region_excluding(
    GenomeRegion const& region,
    bool complement = false
) {
    return [region, complement]( Variant const& variant ){
        return complement ^ is_covered( region, variant );
    };
}

/**
 * @brief Filter function to be used with VariantInputStream to filter by a list of genome regions,
 * by excluding non-covered positions from the stream.
 *
 * This function can be used as a filter with VariantInputStream::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p regions (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p regions (if @p complement is `true`).
 *
 * @see make_variant_filter_by_region_tagging()
 */
inline std::function<bool(Variant const&)> make_variant_filter_by_region_excluding(
    std::shared_ptr<GenomeRegionList> regions,
    bool complement = false
) {
    if( ! regions ) {
        throw std::invalid_argument(
            "Can only used make_variant_filter_by_region_excluding() "
            "with a valid shared pointer to a GenomeRegionList."
        );
    }
    return [regions, complement]( Variant const& variant ){
        return complement ^ is_covered( *regions, variant );
    };
}

/**
 * @copydoc make_variant_filter_by_region_excluding( std::shared_ptr<GenomeRegionList>, bool )
 */
inline std::function<bool(Variant const&)> make_variant_filter_by_region_excluding(
    std::shared_ptr<GenomeLocusSet> loci,
    bool complement = false
) {
    if( ! loci ) {
        throw std::invalid_argument(
            "Can only used make_variant_filter_by_region_excluding() "
            "with a valid shared pointer to a GenomeLocusSet."
        );
    }
    return [loci, complement]( Variant const& variant ){
        return complement ^ is_covered( *loci, variant );
    };
}

// =================================================================================================
//     Filter by region, tagging
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputStream to filter by a genome region,
 * by tagging non-covered positions with the given @p tag.
 *
 * This function can be used as a filter with VariantInputStream::add_transform(), in order
 * to only iterate over Variant%s that are in the given @p region (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p region (if @p complement is `true`).
 *
 * The two tag options are VariantFilterTag::kMaskedPosition and VariantFilterTag::kMaskedRegion,
 * which we check, in order to avoid accidental mistakes. We distinguish between those in the sense
 * that a masked region is meant to be a larger part, where only certain chromosomes or genes are
 * not masked, while a masked position is meant to be a finer scale, that can be decided per
 * position, such as to mark synonymous vs non-synonymous SNPs.
 *
 * @see make_variant_filter_by_region_excluding()
 */
inline std::function<void(Variant&)> make_variant_filter_by_region_tagging(
    GenomeRegion const& region,
    VariantFilterTag tag,
    bool complement = false
) {
    if( tag != VariantFilterTag::kMaskedPosition && tag != VariantFilterTag::kMaskedRegion ) {
        throw std::invalid_argument(
            "Can only use VariantFilterTag::kMaskedPosition or VariantFilterTag::kMaskedRegion "
            "as tags for make_variant_filter_by_region_tagging()."
        );
    }
    return [region, tag, complement]( Variant& variant )
    {
        if( ! variant.status.passing() ) {
            return;
        }
        auto const keep = complement ^ is_covered( region, variant );
        if( ! keep ) {
            variant.status.set( tag );
        }
    };
}

/**
 * @brief Filter function to be used with VariantInputStream to filter by a list of genome regions,
 * by tagging non-covered positions with the given @p tag.
 *
 * This function can be used as a filter with VariantInputStream::add_transform(), in order
 * to only iterate over Variant%s that are in the given @p regions (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p regions (if @p complement is `true`).
 *
 * The two tag options are VariantFilterTag::kMaskedPosition and VariantFilterTag::kMaskedRegion,
 * which we check, in order to avoid accidental mistakes. We distinguish between those in the sense
 * that a masked region is meant to be a larger part, where only certain chromosomes or genes are
 * not masked, while a masked position is meant to be a finer scale, that can be decided per
 * position, such as to mark synonymous vs non-synonymous SNPs.
 *
 * @see make_variant_filter_by_region_excluding()
 */
inline std::function<void(Variant&)> make_variant_filter_by_region_tagging(
    std::shared_ptr<GenomeRegionList> regions,
    VariantFilterTag tag,
    bool complement = false
) {
    if( tag != VariantFilterTag::kMaskedPosition && tag != VariantFilterTag::kMaskedRegion ) {
        throw std::invalid_argument(
            "Can only use VariantFilterTag::kMaskedPosition or VariantFilterTag::kMaskedRegion "
            "as tags for make_variant_filter_by_region_tagging()."
        );
    }
    if( ! regions ) {
        throw std::invalid_argument(
            "Can only used make_variant_filter_by_region_tagging() "
            "with a valid shared pointer to a GenomeRegionList."
        );
    }
    return [regions, tag, complement]( Variant& variant )
    {
        if( ! variant.status.passing() ) {
            return;
        }
        auto const keep = complement ^ is_covered( *regions, variant );
        if( ! keep ) {
            variant.status.set( tag );
        }
    };
}

/**
 * @copydoc make_variant_filter_by_region_tagging( std::shared_ptr<GenomeRegionList>, VariantFilterTag, bool )
 */
inline std::function<void(Variant&)> make_variant_filter_by_region_tagging(
    std::shared_ptr<GenomeLocusSet> loci,
    VariantFilterTag tag,
    bool complement = false
) {
    if( tag != VariantFilterTag::kMaskedPosition && tag != VariantFilterTag::kMaskedRegion ) {
        throw std::invalid_argument(
            "Can only use VariantFilterTag::kMaskedPosition or VariantFilterTag::kMaskedRegion "
            "as tags for make_variant_filter_by_region_tagging()."
        );
    }
    if( ! loci ) {
        throw std::invalid_argument(
            "Can only used make_variant_filter_by_region_tagging() "
            "with a valid shared pointer to a GenomeLocusSet."
        );
    }
    return [loci, tag, complement]( Variant& variant )
    {
        if( ! variant.status.passing() ) {
            return;
        }
        auto const keep = complement ^ is_covered( *loci, variant );
        if( ! keep ) {
            variant.status.set( tag );
        }
    };
}

} // namespace population
} // namespace genesis

#endif // include guard
