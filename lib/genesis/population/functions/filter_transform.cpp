/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/population/functions/filter_transform.hpp"

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
    BaseCounts& sample,
    size_t min_count,
    bool also_n_and_d_counts
) {
    BaseCountsFilterStats stats;
    transform_zero_out_by_min_count( sample, min_count, stats, also_n_and_d_counts );
}

void transform_zero_out_by_min_count(
    BaseCounts& sample,
    size_t min_count,
    BaseCountsFilterStats& stats,
    bool also_n_and_d_counts
) {
    // Short, but does not allow counting...
    // sample.a_count = sample.a_count >= filter.min_count ? sample.a_count : 0;
    // sample.c_count = sample.c_count >= filter.min_count ? sample.c_count : 0;
    // sample.g_count = sample.g_count >= filter.min_count ? sample.g_count : 0;
    // sample.t_count = sample.t_count >= filter.min_count ? sample.t_count : 0;
    // sample.n_count = sample.n_count >= filter.min_count ? sample.n_count : 0;
    // sample.d_count = sample.d_count >= filter.min_count ? sample.d_count : 0;

    // Reset counts if needed, according to min count setting.
    if( sample.a_count < min_count ) {
        sample.a_count = 0;
        ++stats.below_min_count;
    }
    if( sample.c_count < min_count ) {
        sample.c_count = 0;
        ++stats.below_min_count;
    }
    if( sample.g_count < min_count ) {
        sample.g_count = 0;
        ++stats.below_min_count;
    }
    if( sample.t_count < min_count ) {
        sample.t_count = 0;
        ++stats.below_min_count;
    }

    // Also for the two others, but not counted in the statistics
    if( also_n_and_d_counts && sample.n_count < min_count ) {
        sample.n_count = 0;
    }
    if( also_n_and_d_counts && sample.d_count < min_count ) {
        sample.d_count = 0;
    }
}

void transform_zero_out_by_min_count(
    Variant& variant,
    size_t min_count,
    bool also_n_and_d_counts
) {
    BaseCountsFilterStats stats;
    transform_zero_out_by_min_count( variant, min_count, stats, also_n_and_d_counts );
}

void transform_zero_out_by_min_count(
    Variant& variant,
    size_t min_count,
    BaseCountsFilterStats& stats,
    bool also_n_and_d_counts
) {
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_min_count( sample, min_count, stats, also_n_and_d_counts );
    }
}

void transform_zero_out_by_max_count(
    BaseCounts& sample,
    size_t max_count,
    bool also_n_and_d_counts
) {
    BaseCountsFilterStats stats;
    transform_zero_out_by_max_count( sample, max_count, stats, also_n_and_d_counts );
}

void transform_zero_out_by_max_count(
    BaseCounts& sample,
    size_t max_count,
    BaseCountsFilterStats& stats,
    bool also_n_and_d_counts
) {
    if( max_count == 0 ) {
        return;
    }

    // Reset counts if needed, according to max count setting.
    if( sample.a_count > max_count ) {
        sample.a_count = 0;
        ++stats.above_max_count;
    }
    if( sample.c_count > max_count ) {
        sample.c_count = 0;
        ++stats.above_max_count;
    }
    if( sample.g_count > max_count ) {
        sample.g_count = 0;
        ++stats.above_max_count;
    }
    if( sample.t_count > max_count ) {
        sample.t_count = 0;
        ++stats.above_max_count;
    }

    // Also for the two others, but not counted in the statistics
    if( also_n_and_d_counts && sample.n_count > max_count ) {
        sample.n_count = 0;
    }
    if( also_n_and_d_counts && sample.d_count > max_count ) {
        sample.d_count = 0;
    }
}

void transform_zero_out_by_max_count(
    Variant& variant,
    size_t max_count,
    bool also_n_and_d_counts
) {
    BaseCountsFilterStats stats;
    transform_zero_out_by_max_count( variant, max_count, stats, also_n_and_d_counts );
}

void transform_zero_out_by_max_count(
    Variant& variant,
    size_t max_count,
    BaseCountsFilterStats& stats,
    bool also_n_and_d_counts
) {
    for( auto& sample : variant.samples ) {
        transform_zero_out_by_max_count( sample, max_count, stats, also_n_and_d_counts );
    }
}

// =================================================================================================
//     Base Counts Filter
// =================================================================================================

void reset( BaseCountsFilterStats& stats )
{
    stats.passed = 0;
    stats.below_min_count = 0;
    stats.above_max_count = 0;
    stats.empty = 0;
    stats.untolerated_deletion = 0;
    stats.below_min_coverage = 0;
    stats.above_max_coverage = 0;
    stats.not_snp = 0;
    stats.not_biallelic_snp = 0;
}

std::ostream& print_base_counts_filter_stats( std::ostream& os, BaseCountsFilterStats const& stats )
{
    if( stats.passed > 0 ) {
        os << "Passed:               " << stats.passed << "\n";
    }
    // if( stats.below_min_count > 0 ) {
    //     os << "Below min count:      " << stats.below_min_count << "\n";
    // }
    // if( stats.above_max_count > 0 ) {
    //     os << "Above max count:      " << stats.above_max_count << "\n";
    // }
    if( stats.empty > 0 ) {
        os << "Empty (after counts): " << stats.empty << "\n";
    }
    if( stats.untolerated_deletion > 0 ) {
        os << "Untolerated deletion: " << stats.untolerated_deletion << "\n";
    }
    if( stats.below_min_coverage > 0 ) {
        os << "Below min coverage:   " << stats.below_min_coverage << "\n";
    }
    if( stats.above_max_coverage > 0 ) {
        os << "Above max coverage:   " << stats.above_max_coverage << "\n";
    }
    if( stats.not_snp > 0 ) {
        os << "Not SNP:              " << stats.not_snp << "\n";
    }
    if( stats.not_biallelic_snp > 0 ) {
        os << "Not biallelic SNP:    " << stats.not_biallelic_snp << "\n";
    }
    return os;
}

std::string print_base_counts_filter_stats( BaseCountsFilterStats const& stats )
{
    std::stringstream ss;
    print_base_counts_filter_stats( ss, stats );
    return ss.str();
}

bool filter_base_counts(
    BaseCounts& sample,
    BaseCountsFilter const& filter
) {
    BaseCountsFilterStats stats;
    return filter_base_counts( sample, filter, stats );
}

bool filter_base_counts(
    BaseCounts& sample,
    BaseCountsFilter const& filter,
    BaseCountsFilterStats& stats
) {
    // Counts
    if( filter.min_count > 0 ) {
        transform_zero_out_by_min_count( sample, filter.min_count, stats );
    }
    if( filter.max_count > 0 ) {
        transform_zero_out_by_max_count( sample, filter.max_count, stats );
    }
    if(
        ! filter.tolerate_deletions
        && sample.d_count > 0
        && filter.min_count > 0
        && sample.d_count >= filter.min_count
        // && filter.max_count > 0
        // && sample.d_count <= filter.max_count
    ) {
        if( filter.clear_filtered ) {
            sample.clear();
        }
        ++stats.untolerated_deletion;
        return false;
    }

    // Empty?
    auto const sum = nucleotide_sum( sample );
    if( sum == 0 ) {
        if( filter.clear_filtered ) {
            sample.clear();
        }
        ++stats.empty;
        return false;
    }

    // Coverage
    if( sum < filter.min_coverage ) {
        if( filter.clear_filtered ) {
            sample.clear();
        }
        ++stats.below_min_coverage;
        return false;
    }
    if( filter.max_coverage > 0 && sum > filter.max_coverage ) {
        if( filter.clear_filtered ) {
            sample.clear();
        }
        ++stats.above_max_coverage;
        return false;
    }

    // SNPs
    if( filter.only_snps || filter.only_biallelic_snps ) {

        // Determine type of SNP.
        auto const al_count = allele_count( sample );
        if( filter.only_snps && al_count < 2 ) {
            if( filter.clear_filtered ) {
                sample.clear();
            }
            ++stats.not_snp;
            return false;
        }
        if( filter.only_biallelic_snps && al_count != 2 ) {
            if( filter.clear_filtered ) {
                sample.clear();
            }
            ++stats.not_biallelic_snp;
            return false;
        }
    }

    ++stats.passed;
    return true;
}

bool filter_base_counts(
    Variant& variant,
    BaseCountsFilter const& filter,
    bool all_need_pass
) {
    BaseCountsFilterStats stats;
    return filter_base_counts( variant, filter, stats, all_need_pass );
}

bool filter_base_counts(
    Variant& variant,
    BaseCountsFilter const& filter,
    BaseCountsFilterStats& stats,
    bool all_need_pass
) {
    size_t passed_cnt = 0;
    for( auto& sample : variant.samples ) {
        auto const passed = filter_base_counts( sample, filter, stats );
        passed_cnt += static_cast<int>( passed );
    }
    if( all_need_pass ) {
        return passed_cnt == variant.samples.size();
    } else {
        return passed_cnt > 0;
    }
    return false;
}

// =================================================================================================
//     Variant Filter
// =================================================================================================

void reset( VariantFilterStats& stats )
{
    stats.passed = 0;
    stats.empty = 0;
    stats.below_min_coverage = 0;
    stats.above_max_coverage = 0;
    stats.not_snp = 0;
    stats.not_biallelic_snp = 0;
    stats.below_min_count = 0;
    stats.above_max_count = 0;
    stats.not_min_frequency = 0;
}

std::ostream& print_variant_filter_stats( std::ostream& os, VariantFilterStats const& stats )
{
    if( stats.passed > 0 ) {
        os << "Passed:                  " << stats.passed << "\n";
    }
    if( stats.empty > 0 ) {
        os << "Empty (after counts):    " << stats.empty << "\n";
    }
    if( stats.below_min_coverage > 0 ) {
        os << "Below min coverage:      " << stats.below_min_coverage << "\n";
    }
    if( stats.above_max_coverage > 0 ) {
        os << "Above max coverage:      " << stats.above_max_coverage << "\n";
    }
    if( stats.untolerated_deletion > 0 ) {
        os << "Untolerated deletion:    " << stats.untolerated_deletion << "\n";
    }
    if( stats.not_snp > 0 ) {
        os << "Not SNP:                 " << stats.not_snp << "\n";
    }
    if( stats.not_biallelic_snp > 0 ) {
        os << "Not biallelic SNP:       " << stats.not_biallelic_snp << "\n";
    }
    if( stats.below_min_count > 0 ) {
        os << "Below min count for SNP: " << stats.below_min_count << "\n";
    }
    if( stats.above_max_count > 0 ) {
        os << "Above max count for SNP: " << stats.above_max_count << "\n";
    }
    if( stats.not_min_frequency > 0 ) {
        os << "Not min frequency:       " << stats.not_min_frequency << "\n";
    }
    return os;
}

std::string print_variant_filter_stats( VariantFilterStats const& stats )
{
    std::stringstream ss;
    print_variant_filter_stats( ss, stats );
    return ss.str();
}

bool filter_variant(
    Variant const& variant,
    VariantFilter const& filter
) {
    VariantFilterStats stats;
    return filter_variant( variant, filter, stats );
}

bool filter_variant(
    Variant const& variant,
    VariantFilter const& filter,
    VariantFilterStats& stats
) {
    // Needed for all below. Bit of overhead if we do no filtering at all...
    auto const total = merge_base_counts( variant );

    // Empty variants where all samples have zero counts are not interesting and filtered.
    auto const sum = nucleotide_sum( total );
    if( sum == 0 ) {
        ++stats.empty;
        return false;
    }

    // Coverage
    if( filter.min_coverage > 0 && sum < filter.min_coverage ) {
        ++stats.below_min_coverage;
        return false;
    }
    if( filter.max_coverage > 0 && sum > filter.max_coverage ) {
        ++stats.above_max_coverage;
        return false;
    }

    // Check deletions, independently of the SNP status.
    if(
        ! filter.tolerate_deletions
        && total.d_count > 0
        && filter.min_count > 0
        && total.d_count >= filter.min_count
    ) {
        ++stats.untolerated_deletion;
        return false;
    }

    // SNPs
    if( filter.only_snps || filter.only_biallelic_snps ) {
        if( filter.min_count == 0 && filter.max_count == 0 ) {
            // Check without any min or max counts. Just look for pure SNPs.
            // Has to be separated from the min/max count cases, as we might have minor allele
            // snps that we want to ignore, but which would be considered here,
            // for instance when deciding if a position is biallelic.
            auto const al_count = allele_count( total );
            if( filter.only_snps && al_count < 2 ) {
                ++stats.not_snp;
                return false;
            }
            if( filter.only_biallelic_snps && al_count != 2 ) {
                ++stats.not_biallelic_snp;
                return false;
            }
        } else {
            // Check with just the min count applied.
            // We check the two filters separately here, to be able to increment the correct counter.
            auto const al_count_min = allele_count(
                total, filter.min_count
            );
            if( filter.only_snps && al_count_min < 2 ) {
                ++stats.below_min_count;
                return false;
            }
            if( filter.only_biallelic_snps && al_count_min != 2 ) {
                ++stats.below_min_count;
                return false;
            }

            // And again, this time also considering the max count setting.
            if( filter.max_count > 0 ) {
                auto const al_count_min_max = allele_count(
                    total, filter.min_count, filter.max_count
                );
                if( filter.only_snps && al_count_min_max < 2 ) {
                    ++stats.above_max_count;
                    return false;
                }
                if( filter.only_biallelic_snps && al_count_min_max != 2 ) {
                    ++stats.above_max_count;
                    return false;
                }
            }
        }
    }

    // Frequency
    if( filter.min_frequency != 0.0 ) {
        // Input setting sanity check
        if(
            ! std::isfinite( filter.min_frequency ) ||
            filter.min_frequency < 0.0 ||
            filter.min_frequency > 1.0
        ) {
            throw std::runtime_error(
                "Invalid filter.min_frequency == " + std::to_string( filter.min_frequency )
            );
        }

        // Get the frequency, based on whether we can uses the bases or not.
        auto const ref_base = utils::to_upper( variant.reference_base );
        auto const alt_base = utils::to_upper( variant.alternative_base );
        size_t ref_cnt = 0;
        size_t alt_cnt = 0;
        if( ! is_valid_base( ref_base )) {

            // Invalid ref base, will use counts to determine frequency.
            auto const sorted_counts = sorted_base_counts( total );
            ref_cnt = sorted_counts[0].count;
            alt_cnt = sorted_counts[1].count;

        } else if( ! is_valid_base( alt_base )) {

            // Valid ref base, but invalid alt base. Will use ref base and second most common count.
            assert( is_valid_base( ref_base ));
            auto const sorted_counts = sorted_base_counts( variant, true );
            ref_cnt = sorted_counts[0].count;
            alt_cnt = sorted_counts[1].count;

        } else {

            // Both ref and alt base are valid.
            assert( is_valid_base( ref_base ));
            assert( is_valid_base( alt_base ));

            // Use them to determine frequency.
            ref_cnt = get_base_count( total, ref_base );
            alt_cnt = get_base_count( total, alt_base );
        }

        // Compute the frequency.
        auto const cnt_sum = ref_cnt + alt_cnt;
        auto const frequency = static_cast<double>( ref_cnt ) / static_cast<double>( cnt_sum );
        assert( ! std::isfinite( frequency ) || ( frequency >= 0.0 && frequency <= 1.0 ));

        // Now do the filtering
        if(
            ! std::isfinite( frequency ) ||
            frequency < filter.min_frequency ||
            1.0 - frequency < filter.min_frequency
        ) {
            ++stats.not_min_frequency;
            return false;
        }
    }

    ++stats.passed;
    return true;
}

} // namespace population
} // namespace genesis
