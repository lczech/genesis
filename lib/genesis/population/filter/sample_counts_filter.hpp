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
     * @brief Sum of counts across all samples is below the min coverage threshold.
     *
     * @see SampleCountsFilterNumericalParams::min_coverage
     */
    kBelowMinCoverage,

    /**
     * @brief Sum of counts across all samples is above the max coverage threshold.
     *
     * @see SampleCountsFilterNumericalParams::max_coverage
     */
    kAboveMaxCoverage,

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
 * @brief Generate summary counts for a SampleCountsFilterStats counter.
 *
 * The given @p stats contain counts for different reasons of filters that could have failed when
 * filtering a SampleCounts. This function summarizes those stats into three basic categories,
 * and gives their sums:
 *
 *  0. Passing
 *  1. Missing data and other invalids (basically, all non-numeric filters)
 *  2. Numeric filters, such as zero counts, outside of read depth limits, etc
 *
 * This is meant as a broad summary, for instance for user output, where it might not be overly
 * relevant which exact numerical filter got triggered how often by a particular filter, but rather
 * we want to have an overview of which classes or categories of filters got triggered how often.
 *
 * Hence, the returned numbers depend on the exact usage of the SampleCountsFilterTag tags here.
 * If other types of tags are used for the SampleCounts::status instead, this function cannot be used.
 */
std::array<size_t, 3> sample_counts_filter_stats_category_counts(
    SampleCountsFilterStats const& stats
);

// =================================================================================================
//     Filter Functions
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Print stats
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

} // namespace population
} // namespace genesis

#endif // include guard
