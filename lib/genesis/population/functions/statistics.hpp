#ifndef GENESIS_POPULATION_FUNCTIONS_STATISTICS_H_
#define GENESIS_POPULATION_FUNCTIONS_STATISTICS_H_

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
#include "genesis/utils/containers/simple_cache.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Diversity Estimates
// =================================================================================================

/**
 * @brief Compute classic heterozygosity.
 *
 * This is computed as \f$ h = \frac{n}{n-1} \left( 1 - \sum p^2 \right) \f$ with `n` the total
 * nucleotide_sum() (sum of `A`,`C`,`G`,`T` in the sample), and `p` their respective nucleotide
 * frequencies.
 *
 * See Equation 3.1 in
 *
 * > Hahn, M. W. (2018). Molecular Population Genetics.
 * > https://global.oup.com/academic/product/molecular-population-genetics-9780878939657?cc=us&lang=en&
 *
 * for details.
 */
double heterozygosity( PoolSample const& sample );

// =================================================================================================
//     Theta Pi
// =================================================================================================

/**
 * @brief Compute the denominator for the pool-sequencing correction of theta pi according to
 * Kofler et al.
 *
 * See
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte,
 * > A. Futschik, C. Kosiol, C.Schlötterer.</br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.</br>
 * > 2011. PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * for details. We here compute the denominator for a given @p poolsize, with a fix @p min_allele_count,
 * which is identical for each given @p nucleotide_count, and henced cached internally for speedup.
 */
double theta_pi_pool_denominator( size_t poolsize, size_t min_allele_count, size_t nucleotide_count );

/**
 * @brief Compute classic theta pi, that is, the sum of heterozygosities.
 *
 * The function sums heterozygosity() for all samples in the given range.
 */
template<class ForwardIterator>
double theta_pi(
    ForwardIterator begin, ForwardIterator end
) {
    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        pi_sum += heterozygosity( *it );
    }
    return pi_sum;
}

/**
 * @brief Compute theta pi with pool-sequencing correction according to Kofler et al,
 * that is, the sum of heterozygosities divided by the correction denominator.
 *
 * The function sums heterozygosity() for all samples in the given range, and divides each
 * by the respective denominator to correct for error from pool sequencing.
 *
 * See
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte, A.
 * > Futschik, C. Kosiol, C.Schlötterer.</br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.</br>
 * > 2011. PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * for details.
 */
template<class ForwardIterator>
double theta_pi_pool( // get_pi_calculator
    ForwardIterator begin,
    ForwardIterator end,
    size_t poolsize,        // n
    size_t min_allele_count // b
) {
    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        double pi_snp = heterozygosity( *it );
        pi_snp /= theta_pi_pool_denominator( poolsize, min_allele_count, nucleotide_sum( *it ) );
        pi_sum += pi_snp;
    }
    return pi_sum;
}

// =================================================================================================
//     Cache Access
// =================================================================================================

#ifdef DEBUG

genesis::utils::SimpleCache<double, size_t, size_t, size_t>& amnm_cache();
genesis::utils::SimpleCache<double, size_t, size_t, size_t>& theta_pi_pool_denominator_cache();

#endif // DEBUG

} // namespace population
} // namespace genesis

#endif // include guard
