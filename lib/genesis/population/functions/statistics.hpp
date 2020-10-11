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
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/simple_cache.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
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
 * > Hahn, M. W.<br>(2018). Molecular Population Genetics.<br>
 * > https://global.oup.com/academic/product/molecular-population-genetics-9780878939657
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
 * We here compute the denominator for a given @p poolsize, with a fix @p min_allele_count,
 * which is identical for each given @p nucleotide_count, and henced cached internally for speedup.
 *
 * See
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte,
 * > A. Futschik, C. Kosiol, C. Schlötterer.<br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.<br>
 * > (2011) PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * for details. The paper unfortunately does not explain their equations, but there is a hidden
 * document in their code repository that illuminates the situation a bit. See
 * https://sourceforge.net/projects/popoolation/files/correction_equations.pdf
 */
double theta_pi_pool_denominator( size_t poolsize, size_t min_allele_count, size_t nucleotide_count );

/**
 * @brief Compute classic theta pi, that is, the sum of heterozygosities.
 *
 * The function simply sums heterozygosity() for all samples in the given range.
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
 * See theta_pi_pool_denominator() for details.
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
//     Theta Watterson
// =================================================================================================

/**
 * @brief Compute the denominator for the pool-sequencing correction of theta watterson according to
 * Kofler et al.
 *
 * @copydetails theta_pi_pool_denominator()
 */
double theta_watterson_pool_denominator(
    size_t poolsize, size_t min_allele_count, size_t nucleotide_count
);

/**
 * @brief Compute theta watterson with pool-sequencing correction according to Kofler et al.
 */
template<class ForwardIterator>
double theta_watterson_pool( // get_theta_calculator
    ForwardIterator begin,
    ForwardIterator end,
    size_t poolsize,        // n
    size_t min_allele_count // b
) {
    double sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        auto const denom = theta_watterson_pool_denominator(
            poolsize, min_allele_count, nucleotide_sum( *it )
        );
        sum += 1.0 / denom;
    }
    return sum;
}

// =================================================================================================
//     Tajima's D
// =================================================================================================

/**
 * @brief Compute `a_n`, the sum of reciprocals.
 *
 * This is the sum of reciprocals up to `n-1`: \f$ a_n = \sum_{i=1}^{n-1} \frac{1}{i} \f$.
 *
 * See Equation 3.6 in
 *
 * > Hahn, M. W. (2018). Molecular Population Genetics.
 * > https://global.oup.com/academic/product/molecular-population-genetics-9780878939657
 *
 * for details.
 */
double a_n( size_t n );

/**
 * @brief Compute `b_n`, the sum of squared reciprocals.
 *
 * This is the sum of squared reciprocals up to `n-1`: \f$ b_n = \sum_{i=1}^{n-1} \frac{1}{i^2} \f$.
 *
 * See
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte,
 * > A. Futschik, C. Kosiol, C. Schlötterer.<br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.<br>
 * > (2011) PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * for details. The paper unfortunately does not explain their equations, but there is a hidden
 * document in their code repository that illuminates the situation a bit. See
 * https://sourceforge.net/projects/popoolation/files/correction_equations.pdf
 */
double b_n( size_t n );

/**
 * @brief Compute `f*` according to Achaz 2008 and Kofler et al. 2011.
 *
 * This is compuated as \f$ f_{star} = \frac{n - 3}{a_n \cdot (n-1) - n} \f$, and needed for the
 * computation of alpha_star() and beta_star(). See there for some more details, and see
 *
 * > G. Achaz.<br>
 * > Testing for neutrality in samples with sequencing errors.<br>
 * > (2008) Genetics, 179(3), 1409–1424. https://doi.org/10.1534/genetics.107.082198
 *
 * for the original equations.
 */
double f_star( double a_n, double n );

/**
 * @brief Compute `alpha*` according to Achaz 2008 and Kofler et al. 2011.
 *
 * This is needed for the computation of tajima_d_pool() according to
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte,
 * > A. Futschik, C. Kosiol, C. Schlötterer.<br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.<br>
 * > (2011) PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * The paper unfortunately does not explain their equations, but there is a hidden
 * document in their code repository that illuminates the situation a bit. See
 * https://sourceforge.net/projects/popoolation/files/correction_equations.pdf
 *
 * The equation is based on
 *
 * > G. Achaz.<br>
 * > Testing for neutrality in samples with sequencing errors.<br>
 * > (2008) Genetics, 179(3), 1409–1424. https://doi.org/10.1534/genetics.107.082198
 *
 * See there for details.
 */
double alpha_star( double n );

/**
 * @brief Compute `beta*` according to Achaz 2008 and Kofler et al. 2011.
 *
 * @copydetails alpha_star()
 */
double beta_star( double n );

/**
 * @brief Compute the `n_base` term used for Tajima's D in Kofler et al. 2011,
 * following their approach.
 *
 * This term is the expected number of distinct individuals sequenced, which is equivalent to
 * finding the expected number of distinct values selected from a set of integers.
 *
 * The computation of this term in PoPoolation uses a recursive dynamic programming approach to
 * sum over different possibilities of selecting sets of integers. This gets rather slow for
 * larger inputs, and there is an equivalent closed form that we here use instead.
 * See n_base() for details. We here merely offer the original PoPoolation implementation as a
 * point of reference.
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte,
 * > A. Futschik, C. Kosiol, C. Schlötterer.<br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.<br>
 * > (2011) PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * The paper unfortunately does not explain their equations, but there is a hidden
 * document in their code repository that illuminates the situation a bit. See
 * https://sourceforge.net/projects/popoolation/files/correction_equations.pdf
 */
double n_base_matrix( size_t coverage, size_t poolsize );

/**
 * @brief Compute the `n_base` term used for Tajima's D in Kofler et al. 2011, using a faster
 * closed form expression.
 *
 * This term is the expected number of distinct individuals sequenced, which is equivalent to
 * finding the expected number of distinct values selected from a set of integers.
 *
 * The computation in PoPoolation is slowm, see n_base_matrix(). We here instead use a closed
 * form expression following the reasoning of https://math.stackexchange.com/a/72351
 * See there for the derivation of the equation.
 */
double n_base( size_t coverage, size_t poolsize );

/**
 * @brief Compute the denominator for the pool-sequencing correction of Tajima's D according to
 * Kofler et al.
 */
double tajima_d_pool_denominator(
    size_t poolsize,         // n
    size_t min_coverage,     // mincoverage = min_allele_count? b? TODO
    size_t snp_count,
    double theta
);

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 */
template<class ForwardIterator>
double tajima_d_pool( // get_D_calculator
    ForwardIterator begin,
    ForwardIterator end,
    size_t poolsize,         // n
    size_t min_allele_count, // b
    size_t min_coverage,
    double theta_pi,
    double theta_watterson
) {
    // Edge case, following what PoPoolation does in this situation.
    if( begin == end ) {
        return 0.0;
    }
    (void) min_allele_count;

    // We already have the two theta statistics given here, but need to compute the
    // denominator according to Kofler et al for pooled sequences.
    auto const snp_cnt = std::distance( begin, end );
    auto const denom = tajima_d_pool_denominator( poolsize, min_coverage, snp_cnt, theta_watterson );
    return ( theta_pi - theta_watterson ) / denom;
}

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 */
template<class ForwardIterator>
double tajima_d_pool( // get_D_calculator
    ForwardIterator begin,
    ForwardIterator end,
    size_t poolsize,         // n
    size_t min_allele_count, // b
    size_t min_coverage
) {
    // First compute the two theta statistics, then call the other version of this function.
    auto const pi = theta_pi_pool( begin, end, poolsize, min_allele_count );
    auto const tw = theta_watterson_pool( begin, end, poolsize, min_allele_count );
    return tajima_d_pool( begin, end, poolsize, min_allele_count, min_coverage, pi, tw );
}

} // namespace population
} // namespace genesis

#endif // include guard
