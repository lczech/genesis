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

#include "genesis/population/filter/sample_counts_filter_numerical.hpp"

#include "genesis/population/function/functions.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace genesis {
namespace population {

// =================================================================================================
//     Transform by Count
// =================================================================================================

void transform_zero_out_by_min_count(
    SampleCounts& sample,
    size_t min_count,
    bool also_n_and_d_counts
) {
    // Reset counts if needed, according to min count setting.
    sample.a_count = sample.a_count < min_count ? 0 : sample.a_count;
    sample.c_count = sample.c_count < min_count ? 0 : sample.c_count;
    sample.g_count = sample.g_count < min_count ? 0 : sample.g_count;
    sample.t_count = sample.t_count < min_count ? 0 : sample.t_count;

    // Also for the two others, if needed.
    if( also_n_and_d_counts ) {
        sample.n_count = sample.n_count < min_count ? 0 : sample.n_count;
        sample.d_count = sample.d_count < min_count ? 0 : sample.d_count;
    }
}

void transform_zero_out_by_min_count(
    Variant& variant,
    size_t min_count,
    bool also_n_and_d_counts
) {
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_min_count( sample, min_count, also_n_and_d_counts );
    }
}

void transform_zero_out_by_max_count(
    SampleCounts& sample,
    size_t max_count,
    bool also_n_and_d_counts
) {
    // Need an extra check here first.
    if( max_count == 0 ) {
        return;
    }

    // Reset counts if needed, according to max count setting.
    sample.a_count = sample.a_count > max_count ? 0 : sample.a_count;
    sample.c_count = sample.c_count > max_count ? 0 : sample.c_count;
    sample.g_count = sample.g_count > max_count ? 0 : sample.g_count;
    sample.t_count = sample.t_count > max_count ? 0 : sample.t_count;

    // Also for the two others, if needed.
    if( also_n_and_d_counts ) {
        sample.n_count = sample.n_count > max_count ? 0 : sample.n_count;
        sample.d_count = sample.d_count > max_count ? 0 : sample.d_count;
    }
}

void transform_zero_out_by_max_count(
    Variant& variant,
    size_t max_count,
    bool also_n_and_d_counts
) {
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_max_count( sample, max_count, also_n_and_d_counts );
    }
}

// =================================================================================================
//     Sample Counts Filter Numerical
// =================================================================================================

bool apply_sample_counts_filter_numerical(
    SampleCounts& sample,
    SampleCountsFilterNumericalParams const& params,
    SampleCountsFilterStats& stats
) {
    // We do not filter further if the sample has already been determined to be filered out.
    if( ! sample.status.passing() ) {
        return false;
    }

    // -------------------------------------------
    //     Numeric
    // -------------------------------------------

    // Counts
    if( params.min_count > 0 ) {
        transform_zero_out_by_min_count( sample, params.min_count );
    }
    if( params.max_count > 0 ) {
        transform_zero_out_by_max_count( sample, params.max_count );
    }

    // Check deletions.
    if(
        params.deletions_count_limit > 0
        && sample.d_count > 0
        && sample.d_count >= params.deletions_count_limit
    ) {
        // if( params.clear_filtered ) {
        //     sample.clear();
        // }

        sample.status.set( SampleCountsFilterTag::kAboveDeletionsCountLimit );
        ++stats[SampleCountsFilterTag::kAboveDeletionsCountLimit];
        return false;
    }

    // Empty?
    auto const sum = nucleotide_sum( sample );
    if( sum == 0 ) {
        // if( params.clear_filtered ) {
        //     sample.clear();
        // }

        sample.status.set( SampleCountsFilterTag::kEmpty );
        ++stats[SampleCountsFilterTag::kEmpty];
        return false;
    }

    // Read depth
    if( sum < params.min_read_depth ) {
        // if( params.clear_filtered ) {
        //     sample.clear();
        // }

        sample.status.set( SampleCountsFilterTag::kBelowMinReadDepth );
        ++stats[SampleCountsFilterTag::kBelowMinReadDepth];
        return false;
    }
    if( params.max_read_depth > 0 && sum > params.max_read_depth ) {
        // if( params.clear_filtered ) {
        //     sample.clear();
        // }

        sample.status.set( SampleCountsFilterTag::kAboveMaxReadDepth );
        ++stats[SampleCountsFilterTag::kAboveMaxReadDepth];
        return false;
    }

    // -------------------------------------------
    //     SNP vs Invariant
    // -------------------------------------------

    // SNPs
    if( params.only_snps || params.only_biallelic_snps ) {

        // Determine type of SNP.
        auto const al_count = allele_count( sample );
        if( params.only_snps && al_count < 2 ) {
            // if( params.clear_filtered ) {
            //     sample.clear();
            // }

            sample.status.set( SampleCountsFilterTag::kNotSnp );
            ++stats[SampleCountsFilterTag::kNotSnp];
            return false;
        }
        if( params.only_biallelic_snps && al_count != 2 ) {
            // if( params.clear_filtered ) {
            //     sample.clear();
            // }

            sample.status.set( SampleCountsFilterTag::kNotBiallelicSnp );
            ++stats[SampleCountsFilterTag::kNotBiallelicSnp];
            return false;
        }
    }

    // Everything passed.
    return true;
}

bool apply_sample_counts_filter_numerical(
    SampleCounts& sample,
    SampleCountsFilterNumericalParams const& params
) {
    SampleCountsFilterStats stats{};
    return apply_sample_counts_filter_numerical( sample, params, stats );
}

// =================================================================================================
//     Variant Sample Counts Filter Numerical
// =================================================================================================

bool apply_sample_counts_filter_numerical(
    Variant& variant,
    SampleCountsFilterNumericalParams const& params,
    VariantFilterStats& variant_stats,
    SampleCountsFilterStats& sample_count_stats,
    bool all_need_pass
) {
    // We do not filter further if the position has already been determined to be filered out.
    if( ! variant.status.passing() ) {
        return false;
    }

    // Apply the filter to all samples, and count how many passed.
    size_t passed_cnt = 0;
    for( auto& sample : variant.samples ) {
        auto const passed = apply_sample_counts_filter_numerical(
            sample, params, sample_count_stats
        );
        if( passed ) {
            ++passed_cnt;
        }
    }

    // Use that number to decide if the whole Variant is passing or not.
    // If no sample passed, that's a fail for the Variant in either case as well.
    if( passed_cnt == 0 ) {
        variant.status.set( VariantFilterTag::kNoSamplePassed );
        ++variant_stats[VariantFilterTag::kNoSamplePassed];
        return false;
    }

    // If all samples need to pass, we need an extra check.
    if( all_need_pass ) {
        if( passed_cnt == variant.samples.size() ) {
            return true;
        } else {
            variant.status.set( VariantFilterTag::kNotAllSamplesPassed );
            ++variant_stats[VariantFilterTag::kNotAllSamplesPassed];
            return false;
        }
    }

    // Here, some samples passed, and we are not requiring that all pass, so that's a pass overall.
    assert( passed_cnt > 0 );
    assert( !all_need_pass );
    return true;
}

bool apply_sample_counts_filter_numerical(
    Variant& variant,
    SampleCountsFilterNumericalParams const& params,
    bool all_need_pass
) {
    VariantFilterStats variant_stats{};
    SampleCountsFilterStats sample_count_stats{};
    return apply_sample_counts_filter_numerical(
        variant, params, variant_stats, sample_count_stats, all_need_pass
    );
}

} // namespace population
} // namespace genesis
