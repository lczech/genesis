#ifndef GENESIS_POPULATION_FUNCTIONS_FILTER_TRANSFORM_H_
#define GENESIS_POPULATION_FUNCTIONS_FILTER_TRANSFORM_H_

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
#include "genesis/population/functions/functions.hpp"
#include "genesis/population/functions/genome_region.hpp"
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
//     Filter by Region
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputStream to filter by a genome region.
 *
 * This function can be used as a filter with VariantInputStream::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p region (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p region (if @p complement is `true`).
 */
inline std::function<bool(Variant const&)> make_filter_by_region(
    GenomeRegion const& region,
    bool complement = false
) {
    return [region, complement]( Variant const& variant ){
        return complement ^ is_covered( region, variant );
    };
}

/**
 * @brief Filter function to be used with VariantInputStream to filter by a list of genome regions.
 *
 * This function can be used as a filter with VariantInputStream::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p regions (if @p complement is `false`,
 * default), or only over Variant%s that are outside of the @p regions (if @p complement is `true`).
 */
inline std::function<bool(Variant const&)> make_filter_by_region(
    std::shared_ptr<GenomeRegionList> regions,
    bool complement = false
) {
    if( ! regions ) {
        throw std::invalid_argument(
            "Can only used make_filter_by_region() with a valid shared pointer to a GenomeRegionList."
        );
    }
    return [regions, complement]( Variant const& variant ){
        return complement ^ is_covered( *regions, variant );
    };
}

/**
 * @copydoc make_filter_by_region( std::shared_ptr<GenomeRegionList>, bool )
 */
inline std::function<bool(Variant const&)> make_filter_by_region(
    std::shared_ptr<GenomeLocusSet> loci,
    bool complement = false
) {
    if( ! loci ) {
        throw std::invalid_argument(
            "Can only used make_filter_by_region() with a valid shared pointer to a GenomeLocusSet."
        );
    }
    return [loci, complement]( Variant const& variant ){
        return complement ^ is_covered( *loci, variant );
    };
}

// =================================================================================================
//     Transform by Count
// =================================================================================================

// Forward declaration
struct BaseCountsFilterStats;

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
 * Additionally, this overload of the function also writes a statistic: When a count is set to zero,
 * the @p stats value BaseCountsFilterStats::below_min_count is incremented.
 */
void transform_zero_out_by_min_count(
    BaseCounts& sample,
    size_t min_count,
    BaseCountsFilterStats& stats,
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
 * @copydoc transform_zero_out_by_min_count( BaseCounts&, size_t, bool )
 *
 * This overload acts on all Variant::samples in the given @p variant, and also writes a statistic:
 * When a count is set to zero, the @p stats value BaseCountsFilterStats::below_min_count is
 * incremented.
 */
void transform_zero_out_by_min_count(
    Variant& variant,
    size_t min_count,
    BaseCountsFilterStats& stats,
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
 * Additionally, this overload of the function also writes a statistic: When a count is set to zero,
 * the @p stats value BaseCountsFilterStats::below_min_count is incremented.
 */
void transform_zero_out_by_max_count(
    BaseCounts& sample,
    size_t max_count,
    BaseCountsFilterStats& stats,
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

/**
 * @copydoc transform_zero_out_by_max_count( BaseCounts&, size_t, bool )
 *
 * This overload acts on all Variant::samples in the given @p variant, and also writes a statistic:
 * When a count is set to zero, the @p stats value BaseCountsFilterStats::below_min_count is
 * incremented.
 */
void transform_zero_out_by_max_count(
    Variant& variant,
    size_t max_count,
    BaseCountsFilterStats& stats,
    bool also_n_and_d_counts = true
);

// =================================================================================================
//     Filter Base Counts
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Base Counts Filter
// --------------------------------------------------------------------------------------

/**
 * @brief Filter settings to filter and transform BaseCounts.
 *
 * These filters act on a single BaseCounts object, using the filter_base_counts() functions,
 * or the make_filter_base_counts() when using a VariantInputStream.
 *
 * When a filter fails, in addition to reporting this via returning `false` from the filter function,
 * we also change the values in the object by setting counts to 0. This is our way of communication
 * to downstream functions that this sample is filtered out. The reason for this behaviour is that
 * typically, a Variant contains multiple BaseCounts in its Variant::samples vector; hence, when
 * one of those samples is not passing the filters, we might still want to process the others,
 * and so we need a way to signal a failing filter. We could add a bool to BaseCounts indicating
 * whether we want to use or ignore it - but instead, we just reset all counts to 0, and use that
 * as our indicator, with the same effect.
 */
struct BaseCountsFilter
{
    /**
     * @brief If set, samples that fail any filter are reset by setting all counts to zero.
     */
    bool clear_filtered = true;

    // -------------------------------------------
    //     Counts
    // -------------------------------------------

    /**
     * @brief Minimum count for each nucleotide to be considered. All counts below are set to zero.
     */
    size_t min_count = 0;

    /**
     * @brief Maximum count for each nucleotide to be considered. All counts above are set to zero.
     */
    size_t max_count = 0;

    /**
     * @brief Set whether we tolerate BaseCounts%s with a high amount of deletions.
     *
     * If set to `false` (default), we do not tolerate deletions. In that case, if the number of
     * deletions in a sample (given by BaseCounts::d_count) is non-zero and above min_count, the
     * counts will be set to zero, and BaseCountsFilterStats::untolerated_deletion is incremented.
     * (We ignore max_count here, assuming that too many deletions are always bad.)
     */
    bool tolerate_deletions = false;

    // -------------------------------------------
    //     Coverage
    // -------------------------------------------

    /**
     * @brief Minimum coverage expected for a BaseCounts to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) in the reads of a sample is less than
     * the provided value, the BaseCounts is not considered sufficiently covered,
     * and all counts are set to zero.
     */
    size_t min_coverage = 0;

    /**
     * @brief Maximum coverage expected for a BaseCounts to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) in the reads of a sample is greater than
     * the provided value, the BaseCounts is not considered properly covered,
     * and all counts are set to zero.
     */
    size_t max_coverage = 0;

    // -------------------------------------------
    //     SNPs
    // -------------------------------------------

    /**
     * @brief Filter if the sample does not have two or more alleles.
     *
     * A sample is a SNP if at least two of the `A`, `C`, `G`, `T` counts (BaseCounts::a_count,
     * BaseCounts::c_count, BaseCounts::g_count, and BaseCounts::t_count ) are above zero,
     * after testing that they are between min_count and max_count and not deleted.
     */
    bool only_snps = false;

    /**
     * @brief Filter if the sample does not have exactly two alleles.
     *
     * This is closely related to only_snps, but filters for samples where the number of nucleotide
     * counts above zero is exactly two - that is, if there are only reads of two of `A`, `C`,
     * `G`, `T` in the sample.
     */
    bool only_biallelic_snps = false;
};

// --------------------------------------------------------------------------------------
//     Base Counts Filter Stats
// --------------------------------------------------------------------------------------

struct BaseCountsFilterStats
{
    /**
     * @brief Number of BaseCounts objects that passed all filters.
     */
    size_t passed = 0;

    /**
     * @brief Number of nucleotides (`ACGT`) in total across the samples that were below
     * BaseCountsFilter::min_count and hence set to zero.
     */
    size_t below_min_count = 0;

    /**
     * @brief Number of nucleotides (`ACGT`) in total across the samples that were above
     * BaseCountsFilter::max_count and hence set to zero.
     */
    size_t above_max_count = 0;

    /**
     * @brief Number of samples that had zero nucleotide counts, after the min_count and max_count.
     */
    size_t empty = 0;

    /**
    * @brief Number of samples that had too many deletions (above min_count, to be precise).
    * Used when BaseCountsFilter::tolerate_deletions is set.
    */
    size_t untolerated_deletion = 0;

    /**
     * @brief Number of samples whose sum of nucleotides was below BaseCountsFilter::min_coverage,
     * and hence all nucleotide counts were set to zero.
     */
    size_t below_min_coverage = 0;

    /**
     * @brief Number of samples whose sum of nucleotides was above BaseCountsFilter::max_coverage,
     * and hence all nucleotide counts were set to zero.
     */
    size_t above_max_coverage = 0;

    /**
     * @brief Number of samples that were not SNPs, after applying all min_count and max_count
     * filters.
     */
    size_t not_snp = 0;

    /**
     * @brief Number of samples that were not biallelic SNPs, after applying all min_count and
     * max_count filters.
     */
    size_t not_biallelic_snp = 0;
};

/**
 * @brief Reset all counts of a BaseCountsFilterStats @p stats to zero.
 */
void reset( BaseCountsFilterStats& stats );

/**
 * @brief Print a textual representation of the counts collected.
 */
std::ostream& print_base_counts_filter_stats( std::ostream& os, BaseCountsFilterStats const& stats );

/**
 * @brief Print a textual representation of the counts collected.
 */
std::string print_base_counts_filter_stats( BaseCountsFilterStats const& stats );

// --------------------------------------------------------------------------------------
//     filter_base_counts
// --------------------------------------------------------------------------------------

/**
 * @brief Filter one BaseCounts instance.
 *
 * The provided settings are used to check which of the filters need to be applied. If any of them
 * fails, the function returns `false`, and `true` otherwise. See BaseCountsFilter for all
 * settings and filters.
 */
bool filter_base_counts(
    BaseCounts& sample,
    BaseCountsFilter const& filter
);

/**
 * @copydoc filter_base_counts( BaseCounts&, BaseCountsFilter const& )
 *
 * This overload also increments the statistic of the failing or passing filter.
 */
bool filter_base_counts(
    BaseCounts& sample,
    BaseCountsFilter const& filter,
    BaseCountsFilterStats& stats
);

/**
 * @brief Filter all BaseCounts of a given Variant.
 *
 * Simply applies the version of this function for BaseCounts to all Variant::samples.
 * If @p all_need_pass is set, the function returns `true` iff all individual samples passed all
 * filters, and `false` otherwise.
 * If @p all_need_pass is not set, the function returns `true` if any sample passed the filters.
 * In either case, all samples of the @p variant are always processed (no short-circuit, as we want
 * all of them to have the count transformations applied to them).
 */
bool filter_base_counts(
    Variant& variant,
    BaseCountsFilter const& filter,
    bool all_need_pass = false
);

/**
 * @copydoc filter_base_counts( Variant&, BaseCountsFilter const&, bool )
 *
 * This overload also increments the statistic of the failing or passing filter.
 */
bool filter_base_counts(
    Variant& variant,
    BaseCountsFilter const& filter,
    BaseCountsFilterStats& stats,
    bool all_need_pass = false
);

/**
 * @brief Return a functional to transform all BaseCounts samples of a given Variant,
 * so that the functional can be used as a transform with VariantInputStream.
 */
inline std::function<void(Variant&)> make_transform_base_counts(
    BaseCountsFilter const& filter
) {
    return [ filter ]( Variant& variant ){
        filter_base_counts( variant, filter );
    };
}

/**
 * @copydoc make_transform_base_counts( BaseCountsFilter const& )
 *
 * This overload also includes the statistics of the failing or passing filters.
 */
inline std::function<void(Variant&)> make_transform_base_counts(
    BaseCountsFilter const& filter,
    BaseCountsFilterStats& stats
) {
    return [ filter, &stats ]( Variant& variant ){
        filter_base_counts( variant, filter, stats );
    };
}

/**
 * @brief Return a functional to filter all BaseCounts samples of a given Variant,
 * so that the functional can be used as a filter with VariantInputStream.
 */
inline std::function<bool(Variant&)> make_filter_base_counts(
    BaseCountsFilter const& filter,
    bool all_need_pass = false
) {
    return [ filter, all_need_pass ]( Variant& variant ){
        return filter_base_counts( variant, filter, all_need_pass );
    };
}

/**
 * @copydoc make_filter_base_counts( BaseCountsFilter const&, bool )
 *
 * This overload also includes the statistics of the failing or passing filters.
 */
inline std::function<bool(Variant&)> make_filter_base_counts(
    BaseCountsFilter const& filter,
    BaseCountsFilterStats& stats,
    bool all_need_pass = false
) {
    return [ filter, &stats, all_need_pass ]( Variant& variant ){
        return filter_base_counts( variant, filter, stats, all_need_pass );
    };
}

// =================================================================================================
//     Filter Variant
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Variant Filter
// --------------------------------------------------------------------------------------

struct VariantFilter
{
    // -------------------------------------------
    //     Coverage
    // -------------------------------------------

    /**
     * @brief Minimum coverage expected for the whole Variant to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) across all samples is less than
     * the provided value, the Variant is not considered sufficiently covered.
     */
    size_t min_coverage = 0;

    /**
     * @brief Maximum coverage expected for the whole Variant to be considered covered.
     *
     * If the sum of nucleotide counts (`A`, `C`, `G`, `T`) across all samples is greater than
     * the provided value, the Variant is not considered properly covered.
     */
    size_t max_coverage = 0;

    // -------------------------------------------
    //     Counts
    // -------------------------------------------

    /**
     * @brief Minimum count for each nucleotide to be considered a SNP for the whole Variant.
     *
     * If `only_snps` or `only_biallelic_snps` is given, the bases that are considered for
     * that need to have at least `min_count` count. Furthermore, if `tolerate_deletions == false`
     * (default), this min count is used to determine when the number of delections
     * (BaseCounts::d_count) is too high, and hence needs to be filtered out.
     */
    size_t min_count = 0;

    /**
     * @brief Maximum count for each nucleotide to be considered a SNP for the whole Variant.
     *
     * If `only_snps` or `only_biallelic_snps` is given, the bases that are considered for
     * that need to have at most `max_count` count. This is probably not really needed
     * in practice, but included here for completeness.
     */
    size_t max_count = 0;

    /**
     * @brief Set whether we tolerate Variant%s with a high amount of deletions.
     *
     * If set to `false` (default), we do not tolerate deletions. In that case, if the number of
     * deletions in the sum of samples (given by their BaseCounts::d_count) is non-zero and above
     * min_count, the Variant is filtered out, and VariantFilterStats::untolerated_deletion is
     * incremented. (We ignore max_count here, assuming that too many deletions are always bad.)
     */
    bool tolerate_deletions = false;

    // -------------------------------------------
    //     SNPs
    // -------------------------------------------

    /**
     * @brief Filter if the Variant does not have two or more alleles.
     *
     * A Variant is a SNP if at least two of the `A`, `C`, `G`, `T` counts of the merged samples
     * are above zero.
     */
    bool only_snps = false;

    /**
     * @brief Filter if the Variant does not have exactly two alleles.
     *
     * This is closely related to only_snps, but filters for Variants where the number of nucleotide
     * counts above zero is exactly two - that is, if there are only reads of two of `A`, `C`,
     * `G`, `T` in the Variant.
     */
    bool only_biallelic_snps = false;

    // -------------------------------------------
    //     Frequency
    // -------------------------------------------

    /**
     * @brief Minimum allele frequency that needs to be achieved.
     *
     * If the Variant has a Variant::reference_base and Variant::alternative_base, those are used.
     * If either the alternative or both bases are missing, the respective bases with the highest
     * counts are used instead. Then, the allele frequency `af` is computed. If `af` or `1.0 - af`
     * is below this min_frequency, the filter is considered failed.
     */
    double min_frequency = 0.0;
};

// --------------------------------------------------------------------------------------
//     Variant Filter Stats
// --------------------------------------------------------------------------------------

struct VariantFilterStats
{
    /**
     * @brief Number of Variant%s that passed all filters.
     */
    size_t passed = 0;

    /**
     * @brief Number of Variants that had zero nucleotide counts across all samples.
     */
    size_t empty = 0;
    /**
     * @brief Number of Variant%s whose sum of nucleotides was below VariantFilter::min_coverage.
     */
    size_t below_min_coverage = 0;

    /**
     * @brief Number of Variant%s whose sum of nucleotides was above VariantFilter::max_coverage.
     */
    size_t above_max_coverage = 0;

    /**
     * @brief Number of Variant%s that were not SNPs, i.e., that were invariants.
     *
     * This counts the number of Variants that passed the other SNP-related filters, such as
     * min_count and max_count
     */
    size_t not_snp = 0;

    /**
     * @brief Number of Variant%s that were not biallelic SNPs.
     *
     * This counts how many Variants were SNPs but not biallelic. It hence indicates how many
     * Variants were filtered out because of the only_biallelic_snps filter setting.
     */
    size_t not_biallelic_snp = 0;

    /**
     * @brief Number of Variant%s whose sum of nucleotides was below VariantFilter::min_count.
     *
     * That is, those Variant%s would have counted as a SNP if the min_count setting
     * wasn't used. This is hence useful to see how many Variants were filtered out because of
     * that setting.
     *
     * Note though that we do not make a distinction between biallelic and multialleleic SNPs here
     * any more for simplicity. This counts any position that was filtered out for not being a
     * SNP according to the only_snps and/or only_biallelic_snps after considering min_count.
     */
    size_t below_min_count = 0;

    /**
     * @brief Number of Variant%s whose sum of nucleotides was above VariantFilter::max_count.
     *
     * Same as below_min_count, but for the max_count setting instead.
     */
    size_t above_max_count = 0;

    /**
    * @brief Number of Variant%s that had too many deletions (above min_count, to be precise).
    * Used when VariantFilter::tolerate_deletions is set.
    */
    size_t untolerated_deletion = 0;

    /**
     * @brief Number of Variant%s that did not have the minimum frequency.
     */
    size_t not_min_frequency = 0;
};

/**
 * @brief Reset all counts of a VariantFilterStats @p stats to zero.
 */
void reset( VariantFilterStats& stats );

/**
 * @brief Print a textual representation of the counts collected.
 */
std::ostream& print_variant_filter_stats( std::ostream& os, VariantFilterStats const& stats );

/**
 * @brief Print a textual representation of the counts collected.
 */
std::string print_variant_filter_stats( VariantFilterStats const& stats );

// --------------------------------------------------------------------------------------
//     filter_variant
// --------------------------------------------------------------------------------------

/**
 * @brief Filter a given Variant.
 *
 * The function applies the given @p filter settings, increments the @p stats for failing filters
 * for the first filter that fails, and returns whether any filter failed (`false`),
 * or all passed (`true`).
 */
bool filter_variant(
    Variant const& variant,
    VariantFilter const& filter
);

/**
 * @copydoc filter_variant( Variant const&, VariantFilter const& )
 *
 * This overload also increments the statistic of the failing or passing filter.
 */
bool filter_variant(
    Variant const& variant,
    VariantFilter const& filter,
    VariantFilterStats& stats
);

/**
 * @brief Return a functional to filter Variant%s that can be used as a filter
 * with VariantInputStream.
 */
inline std::function<bool(Variant const&)> make_filter_variant(
    VariantFilter const& filter
) {
    return [&]( Variant const& variant ){
        return filter_variant( variant, filter );
    };
}

/**
 * @copydoc make_filter_variant( VariantFilter const& )
 *
 * This overload also includes the statistics of the failing or passing filter.
 */
inline std::function<bool(Variant const&)> make_filter_variant(
    VariantFilter const& filter,
    VariantFilterStats& stats
) {
    return [&]( Variant const& variant ){
        return filter_variant( variant, filter, stats );
    };
}

} // namespace population
} // namespace genesis

#endif // include guard
