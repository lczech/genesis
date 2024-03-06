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

#include "genesis/population/functions/subsample.hpp"

#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/math/distribution.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Subscaling
// =================================================================================================

void transform_subscale(
    BaseCounts& sample,
    size_t max_coverage
) {
    // Get the total sum. If this does not exceed the max, we are done already.
    size_t const total_sum = sample.a_count + sample.c_count + sample.g_count + sample.t_count;
    if( total_sum <= max_coverage ) {
        return;
    }

    // Scale down the numbers, which rounds to the lower integer.
    // We store the result in intermediate values first, as we might need the originals later.
    double const scale = static_cast<double>(max_coverage) / static_cast<double>(total_sum);
    size_t a_count = sample.a_count * scale;
    size_t c_count = sample.c_count * scale;
    size_t g_count = sample.g_count * scale;
    size_t t_count = sample.t_count * scale;

    // For completeness, we also scale the n and d counts, but they do not influence our counts,
    // as we do not want them to dominate.
    sample.n_count *= scale;
    sample.d_count *= scale;

    // Now we might have some remainder due to the integer rounding, which we want to proportionally
    // distribute across the numbers, so that the largest count gets most. We only processed four
    // numbers, so the remainder of the rounding is less than 4.
    size_t const new_sum = a_count + c_count + g_count + t_count;
    size_t remainder = max_coverage - new_sum;
    assert( remainder < 4 );

    // Now we distribute the remainder across the counts, starting at the highest, to stay
    // as close as possible to proportional scaling. This is a bit expensive, but should be okay.
    // Well okay, we can run 10k calls of this function in 1ms. Probalby okay.
    if( remainder > 0 ) {

        // First we compute the fractions of each nucleotide, and get a sorting order of this,
        // based on the original counts. We also get pointers to the values for index access.
        auto const frac = std::array<double, 4>{{
            static_cast<double>( sample.a_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.c_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.g_count ) / static_cast<double>( total_sum ),
            static_cast<double>( sample.t_count ) / static_cast<double>( total_sum )
        }};
        auto const order = nucleotide_sorting_order( frac );
        auto const count_refs = std::array<size_t*, 4>{{ &a_count, &c_count, &g_count, &t_count }};

        // Now we distribute, so that the remainder is split proportionally. We have 1-3 counts
        // to distribute. We can think of this as splitting the unit interval into 1-3 intervals,
        // and give extra counts to whichever nucleotide "dominates" that interval, i.e., holds
        // the majority range in the interval, as determined by the interval midpoint.
        switch( remainder ) {
            case 1: {
                // Only a single count to distribute. Goes to the highest.
                ++(*count_refs[order[0]]);
                break;
            }
            case 2: {
                // Two counts to distribute. First to the highest, and second goes to whoever has
                // the majority in the interval [0.5, 1.0], which depends on whether the highest
                // count is more than 75%, in which case it also has the majority in the second
                // interval. If so, it gets another count; otherwise, the second highest gets it.
                ++(*count_refs[order[0]]);
                if( frac[order[0]] > 0.75 ) {
                    ++(*count_refs[order[0]]);
                } else {
                    ++(*count_refs[order[1]]);
                }
                break;
            }
            case 3: {
                // With three remaining counts, we again give one to the highest nucleotide either way.
                // Then, the "middle" interval [0.333, 0.666] is dominated by the highest again
                // if it has more than 50% of the counts. Otherwise, the second highest nucleotide
                // gets the count. This interval can never be domonated by the third or fourth
                // nucleotide, as that would imply that they have a higher fraction, and wouldn't
                // be third or fourth then. Lastly, the third interval [0.666, 1.0] can be dominated
                // by either the first, second, or third nucleotide. THis is again decided by
                // who has the majority at the interval midpoint.
                ++(*count_refs[order[0]]);
                if( frac[order[0]] > 0.5 ) {
                    ++(*count_refs[order[0]]);
                } else  {
                    ++(*count_refs[order[1]]);
                }
                if( frac[order[0]] > 5.0 / 6.0 ) {
                    ++(*count_refs[order[0]]);
                } else if( frac[order[0]] + frac[order[1]] > 5.0 / 6.0 ) {
                    ++(*count_refs[order[1]]);
                } else {
                    ++(*count_refs[order[2]]);
                }
                break;
            }
            default: {
                assert( false );
            }
        }
    }

    // Now set the values of the sample to our computed counts.
    assert( a_count + c_count + g_count + t_count == max_coverage );
    sample.a_count = a_count;
    sample.c_count = c_count;
    sample.g_count = g_count;
    sample.t_count = t_count;
}

void transform_subscale(
    Variant& variant,
    size_t max_coverage
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        transform_subscale( sample, max_coverage );
    }
}

// =================================================================================================
//     Subsampling
// =================================================================================================

/**
 * @brief Local helper function to avoid code duplication. Takes the sampler (with or without
 * replacement) and performs the resampling of base counts.
 */
template<typename Sampler>
void transform_subsample_(
    BaseCounts& sample,
    size_t max_coverage,
    Sampler sampler
) {
    // Get the total sum. If this does not exceed the max, we are done already.
    // We do not want the n and d counts to influce the total coverage here.
    size_t const total_sum = sample.a_count + sample.c_count + sample.g_count + sample.t_count;
    if( total_sum <= max_coverage ) {
        return;
    }

    // Make a random draw from a multinomial distrubiton with the counts.
    // Here, we also take n and d into account for the resampling.
    auto const new_counts = sampler(
        std::vector<size_t>{{
            sample.a_count,
            sample.c_count,
            sample.g_count,
            sample.t_count,
            sample.n_count,
            sample.d_count
        }},
        max_coverage
    );
    assert( new_counts.size() == 6 );

    // Set the sample counts
    sample.a_count = new_counts[0];
    sample.c_count = new_counts[1];
    sample.g_count = new_counts[2];
    sample.t_count = new_counts[3];
    sample.n_count = new_counts[4];
    sample.d_count = new_counts[5];
}

void transform_subsample_with_replacement(
    BaseCounts& sample,
    size_t max_coverage
) {
    // Call the local helper function template, to avoid code duplication.
    return transform_subsample_<std::vector<size_t>(*)(std::vector<size_t> const&, size_t)>(
        sample, max_coverage, utils::multinomial_distribution
    );
}

void transform_subsample_with_replacement(
    Variant& variant,
    size_t max_coverage
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        transform_subsample_with_replacement( sample, max_coverage );
    }
}

void transform_subsample_without_replacement(
    BaseCounts& sample,
    size_t max_coverage
) {
    // Call the local helper function template, to avoid code duplication.
    return transform_subsample_<std::vector<size_t>(*)(std::vector<size_t> const&, size_t)>(
        sample, max_coverage, utils::multivariate_hypergeometric_distribution
    );
}

void transform_subsample_without_replacement(
    Variant& variant,
    size_t max_coverage
) {
    // Call the transformation for each sample in the variant.
    for( auto& sample : variant.samples ) {
        transform_subsample_without_replacement( sample, max_coverage );
    }
}

} // namespace population
} // namespace genesis
