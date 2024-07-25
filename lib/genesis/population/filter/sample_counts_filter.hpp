#ifndef GENESIS_POPULATION_FILTER_SAMPLE_COUNTS_FILTER_H_
#define GENESIS_POPULATION_FILTER_SAMPLE_COUNTS_FILTER_H_

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
#include "genesis/population/filter/filter_stats.hpp"
#include "genesis/population/filter/filter_status.hpp"
#include "genesis/population/variant.hpp"

#include <array>
#include <cassert>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     SampleCounts Filter Tag
// =================================================================================================

enum class SampleCountsFilterTag : FilterStatus::IntType
{
    /**
     * @brief Sample has passed all filters.
     */
    kPassed = 0,

    // -------------------------------------------
    //     Position
    // -------------------------------------------

    /**
     * @brief Position has been masked out from processing.
     *
     * This can be due to, e.g., via a RegionLocus set from a fasta file, see read_mask_fasta().
     * We distinguish this from kMaskedRegion purely for semantic reasons. Both filters are due to
     * some user-specified position-based filter, and created by similar functions. However, we
     * generally mean to indicate that a masked position is due to some fine-grained positional
     * filter, while masked regions are meant to indicate filters for larger regions such as
     * chromsosomes or genes.
     */
    kMaskedPosition,

    /**
     * @brief Position is part of a masked region.
     *
     * See kMaskedPosition for details on the distrinction between the two.
     */
    kMaskedRegion,

    // -------------------------------------------
    //     Missing and Invalid
    // -------------------------------------------

    /**
    * @brief Position is missing in the input data.
    */
    kMissing,

    /**
     * @brief Generic indicator that the sample has not passed a filter.
     *
     * Not used at the moment internally, but included here as a simple catch-all value if no
     * further distrinction for the filter that failed is needed.
     */
    kNotPassed,

    /**
     * @brief Generic indicator that the sample is invalid.
     *
     * Not used at the moment internally. Similar to kNotPassed, this is a generic value for
     * invalid samples.
     */
    kInvalid,

    // -------------------------------------------
    //     Numeric
    // -------------------------------------------

    /**
     * @brief Zero nucleotide counts, after zeroing out counts based on the min_count and max_count.
     */
    kEmpty,

    /**
     * @brief Sum of counts across all nucleotide counts is below the min read depth threshold.
     *
     * @see SampleCountsFilterNumericalParams::min_read_depth
     */
    kBelowMinReadDepth,

    /**
     * @brief Sum of counts across all nucleotide counts is above the max read depth threshold.
     *
     * @see SampleCountsFilterNumericalParams::max_read_depth
     */
    kAboveMaxReadDepth,

    /**
     * @brief Too many deletions at the position.
     *
     * @see SampleCountsFilterNumericalParams::deletions_count_limit
     */
    kAboveDeletionsCountLimit,

    /**
     * @brief Invariant position, not a SNP.
     *
     * @see SampleCountsFilterNumericalParams::only_snps
     */
    kNotSnp,

    /**
     * @brief SNP position, but not biallelic, i.e., has more than one alternative.
     *
     * @see SampleCountsFilterNumericalParams::only_biallelic_snps
     */
    kNotBiallelicSnp,

    // -------------------------------------------
    //     Special
    // -------------------------------------------

    kEnd
};

// =================================================================================================
//     SampleCounts Filter Tag Categories
// =================================================================================================

/**
 * @brief List of filter categories for a SampleCounts.
 *
 * We summarize certain filters into categories. This is more useful for users than to have all
 * of the above detail filter tags. Most of the time, we are mostly interested in these categories
 * here; it might not be worth having the above detail tag list in the first place.
 */
enum class SampleCountsFilterTagCategory : FilterStatus::IntType
{
    /**
     * @brief SampleCounts has passed all filters.
     */
    kPassed = 0,

    /**
     * @brief Position is masked.
     */
    kMasked,

    /**
     * @brief Position is missing or otherwise invalid.
     */
    kMissingInvalid,

    /**
     * @brief Any of the numeric variant filters failed.
     */
    kNumeric,

    /**
     * @brief End of the enum values.
     */
    kEnd
};

// =================================================================================================
//     SampleCounts Filter Policy
// =================================================================================================

/**
 * @brief Policy helper to decide how to treat filtered SampleCounts.
 *
 * In several functions where we need to take the SampleCounts samples of a Variant into account,
 * we need to decide on whether we want to use all of them, or only those that are passing.
 * For instance, when merging samples, this is important. This policy allows to select the
 * needed behaviour.
 */
enum class SampleCountsFilterPolicy
{
    kAll,
    kOnlyPassing
};

// =================================================================================================
//     SampleCounts Filter Stats
// =================================================================================================

/**
 * @brief Counts of how many SampleCounts%s with each SampleCountsFilterTag occured in some data.
 */
using SampleCountsFilterStats = FilterStats<SampleCountsFilterTag>;

/**
 * @brief Counts of how many SampleCounts with each SampleCountsFilterTagCategory occured in some data.
 *
 * This is a convenient summary of the SampleCountsFilterStats, where not the full level of detail
 * is needed, for instance for user output.
 */
using SampleCountsFilterCategoryStats = FilterStats<SampleCountsFilterTagCategory>;

/**
 * @brief For a given @p tag, return its category tag.
 */
SampleCountsFilterTagCategory sample_counts_filter_tag_to_category( SampleCountsFilterTag tag );

/**
 * @brief Generate summary counts for a SampleCountsFilterStats counter.
 *
 * The given @p stats contain counts for different reasons of filters that could have failed when
 * filtering a SampleCounts. This function summarizes those stats into three basic categories,
 * and gives their sums.
 *
 * This is meant as a broad summary, for instance for user output, where it might not be overly
 * relevant which exact numerical filter got triggered how often by a particular filter, but rather
 * we want to have an overview of which classes or categories of filters got triggered how often.
 */
SampleCountsFilterCategoryStats sample_counts_filter_stats_category_counts(
    SampleCountsFilterStats const& stats
);

/**
 * @brief Overload that only reports back a single category sum of the filter stats.
 */
size_t sample_counts_filter_stats_category_counts(
    SampleCountsFilterStats const& stats, SampleCountsFilterTagCategory category
);

// =================================================================================================
//     Filter Functions
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Print sample stats
// --------------------------------------------------------------------------------------

/**
 * @brief Print a textual representation of the counts collected.
 */
std::ostream& print_sample_counts_filter_stats(
    std::ostream& os,
    SampleCountsFilterStats const& stats,
    bool verbose = false
);

/**
 * @brief Print a textual representation of the counts collected.
 */
std::string print_sample_counts_filter_stats(
    SampleCountsFilterStats const& stats,
    bool verbose = false
);

// --------------------------------------------------------------------------------------
//     Print category stats
// --------------------------------------------------------------------------------------

std::ostream& print_sample_counts_filter_category_stats(
    std::ostream& os,
    SampleCountsFilterCategoryStats const& stats,
    bool verbose = false
);

std::string print_sample_counts_filter_category_stats(
    SampleCountsFilterCategoryStats const& stats,
    bool verbose = false
);

inline std::ostream& print_sample_counts_filter_category_stats(
    std::ostream& os,
    SampleCountsFilterStats const& stats,
    bool verbose = false
) {
    return print_sample_counts_filter_category_stats(
        os, sample_counts_filter_stats_category_counts( stats ), verbose
    );
}

inline std::string print_sample_counts_filter_category_stats(
    SampleCountsFilterStats const& stats,
    bool verbose = false
) {
    return print_sample_counts_filter_category_stats(
        sample_counts_filter_stats_category_counts( stats ), verbose
    );
}

} // namespace population
} // namespace genesis

#endif // include guard
