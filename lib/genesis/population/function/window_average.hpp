#ifndef GENESIS_POPULATION_FUNCTION_WINDOW_AVERAGE_H_
#define GENESIS_POPULATION_FUNCTION_WINDOW_AVERAGE_H_

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
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/window/base_window.hpp"
#include "genesis/population/window/window_view.hpp"

#include <cassert>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

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
     * positions that have been processed in total, except for any missing data.
     * As the stats get incremented each time a position is processed, this sum reflects every entry
     * that was exposed to some filter and not already completely removed, via e.g., region filters.
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
    kValidSnps,

    /**
     * @brief Simply report the total sum, with no averaging, i.e., the absolute value of the metric.
     */
    kSum,

    /**
     * @brief Use exactly the provided loci as set in the window of a GenomeLocusSet.
     *
     * This bypasses all the above data-based ways of determining the denominator for window
     * averaging, and instead uses a user-provided mask in form of a GenomeLocusSet. Within the
     * window, all positions that are set to `true` in that mask are considered to be valid,
     * and their count is used as the denominator.
     */
    kProvidedLoci
};

/**
 * @brief Get the length of a given Window.
 *
 * This is needed for the special case of a WindowView over the whole genome, which we indicate by
 * WindowView::is_whole_genome() being set. In this case, the length is not contiguous along
 * a single chromosome. In all other window cases, we simply use the first and last position
 * of the window, via BaseWindow::width().
 */
template<class D>
inline size_t get_window_length( BaseWindow<D> const& window )
{
    // If the window is of type GenomeWindowStream, its total length is the sum of all lengths
    // of the chromosomes that the genome has covered.
    if( window.is_whole_genome() ) {
        size_t window_length = 0;
        for( auto const& chr_len : window.chromosomes() ) {
            window_length += chr_len.second;
        }
        return window_length;
    }

    // In all other cases, we simply use the window width function.
    return window.width();
}

/**
 * @brief Get the count of provided loci in a window.
 */
template<class D>
inline size_t get_window_provided_loci_count(
    BaseWindow<D> const& window,
    std::shared_ptr<GenomeLocusSet> provided_loci
) {
    // We need a provided loci mask for this function.
    if( !provided_loci ) {
        throw std::invalid_argument(
            "Cannot comput window average denominator from provided loci mask, "
            "as no such mask was provided."
        );
    }

    // Helper function that takes a chromosome and positions on it, and returns the number
    // of set loci on the provided mask in that window.
    auto get_chr_loci_count_ = [&provided_loci]( std::string const& chr, size_t first, size_t last )
    {
        // Position checks. Should not happen if our internal usage is correct.
        if( first == 0 || last == 0 || first > last ) {
            throw std::invalid_argument(
                "Invalid positions first=" + std::to_string( first ) + " last=" +
                std::to_string(last) + " on chromosome \"" + chr +
                "\" for computing provided loci mask window denominator."
            );
        }

        // Get the chromosome. This can fail if the user did not provide a fitting mask.
        if( ! provided_loci->has_chromosome( chr )) {
            throw std::runtime_error(
                "Cannot compute provided loci on chromosome \"" +  chr +
                "\", as the provided loci mask does not contain the chromosome."
            );
        }
        auto const& bv = provided_loci->chromosome_positions( chr );

        // Mask check. In our internal usage, this should not fail, but we check anyway,
        // in case this function is called with a mask that is not meant for the given purpose.
        if( bv.get(0) ) {
            throw std::invalid_argument(
                "Invvalid provided loci mask with bit 0 set."
            );
        }

        // Another check based on user data. Can fail if the user did not provide a fitting mask.
        if( last >= bv.size() ) {
            throw std::runtime_error(
                "Cannot compute provided loci on chromosome \"" +  chr +
                "\", as the provided loci mask for the chromosome has length " +
                std::to_string( bv.size() - 1 ) + ", but the window covers positions " +
                std::to_string( first ) + "-" + std::to_string( last )
            );
        }

        // Finally, we have checked everything. Our first and last position are both inclusive,
        // while the bitvector count uses past-the-end, so we need to add one here for the last.
        return bv.count( first, last + 1 );
    };

    // If the window is a WindowStream over a whole genome, we use all its chromosomes.
    // This might not cover all chromosomes that the provided loci have data for,
    // in case a region filter was applied, so we want to account for that.
    // We are also rather strict in the process, to avoid accidental mismatches on the user side.
    // We count all positions between the beginning of the chromosome, and the end as either
    // specified in the data, or, if a sequence dict was given to the genome window stream, from that.
    if( window.is_whole_genome() ) {
        size_t loci_count = 0;
        for( auto const& chr_len : window.chromosomes() ) {
            loci_count += get_chr_loci_count_( chr_len.first, 1, chr_len.second );
        }
        return loci_count;
    }

    // Here we are in the normal case for all other window types. For those, we just simply use
    // their first and last positions. For ChromosomeWindowStream, this is similar as for the above
    // whole genome WindowStream, and is also be based on the sequence dict, if given, or on the data.
    return get_chr_loci_count_(
        window.chromosome(), window.first_position(), window.last_position()
    );
}

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
template<class D>
inline double window_average_denominator(
    WindowAveragePolicy policy,
    BaseWindow<D> const& window,
    std::shared_ptr<GenomeLocusSet> provided_loci,
    VariantFilterStats const& variant_filter_stats,
    SampleCountsFilterStats const& sample_counts_filter_stats
) {
    // We cannot have processed more samples than there were passing variants,
    // as we only should have processed a sample once its variant is found to be passing.
    // In case of the FST processor, we make sure that only one of the samples gets recorded
    // in the stats, so this works there as well. We do not simply assert this here,
    // as a misuse of the filters could result in an error here, which would be on the user
    // side, and so is an exception.
    // We skip this test when using the sum anyway, as in those cases, we might not have
    // the correct filter stats available in the first place.
    if(
        policy != WindowAveragePolicy::kSum &&
        sample_counts_filter_stats.sum() > variant_filter_stats[VariantFilterTag::kPassed]
    ) {
        throw std::invalid_argument(
            "Invalid stat counts with sample_counts_filter_stats.sum() > "
            "variant_filter_stats[VariantFilterTag::kPassed]"
        );
    }

    // Now select which value we want to return.
    switch( policy ) {
        case WindowAveragePolicy::kWindowLength: {
            return get_window_length( window );
        }
        case WindowAveragePolicy::kAvailableLoci: {
            auto const missing = variant_filter_stats_category_counts(
                variant_filter_stats, VariantFilterTagCategory::kMissingInvalid
            );
            return variant_filter_stats.sum() - missing;
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
        case WindowAveragePolicy::kSum: {
            return 1.0;
        }
        case WindowAveragePolicy::kProvidedLoci: {
            return get_window_provided_loci_count( window, provided_loci );
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
