#ifndef GENESIS_UTILS_MATH_COMMON_H_
#define GENESIS_UTILS_MATH_COMMON_H_

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
 * @ingroup utils
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Constants and General Functions
// =================================================================================================

/**
 * @brief Make the world go round.
 */
constexpr double PI = 3.141592653589793238463;

inline double circumference( double radius )
{
    return 2.0 * PI * radius;
}

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

// =================================================================================================
//     Number Handling
// =================================================================================================

/**
 * @brief Calculate the absolute differenence between two values.
 *
 * This function is particularly useful for unsigned types, as subtracting them and then using
 * std::abs() does not work for them.
 */
template< typename T > inline constexpr
T abs_diff( T const& lhs, T const& rhs )
{
    return ((lhs > rhs) ? (lhs - rhs) : (rhs - lhs));
}

/**
 * @brief Implementation of signum(T x) for unsigned types. See there for details.
 */
template <typename T> inline constexpr
int signum(T x, std::false_type )
{
    // The tag type `std::false_type` is unnamed in order to avoid compiler warnings about an
    // unused parameter. As this function is `constexpr`, we cannot shut this warning down by
    // using `(void) param`, so making it unnamed is a reasonable solution in this case.
    return T(0) < x;
}

/**
 * @brief Implementation of signum(T x) for signed types. See there for details.
 */
template <typename T> inline constexpr
int signum(T x, std::true_type )
{
    // The tag type `std::false_type` is unnamed in order to avoid compiler warnings about an
    // unused parameter. As this function is `constexpr`, we cannot shut this warning down by
    // using `(void) param`, so making it unnamed is a reasonable solution in this case.
    return (T(0) < x) - (x < T(0));
}

/**
 * @brief Get the sign of a value, which is either -1, 0, or 1.
 *
 * Works for all numerical types. We use two tagged implementations for signed and unsigned types
 * in order to avoid compiler warnings. See
 * http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
 * for details.
 */
template <typename T> inline constexpr
int signum(T x)
{
    return signum( x, std::is_signed<T>() );
}

/**
 * @brief Check whether two doubles are almost equal, using a relative epsilon to compare them.
 */
inline bool almost_equal_relative(
    double lhs,
    double rhs,
    double max_rel_diff = std::numeric_limits<double>::epsilon()
) {
    // Calculate the difference.
    auto diff = std::abs( lhs - rhs );

    // Find the larger number.
    auto largest = std::max( std::abs( lhs ), std::abs( rhs ));

    // Do the comparison.
    return ( diff <= largest * max_rel_diff );
}

/**
 * @brief Retun the value of `x`, rounded to the decimal digit given by `accuracy_order`.
 */
inline double round_to( double x, size_t accuracy_order )
{
    double factor = std::pow( 10, accuracy_order );
    return std::round( x * factor ) / factor;
}

/**
 * @brief Calculate the power `base^exp` for positive integer values.
 *
 * Remark: This overflows quite easily. The function does not check whether the desired power
 * actually fits within `size_t`. Use is_valid_int_pow() to check for this first.
 *
 * Rationale for this function: One could argue that int powers are not really useful, particularly
 * because of the fast overflow. However, using doubles leads to rounding errors, at least for
 * bigger numbers. So, within the valid range, this function is more accurate. Also, it is faster.
 */
inline size_t int_pow( size_t base, size_t exp )
{
    // Using Exponentiation by squaring, see
    // http://stackoverflow.com/a/101613/4184258
    size_t result = 1;
    while( exp )  {
        if( exp & 1 ) {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }
    return result;
}

/**
 * @brief Return whether the given power can be stored within a `size_t`.
 *
 * Use int_pow() to calculate the actual value of the power.
 */
inline bool is_valid_int_pow( size_t base, size_t exp )
{
    return std::pow( base, exp ) < static_cast<double>( std::numeric_limits<size_t>::max() );
}

/**
 * @brief Square of a number.
 *
 * Simply \f$ x^2 \f$, but faster than `std::pow` for the low exponent. Meant as an abbreviation
 * where the argument is not already a variable, e.g., `squared( x - 1.0 )`,
 * without the need to store the intermediate argument term.
 */
inline constexpr double squared( double x )
{
    return x * x;
}

/**
 * @brief Cube of a number.
 *
 * Simply \f$ x^3 \f$, but faster than `std::pow` for the low exponent. Meant as an abbreviation
 * where the argument is not already a variable, e.g., `cubed( x - 1.0 )`,
 * without the need to store the intermediate argument term.
 */
inline constexpr double cubed( double x )
{
    return x * x * x;
}

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Helper function that cleans two ranges of `double` of the same length from non-finite values.
 *
 * This function is used for cleaning data input. It iterates both same-length ranges in parallel
 * and copies pairs elements to the two result vectors (one for each range), if both values are
 * finite. The result vectors thus have equal size.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
std::pair<std::vector<double>, std::vector<double>> finite_pairs(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b
) {
    // Prepare result.
    std::vector<double> vec_a;
    std::vector<double> vec_b;

    // Iterate in parallel.
    while( first_a != last_a && first_b != last_b ) {
        if( std::isfinite( *first_a ) && std::isfinite( *first_b ) ) {
            vec_a.push_back( *first_a );
            vec_b.push_back( *first_b );
        }
        ++first_a;
        ++first_b;
    }
    if( first_a != last_a || first_b != last_b ) {
        throw std::runtime_error(
            "Ranges need to have same length."
        );
    }

    assert( vec_a.size() == vec_b.size() );
    return { vec_a, vec_b };
}

/**
 * @brief Iterate two ranges of `double` values in parallel, and execute a function for
 * each pair of values from the two ranges where both values are finite.
 *
 * The ranges need to have the same length.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
void for_each_finite_pair(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b,
    std::function<void( double value_a, double value_b )> execute
) {
    // Iterate in parallel.
    while( first_a != last_a && first_b != last_b ) {
        if( std::isfinite( *first_a ) && std::isfinite( *first_b ) ) {
            execute( *first_a, *first_b );
        }
        ++first_a;
        ++first_b;
    }

    // Check if the ranges have the same length.
    if( first_a != last_a || first_b != last_b ) {
        throw std::runtime_error( "Ranges need to have same length." );
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
