#ifndef GENESIS_POPULATION_FUNCTIONS_DIVERSITY_POOL_FUNCTIONS_H_
#define GENESIS_POPULATION_FUNCTIONS_DIVERSITY_POOL_FUNCTIONS_H_

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
#include "genesis/population/functions/functions.hpp"
#include "genesis/population/variant.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Diversity Pool Settings
// =================================================================================================

/**
 * @brief Select how to compute the denominator for the pool sequencing correction of Tajima's D.
 */
enum class TajimaDenominatorPolicy
{
    /**
     * @brief Do not correct Tajima's D at all.
     *
     * Deriving a valid correction for Tajima's D in the context of pool sequencing is very tricky,
     * and coming up with estimators that correct for all biases and noises is hard.
     * It involves knowing about the covariance of frequencies across sites, which again
     * has a demographic component (How has the randomness from pool sequencing affected the
     * sites?), and a pool sequencing component (How does the randomness in the allele
     * frequencies at the sites vary?), which seems rather complicated to derive and use.
     *
     * So instead, we here simply use _no_ correction at all. Hence, values cannot be interpreted
     * absolutely, and are not comparable to values of classic (non-pool-sequence) Tajima's D.
     * Still, knowing their sign, and comparing them relative to each other across windows,
     * might yield valuable insight.
     */
    kUncorrected,

    /**
     * @brief Replicate the original behaviour of PoPoolation <= 1.2.2.
     *
     * There are two major bugs (as far as we are aware) in the PoPoolation implementation
     * up until (and including) version 1.2.2:
     *
     *  1. They compute the empirical pool size (expeted number of individuals sequenced) as
     *     n_base(), based on poolsize alone, and do not take the coverage into account at all.
     *  2. They do not use alpha star, but set it to be equal to beta star instead.
     *
     * Using this option, one can voluntarily activate these bugs here as well, in order to get
     * results that are comparable with PoPoolation results.
     */
    kWithPopoolatioBugs,

    /**
     * @brief Fix the bugs of the original PoPoolation, but still use their way of computing
     * the empirical pool size via n_base().
     *
     * With the two bugs of PoPoolation fixed, they still use the user-provided min_coverage
     * (also a setting here) as input for the n_base() function to compute the empirical pool size.
     * We think that this is not ideal, and gives wrong estimates of the number of individuals
     * sequenced. Still, we offer this behaviour here, as a means to compute what we think
     * PoPoolation _intended_ to compute without their more obvious bugs.
     */
    kWithoutPopoolatioBugs,

    /**
     * @brief Use the empirical minimum coverage found in each window for the empirical pool size
     * instead of n_base().
     *
     * This is a conservative estimator that in our assessment makes more sense to use than
     * the user-provided minimum coverage setting.
     */
    kEmpiricalMinCoverage,

    /**
     * @brief Instead of using n_base() to obtain the number of individuals sequenced (empirical
     * pool size), simply use the poolsize directly.
     *
     * This is another estimator, that does not use n_base() at all, and just assumes that
     * the number of individuals sequenced is equal to the pool size.
     */
    kPoolsize
};

/**
 * @brief Settings used by different pool-sequencing corrected diversity statistics.
 *
 * These settings are used by DiversityPoolCalculator, and for example by theta_pi_pool(),
 * theta_watterson_pool(), and tajima_d_pool(), in order to have them in a central place,
 * and avoid ordering confusion of function arguments, that would result from having to provide
 * them individually.
 *
 * Note in particular the setting tajima_denominator_policy, which controls how we correct
 * the denominator the computation of Tajima's D.
 */
struct DiversityPoolSettings
{
    size_t min_count = 0;
    size_t min_coverage = 0;
    size_t max_coverage = 0;

    TajimaDenominatorPolicy tajima_denominator_policy = TajimaDenominatorPolicy::kUncorrected;
};

// =================================================================================================
//     Theta Pi
// =================================================================================================

/**
 * @brief Compute classic heterozygosity.
 *
 * This is computed as \f$ h = \frac{n}{n-1} \left( 1 - \sum p^2 \right) \f$ with `n` the total
 * nucleotide_sum() (sum of `A`,`C`,`G`,`T` in the sample), and `p` their respective nucleotide
 * frequencies, with @p with_bessel, or without Bessel's correction in the beginning of the equation
 * when @p with_bessel is set to `false` (default).
 *
 * See Equation 3.1 in
 *
 * > Hahn, M. W.<br>(2018). Molecular Population Genetics.<br>
 * > https://global.oup.com/academic/product/molecular-population-genetics-9780878939657
 *
 * for details.
 */
double heterozygosity( BaseCounts const& sample, bool with_bessel = false );

/**
 * @brief Compute classic theta pi, that is, the sum of heterozygosities.
 *
 * The function simply sums heterozygosity() for all samples in the given range.
 * If @p with_bessel is set, Bessel's correction for the total nucleotide count is used.
 */
template<class ForwardIterator>
double theta_pi(
    ForwardIterator begin, ForwardIterator end,
    bool with_bessel = true
) {
    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        pi_sum += heterozygosity( *it, with_bessel );
    }
    return pi_sum;
}

/**
 * @brief Compute classic theta pi (within a population), that is, the sum of heterozygosities
 * including Bessel's correction for total nucleotide sum at each position, and Bessel's correction
 * for the pool size.
 *
 * This is the same computation used for theta pi within in the FST computation
 * of f_st_pool_unbiased(). It does _not_ use the pool seq correction of Kofler et al.
 */
template<class ForwardIterator>
double theta_pi_within_pool(
    size_t poolsize,
    ForwardIterator begin, ForwardIterator end
) {
    auto const psb = static_cast<double>( poolsize ) / ( static_cast<double>( poolsize ) - 1.0 );
    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        // Apply heterozygosity with Bessel's for nucleotide count, and Bessel's for pool size.
        pi_sum += heterozygosity( *it, true ) * psb;
    }
    return pi_sum;
}

/**
 * @brief Compute the denominator for the pool-sequencing correction of theta pi according to
 * Kofler et al.
 *
 * We here compute the denominator for a given @p poolsize, with a fix DiversityPoolSettings::min_count.
 * Values are identical for each given @p nucleotide_count, and henced cached internally for speedup.
 *
 * @see theta_pi_pool()
 * @see DiversityPoolCalculator
 */
double theta_pi_pool_denominator(
    DiversityPoolSettings const& settings,
    size_t poolsize,
    size_t nucleotide_count
);

/**
 * @brief Compute theta pi with pool-sequencing correction according to Kofler et al,
 * that is, the sum of heterozygosities divided by the correction denominator.
 *
 * The function sums heterozygosity() for all samples in the given range, including Bessel's
 * correction for the total nucleotide count at each position, and divides each by the respective
 * theta_pi_pool_denominator() to correct for error from pool sequencing.
 *
 * The provided range between @p begin and @p end is expected to be already filtered and transformed
 * as needed. See DiversityPoolCalculator for details on this.
 */
template<class ForwardIterator>
double theta_pi_pool( // get_pi_calculator
    DiversityPoolSettings const& settings,
    size_t poolsize,
    ForwardIterator begin,
    ForwardIterator end
) {
    // PoPoolation variable names:
    // poolsize:  n
    // min_count: b

    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        double const pi_snp = heterozygosity( *it, true );
        double const denom  = theta_pi_pool_denominator(
            settings, poolsize, nucleotide_sum( *it )
        );
        if( std::isfinite( pi_snp ) && std::isfinite( denom )) {
            pi_sum += ( pi_snp / denom );
        }
    }
    return pi_sum;
}

/**
 * @brief Compute theta pi with pool-sequencing correction according to Kofler et al,
 * for a single BaseCounts.
 *
 * The function computes the heterozygosity() for the given @p sample, including Bessel's
 * correction for the total nucleotide count at each position, and divides it by the
 * theta_pi_pool_denominator() to correct for error from pool sequencing.
 *
 * @see DiversityPoolCalculator
 */
inline double theta_pi_pool(
    DiversityPoolSettings const& settings,
    size_t poolsize,
    BaseCounts const& sample
) {
    auto const h = heterozygosity( sample, true );
    auto const d = theta_pi_pool_denominator( settings, poolsize, nucleotide_sum( sample ));
    return h / d;
}

// =================================================================================================
//     Theta Watterson
// =================================================================================================

/**
 * @brief Compute the denominator for the pool-sequencing correction of theta watterson according to
 * Kofler et al.
 *
 * @copydetails theta_pi_pool_denominator()
 *
 * @see theta_watterson_pool()
 */
double theta_watterson_pool_denominator(
    DiversityPoolSettings const& settings,
    size_t poolsize,
    size_t nucleotide_count
);

/**
 * @brief Compute theta watterson with pool-sequencing correction according to Kofler et al.
 *
 * The provided range between @p begin and @p end is expected to be already filtered and transformed
 * as needed. See DiversityPoolCalculator for details on this.
 */
template<class ForwardIterator>
double theta_watterson_pool( // get_theta_calculator
    DiversityPoolSettings const& settings,
    size_t poolsize,
    ForwardIterator begin,
    ForwardIterator end
) {
    // PoPoolation variable names:
    // poolsize:  n
    // min_count: b

    double sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        auto const denom = theta_watterson_pool_denominator(
            settings, poolsize, nucleotide_sum( *it )
        );
        if( std::isfinite( denom )) {
            sum += 1.0 / denom;
        }
    }
    return sum;
}

/**
 * @brief Compute theta watterson with pool-sequencing correction according to Kofler et al,
 * for a single BaseCounts sample.
 *
 * @see DiversityPoolCalculator
 */
inline double theta_watterson_pool(
    DiversityPoolSettings const& settings,
    size_t poolsize,
    BaseCounts const& sample
) {
    return 1.0 / theta_watterson_pool_denominator( settings, poolsize, nucleotide_sum( sample ));
}

// =================================================================================================
//     Tajima's D Helper Functions
// =================================================================================================

/**
 * @brief Compute `a_n`, the sum of reciprocals.
 *
 * This is the sum of reciprocals up to `n-1`, which is
 * \f$ a_n = \sum_{i=1}^{n-1} \frac{1}{i} \f$.
 *
 * See Equation 3.6 in
 *
 * > Hahn, M. W. (2018). Molecular Population Genetics.
 * > https://global.oup.com/academic/product/molecular-population-genetics-9780878939657
 *
 * for details.
 *
 * Note that we are implementing this for `double` @p n, instead of an unsigned integer type,
 * as some variants of the tajima_d() computation actually use n_base() to get an "effective"
 * pool size. That is kind of wrong, but we have implemented it here for comparability with
 * PoPoolation. In these cases, we round `n` to the nearest integer first.
 * For any actual integer numbers of pool sizes, `double` has enough precision to accurately
 * stor that integer value, so there is no loss of accuracy in those cases.
 *
 * @see b_n(), the sum of squared reciprocals.
 * @see tajima_d_pool()
 */
double a_n( double n );

/**
 * @brief Compute `b_n`, the sum of squared reciprocals.
 *
 * This is the sum of squared reciprocals up to `n-1`, which is
 * \f$ b_n = \sum_{i=1}^{n-1} \frac{1}{i^2} \f$.
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
 *
 * See also tne note in a_n() about the usage of `double` here for the argument.
 *
 * @see a_n(), the sum of reciprocals.
 * @see tajima_d_pool()
 */
double b_n( double n );

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
 *
 * @see tajima_d_pool()
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
 *
 * @see tajima_d_pool()
 */
double alpha_star( double n );

/**
 * @brief Compute `beta*` according to Achaz 2008 and Kofler et al. 2011.
 *
 * @copydetails alpha_star()
 *
 * @see tajima_d_pool()
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
 *
 * @see tajima_d_pool()
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
 *
 * @see tajima_d_pool()
 */
double n_base( size_t coverage, size_t poolsize );

// =================================================================================================
//     Tajima's D
// =================================================================================================

/**
 * @brief Compute the denominator for the pool-sequencing correction of Tajima's D according to
 * Kofler et al.
 *
 * @see tajima_d_pool()
 */
double tajima_d_pool_denominator(
    DiversityPoolSettings const& settings,
    double theta,
    size_t poolsize,
    size_t snp_count,
    size_t empirical_min_coverage = 0
);

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 *
 * The argument @p snp_count is meant to be the total number of SNPs that have been
 * processed to get the values for @p theta_pi and @p theta_watterson.
 *
 * The argument @p empirical_min_coverage is only needed when using @p settings
 * with TajimaDenominatorPolicy::kEmpiricalMinCoverage
 */
inline double tajima_d_pool(
    DiversityPoolSettings const& settings,
    double theta_pi,
    double theta_watterson,
    size_t poolsize,
    size_t snp_count,
    size_t empirical_min_coverage = 0
) {
    // Edge case, following what PoPoolation does in this situation.
    if( snp_count == 0 ) {
        return 0.0;
    }

    // We already have the two theta statistics given here, but need to compute the
    // denominator according to Kofler et al for pooled sequences.
    auto const denom = tajima_d_pool_denominator(
        settings, theta_watterson, poolsize, snp_count, empirical_min_coverage
    );
    return ( theta_pi - theta_watterson ) / denom;
}

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 *
 * The provided range between @p begin and @p end is expected to be already filtered and transformed
 * as needed. We use the full size of that range as the number of SNPs; hence, when instead calling
 * this function with a range that still contains non-SNP positions, the result might be wrong.
 * See DiversityPoolCalculator for details on this.
 */
template<class ForwardIterator>
double tajima_d_pool( // get_D_calculator
    DiversityPoolSettings const& settings,
    double theta_pi,
    double theta_watterson,
    size_t poolsize,
    ForwardIterator begin,
    ForwardIterator end
) {
    // If we need the empirical min coverage, compute it.
    // If not, we can skip this step.
    size_t empirical_min_coverage = std::numeric_limits<size_t>::max();
    if( settings.tajima_denominator_policy == TajimaDenominatorPolicy::kEmpiricalMinCoverage ) {
        for( auto& it = begin; it != end; ++it ) {
            auto const cov = nucleotide_sum( *it );
            if( cov < empirical_min_coverage ) {
                empirical_min_coverage = cov;
            }
        }
    }

    auto const snp_count = std::distance( begin, end );
    return tajima_d_pool(
        settings, theta_pi, theta_watterson, poolsize, snp_count, empirical_min_coverage
    );
}

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 *
 * This overload of the function is computing theta_pi and theta_watterson first, and hence
 * inefficient in cases where those have already been computed elsewhere.
 *
 * Same as tajima_d_pool( DiversityPoolSettings const&, size_t, double, double, ForwardIterator, ForwardIterator ), we also expect the range to be filtered already. See there, and see
 * DiversityPoolCalculator for details.
 */
template<class ForwardIterator>
double tajima_d_pool( // get_D_calculator
    DiversityPoolSettings const& settings,
    size_t poolsize,
    ForwardIterator begin,
    ForwardIterator end
) {
    // First compute the two theta statistics, then call the other version of this function.
    auto const pi = theta_pi_pool( settings, poolsize, begin, end );
    auto const tw = theta_watterson_pool( settings, poolsize, begin, end );
    return tajima_d_pool( settings, pi, tw, poolsize, begin, end );
}

} // namespace population
} // namespace genesis

#endif // include guard
