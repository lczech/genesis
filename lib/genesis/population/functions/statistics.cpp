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

#include "genesis/population/functions/statistics.hpp"

#include "genesis/utils/containers/simple_cache.hpp"
#include "genesis/utils/math/common.hpp"

#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

/**
 * @brief Local helper function to compute values for the denominator.
 */
static double amnm_( // get_aMnm_buffer
    size_t poolsize,         // n
    size_t nucleotide_count, // M (coverage)
    size_t allele_frequency  // m, m_it (running variable for b .. M-b)
) {
    // The terminology in PoPoolation is confusing and differs completely from the paper,
    // their code is not well documented, and their binomial_term function combines other aspects
    // of the computation than just computing the binomial distribution,
    // hence adding to the confusion. Let's try to disentangle:
    //
    // It seems, we want a binomial distribution with n being the coverage/nucleotide_count/M,
    // and k being the allele_frequency/m, and we want p being r (1..poolsize-1) divided by poolsize,
    // using the r from the below loop (which confusingly is also called k in PoPoolation).
    // What a mess.

    double result = 0.0;
    for( size_t r = 1; r <= poolsize - 1; ++r ) {
        double const p = static_cast<double>( r ) / static_cast<double>( poolsize );
        double const binom = utils::binomial_distribution( allele_frequency, nucleotide_count, p );
        result += binom / static_cast<double>( r );
    }
    return result;
}

/**
 * @brief Cache for the above function. Can be called as if it was the function itself.
 */
static genesis::utils::SimpleCache<double, size_t, size_t, size_t>
amnm_cache_{ amnm_ };

// =================================================================================================
//     Diversity Estimates
// =================================================================================================

double heterozygosity( PoolSample const& sample )
{
    double h = 1.0;
    double const nt_cnt = static_cast<double>( nucleotide_sum( sample ));
    h -= std::pow( static_cast<double>( sample.a_count ) / nt_cnt, 2 );
    h -= std::pow( static_cast<double>( sample.c_count ) / nt_cnt, 2 );
    h -= std::pow( static_cast<double>( sample.g_count ) / nt_cnt, 2 );
    h -= std::pow( static_cast<double>( sample.t_count ) / nt_cnt, 2 );
    h *= nt_cnt / ( nt_cnt - 1.0 );
    return h;
}

// =================================================================================================
//     Theta Pi
// =================================================================================================

/**
 * @brief Local function for computing the theta pi denominator.
 *
 * We relay the computation to this local function, so that we can use a cache, see below.
 */
static double theta_pi_pool_denominator_( // get_pidiv_buffer
    size_t poolsize,         // n
    size_t min_allele_count, // b
    size_t nucleotide_count  // M
) {
    // Boundary: if not held, we return zero, and that would not be a useful denominator.
    if( 2 * min_allele_count > nucleotide_count ) {
        throw std::invalid_argument(
            "Cannot compute theta_pi_pool_denominator with min_allele_count = " +
            std::to_string( min_allele_count ) + " and nucleotide_count = " +
            std::to_string( nucleotide_count )
        );
    }

    // Iterate all allele frequencies in between the min and max-min boundaries.
    double div = 0.0;
    for( size_t m_it = min_allele_count; m_it <= ( nucleotide_count - min_allele_count ); ++m_it ) {
        // We iterate from b to M-b (in PoPoolation terminology), inclusively.
        // Use double values however for the computations.
        double const m = static_cast<double>( m_it );
        double const M = static_cast<double>( nucleotide_count );

        // Compute the term. We here use the cache, which also computes results if not yet cached.
        double term = ( 2.0 * m * ( M - m )) / ( M * ( M - 1.0 ));
        div += term * amnm_cache_( poolsize, nucleotide_count, m_it );
    }
    return div;
}

/**
 * @brief Cache for the above function. Can be called as if it was the function itself.
 */
static genesis::utils::SimpleCache<double, size_t, size_t, size_t>
theta_pi_pool_denominator_cache_{ theta_pi_pool_denominator_ };

double theta_pi_pool_denominator(
    size_t poolsize,         // n
    size_t min_allele_count, // b
    size_t nucleotide_count  // M
) {
    // Simply return the cached value (which computes them first if not yet cached).
    return theta_pi_pool_denominator_cache_( poolsize, min_allele_count, nucleotide_count );
}

// =================================================================================================
//     Cache Access
// =================================================================================================

#ifdef DEBUG

genesis::utils::SimpleCache<double, size_t, size_t, size_t>& amnm_cache()
{
    return amnm_cache_;
}

genesis::utils::SimpleCache<double, size_t, size_t, size_t>& theta_pi_pool_denominator_cache()
{
    return theta_pi_pool_denominator_cache_;
}

#endif // DEBUG

} // namespace population
} // namespace genesis
