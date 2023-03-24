#ifndef GENESIS_UTILS_MATH_BINOMIAL_H_
#define GENESIS_UTILS_MATH_BINOMIAL_H_

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
 * @ingroup utils
 */

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Binomial Functions
// =================================================================================================

/**
 * @brief Largest value for `n` when computing binomial coefficients where no value of `k` causes
 * the result to overflow.
 *
 * See binomial_coefficient_approx() for details.
 */
constexpr size_t MAX_BINOMIAL_COEFFICIENT_N = 1024;

/**
 * @brief Return the logarithm (base e) of the factorial of @p n, that is `log(n!)`.
 *
 * This is implemented via a precise lookup table for `n < 1024`, and uses Stirling's approximation
 * for larger values, with a slight error in the order of less than 1/(360 * n^3), that is,
 * less than 2.59e-12 for all `n >= 1024`.
 * See https://www.johndcook.com/blog/2010/08/16/how-to-compute-log-factorial/ for details.
 */
double log_factorial( size_t n );

/**
 * @brief Compute the binomial coefficient, that is @p n choose @p k, for two integer numbers.
 *
 * The function throws for invalid arguments (n or k equal to 0, or k larger than n),
 * or on overflow. For all `n < 63`, this does not overflow with 64 bit numbers.
 */
size_t binomial_coefficient( size_t n, size_t k );

/**
 * @brief Compute the binomial coefficient, that is @p n choose @p k, for two integer numbers,
 * for large numbers.
 *
 * The precise integer function binomial_coefficient() can only handle values up to `n == 62`
 * (for non-small `k` at least). That is, at `n == 63, k == 29`, we have exhausted the range
 * of 64bit numbers.
 *
 * To also be able to work with larger @p n and @p k, we here offer an approximation that returns
 * a `double` value instead, and hence can compute binomial coefficients of up to `n == 1024`
 * (exclusive), which for `k == n/2` is about as large as double precision allows.
 *
 * For `n > 1024`, either an exception is thrown, or, if `lenient == true`, positive infinity
 * is returned.
 *
 * The returned values for all @p n and @p k that are also valid with the integer version
 * binomial_coefficient() yield identical results (up to double precision). That is, in particular
 * for all `n < 63`, as well as for larger `n` with small `k`.
 *
 * Note furthermore that we use a lookup table for the bulk of the computation here.
 * Hence, for larger numbers, this function is also faster than explicity computing the values.
 */
double binomial_coefficient_approx( size_t n, size_t k, bool lenient = false );

/**
 * @brief Compute the probability mass function for a binomial distribution.
 *
 * Note that we reverse the order of @p k and @p n here compared to binomial_coefficient() here,
 * in order to comply with common notation.
 *
 * For `n > 1024`, either an exception is thrown, or, if `lenient == true`, positive infinity
 * is returned. See binomial_coefficient_approx() for details.
 */
inline double binomial_distribution( size_t k, size_t n, double p, bool lenient = false )
{
    if( ! std::isfinite(p) || p < 0.0 || p > 1.0 ) {
        throw std::invalid_argument(
            "Cannot compute binomial distribution with p outside of [ 0, 1 ]"
        );
    }
    double const coeff = binomial_coefficient_approx( n, k, lenient );
    return coeff * std::pow( p, k ) * std::pow( 1.0 - p, n - k );
}

} // namespace utils
} // namespace genesis

#endif // include guard
