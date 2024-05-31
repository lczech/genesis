#ifndef GENESIS_UTILS_MATH_CORRELATION_H_
#define GENESIS_UTILS_MATH_CORRELATION_H_

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/ranking.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Pearson Correlation Coefficient
// =================================================================================================


/**
 * @brief Calculate the Pearson Correlation Coefficient between two ranges of `double`.
 *
 * Both ranges need to have the same length. Then, the function calculates the PCC
 * between the pairs of entries of both ranges. It skipes entries where any of the two values
 * is not finite.
 *
 * If each pair of entries in the ranges contains at leat one non-finite value, that is, if there
 * are no pairs of finite values, a `quiet_NaN` is returned. Furtheremore, if one of the ranges
 * has a standard deviation of `0.0`, e.g., because all its entries are `0.0` themselves,
 * a division by 0 occurs, leading to a `NaN` as well.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double pearson_correlation_coefficient(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b
) {
    // Calculate means.
    double mean_a = 0.0;
    double mean_b = 0.0;
    size_t count = 0;
    for_each_finite_pair(
        first_a, last_a,
        first_b, last_b,
        [&]( double val_a, double val_b ){
            mean_a += val_a;
            mean_b += val_b;
            ++count;
        }
    );
    if( count == 0 ) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    assert( count > 0 );
    mean_a /= static_cast<double>( count );
    mean_b /= static_cast<double>( count );

    // Calculate PCC parts.
    double numerator = 0.0;
    double std_dev_a = 0.0;
    double std_dev_b = 0.0;
    for_each_finite_pair(
        first_a, last_a,
        first_b, last_b,
        [&]( double val_a, double val_b ){
            double const d1 = val_a - mean_a;
            double const d2 = val_b - mean_b;
            numerator += d1 * d2;
            std_dev_a += d1 * d1;
            std_dev_b += d2 * d2;
        }
    );

    // Calculate PCC, and assert that it is in the correct range
    // (or not a number, which can happen if the std dev is 0.0, e.g. in all-zero vectors).
    auto const pcc = numerator / ( std::sqrt( std_dev_a ) * std::sqrt( std_dev_b ) );
    assert(( -1.0 <= pcc && pcc <= 1.0 ) || ( ! std::isfinite( pcc ) ));
    return pcc;
}

/**
 * @brief Calculate the Pearson Correlation Coefficient between the entries of two vectors.
 *
 * @copydetails pearson_correlation_coefficient( ForwardIteratorA first_a, ForwardIteratorA last_a, ForwardIteratorB first_b, ForwardIteratorB last_b ).
 */
inline double pearson_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
) {
    return pearson_correlation_coefficient(
        vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end()
    );
}

// =================================================================================================
//     Spearman's Correlation Coefficient
// =================================================================================================

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between two ranges of `double`.
 *
 * Both ranges need to have the same length. Then, the function calculates Spearmans's Rho
 * between the pairs of entries of both vectors. Ranking is done via
 * @link ranking_fractional() fractional ranking@endlink.
 * Pairs of entries which contain non-finite values are skipped.
 */
template <class RandomAccessIteratorA, class RandomAccessIteratorB>
double spearmans_rank_correlation_coefficient(
    RandomAccessIteratorA first_a, RandomAccessIteratorA last_a,
    RandomAccessIteratorB first_b, RandomAccessIteratorB last_b
) {
    // Get cleaned results. We need to make these copies, as we need to calculate the fractional
    // ranking on them, which would change if we used our normal for_each_finite_pair here...
    auto const cleaned = finite_pairs( first_a, last_a, first_b, last_b );

    // Get the ranking of both vectors.
    auto const ranks_a = ranking_fractional( cleaned.first );
    auto const ranks_b = ranking_fractional( cleaned.second );
    assert( ranks_a.size() == ranks_b.size() );

    return pearson_correlation_coefficient( ranks_a, ranks_b );
}

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between the entries of two vectors.
 *
 * @copydetails spearmans_rank_correlation_coefficient( RandomAccessIteratorA first_a, RandomAccessIteratorA last_a, RandomAccessIteratorB first_b, RandomAccessIteratorB last_b )
 */
inline double spearmans_rank_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
) {
    return spearmans_rank_correlation_coefficient(
        vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end()
    );
}

// =================================================================================================
//     Kendall Tau Correlation Coefficient
// =================================================================================================

/**
 * @brief Method for computing Kendall's Tau.
 *
 * See kendalls_tau_correlation_coefficient() for the function that uses this.
 * See https://en.wikipedia.org/wiki/Kendall_rank_correlation_coefficient and
 * https://docs.scipy.org/doc/scipy-1.12.0/reference/generated/scipy.stats.kendalltau.html#scipy.stats.kendalltau
 * for details on the different methods.
 */
enum class KendallsTauMethod
{
    /**
     * @brief Compute Tau-a, which does not make any adjustment for ties.
     */
    kTauA,

    /**
     * @brief Compute Tau-b, which _does_ adjustments for ties.
     */
    kTauB,

    /**
     * @brief Compute Tau-c, (also called Stuart-Kendall Tau-c).
     */
    kTauC,
};

/**
 * @brief Compute Kendall's Tau Correlation Coefficient.
 *
 * This function computes Kendall's tau, if requested accounting for ties,
 * and using Knight's algorithm for speed.
 *
 * >  Knight, W. (1966). "A Computer Method for Calculating Kendall's Tau with Ungrouped Data".
 * > Journal of the American Statistical Association. 61 (314): 436–439.
 * > doi:10.2307/2282833. JSTOR 2282833.
 *
 * See kendalls_tau_correlation_coefficient_naive() for the naive, slow version.
 */
double kendalls_tau_correlation_coefficient(
    std::vector<double> const& x,
    std::vector<double> const& y,
    KendallsTauMethod method = KendallsTauMethod::kTauB
);

/**
 * @copydoc kendalls_tau_correlation_coefficient(
 *     std::vector<double> const&, std::vector<double> const&, KendallsTauMethod
 * )
 */
template <class InputIteratorA, class InputIteratorB>
double kendalls_tau_correlation_coefficient(
    InputIteratorA first_a, InputIteratorA last_a,
    InputIteratorB first_b, InputIteratorB last_b,
    KendallsTauMethod method = KendallsTauMethod::kTauB
) {
    // Use cleaned results with only finite values. We need those internally anyway to get proper
    // ranking, and by doing it here already, we can save another copy of the data internally.
    auto const cleaned = finite_pairs( first_a, last_a, first_b, last_b );
    return kendalls_tau_correlation_coefficient( cleaned.first, cleaned.second, method );
}

/**
 * @brief Compute a simple version of Kendall's Tau Correlation Coefficient.
 *
 * This version uses a naive nested loop over the elements.
 * It is mainly provided for testing the more advanced implementation,
 * see kendalls_tau_correlation_coefficient() for details.
 */
double kendalls_tau_correlation_coefficient_naive(
    std::vector<double> const& x,
    std::vector<double> const& y,
    KendallsTauMethod method = KendallsTauMethod::kTauB
);

// =================================================================================================
//     Fisher z-transformation
// =================================================================================================

/**
 * @brief Apply Fisher z-transformation to a correlation coefficient.
 *
 * The coefficient can be calculated with pearson_correlation_coefficient() or
 * spearmans_rank_correlation_coefficient() and has to be in range `[ -1.0, 1.0 ]`.
 *
 * There is also a version of this function for a vector of coefficients.
 */
inline double fisher_transformation( double correlation_coefficient )
{
    auto const r = correlation_coefficient;
    if( r < -1.0 || r > 1.0 ) {
        throw std::invalid_argument(
            "Cannot apply fisher transformation to value " + std::to_string( r ) +
            " outside of [ -1.0, 1.0 ]."
        );
    }

    // LOG_DBG << "formula " << 0.5 * log( ( 1.0 + r ) / ( 1.0 - r ) );
    // LOG_DBG << "simple  " << std::atanh( r );
    return std::atanh( r );
}

/**
 * @brief Apply Fisher z-transformation to a vector of correlation coefficients.
 *
 * See fisher_transformation( double ) for details.
 */
inline std::vector<double> fisher_transformation( std::vector<double> const& correlation_coefficients )
{
    auto res = correlation_coefficients;
    for( auto& elem : res ) {
        elem = fisher_transformation( elem );
    }
    return res;
}

} // namespace utils
} // namespace genesis

#endif // include guard
