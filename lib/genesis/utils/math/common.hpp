#ifndef GENESIS_UTILS_MATH_COMMON_H_
#define GENESIS_UTILS_MATH_COMMON_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup utils
 */

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Constants
// =================================================================================================

/**
 * @brief Make the world go round.
 */
constexpr double PI = 3.141592653589793238463;

// =================================================================================================
//     Structures and Classes
// =================================================================================================

/**
 * @brief Store a pair of min and max values.
 *
 * This notation is simply more readable than the std default of using a `pair<T, T>`.
 */
template< typename T >
struct MinMaxPair
{
    T min;
    T max;
};

/**
 * @brief Store a mean and a standard deviation value.
 *
 * This notation is simply more readable than the std default of using a `pair<T, T>` for such
 * types.
 */
struct MeanStddevPair
{
    double mean;
    double stddev;
};

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
    return std::pow( base, exp ) < std::numeric_limits<size_t>::max();
}

} // namespace utils
} // namespace genesis

#endif // include guard
