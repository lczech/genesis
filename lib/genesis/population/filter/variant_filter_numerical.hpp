#ifndef GENESIS_POPULATION_FILTER_VARIANT_FILTER_NUMERICAL_H_
#define GENESIS_POPULATION_FILTER_VARIANT_FILTER_NUMERICAL_H_

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
#include "genesis/population/filter/sample_counts_filter_numerical.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
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
//     Variant Filter Numerical Params
// =================================================================================================

struct VariantFilterNumericalParams
{
    // -------------------------------------------
    //     Numeric
    // -------------------------------------------

    /**
     * @brief Minimum read depth expected for the whole Variant to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) across all samples is less than
     * the provided value, the Variant is not considered sufficiently covered.
     *
     * @see VariantFilterTag::kBelowMinReadDepth
     */
    size_t min_read_depth = 0;

    /**
     * @brief Maximum read depth expected for the whole Variant to be considered covered.
     *
     * If the provided value is greater than 0, and the sum of nucleotide counts (`A`, `C`, `G`, `T`)
     * across all samples is greater than the provided value, the Variant is filtered out.
     * This is meant to exclude positions with spuriously high read_depth that might reuslt from
     * repeat regions and other spurious data.
     *
     * @see VariantFilterTag::kAboveMaxReadDepth
     */
    size_t max_read_depth = 0;

    /**
     * @brief Maximum number of deletions at a position before being filtered out.
     *
     * If this is set to a value greater than 0, and the sum of deletions at the position
     * (determined by their SampleCounts::d_count) is equal to or greater than this value,
     * the position is filtered out.
     *
     * @see VariantFilterTag::kAboveDeletionsCountLimit
     */
    size_t deletions_count_limit = 0;

    // -------------------------------------------
    //     SNP vs Invariant
    // -------------------------------------------

    /**
     * @brief Filter if the Variant does not have two or more alleles.
     *
     * A Variant is a SNP if at least two of the `A`, `C`, `G`, `T` counts of the merged samples
     * are above zero.
     *
     * @see VariantFilterTag::kNotSnp
     */
    bool only_snps = false;

    /**
     * @brief Filter if the Variant does not have exactly two alleles.
     *
     * This is closely related to only_snps, but filters for Variants where the number of nucleotide
     * counts above zero is exactly two - that is, if there are only reads of two of `A`, `C`,
     * `G`, `T` in the Variant. If snp_min_count or snp_max_count are given, these are also taken
     * into account to determine the SNP status of the position.
     *
     * @see VariantFilterTag::kNotBiallelicSnp
     */
    bool only_biallelic_snps = false;

    /**
     * @brief Minimum count for each nucleotide to be considered a SNP for the whole Variant.
     *
     * If `only_snps` or `only_biallelic_snps` is given, the bases that are considered for
     * that need to have at least `snp_min_count` count.
     *
     * @see VariantFilterTag::kBelowSnpMinCount
     */
    size_t snp_min_count = 0;

    /**
     * @brief Maximum count for each nucleotide to be considered a SNP for the whole Variant.
     *
     * If `only_snps` or `only_biallelic_snps` is given, the bases that are considered for
     * that need to have at most `snp_max_count` count. This is probably not really needed
     * in practice, but included here for completeness.
     *
     * @see VariantFilterTag::kAboveSnpMaxCount
     */
    size_t snp_max_count = 0;

    /**
     * @brief Minimum allele frequency that needs to be achieved.
     *
     * Only applied if `only_snps` or `only_biallelic_snps` is set. Then, the allele frequency at
     * the position is computed based on the counts of two bases:
     * If the Variant has a Variant::reference_base and Variant::alternative_base, those are used.
     * If either the alternative or both bases are missing, the respective bases with the highest
     * counts are used instead. From these, the allele frequency `af` is computed.
     * If `af` or `1.0 - af` is below this snp_min_allele_frequency, the filter is considered failed.
     * This hence only tests for biallelic frequencies.
     *
     * @see VariantFilterTag::kBelowMinAlleleFreq
     */
    double snp_min_allele_frequency = 0.0;
};

// =================================================================================================
//     Variant Filter Numerical Functions
// =================================================================================================

// --------------------------------------------------------------------------------------
//     apply_variant_filter_numerical
// --------------------------------------------------------------------------------------

/**
 * @brief Filter a given Variant based on the numerical properties of the counts.
 *
 * The function applies the filter using the given @p params settings. If any filter fails, the
 * function sets the Variant::status to the corresponding VariantFilterTag value, and increments
 * the counter for the @p stats for the failing filter, both for the first filter that fails.
 * It returns whether any filter failed (in which case, `false` is returned), or all passed (`true`).
 */
bool apply_variant_filter_numerical(
    Variant& variant,
    VariantFilterNumericalParams const& params,
    VariantFilterStats& stats
);

/**
 * @copydoc apply_variant_filter_numerical( Variant&, VariantFilterNumericalParams const&, VariantFilterStats& )
 *
 * This overload simply omits the incrementing of the VariantFilterStats counter.
 */

bool apply_variant_filter_numerical(
    Variant& variant,
    VariantFilterNumericalParams const& params
);

// --------------------------------------------------------------------------------------
//     make_variant_filter_numerical_excluding
// --------------------------------------------------------------------------------------

/**
 * @brief Return a functional to numerically filter Variant%s in an VariantInputStream, excluding
 * the ones that do not pass the filters.
 *
 * The function uses apply_variant_filter_numerical(), which returns `true` or `false` depending
 * on whether the filtering determined that the Variant should be kept. It can hence be used with
 * @link genesis::utils::GenericInputStream::add_transform_filter() GenericInputStream::add_transform_filter()@endlink
 * to exclude positions fully from the stream.
 *
 * See make_variant_filter_numerical_tagging() for an alternative that instead simply sets
 * the Variant::status to an appropriate VariantFilterTag, but does not exclude it from the stream.
 */
inline std::function<bool(Variant&)> make_variant_filter_numerical_excluding(
    VariantFilterNumericalParams const& params
) {
    return [params]( Variant& variant ){
        return apply_variant_filter_numerical( variant, params );
    };
}

/**
 * @copydoc make_variant_filter_numerical_excluding( VariantFilterNumericalParams const& )
 *
 * This overload also includes the statistics of the failing or passing filter.
 */
inline std::function<bool(Variant&)> make_variant_filter_numerical_excluding(
    VariantFilterNumericalParams const& params,
    VariantFilterStats& stats
) {
    return [params, &stats]( Variant& variant ){
        return apply_variant_filter_numerical( variant, params, stats );
    };
}

// --------------------------------------------------------------------------------------
//     make_variant_filter_numerical_tagging
// --------------------------------------------------------------------------------------

/**
 * @brief Return a functional to numerically filter Variant%s in an VariantInputStream, tagging
 * the ones that do not pass the filters.
 *
 * The function uses apply_variant_filter_numerical(), tagging whether the filtering determined
 * that the Variant should be kept. It can hence be used with
 * @link genesis::utils::GenericInputStream::add_transform() GenericInputStream::add_transform()@endlink
 * to mark filtered positions in the stream.
 *
 * See make_variant_filter_numerical_excluding() for an alternative that instead excludes
 * the Variant::status from the stream.
 */
inline std::function<void(Variant&)> make_variant_filter_numerical_tagging(
    VariantFilterNumericalParams const& params
) {
    return [params]( Variant& variant ){
        apply_variant_filter_numerical( variant, params );
    };
}

/**
 * @copydoc make_variant_filter_numerical_tagging( VariantFilterNumericalParams const& )
 *
 * This overload also includes the statistics of the failing or passing filter.
 */
inline std::function<void(Variant&)> make_variant_filter_numerical_tagging(
    VariantFilterNumericalParams const& params,
    VariantFilterStats& stats
) {
    return [params, &stats]( Variant& variant ){
        apply_variant_filter_numerical( variant, params, stats );
    };
}

/**
 * @copydoc make_variant_filter_numerical_tagging( VariantFilterNumericalParams const& )
 *
 * This overload additionally runs apply_sample_counts_filter_numerical() on all samples,
 * i.e., it additionally does the same as make_sample_counts_filter_numerical_tagging().
 * This is meant as a convenience function that just does all the typical numercial filtering at once.
 */
inline std::function<void(Variant&)> make_variant_filter_numerical_tagging(
    SampleCountsFilterNumericalParams const& sample_count_params,
    VariantFilterNumericalParams const& variant_params,
    bool all_need_pass = false
) {
    return [sample_count_params, variant_params, all_need_pass]( Variant& variant ){
        apply_sample_counts_filter_numerical(
            variant, sample_count_params, all_need_pass
        );
        apply_variant_filter_numerical( variant, variant_params );
    };
}

/**
 * @copydoc make_variant_filter_numerical_tagging( VariantFilterNumericalParams const& )
 *
 * This overload additionally runs apply_sample_counts_filter_numerical() on all samples,
 * i.e., it additionally does the same as make_sample_counts_filter_numerical_tagging().
 * This is meant as a convenience function that just does all the typical numercial filtering at
 * once. The Variant filter is also set to fitting non-passing values if
 *
 * This overload also includes the statistics of the failing or passing filter.
 */
inline std::function<void(Variant&)> make_variant_filter_numerical_tagging(
    SampleCountsFilterNumericalParams const& sample_count_params,
    VariantFilterNumericalParams const& variant_params,
    VariantFilterStats& variant_stats,
    SampleCountsFilterStats& sample_count_stats,
    bool all_need_pass = false
) {
    return [variant_params, sample_count_params, &variant_stats, &sample_count_stats, all_need_pass](
        Variant& variant
    ){
        apply_sample_counts_filter_numerical(
            variant, sample_count_params, variant_stats, sample_count_stats, all_need_pass
        );
        apply_variant_filter_numerical( variant, variant_params, variant_stats );
    };
}

} // namespace population
} // namespace genesis

#endif // include guard
