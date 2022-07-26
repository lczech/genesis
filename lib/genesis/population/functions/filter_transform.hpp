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
#include "genesis/population/functions/functions.hpp"
#include "genesis/population/functions/genome_region.hpp"
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
//     Filter Helpers
// =================================================================================================

/**
 * @brief Select how Variant filter functions that evaluate properties of the Variant::samples
 * (BaseCounts) objects behave when the filter is not `true` or `false` for all samples.
 */
enum class SampleFilterType
{
    /**
     * @brief The filter returns `true` only if _all_ of the BaseCounts samples in the Variant
     * return `true` for a given predicate. This is logical AND.
     */
    kConjunction,

    /**
     * @brief The filter returns `true` if _any_ of the BaseCounts samples in the Variant return
     * `true` for a given predicate. This is logical OR.
     */
    kDisjunction,

    /**
     * @brief The filter is applied to the merged BaseCounts of all samples in the Variant.
     *
     * In this special case, only one BaseCounts object is subjected to the filter function,
     * and hence no logical compbination of the outcome is needed.
     */
    kMerge
};

// =================================================================================================
//     Filter by Status
// =================================================================================================

/**
 * @brief Filter a Variant based on a @p predicate that is applied to the result of a status()
 * call on the BaseCounts of the @p variant.
 *
 * See status() for details on the data of type BaseCountsStatus that @p predicate can use.
 * This function applies the @p predicate to the BaseCounts samples of the @p variant
 * (or to the merged one, depending on @p type, see also below), and returns whether the filter
 * @p predicate passed or not.
 *
 * Note that different @p type values have a distinct effect here:
 * It might happen that all samples individually pass the @p predicate, but their merged counts
 * do not, or vice versa. Hence, this choice needs to be made depending on downstream needs.
 * For example, if we are filtering for Variants that are SNPs (where there exist at least two
 * counts in `[ACGT]` that are non-zero), individual samples might only have one base count greater
 * than zero, in which case they are not considered to be a SNP.
 * However, if those non-zero counts are not for the same base in all samples, their merged counts
 * will be non-zero for more than one base, and hence considered a SNP.
 */
bool filter_by_status(
    std::function<bool(BaseCountsStatus const&)> predicate,
    Variant const& variant,
    SampleFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
);

/**
 * @copybrief filter_by_status( std::function<bool(BaseCountsStatus const&)>, Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *
 * Same as
 * @link
 *     filter_by_status( std::function<bool(BaseCountsStatus const&)>, Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *     filter_by_status( std::function<...>, Variant const&, ... )
 * @endlink,
 * but returns a callback to be used as a filter, e.g., with VariantInputIterator::add_filter().
 */
inline std::function<bool(Variant const&)> filter_by_status(
    std::function<bool(BaseCountsStatus const&)> predicate,
    SampleFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
) {
    return [=]( Variant const& variant ){
        return filter_by_status(
            predicate, variant, type, min_coverage, max_coverage, min_count, tolerate_deletions
        );
    };
}

/**
 * @brief Filter a Variant based on whether the sample counts are SNPs,
 * that is, more than one count in `[ACGT]` is non-zero.
 *
 * This function checks that the samples are covered (BaseCountsStatus::is_covered) and have more
 * than one non-zero count (BaseCountsStatus::is_snp).
 *
 * See status() for details, and see
 * @link
 *     filter_by_status( std::function<bool(BaseCountsStatus const&)>, Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *     filter_by_status()
 * @endlink
 * for details on the processing, in particular the @p type argument.
 */
inline bool filter_is_snp(
    Variant const& variant,
    SampleFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
) {
    return filter_by_status(
        []( BaseCountsStatus const& stat ){
            return stat.is_covered && stat.is_snp;
        },
        variant, type, min_coverage, max_coverage, min_count, tolerate_deletions
    );
}

/**
 * @copybrief filter_is_snp( Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *
 * Same as
 * @link
 *     filter_is_snp( Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *     filter_is_snp( Variant const&, ... )
 * @endlink,
 * but returns a callback to be used as a filter, e.g., with VariantInputIterator::add_filter().
 */
inline std::function<bool(Variant const&)> filter_is_snp(
    SampleFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
) {
    return [=]( Variant const& variant ){
        return filter_is_snp(
            variant, type, min_coverage, max_coverage, min_count, tolerate_deletions
        );
    };
}

/**
 * @brief Filter a Variant based on whether the sample counts are biallelic SNPs,
 * that is, exactly two base counts in `[ACGT]` are non-zero.
 *
 * Same as
 * @link
 *     filter_is_snp( Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *     filter_is_snp( Variant const&, ... )
 * @endlink,
 * but additionally checks that the SNP is biallelic (BaseCountsStatus::is_biallelic).
 * See there for more details.
 */
inline bool filter_is_biallelic_snp(
    Variant const& variant,
    SampleFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
) {
    return filter_by_status(
        []( BaseCountsStatus const& stat ){
            return stat.is_covered && stat.is_snp && stat.is_biallelic;
        },
        variant, type, min_coverage, max_coverage, min_count, tolerate_deletions
    );
}

/**
 * @copybrief filter_is_biallelic_snp( Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *
 * Same as
 * @link
 *     filter_is_biallelic_snp( Variant const&, SampleFilterType, size_t, size_t, size_t, bool )
 *     filter_is_biallelic_snp( Variant const&, ... )
 * @endlink,
 * but returns a callback to be used as a filter, e.g., with VariantInputIterator::add_filter().
 */
inline std::function<bool(Variant const&)> filter_is_biallelic_snp(
    SampleFilterType type,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
) {
    return [=]( Variant const& variant ){
        return filter_is_biallelic_snp(
            variant, type, min_coverage, max_coverage, min_count, tolerate_deletions
        );
    };
}

// =================================================================================================
//     Filter by Count
// =================================================================================================

// TODO

// variant_filter_min_coverage
// variant_filter_max_coverage
// variant_filter_min_max_coverage
//
// variant_filter_min_frequency
// variant_filter_max_frequency
// variant_filter_min_max_frequency

// bool filter_by_min_maf_count( BaseCounts const& sample, size_t min_count );
//
// bool filter_by_min_maf_count( Variant const& variant, size_t min_count, SampleFilterType type );
//
// bool filter_by_max_count( BaseCounts const& sample, size_t max_count );
//
// bool filter_by_max_count( Variant const& variant, size_t max_count, SampleFilterType type );

// =================================================================================================
//     Filter by Region
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputIterator to filter by a genome region.
 *
 * This function can be used as a filter with VariantInputIterator::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p region (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p region (if @p complement is `true`).
 */
inline std::function<bool(Variant const&)> filter_by_region(
    GenomeRegion const& region,
    bool complement = false
) {
    return [region, complement]( Variant const& variant ){
        return complement ^ is_covered( region, variant );
    };
}

/**
 * @brief Filter function to be used with VariantInputIterator to filter by a list of genome regions.
 *
 * This function can be used as a filter with VariantInputIterator::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p regions (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p regions (if @p complement is `true`).
 */
inline std::function<bool(Variant const&)> filter_by_region(
    std::shared_ptr<GenomeRegionList> regions,
    bool complement = false
) {
    if( ! regions ) {
        throw std::invalid_argument(
            "Can only used filter_by_region() with a valid shared pointer to a GenomeRegionList."
        );
    }
    return [regions, complement]( Variant const& variant ){
        return complement ^ is_covered( *regions, variant );
    };
}

/**
 * @copydoc filter_by_region( std::shared_ptr<GenomeRegionList>, bool )
 */
inline std::function<bool(Variant const&)> filter_by_region(
    std::shared_ptr<GenomeLocusSet> loci,
    bool complement = false
) {
    if( ! loci ) {
        throw std::invalid_argument(
            "Can only used filter_by_region() with a valid shared pointer to a GenomeLocusSet."
        );
    }
    return [loci, complement]( Variant const& variant ){
        return complement ^ is_covered( *loci, variant );
    };
}

// /**
//  * @copydoc filter_by_region( std::shared_ptr<GenomeRegionList>, bool )
//  *
//  * This version of the function can be used if the @p regions is not given as a `std::shared_ptr`.
//  * The parameter @p copy_regions is an optimization. By default, the function stores a copy of the
//  * @p regions, in order to make sure that it is available. However, if it is guaranteed that
//  * the @p regions object stays in scope during the VariantInputIterator's lifetime, this copy
//  * can be avoided.
//  */
// inline std::function<bool(Variant const&)> filter_by_region(
//     GenomeRegionList const& regions,
//     bool complement = false,
//     bool copy_regions = false
// ) {
//     if( copy_regions ) {
//         return [regions, complement]( Variant const& variant ){
//             return complement ^ is_covered( regions, variant );
//         };
//     } else {
//         return [&regions, complement]( Variant const& variant ){
//             return complement ^ is_covered( regions, variant );
//         };
//     }
// }

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
//     Transform by Count
// =================================================================================================

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * below the given @p min_count are set to zero.
 */
void transform_zero_out_by_min_count( BaseCounts& sample, size_t min_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * of its samples to zero if @p min_count is not reached for that nucleotide.
 */
void transform_zero_out_by_min_count( Variant& variant, size_t min_count );

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p max_count is exceeded for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * above the given @p max_count are set to zero.
 */
void transform_zero_out_by_max_count( BaseCounts& sample, size_t max_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * of its samples to zero if @p max_count is exceeded for that nucleotide.
 */
void transform_zero_out_by_max_count( Variant& variant, size_t max_count );

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached or if @p max_count is exceeded for that nucleotide.
 *
 * This is the same as running transform_zero_out_by_min_count() and
 * transform_zero_out_by_max_count() individually.
 */
void transform_zero_out_by_min_max_count( BaseCounts& sample, size_t min_count, size_t max_count );

/**
 * @brief Transform a @p variant by setting any nucleotide count (`A`, `C`, `G`, `T`) of its samples
 * to zero if @p min_count is not reached or if @p max_count is exceeded for that nucleotide.
 */
void transform_zero_out_by_min_max_count( Variant& variant, size_t min_count, size_t max_count );

} // namespace population
} // namespace genesis

#endif // include guard
