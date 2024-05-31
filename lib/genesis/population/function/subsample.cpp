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

#include "genesis/population/function/subsample.hpp"

#include "genesis/population/function/functions.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/distribution.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Scaling
// =================================================================================================

void rescale_counts_(
    SampleCounts& sample,
    size_t target_depth,
    bool skip_if_below_target_depth
) {
    // Get the total sum. If this does not exceed the max, we are done already.
    size_t const total_sum = sample_counts_sum(sample);
    if( skip_if_below_target_depth && total_sum <= target_depth ) {
        return;
    }

    // Scale the numbers, which rounds to the lower integer.
    // We store the result in intermediate values first, as we might need the originals later.
    double const scale = static_cast<double>(target_depth) / static_cast<double>(total_sum);
    size_t a_count = static_cast<size_t>( static_cast<double>( sample.a_count ) * scale );
    size_t c_count = static_cast<size_t>( static_cast<double>( sample.c_count ) * scale );
    size_t g_count = static_cast<size_t>( static_cast<double>( sample.g_count ) * scale );
    size_t t_count = static_cast<size_t>( static_cast<double>( sample.t_count ) * scale );
    size_t n_count = static_cast<size_t>( static_cast<double>( sample.n_count ) * scale );
    size_t d_count = static_cast<size_t>( static_cast<double>( sample.d_count ) * scale );

    // Now we might have some remainder due to the integer rounding, which we want to proportionally
    // distribute across the numbers, so that the largest count gets most. We only processed 6
    // numbers, so the remainder of the rounding is less than 6.
    size_t const new_sum = a_count + c_count + g_count + t_count + n_count + d_count;
    assert( new_sum <= target_depth );
    size_t const remainder = target_depth - new_sum;
    assert( remainder < 6 );

    // Now we distribute the remainder across the counts, starting at the highest, to stay
    // as close as possible to proportional scaling. This is a bit expensive, but should be okay.
    // Well yes, quick test shows that 1000 calls of this function take about 1ms... fast enough!
    if( remainder > 0 ) {

        // First we compute the fractions of each nucleotide, and get a sorting order of this,
        // based on the original counts. We also get pointers to the values for index access.
        auto frac = std::array<double, 6>{{
            static_cast<double>( sample.a_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.c_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.g_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.t_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.n_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.d_count ) / static_cast<double>( total_sum )
        }};

        // We use a fixed length sorting algorithm here, as the standard sort is orders of magnitue
        // slower, which would add significant runtime to using this function for every position
        // in the genome.
        // auto const sorting_order = utils::sort_indices( frac.begin(), frac.end() );
        auto const sorting_order = sample_counts_sorting_order( frac );

        // We need index-based access to the counts, so we make a list of pointers.
        auto const count_refs = std::array<size_t*, 6>{{
            &a_count, &c_count, &g_count, &t_count, &n_count, &d_count
        }};

        // Now we distribute, so that the remainder is split proportionally. We have 1-5 counts
        // to distribute. We can think of this as splitting the unit interval into 1-5 intervals,
        // and distribute one additional count per interval. To this end, we give extra counts
        // to whichever nucleotide "dominates" that interval, i.e., holds the majority (more than
        // half of the interval range). That can be done by simply checking which one has the
        // largest fraction still, and reducing every time we use the fraction.
        double const interval_len = 1.0 / static_cast<double>( remainder );
        for( size_t i = 0; i < remainder; ++i ) {

            // Find the count that has the largest fraction still.
            // We can stop early if the fraction is more than a whole interval, as in that case,
            // it for sure will be the majority in the interval, as the fractions are sorted.
            double max_f = 0.0;
            size_t max_k = 0;
            for( size_t k = 0; k < 6; ++k ) {
                if( frac[sorting_order[k]] > max_f ) {
                    max_f = frac[sorting_order[k]];
                    max_k = k;
                }
                if( frac[sorting_order[k]] >= interval_len ) {
                    break;
                }
            }

            // Assign one of the remainder to that count, and reduce its fraction, so that in the
            // next iteration, it does not contribute as much any more. This can set the fraction
            // below zero, but that doesn't matter for the comparisons above. Any fraction that is
            // reduced to below zero here had less than one intervals worth of range anyway, so
            // it cannot be the majority in any further intervals later any more.
            frac[sorting_order[max_k]] -= interval_len;
            ++(*count_refs[sorting_order[max_k]]);
        }
    }

    // Now set the values of the sample to our computed counts.
    sample.a_count = a_count;
    sample.c_count = c_count;
    sample.g_count = g_count;
    sample.t_count = t_count;
    sample.n_count = n_count;
    sample.d_count = d_count;
    assert( sample_counts_sum( sample ) == target_depth );
}

void subscale_counts(
    SampleCounts& sample,
    size_t max_depth
) {
    rescale_counts_( sample, max_depth, true );
}

void subscale_counts(
    Variant& variant,
    size_t max_depth
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        rescale_counts_( sample, max_depth, true );
    }
}

void rescale_counts(
    SampleCounts& sample,
    size_t target_depth
) {
    rescale_counts_( sample, target_depth, false );
}

void rescale_counts(
    Variant& variant,
    size_t target_depth
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        rescale_counts_( sample, target_depth, false );
    }
}

// =================================================================================================
//     Sampling
// =================================================================================================

/**
 * @brief Local helper function to avoid code duplication. Takes the distribution (with or without
 * replacement) and performs the resampling of base counts.
 */
template<typename Distribution>
void resample_counts_(
    SampleCounts& sample,
    size_t max_depth,
    Distribution distribution,
    bool skip_if_below_target_depth
) {
    // Get the total sum. If this does not exceed the max, we are done already.
    size_t const total_sum = sample_counts_sum(sample);
    if( skip_if_below_target_depth && total_sum <= max_depth ) {
        return;
    }

    // Make a random draw from a multivariate distrubiton with the counts.
    auto const new_counts = distribution(
        std::vector<size_t>{{
            sample.a_count,
            sample.c_count,
            sample.g_count,
            sample.t_count,
            sample.n_count,
            sample.d_count
        }},
        max_depth
    );
    assert( new_counts.size() == 6 );

    // Set the sample counts
    sample.a_count = new_counts[0];
    sample.c_count = new_counts[1];
    sample.g_count = new_counts[2];
    sample.t_count = new_counts[3];
    sample.n_count = new_counts[4];
    sample.d_count = new_counts[5];
    assert( sample_counts_sum( sample ) == max_depth );
}

void subsample_counts_with_replacement(
    SampleCounts& sample,
    size_t max_depth
) {
    // Call the local helper function template, to avoid code duplication.
    return resample_counts_<std::vector<size_t>(*)(std::vector<size_t> const&, size_t)>(
        sample, max_depth, utils::multinomial_distribution, true
    );
}

void subsample_counts_with_replacement(
    Variant& variant,
    size_t max_depth
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        subsample_counts_with_replacement( sample, max_depth );
    }
}

void resample_counts(
    SampleCounts& sample,
    size_t target_depth
) {
    // Call the local helper function template, to avoid code duplication.
    return resample_counts_<std::vector<size_t>(*)(std::vector<size_t> const&, size_t)>(
        sample, target_depth, utils::multinomial_distribution, false
    );
}

void resample_counts(
    Variant& variant,
    size_t target_depth
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        resample_counts( sample, target_depth );
    }
}

void subsample_counts_without_replacement(
    SampleCounts& sample,
    size_t max_depth
) {
    // Call the local helper function template, to avoid code duplication.
    return resample_counts_<std::vector<size_t>(*)(std::vector<size_t> const&, size_t)>(
        sample, max_depth, utils::multivariate_hypergeometric_distribution, true
    );
}

void subsample_counts_without_replacement(
    Variant& variant,
    size_t max_depth
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        subsample_counts_without_replacement( sample, max_depth );
    }
}

} // namespace population
} // namespace genesis
