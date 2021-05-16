/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
//     F_ST Conventional Helper Functions
// =================================================================================================

std::tuple<double, double, double> f_st_conventional_pool_pi_snp(
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
//     F_ST Asymptotically Unbiased (Karlsson) Helper Functions
// =================================================================================================

FstAN f_st_asymptotically_unbiased_a1n1a2n2( BaseCounts const& p1, BaseCounts const& p2 )
{
    // get_a1a2n1n2
    // We do not want expensive sorting and looking for nucleotide characters here,
    // so instead, we use use sorting indices over arrays, and swap values in a sorting network
    // to quickly find the largest two frequencies. Neat.

    // Get frequencies in sample 1
    size_t const p1_cnts[] = { p1.a_count, p1.c_count, p1.g_count, p1.t_count };
    double const p1_nt_cnt = static_cast<double>( nucleotide_sum( p1 )); // eucov
    double const p1_freqs[] = {
        static_cast<double>( p1.a_count ) / p1_nt_cnt,
        static_cast<double>( p1.c_count ) / p1_nt_cnt,
        static_cast<double>( p1.g_count ) / p1_nt_cnt,
        static_cast<double>( p1.t_count ) / p1_nt_cnt
    };

    // Get frequencies in sample 2
    size_t const p2_cnts[] = { p2.a_count, p2.c_count, p2.g_count, p2.t_count };
    double const p2_nt_cnt = static_cast<double>( nucleotide_sum( p2 )); // eucov
    double const p2_freqs[] = {
        static_cast<double>( p2.a_count ) / p2_nt_cnt,
        static_cast<double>( p2.c_count ) / p2_nt_cnt,
        static_cast<double>( p2.g_count ) / p2_nt_cnt,
        static_cast<double>( p2.t_count ) / p2_nt_cnt
    };

    // Edge case. If there are no counts at all, we return empty.
    // The follow up function f_st_asymptotically_unbiased_nkdk() will also catch this edge case,
    // return zeros as well, and nothing will be added to the total F_ST sum.
    if( p1_nt_cnt == 0.0 || p2_nt_cnt == 0.0 ) {
        return FstAN{};
    }

    // Compute their averages.
    double const avg_freqs[] = {
        ( p1_freqs[0] + p2_freqs[0] ) / 2.0,
        ( p1_freqs[1] + p2_freqs[1] ) / 2.0,
        ( p1_freqs[2] + p2_freqs[2] ) / 2.0,
        ( p1_freqs[3] + p2_freqs[3] ) / 2.0
    };

    // Sort quickly via sorting network, putting large values first.
    // See https://stackoverflow.com/a/25070688/4184258
    // We however do not directly sort the values, as we instead need the sorting order
    // to retrieve the values from the original counts. So, we sort their indices instead,
    // using an array for simplicity of notation (all compiled away, and equivalent to using
    // index_a = 0; index_b = 1;... instead here).
    // Technically, there might be a better solution, as we are not interested in the order of
    // the last two values. But seriously, that won't safe enough to justify the effort.
    size_t indices[] = { 0, 1, 2, 3 };
    if( avg_freqs[indices[0]] < avg_freqs[indices[1]] ) {
        std::swap( indices[0], indices[1] );
    }
    if( avg_freqs[indices[2]] < avg_freqs[indices[3]] ) {
        std::swap( indices[2], indices[3] );
    }
    if( avg_freqs[indices[0]] < avg_freqs[indices[2]] ) {
        std::swap( indices[0], indices[2] );
    }
    if( avg_freqs[indices[1]] < avg_freqs[indices[3]] ) {
        std::swap( indices[1], indices[3] );
    }
    if( avg_freqs[indices[1]] < avg_freqs[indices[2]] ) {
        std::swap( indices[1], indices[2] );
    }

    // Now they are sorted, largest ones first.
    assert( avg_freqs[indices[0]] >= avg_freqs[indices[1]] );
    assert( avg_freqs[indices[1]] >= avg_freqs[indices[2]] );
    assert( avg_freqs[indices[2]] >= avg_freqs[indices[3]] );

    // Error check. We only want biallelic SNPs, so we check that the smallesst two values
    // here are actually zero.
    if( avg_freqs[indices[2]] != 0.0 || avg_freqs[indices[3]] != 0.0 ) {
        throw std::runtime_error(
            "In f_st_asymptotically_unbiased(): Expecting biallelic SNPs where only "
            "two counts are > 0, but found more non-zero counts."
        );
    }

    // We checked that we have biallelic counts here. Assert this again.
    assert( p1_freqs[indices[2]] == 0.0 && p1_freqs[indices[3]] == 0.0 );
    assert( p2_freqs[indices[2]] == 0.0 && p2_freqs[indices[3]] == 0.0 );
    assert( p1_cnts[indices[2]] == 0 && p1_cnts[indices[3]] == 0 );
    assert( p2_cnts[indices[2]] == 0 && p2_cnts[indices[3]] == 0 );

    // Prepare result. We use explict names here for clarity;
    // the compiler will get rid of these copies (hopefully).
    FstAN result;
    result.a_1 = static_cast<double>( p1_cnts[indices[0]] );
    result.n_1 = static_cast<double>( p1_cnts[indices[0]] + p1_cnts[indices[1]] );
    result.a_2 = static_cast<double>( p2_cnts[indices[0]] );
    result.n_2 = static_cast<double>( p2_cnts[indices[0]] + p2_cnts[indices[1]] );
    return result;
}

std::pair<double, double> f_st_asymptotically_unbiased_nkdk( FstAN const& fstan ) // calculate_nk_dk
{
    using namespace genesis::utils;

    // Edge case
    if( fstan.n_1 <= 1.0 || fstan.n_2 <= 1.0 ) {
        return { 0.0, 0.0 };
    }
    assert( fstan.n_1 > 1.0 );
    assert( fstan.n_2 > 1.0 );
    assert( fstan.a_1 <= fstan.n_1 );
    assert( fstan.a_2 <= fstan.n_2 );

    // calculate_h1, calculate_h2
    double const h1 = ( fstan.a_1 * ( fstan.n_1 - fstan.a_1 )) / ( fstan.n_1 * ( fstan.n_1 - 1.0 ));
    double const h2 = ( fstan.a_2 * ( fstan.n_2 - fstan.a_2 )) / ( fstan.n_2 * ( fstan.n_2 - 1.0 ));

    // calculate_nk_dk
    double const sqr = squared(( fstan.a_1 / fstan.n_1 ) - ( fstan.a_2 / fstan.n_2 ));
    double const sub = ( h1 / fstan.n_1 + h2 / fstan.n_2 );
    double const nk = sqr - sub;
    double const dk = nk + h1 + h2;

    return { nk, dk };
}

} // namespace population
} // namespace genesis
