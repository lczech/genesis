/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/functions/structure.hpp"

#include "genesis/population/functions/base_counts.hpp"
#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     F_ST Pool Kofler
// =================================================================================================

std::tuple<double, double, double> f_st_pool_kofler_pi_snp(
    BaseCounts const& p1, BaseCounts const& p2
) {
    using namespace genesis::utils;

    // _pi / _uncorrectedPiPerSNPFromFreqs
    auto pi_snp_ = [](
        double freq_a, double freq_c, double freq_g, double freq_t, double nt_cnt
    ) {
        double result = 1.0;
        result -= squared( freq_a );
        result -= squared( freq_c );
        result -= squared( freq_g );
        result -= squared( freq_t );
        result *= nt_cnt / ( nt_cnt - 1.0 );
        return result;
    };

    // _calculateSNPFrequencies
    // We cannot/do not want to simply call our heterozygosity() function here, as we need to
    // re-use the frequencies anyway to compute their average, so we do everything at once here.

    // Get frequencies in sample 1
    double const p1_nt_cnt = static_cast<double>( nucleotide_sum( p1 )); // eucov
    double const p1_freq_a = static_cast<double>( p1.a_count ) / p1_nt_cnt;
    double const p1_freq_c = static_cast<double>( p1.c_count ) / p1_nt_cnt;
    double const p1_freq_g = static_cast<double>( p1.g_count ) / p1_nt_cnt;
    double const p1_freq_t = static_cast<double>( p1.t_count ) / p1_nt_cnt;

    // Get frequencies in sample 2
    double const p2_nt_cnt = static_cast<double>( nucleotide_sum( p2 )); // eucov
    double const p2_freq_a = static_cast<double>( p2.a_count ) / p2_nt_cnt;
    double const p2_freq_c = static_cast<double>( p2.c_count ) / p2_nt_cnt;
    double const p2_freq_g = static_cast<double>( p2.g_count ) / p2_nt_cnt;
    double const p2_freq_t = static_cast<double>( p2.t_count ) / p2_nt_cnt;

    // Compute their average
    double const min_cnt = std::min( p1_nt_cnt, p2_nt_cnt );
    double const avg_a = ( p1_freq_a + p2_freq_a ) / 2.0;
    double const avg_c = ( p1_freq_c + p2_freq_c ) / 2.0;
    double const avg_g = ( p1_freq_g + p2_freq_g ) / 2.0;
    double const avg_t = ( p1_freq_t + p2_freq_t ) / 2.0;

    // _calculatePivalues / _pi / _uncorrectedPiPerSNPFromFreqs
    auto const p1_pi = pi_snp_( p1_freq_a, p1_freq_c, p1_freq_g, p1_freq_t, p1_nt_cnt );
    auto const p2_pi = pi_snp_( p2_freq_a, p2_freq_c, p2_freq_g, p2_freq_t, p2_nt_cnt );
    auto const pp_pi = pi_snp_( avg_a, avg_c, avg_g, avg_t, min_cnt );

    return std::tuple<double, double, double>{ p1_pi, p2_pi, pp_pi };
}

// =================================================================================================
//     F_ST Pool Karlsson
// =================================================================================================

std::pair<double, double> f_st_pool_karlsson_nkdk(
    std::pair<SortedBaseCounts, SortedBaseCounts> const& sample_counts
) {
    // PoPoolation2 function: calculate_nk_dk
    using namespace genesis::utils;

    // Error check. We only want biallelic SNPs, so we check that the smallesst two values
    // here are actually zero.
    if(
        sample_counts.first[2].count  != 0 || sample_counts.first[3].count  != 0 ||
        sample_counts.second[2].count != 0 || sample_counts.second[3].count != 0
    ) {
        throw std::runtime_error(
            "In f_st_asymptotically_unbiased(): Expecting biallelic SNPs where only "
            "two nucleotide counts are > 0, but found more non-zero counts."
        );
    }

    // We checked that we have biallelic counts here. Assert this again.
    // Also assert that the bases are in the same order in both samples.
    assert( sample_counts.first[2].count  == 0 && sample_counts.first[3].count  == 0 );
    assert( sample_counts.second[2].count == 0 && sample_counts.second[3].count == 0 );
    assert(
        sample_counts.first[0].base == sample_counts.second[0].base &&
        sample_counts.first[1].base == sample_counts.second[1].base &&
        sample_counts.first[2].base == sample_counts.second[2].base &&
        sample_counts.first[3].base == sample_counts.second[3].base
    );

    // Get the major allele count (`a` here and in PoPoolation2),
    // the minor allele count (`b` here, not used in PoPoolation2 under that name),
    // and the total coverage (`n` here and in PoPoolation2).
    auto const a_1 = static_cast<double>( sample_counts.first[0].count );
    auto const b_1 = static_cast<double>( sample_counts.first[1].count );
    auto const n_1 = a_1 + b_1;
    auto const a_2 = static_cast<double>( sample_counts.second[0].count );
    auto const b_2 = static_cast<double>( sample_counts.second[1].count );
    auto const n_2 = a_2 + b_2;

    // Edge case
    if( n_1 <= 1.0 || n_2 <= 1.0 ) {
        return { 0.0, 0.0 };
    }
    assert( n_1 > 1.0 );
    assert( n_2 > 1.0 );
    assert( a_1 <= n_1 );
    assert( a_2 <= n_2 );

    // PoPoolation2 functions: calculate_h1, calculate_h2
    double const h1 = ( a_1 * b_1 ) / ( n_1 * ( n_1 - 1.0 ));
    double const h2 = ( a_2 * b_2 ) / ( n_2 * ( n_2 - 1.0 ));

    // PoPoolation2 function: calculate_nk_dk
    double const sqr = squared(( a_1 / n_1 ) - ( a_2 / n_2 ));
    double const sub = ( h1 / n_1 + h2 / n_2 );
    double const nk = sqr - sub;
    double const dk = nk + h1 + h2;

    return { nk, dk };
}

} // namespace population
} // namespace genesis
