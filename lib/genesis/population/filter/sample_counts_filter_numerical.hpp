#ifndef GENESIS_POPULATION_FILTER_SAMPLE_COUNTS_FILTER_NUMERICAL_H_
#define GENESIS_POPULATION_FILTER_SAMPLE_COUNTS_FILTER_NUMERICAL_H_

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"

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
//     Transform by Count
// =================================================================================================

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p min_count is not reached for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * below the given @p min_count are set to zero.
 *
 * If @p also_n_and_d_counts is set (default), this filtering is also done for BaseCounts::n_count
 * and BaseCounts::d_count, although they are not taken into account in the statistics.
 */
void transform_zero_out_by_min_count(
    BaseCounts& sample,
    size_t min_count,
    bool also_n_and_d_counts = true
);

/**
 * @copydoc transform_zero_out_by_min_count( BaseCounts&, size_t, bool )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void transform_zero_out_by_min_count(
    Variant& variant,
    size_t min_count,
    bool also_n_and_d_counts = true
);

/**
 * @brief Transform a BaseCounts @p sample by setting any nucleotide count (`A`, `C`, `G`, `T`)
 * to zero if @p max_count is exceeded for that nucleotide.
 *
 * This transformation is used as a type of quality control. All nucleotide counts (that is,
 * BaseCounts::a_count, BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count) that are
 * above the given @p max_count are set to zero.
 *
 * If @p also_n_and_d_counts is set (default), this filtering is also done for BaseCounts::n_count
 * and BaseCounts::d_count, although they are not taken into account in the statistics.
 */
void transform_zero_out_by_max_count(
    BaseCounts& sample,
    size_t max_count,
    bool also_n_and_d_counts = true
);

/**
 * @copydoc transform_zero_out_by_max_count( BaseCounts&, size_t, bool )
 *
 * This overload acts on all Variant::samples in the given @p variant.
 */
void transform_zero_out_by_max_count(
    Variant& variant,
    size_t max_count,
    bool also_n_and_d_counts = true
);

// =================================================================================================
//     Sample Counts Filter Numerical Params
// =================================================================================================

/**
 * @brief Filter settings to filter and transform BaseCounts.
 *
 * These filters act on a single BaseCounts object, using the apply_sample_counts_filter_numerical()
 * functions, or the make_sample_counts_filter_numerical_tagging() functions.
 *
 * When a filter fails, in addition to reporting this via returning `false` from the filter function,
 * we also set the BaseCount::status accordingly to indicate the failure. This way, downstream
 * functions can ignore this sample as being filtered out.
 */
struct BaseCountsFilterNumericalParams
{
    // -------------------------------------------
    //     Numeric
    // -------------------------------------------

    /**
     * @brief Minimum count for each nucleotide to be considered. All counts below are set to zero.
     *
     * This filter merely sets counts to zero, but does not effect the filter result.
     */
    size_t min_count = 0;

    /**
     * @brief Maximum count for each nucleotide to be considered. All counts above are set to zero.
     *
     * This filter merely sets counts to zero, but does not effect the filter result.
     */
    size_t max_count = 0;

    /**
     * @brief Minimum coverage expected for a BaseCounts to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) in the reads of a sample is less than
     * the provided value, the BaseCounts is not considered sufficiently covered,
     * and all counts are set to zero.
     *
     * @see BaseCountsFilterTag::kBelowMinCoverage
     */
    size_t min_coverage = 0;

    /**
     * @brief Maximum coverage expected for a BaseCounts to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) in the reads of a sample is greater than
     * the provided value, the BaseCounts is not considered properly covered,
     * and all counts are set to zero.
     *
     * @see BaseCountsFilterTag::kAboveMaxCoverage
     */
    size_t max_coverage = 0;

    /**
     * @brief Maximum number of deletions at a position before being filtered out.
     *
     * If this is set to a value greater than 0, and the number of deletions at the position
     * (determined by the BaseCounts::d_count) is equal to or greater than this value,
     * the sample is filtered out by setting its status accordingly.
     *
     * @see SampleCountsFilterTag::kAboveDeletionsCountLimit
     */
    size_t deletions_count_limit = 0;

    // -------------------------------------------
    //     SNP vs Invariant
    // -------------------------------------------

    /**
     * @brief Filter if the sample does not have two or more alleles.
     *
     * A sample is a SNP if at least two of the `A`, `C`, `G`, `T` counts (BaseCounts::a_count,
     * BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count ) are above zero,
     * after testing that they are between min_count and max_count and not deleted.
     *
     * @see BaseCountsFilterTag::kNotSnp
     */
    bool only_snps = false;

    /**
     * @brief Filter if the sample does not have exactly two alleles.
     *
     * This is closely related to only_snps, but filters for samples where the number of nucleotide
     * counts above zero is exactly two - that is, if there are only reads of two of `A`, `C`,
     * `G`, `T` in the sample.
     *
     * @see BaseCountsFilterTag::kNotBiallelicSnp
     */
    bool only_biallelic_snps = false;
};

// =================================================================================================
//     Sample Counts Filter Numerical Functions
// =================================================================================================

// --------------------------------------------------------------------------------------
//     apply_sample_counts_filter_numerical
// --------------------------------------------------------------------------------------

/**
 * @brief Filter a given BaseCounts based on the numerical properties of the counts.
 *
 * The function applies the filter using the given @p params settings. If any filter fails, the
 * function sets the BaseCounts::status to the corresponding BaseCountsFilterTag value, and increments
 * the counter for the @p stats for the failing filter, both for the first filter that fails.
 * It returns whether any filter failed (in which case, `false` is returned), or all passed (`true`).
 */
bool apply_sample_counts_filter_numerical(
    BaseCounts& sample,
    BaseCountsFilterNumericalParams const& params,
    BaseCountsFilterStats& stats
);

/**
 * @copydoc apply_sample_counts_filter_numerical( BaseCounts&, BaseCountsFilterNumericalParams const&, BaseCountsFilterStats& )
 *
 * This overload simply omits the incrementing of the BaseCountsFilterStats counter.
 */
bool apply_sample_counts_filter_numerical(
    BaseCounts& sample,
    BaseCountsFilterNumericalParams const& params
);

/**
 * @brief Filter a given BaseCounts based on the numerical properties of the counts.
 *
 * This function applies the version of this function for BaseCounts to all Variant::samples.
 * If @p all_need_pass is set, the function returns `true` iff all individual samples passed all
 * filters, and `false` otherwise, and sets the Variant::status to
 * VariantFilterTag::kNotAllSamplesPassed. If @p all_need_pass is not set, the function returns
 * `true` if any sample passed the filters. In either case, all samples of the @p variant are always
 * processed (no short-circuit, as we want all of them to have the count transformations applied to
 * them). If _all_ of them fail the filter settings, the Variant::status is set to
 * VariantFilterTag::kNoSamplePassed, independently of @p all_need_pass.
 */
bool apply_sample_counts_filter_numerical(
    Variant& variant,
    BaseCountsFilterNumericalParams const& params,
    VariantFilterStats& variant_stats,
    BaseCountsFilterStats& sample_count_stats,
    bool all_need_pass = false
);

/**
 * @copydoc apply_sample_counts_filter_numerical( Variant&, BaseCountsFilterNumericalParams const&, BaseCountsFilterStats&, bool )
 *
 * This overload simply omits the incrementing of the BaseCountsFilterStats counter.
 */
bool apply_sample_counts_filter_numerical(
    Variant& variant,
    BaseCountsFilterNumericalParams const& params,
    bool all_need_pass = false
);

// --------------------------------------------------------------------------------------
//     make_sample_counts_filter_numerical
// --------------------------------------------------------------------------------------

/**
 * @brief Return a functional to numerically filter the BaseCount samples in a Variant  tagging
 * the ones that do not pass the filters, and potentially tagging the Variant.
 *
 * The function uses apply_sample_counts_filter_numerical(), modifying the samples, and tagging
 * whether the filtering determined that the samples should be kept. It can hence be used with
 * @link genesis::utils::GenericInputStream::add_transform() GenericInputStream::add_transform()@endlink
 * to mark filtered positions in the stream. alternative that instead excludes
 * the Variant::status from the stream.
 */
inline std::function<void(Variant&)> make_sample_counts_filter_numerical_tagging(
    BaseCountsFilterNumericalParams const& params,
    bool all_need_pass = false
) {
    return [params, all_need_pass]( Variant& variant ){
        apply_sample_counts_filter_numerical( variant, params, all_need_pass );
    };
}

/**
 * @copydoc make_sample_counts_filter_numerical_tagging( BaseCountsFilterNumericalParams const& )
 *
 * This overload also includes the statistics of the failing or passing filter.
 */
inline std::function<void(Variant&)> make_sample_counts_filter_numerical_tagging(
    BaseCountsFilterNumericalParams const& params,
    VariantFilterStats& variant_stats,
    BaseCountsFilterStats& sample_count_stats,
    bool all_need_pass = false
) {
    return [params, &variant_stats, &sample_count_stats, all_need_pass]( Variant& variant ){
        apply_sample_counts_filter_numerical(
            variant, params, variant_stats, sample_count_stats, all_need_pass
        );
    };
}

} // namespace population
} // namespace genesis

#endif // include guard
