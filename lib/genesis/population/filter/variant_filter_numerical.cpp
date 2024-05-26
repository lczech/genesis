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

#include "genesis/population/filter/variant_filter_numerical.hpp"

#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Filter Numerical
// =================================================================================================

bool apply_variant_filter_numerical(
    Variant& variant,
    VariantFilterNumericalParams const& params,
    VariantFilterStats& stats
) {
    // We do not filter further if the position has already been determined to be filered out.
    if( ! variant.status.passing() ) {
        return false;
    }

    // Get the combined sum of all (passing) samples. Needed for all below.
    auto const total = merge_sample_counts( variant, SampleCountsFilterPolicy::kOnlyPassing );

    // -------------------------------------------
    //     Numeric
    // -------------------------------------------

    // Empty variants where all samples have zero counts are not interesting and filtered.
    auto const sum = nucleotide_sum( total );
    if( sum == 0 ) {
        variant.status.set( VariantFilterTag::kEmpty );
        ++stats[VariantFilterTag::kEmpty];
        return false;
    }

    // ReadDepth
    if( params.min_read_depth > 0 && sum < params.min_read_depth ) {
        variant.status.set( VariantFilterTag::kBelowMinReadDepth );
        ++stats[VariantFilterTag::kBelowMinReadDepth];
        return false;
    }
    if( params.max_read_depth > 0 && sum > params.max_read_depth ) {
        variant.status.set( VariantFilterTag::kAboveMaxReadDepth );
        ++stats[VariantFilterTag::kAboveMaxReadDepth];
        return false;
    }

    // Check deletions, independently of the SNP status.
    if(
        params.deletions_count_limit > 0
        && total.d_count > 0
        && total.d_count >= params.deletions_count_limit
    ) {
        variant.status.set( VariantFilterTag::kAboveDeletionsCountLimit );
        ++stats[VariantFilterTag::kAboveDeletionsCountLimit];
        return false;
    }

    // -------------------------------------------
    //     SNP vs Invariant
    // -------------------------------------------

    // Everything below from here is only applied if we filter for SNPs
    if( ! params.only_snps && ! params.only_biallelic_snps ) {
        return true;
    }

    // SNPs
    if( params.snp_min_count == 0 && params.snp_max_count == 0 ) {
        // Check without any min or max counts. Just look for pure SNPs.
        // Has to be separated from the min/max count cases, as we might have minor allele
        // snps that we want to ignore, but which would be considered here,
        // for instance when deciding if a position is biallelic.
        auto const al_count = allele_count( total );
        if( params.only_snps && al_count < 2 ) {
            variant.status.set( VariantFilterTag::kNotSnp );
            ++stats[VariantFilterTag::kNotSnp];
            return false;
        }
        if( params.only_biallelic_snps && al_count != 2 ) {
            variant.status.set( VariantFilterTag::kNotBiallelicSnp );
            ++stats[VariantFilterTag::kNotBiallelicSnp];
            return false;
        }
    } else {
        // Check with just the min count applied.
        // We check the two filters separately here, to be able to increment the correct counter.
        auto const al_count_min = allele_count(
            total, params.snp_min_count
        );
        if( params.only_snps && al_count_min < 2 ) {
            variant.status.set( VariantFilterTag::kBelowSnpMinCount );
            ++stats[VariantFilterTag::kBelowSnpMinCount];
            return false;
        }
        if( params.only_biallelic_snps && al_count_min != 2 ) {
            variant.status.set( VariantFilterTag::kBelowSnpMinCount );
            ++stats[VariantFilterTag::kBelowSnpMinCount];
            return false;
        }

        // And again, this time also considering the max count setting.
        if( params.snp_max_count > 0 ) {
            auto const al_count_min_max = allele_count(
                total, params.snp_min_count, params.snp_max_count
            );
            if( params.only_snps && al_count_min_max < 2 ) {
                variant.status.set( VariantFilterTag::kAboveSnpMaxCount );
                ++stats[VariantFilterTag::kAboveSnpMaxCount];
                return false;
            }
            if( params.only_biallelic_snps && al_count_min_max != 2 ) {
                variant.status.set( VariantFilterTag::kAboveSnpMaxCount );
                ++stats[VariantFilterTag::kAboveSnpMaxCount];
                return false;
            }
        }
    }

    // -------------------------------------------
    //     Allele frequency
    // -------------------------------------------

    // Frequency
    assert( params.only_snps || params.only_biallelic_snps );
    if( params.snp_min_allele_frequency != 0.0 ) {
        // Input setting sanity check
        if(
            ! std::isfinite( params.snp_min_allele_frequency ) ||
            params.snp_min_allele_frequency < 0.0 ||
            params.snp_min_allele_frequency > 1.0
        ) {
            throw std::runtime_error(
                "Invalid VariantFilterNumericalParams::snp_min_allele_frequency == " +
                std::to_string( params.snp_min_allele_frequency )
            );
        }

        // Get the frequency, based on whether we can uses the bases or not.
        auto const ref_base = utils::to_upper( variant.reference_base );
        auto const alt_base = utils::to_upper( variant.alternative_base );
        size_t ref_cnt = 0;
        size_t alt_cnt = 0;
        if( ! is_valid_base( ref_base )) {

            // Invalid ref base, will use counts to determine frequency.
            auto const sorted_counts = sorted_sample_counts( total );
            ref_cnt = sorted_counts[0].count;
            alt_cnt = sorted_counts[1].count;

        } else if( ! is_valid_base( alt_base )) {

            // Valid ref base, but invalid alt base. Will use ref base and second most common count.
            assert( is_valid_base( ref_base ));
            auto const sorted_counts = sorted_sample_counts(
                variant, true, SampleCountsFilterPolicy::kOnlyPassing
            );
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
            frequency < params.snp_min_allele_frequency ||
            1.0 - frequency < params.snp_min_allele_frequency
        ) {
            variant.status.set( VariantFilterTag::kBelowMinAlleleFreq );
            ++stats[VariantFilterTag::kBelowMinAlleleFreq];
            return false;
        }
    }

    // Everything passed.
    return true;
}

bool apply_variant_filter_numerical(
    Variant& variant,
    VariantFilterNumericalParams const& params
) {
    VariantFilterStats stats{};
    return apply_variant_filter_numerical( variant, params, stats );
}

} // namespace population
} // namespace genesis
