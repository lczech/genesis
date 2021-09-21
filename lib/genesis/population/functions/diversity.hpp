#ifndef GENESIS_POPULATION_FUNCTIONS_DIVERSITY_H_
#define GENESIS_POPULATION_FUNCTIONS_DIVERSITY_H_

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

#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/functions/filter_transform.hpp"
#include "genesis/population/functions/variant.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/filter_iterator.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/transform_iterator.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Diversity Estimates
// =================================================================================================

/**
 * @brief Settings used by different pool-sequencing corrected diversity statistics.
 *
 * These settings are used for example by theta_pi_pool(), theta_watterson_pool(), and
 * tajima_d_pool(), as well as pool_diversity_measures(), in order to have them in a central place,
 * and avoid ordering confusion that would result from having to provide them individually.
 *
 * Note in particular the setting `with_popoolation_bugs`: There are two major bugs (as far as
 * we are aware) in the PoPoolation implementation:
 *
 *  1. They compute the `n_base` term based on poolsize alone, and do not take the coverage into
 *     account at all.
 *  2. They do not use alpha star, but set it to be equal to beta star instead.
 *
 * Using this option, one can voluntarily activate these bugs here as well, in order to get
 * results that are comparable with PoPoolation results.
 */
struct PoolDiversitySettings
{
    size_t window_width = 0;
    size_t window_stride = 0;
    size_t min_phred_score = 0;

    size_t poolsize = 0;
    size_t min_allele_count = 0;
    size_t min_coverage = 0;
    size_t max_coverage = 0;
    double min_coverage_fraction = 0.0;
    bool with_popoolation_bugs = false;
};

/**
 * @brief Data struct to collect all diversity statistics computed by pool_diversity_measures().
 */
struct PoolDiversityResults
{
    double theta_pi_absolute = 0;
    double theta_pi_relative = 0;
    double theta_watterson_absolute = 0;
    double theta_watterson_relative = 0;
    double tajima_d = 0;

    size_t variant_count = 0;
    size_t snp_count = 0;
    size_t coverage_count = 0;
    double coverage_fraction = 0.0;
};

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
double heterozygosity( BaseCounts const& sample );

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
double theta_pi_pool_denominator(
    PoolDiversitySettings const& settings,
    size_t nucleotide_count
);

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
    PoolDiversitySettings const& settings,
    ForwardIterator begin,
    ForwardIterator end
) {
    // PoPoolation variable names:
    // poolsize:         n
    // min_allele_count: b

    double pi_sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        double pi_snp = heterozygosity( *it );
        pi_snp /= theta_pi_pool_denominator( settings, nucleotide_sum( *it ));
        pi_sum += pi_snp;
    }
    return pi_sum;
}

/**
 * @brief Compute theta pi with pool-sequencing correction according to Kofler et al,
 * for a single BaseCounts, that is, its heterozygosity() divided by the correction denominator.
 */
inline double theta_pi_pool(
    PoolDiversitySettings const& settings,
    BaseCounts const& sample
) {
    return heterozygosity( sample ) / theta_pi_pool_denominator( settings, nucleotide_sum( sample ));
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
    PoolDiversitySettings const& settings,
    size_t nucleotide_count
);

/**
 * @brief Compute theta watterson with pool-sequencing correction according to Kofler et al.
 */
template<class ForwardIterator>
double theta_watterson_pool( // get_theta_calculator
    PoolDiversitySettings const& settings,
    ForwardIterator begin,
    ForwardIterator end
) {
    // PoPoolation variable names:
    // poolsize:         n
    // min_allele_count: b

    double sum = 0.0;
    for( auto& it = begin; it != end; ++it ) {
        sum += 1.0 / theta_watterson_pool_denominator( settings, nucleotide_sum( *it ));
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
    PoolDiversitySettings const& settings,
    size_t snp_count,
    double theta
);

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 */
template<class ForwardIterator>
double tajima_d_pool( // get_D_calculator
    PoolDiversitySettings const& settings,
    ForwardIterator begin,
    ForwardIterator end,
    double theta_pi,
    double theta_watterson
) {
    // Edge case, following what PoPoolation does in this situation.
    if( begin == end ) {
        return 0.0;
    }

    // We already have the two theta statistics given here, but need to compute the
    // denominator according to Kofler et al for pooled sequences.
    auto const snp_cnt = std::distance( begin, end );
    auto const denom = tajima_d_pool_denominator( settings, snp_cnt, theta_watterson );
    return ( theta_pi - theta_watterson ) / denom;
}

/**
 * @brief Compute the pool-sequencing corrected version of Tajima's D according to
 * Kofler et al.
 */
template<class ForwardIterator>
double tajima_d_pool( // get_D_calculator
    PoolDiversitySettings const& settings,
    ForwardIterator begin,
    ForwardIterator end
) {
    // First compute the two theta statistics, then call the other version of this function.
    auto const pi = theta_pi_pool( settings, begin, end );
    auto const tw = theta_watterson_pool( settings, begin, end );
    return tajima_d_pool( settings, begin, end, pi, tw );
}

// =================================================================================================
//     Diversity Measures
// =================================================================================================

/**
 * @brief Compute Theta Pi, Theta Watterson, and Tajia's D in their pool-sequencing corrected
 * versions according to Kofler et al.
 *
 * This is a high level function that is meant as a simple example of how to compute these
 * statistics. See theta_pi_pool(), theta_watterson_pool(), and tajima_d_pool() for details.
 * It takes care of most options offered by PoPoolation (as given by @p settings here),
 * except for the window width and stride and minimum phred quality score, which have to be
 * applied before filling the window (or whatever other range is used as input here) before
 * calling this function.
 *
 * Furthermore, results here are not filtered aftwards, so any filtering based on e.g.,
 * minimum covered fraction has to be done downstream.
 */
template<class ForwardIterator>
PoolDiversityResults pool_diversity_measures(
    PoolDiversitySettings const& settings,
    ForwardIterator begin,
    ForwardIterator end
) {
    PoolDiversityResults results;

    // Filter all counts that are below the given min count.
    // The following is inefficient, as we do the transform multiple times in all the loops
    // below (note that all statistics functions also loop at least once!). But this function
    // here is meant as a high level variant, and we have to live with that.
    // Better solution is to run transform_by_min_count() already when adding the samples
    // to the range that we loop over here.
    auto min_filtered_range = utils::make_transform_range(
        [&]( BaseCounts const& sample ){
            auto copy = sample;
            transform_by_min_count( copy, settings.min_allele_count );
            return copy;
        },
        begin, end
    );

    // Count how many SNPs there are in total, and how many sites have the needed coverage.
    for( auto it = min_filtered_range.begin(); it != min_filtered_range.end(); ++it ) {
        auto const stat = status(
            *it, settings.min_coverage, settings.max_coverage, settings.min_allele_count
        );

        // Make sure that we are dealing with the correct types here.
        // (Just in case we ever refactor the class where these values are stored.)
        static_assert(
            std::is_same<decltype(stat.is_snp), bool>::value,
            "Expect bool type for BaseCountsStatus::is_snp"
        );
        static_assert(
            std::is_same<decltype(stat.is_covered), bool>::value,
            "Expect bool type for BaseCountsStatus::is_covered"
        );
        static_assert( static_cast<size_t>( true )  == 1, "Expect true == 1" );
        static_assert( static_cast<size_t>( false ) == 0, "Expect false == 0" );

        // Add them up.
        ++results.variant_count;
        results.snp_count      += static_cast<size_t>( stat.is_snp );
        results.coverage_count += static_cast<size_t>( stat.is_covered );
    }

    // Compute coverage over the given range.
    auto const coverage = static_cast<double>( results.coverage_count );
    results.coverage_fraction = coverage / static_cast<double>( settings.window_width );

    // Make a filter that only allows samples that are SNPs and have the needed coverage.
    auto covered_snps_range = utils::make_filter_range( [&]( BaseCounts const& sample ){
        auto stat = status(
            sample, settings.min_coverage, settings.max_coverage, settings.min_allele_count
        );
        return stat.is_snp && stat.is_covered;
    }, min_filtered_range.begin(), min_filtered_range.end() );

    // Compute all values.
    results.theta_pi_absolute = theta_pi_pool(
        settings, covered_snps_range.begin(), covered_snps_range.end()
    );
    results.theta_watterson_absolute = theta_watterson_pool(
        settings, covered_snps_range.begin(), covered_snps_range.end()
    );
    results.tajima_d = tajima_d_pool(
        settings, covered_snps_range.begin(), covered_snps_range.end(),
        results.theta_pi_absolute, results.theta_watterson_absolute
    );
    results.theta_pi_relative        = results.theta_pi_absolute        / coverage;
    results.theta_watterson_relative = results.theta_watterson_absolute / coverage;

    return results;
}

} // namespace population
} // namespace genesis

#endif // include guard
