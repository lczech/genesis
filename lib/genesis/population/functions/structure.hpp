#ifndef GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_
#define GENESIS_POPULATION_FUNCTIONS_STRUCTURE_H_

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

#include "genesis/population/functions/pool_sample.hpp"
#include "genesis/population/pool_sample.hpp"
#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Population Structure
// =================================================================================================

/**
 * @brief Compute the conventional F_ST statistic for pool-sequenced data,
 * following Kofler et al.
 */
template<class ForwardIterator1, class ForwardIterator2>
double f_st_conventional_pool( // get_conventional_fstcalculator
    size_t p1_poolsize, size_t p2_poolsize,
    ForwardIterator1 p1_begin, ForwardIterator1 p1_end,
    ForwardIterator2 p2_begin, ForwardIterator2 p2_end
) {
    using namespace genesis::utils;

    // Edge and error cases
    if( p1_poolsize <= 1 || p2_poolsize <= 1 ) {
        throw std::invalid_argument( "Cannot run fst_pool() with poolsizes <= 1" );
    }

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

    // Theta Pi values for the two populations and their combination
    double p1_pi_sum = 0.0;
    double p2_pi_sum = 0.0;
    double pp_pi_sum = 0.0;

    auto p1_it = p1_begin;
    auto p2_it = p2_begin;
    while( p1_it != p1_end && p2_it != p2_end ) {
        // _calculateSNPFrequencies

        // Get frequencies in sample 1
        double const p1_nt_cnt = static_cast<double>( nucleotide_sum( *p1_it )); // eucov
        double const p1_freq_a = static_cast<double>( p1_it->a_count ) / p1_nt_cnt;
        double const p1_freq_c = static_cast<double>( p1_it->c_count ) / p1_nt_cnt;
        double const p1_freq_g = static_cast<double>( p1_it->g_count ) / p1_nt_cnt;
        double const p1_freq_t = static_cast<double>( p1_it->t_count ) / p1_nt_cnt;

        // Get frequencies in sample 2
        double const p2_nt_cnt = static_cast<double>( nucleotide_sum( *p2_it )); // eucov
        double const p2_freq_a = static_cast<double>( p2_it->a_count ) / p2_nt_cnt;
        double const p2_freq_c = static_cast<double>( p2_it->c_count ) / p2_nt_cnt;
        double const p2_freq_g = static_cast<double>( p2_it->g_count ) / p2_nt_cnt;
        double const p2_freq_t = static_cast<double>( p2_it->t_count ) / p2_nt_cnt;

        // Compute their average
        double const min_cnt = std::min( p1_nt_cnt, p2_nt_cnt );
        double const avg_a = ( p1_freq_a + p2_freq_a ) / 2.0;
        double const avg_c = ( p1_freq_c + p2_freq_c ) / 2.0;
        double const avg_g = ( p1_freq_g + p2_freq_g ) / 2.0;
        double const avg_t = ( p1_freq_t + p2_freq_t ) / 2.0;

        // _calculatePivalues / _pi / _uncorrectedPiPerSNPFromFreqs
        p1_pi_sum += pi_snp_( p1_freq_a, p1_freq_c, p1_freq_g, p1_freq_t, p1_nt_cnt );
        p2_pi_sum += pi_snp_( p2_freq_a, p2_freq_c, p2_freq_g, p2_freq_t, p2_nt_cnt );
        pp_pi_sum += pi_snp_( avg_a, avg_c, avg_g, avg_t, min_cnt );

        // Next pair of entries
        ++p1_it;
        ++p2_it;
    }
    if( p1_it != p1_end || p2_it != p2_end ) {
        throw std::invalid_argument( "In fst_pool(): Provided ranges have different length." );
    }

    // Normalize by poolsize
    assert( p1_poolsize > 1 && p2_poolsize > 1 );
    size_t const pp_poolsize = std::min( p1_poolsize, p2_poolsize );
    p1_pi_sum *= static_cast<double>( p1_poolsize ) / static_cast<double>( p1_poolsize - 1 );
    p2_pi_sum *= static_cast<double>( p2_poolsize ) / static_cast<double>( p2_poolsize - 1 );
    pp_pi_sum *= static_cast<double>( pp_poolsize ) / static_cast<double>( pp_poolsize - 1 );

    // _calculateFstValues
    double const pp_avg = ( p1_pi_sum + p2_pi_sum ) / 2.0;
    return ( pp_pi_sum - pp_avg ) / pp_pi_sum;
}

} // namespace population
} // namespace genesis

#endif // include guard
