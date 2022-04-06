#ifndef GENESIS_POPULATION_FUNCTIONS_FILTER_TRANSFORM_H_
#define GENESIS_POPULATION_FUNCTIONS_FILTER_TRANSFORM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/functions/genome_region.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/variant.hpp"

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     General Filtering
// =================================================================================================

/**
 * @brief Select how to filter Variant%s based on whether a predicate is true for any or for all
 * of the BaseCounts samples in the Variant.
 */
enum class VariantFilterType
{
    /**
     * @brief The filter returns `true` if _any_ of the BaseCounts samples in the Variant return
     * `true` for a given predicate.
     */
    kAny,

    /**
     * @brief The filter returns `true` if _all_ of the BaseCounts samples in the Variant return
     * `true` for a given predicate.
     */
    kAll
};

/**
 * @brief
 *
 * See status() for details.
 */
bool filter_is_snp(
    Variant const& variant,
    VariantFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
);

// =================================================================================================
//     Filters
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputIterator to filter by a genome region.
 *
 * This function can be used as a filter with VariantInputIterator::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p region (if @p inclusive is `true`, default),
 * or only over Variant%s that are outside of the @p region (if @p inclusive is `false`).
 */
inline std::function<bool(Variant const&)> variant_filter_region(
    GenomeRegion const& region,
    bool inclusive = true
) {
    return [region, inclusive]( Variant const& variant ){
        return (!inclusive) ^ is_covered( region, variant );
    };
}

/**
 * @brief Filter function to be used with VariantInputIterator to filter by a list of genome regions.
 *
 * This function can be used as a filter with VariantInputIterator::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p regions (if @p inclusive is `true`, default),
 * or only over Variant%s that are outside of the @p regions (if @p inclusive is `false`).
 *
 * The parameter @p copy_regions is an optimization. By default, the function stores a copy of the
 * @p regions, in order to make sure that it is available. However, if it is guaranteed that
 * the @p regions object stays in scope during the VariantInputIterator's livetime, this copy
 * can be avoided.
 */
inline std::function<bool(Variant const&)> variant_filter_region(
    GenomeRegionList const& regions,
    bool inclusive = true,
    bool copy_regions = true
) {
    if( copy_regions ) {
        return [regions, inclusive]( Variant const& variant ){
            return (!inclusive) ^ is_covered( regions, variant );
        };
    } else {
        return [&, inclusive]( Variant const& variant ){
            return (!inclusive) ^ is_covered( regions, variant );
        };
    }
}

// TODO

// for all and per sample?!
// we need an NA entry for samples! all zeroes?!
//
// --> total sum, and per sample.
// better: option: all of them have to be good, or at least one of them has to be good
//
// variant_filter_min_coverage
// variant_filter_max_coverage
// variant_filter_min_max_coverage
//
// variant_filter_min_frequency
// variant_filter_max_frequency
// variant_filter_min_max_frequency
//
// is snp
//
// is biallelic

// =================================================================================================
//     Filtering by count
// =================================================================================================

// bool filter_by_min_maf_count( BaseCounts const& sample, size_t min_count );
//
// bool filter_by_min_maf_count( Variant const& variant, size_t min_count, VariantFilterType type );
//
// bool filter_by_max_count( BaseCounts const& sample, size_t max_count );
//
// bool filter_by_max_count( Variant const& variant, size_t max_count, VariantFilterType type );

// =================================================================================================
//     Transformations
// =================================================================================================

// bascially, all of the above filters, but as transforms that set stuff to 0 intead of filtering
//
// inline std::functiom<void(Variant&)> variant_transform_min_counts( size_t min_count )
// {
//     return [min_count]( Variant& variant ){
//         for( auto& sample : variant.samples ) {
//             transform_min_count( sample, min_count );
//             --> add this function for variants as well first, and use this
//             (basically just a loop over the other one)
//             -->> also make this for max and min max, and use these.
//
//             --->> then, these already have the function signature that is needed for the iterator~
//             no need to do a lambda that just calles it!
//             --> ah no, because we need to capture the min count setting....
//         }
//     };
// }
//
// min count to 0
// max count to 0
// min max count to 0

// =================================================================================================
//     Transforming by count
// =================================================================================================

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * below the given @p min_count are set to zero.
 */
void transform_by_min_count( BaseCounts& sample, size_t min_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * of its samples to zero if @p min_count is not reached for that nucleotide.
 */
void transform_by_min_count( Variant& variant, size_t min_count );

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p max_count is exceeded for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * above the given @p max_count are set to zero.
 */
void transform_by_max_count( BaseCounts& sample, size_t max_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * of its samples to zero if @p max_count is exceeded for that nucleotide.
 */
void transform_by_max_count( Variant& variant, size_t max_count );

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached or if @p max_count is exceeded for that nucleotide.
 *
 * This is the same as running transform_by_min_count() and transform_by_max_count() individually.
 */
void transform_by_min_max_count( BaseCounts& sample, size_t min_count, size_t max_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`) of its samples
 * to zero if @p min_count is not reached or if @p max_count is exceeded for that nucleotide.
 */
void transform_by_min_max_count( Variant& variant, size_t min_count, size_t max_count );

} // namespace population
} // namespace genesis

#endif // include guard
