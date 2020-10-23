/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/functions/structure.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     F_ST Conventional
// =================================================================================================

std::tuple<double, double, double> f_st_conventional_pool_pi_snp(
    PoolSample const& p1, PoolSample const& p2
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

    return { p1_pi, p2_pi, pp_pi };
}

} // namespace population
} // namespace genesis
