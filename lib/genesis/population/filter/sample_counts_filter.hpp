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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/filter/filter_stats.hpp"
#include "genesis/population/filter/filter_status.hpp"
#include "genesis/population/variant.hpp"

#include <cassert>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     BaseCounts Filter Tag
// =================================================================================================

enum class BaseCountsFilterTag : FilterStatus::IntType
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
     * @see BaseCountsFilterNumericalParams::min_coverage
     */
    kBelowMinCoverage,

    /**
     * @brief Sum of counts across all samples is above the max coverage threshold.
     *
     * @see BaseCountsFilterNumericalParams::max_coverage
     */
    kAboveMaxCoverage,

    /**
     * @brief Too many deletions at the position.
     *
     * @see BaseCountsFilterNumericalParams::deletions_count_limit
     */
    kAboveDeletionsCountLimit,

    /**
     * @brief Invariant position, not a SNP.
     *
     * @see BaseCountsFilterNumericalParams::only_snps
     */
    kNotSnp,

    /**
     * @brief SNP position, but not biallelic, i.e., has more than one alternative.
     *
     * @see BaseCountsFilterNumericalParams::only_biallelic_snps
     */
    kNotBiallelicSnp,

    // -------------------------------------------
    //     Special
    // -------------------------------------------

    kEnd
};

// =================================================================================================
//     BaseCounts Filter Stats
// =================================================================================================

/**
 * @brief Counts of how many BaseCounts%s with each BaseCountsFilterTag occured in some data.
 */
using BaseCountsFilterStats = FilterStats<BaseCountsFilterTag>;

// =================================================================================================
//     Filter Functions
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Print stats
// --------------------------------------------------------------------------------------

/**
 * @brief Print a textual representation of the counts collected.
 */
std::ostream& print_base_counts_filter_stats(
    std::ostream& os,
    BaseCountsFilterStats const& stats,
    bool verbose = false
);

/**
 * @brief Print a textual representation of the counts collected.
 */
std::string print_base_counts_filter_stats(
    BaseCountsFilterStats const& stats,
    bool verbose = false
);

} // namespace population
} // namespace genesis

#endif // include guard
