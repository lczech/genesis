#ifndef GENESIS_POPULATION_FUNCTION_AVERAGE_H_
#define GENESIS_POPULATION_FUNCTION_AVERAGE_H_

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

#include "genesis/population/filter/filter_stats.hpp"
#include "genesis/population/filter/filter_status.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Window Averaging
// =================================================================================================

/**
 * @brief Select the method to use for computing window averages of statistic estimators.
 *
 * When computing diversity metrics or FST in windows, we often want to compute an average across
 * a window. Data might have positions that are missing, have low read depth, fail some other filter,
 * or simply might only consist of the SNPs, if some SNP calling was applied before. Hence, we need
 * different strategies to compute the per-site average across windows.
 */
enum class WindowAveragePolicy
{
    /**
     * @brief Simply report the total sum, with no averaging, i.e., the absolute value of the metric.
     */
    kAbsoluteSum,

    /**
     * @brief Use the window length.
     *
     * This does not take any characteristics of the data into account. This might underestimate
     * diversity in regions with low coverage, as then, we might have positions with no coverage,
     * so that we do not compute a value there, but they are still used in the denominator
     * for computing the relative value.
     */
    kWindowLength,

    /**
     * @brief Use the number of positions for which there was data at all, independent of all filter
     * settings. This can be useful when SNP calling was applied beforehand.
     *
     * Technically, this simply uses the sum of the variant filter stats to get the number of
     * positions that have been processed in total. As the stats get incremented each time a position
     * is processed, this sum reflects every entry that was exposed to some filter and not already
     * completely removed, via e.g., region filters.
     */
    kAvailableLoci,

    /**
     * @brief Use the number of positions that passed all quality and numerical filters, excluding
     * the SNP-related filters.
     *
     * That is, these positions are of high quality, and include both the SNPs and the invariant
     * positions. In the absence of any particular circumstances, this is the recommended option.
     * This can also be used in combination with a mask file, in order to specify loci that are
     * to be considered valid, even in the absence of actual data in the input.
     *
     * Technically, this uses the passing positions of the sample stats (that passed all,
     * including the SNP-related, filters), as well as the sum of all SNP-related variant filter
     * stats. That sum is the number of positions that passed all previous filters, such as numerical
     * ones, but failed in the last step, i.e., are not SNPs according to the SNP-related filter
     * settings.
     * See variant_filter_stats_category_counts() for a function that does something similar.
     */
    kValidLoci,

    /**
     * @brief Use the number of SNPs only.
     *
     * This will overestimate the average, but might be useful depending on the given type of data.
     * Note that if the data only consists of SNPs in the first place, this is identical to
     * kValidLoci anyway.
     *
     * Technically, this uses the sum of passing sample filters.
     * We could use the passing variant filter count here instead, but that might be too big,
     * as it could contain positions that passed the total variant filters, but failed for a
     * particular sample. So we are conservative here, and only use the number of postions
     * that passed everything.
     */
    kValidSnps
};

/**
 * @brief Get the denoninator to use for averaging an estimator across a window.
 *
 * This simply uses the @p policy to make a selection of which of the given input numbers to select.
 * The function is meant as the one place where we make this choice, in order for all estimators
 * to work the same.
 *
 * The function takes all possible stats and numbers as input, in order to guarantee that they
 * are all available. This also enforces correct usage of the calculators and processors,
 * as neither number can be omitted by accident.
 */
inline double window_average_denominator(
    WindowAveragePolicy policy,
    size_t window_length,
    VariantFilterStats const& variant_filter_stats,
    SampleCountsFilterStats const& sample_counts_filter_stats
) {
    // We cannot have processed more samples than there were passing variants,
    // as we only should have processed a sample once its variant is found to be passing.
    // In case of the FST processor, we make sure that only one of the samples gets recorded
    // in the stats, so this works there as well. We do not simply assert this here,
    // as a misuse of the filters could result in an error here, which would be on the user
    // side, and so is an exception.
    if( sample_counts_filter_stats.sum() > variant_filter_stats[VariantFilterTag::kPassed] ) {
        throw std::invalid_argument(
            "Invalid stat counts with ""sample_counts_filter_stats.sum() > "
            "variant_filter_stats[VariantFilterTag::kPassed]"
        );
    }

    // Now select which value we want to return.
    switch( policy ) {
        case WindowAveragePolicy::kAbsoluteSum: {
            return 1.0;
        }
        case WindowAveragePolicy::kWindowLength: {
            return window_length;
        }
        case WindowAveragePolicy::kAvailableLoci: {
            return variant_filter_stats.sum();
        }
        case WindowAveragePolicy::kValidLoci: {
            // Here, we use the number of positions that passed all total variant filters
            // except for being a SNP, as well as the per-sample count of postions that
            // furthermore passed all sample positions.
            auto const valid_non_snps = variant_filter_stats_category_counts(
                variant_filter_stats, VariantFilterTagCategory::kInvariant
            );
            auto const valid_snps = sample_counts_filter_stats_category_counts(
                sample_counts_filter_stats, SampleCountsFilterTagCategory::kPassed
            );
            return valid_non_snps + valid_snps;
        }
        case WindowAveragePolicy::kValidSnps: {
            return sample_counts_filter_stats[SampleCountsFilterTag::kPassed];
        }
        default: {
            throw std::invalid_argument( "Invalid WindowAveragePolicy" );
        }
    }
    return std::numeric_limits<double>::quiet_NaN();
}

} // namespace population
} // namespace genesis

#endif // include guard
