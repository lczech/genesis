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

/**
 * @brief Store the values of quartiles: `q0 == min`, `q1 == 25%`, `q2 == 50%`, `q3 == 75%`,
 * `q4 == max`.
 */
struct Quartiles
{
    double q0 = 0.0;
    double q1 = 0.0;
    double q2 = 0.0;
    double q3 = 0.0;
    double q4 = 0.0;
};

// =================================================================================================
//     Statistics
// =================================================================================================

/**
 * @brief Calcualte the mean and standard deviation of a `vector` of `double`.
 *
 * If the resulting standard deviation is below the given @p epsilon (e.g, `0.0000001`), it is
 * "corrected" to be `1.0` instead. This is an inelegant (but usual) way to handle near-zero values,
 * which for some use cases would cause problems like a division by zero later on.
 * By default, @p epsilon is `-1.0`, which deactivates this check - a standard deviation can never
 * be below `0.0`.
 */
MeanStddevPair mean_stddev( std::vector<double> const& vec, double epsilon = -1.0 );

/**
 * @brief Calculate the median value of a `vector` of `double`.
 *
 * The vector has to be sorted.
 */
double median( std::vector<double> const& vec );

/**
 * @brief Calculate the Quartiles of a `vector` of `double`.
 *
 * The vector has to be sorted.
 */
Quartiles quartiles( std::vector<double> const& vec );

/**
 * @brief Calculate the Pearson Correlation Coefficient between the entries of two vectors.
 *
 * Both vectors need to have the same size. Then, the function calculates the PCC
 * between the pairs of entries of both vectors.
 */
double pearson_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
);

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between the entries of two vectors.
 *
 * Both vectors need to have the same size. Then, the function calculates Spearmans's Rho
 * between the pairs of entries of both vectors.
 */
double spearmans_rank_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
);

/**
 * @brief Apply Fisher z-transformation to a correlation coefficient.
 *
 * The coefficient can be calculated with pearson_correlation_coefficient() or
 * spearmans_rank_correlation_coefficient() and has to be in range `[ -1.0, 1.0 ]`.
 *
 * There is also a version of this function for a vector of coefficients.
 * See also matrix_col_pearson_correlation_coefficient(),
 * matrix_row_pearson_correlation_coefficient(), matrix_col_spearmans_rank_correlation_coefficient()
 * and matrix_row_spearmans_rank_correlation_coefficient() for matrix versions.
 */
double fisher_transformation( double correlation_coefficient );

/**
 * @brief Apply Fisher z-transformation to a vector of correlation coefficients.
 *
 * See the single value version for details.
 */
std::vector<double> fisher_transformation( std::vector<double> const& correlation_coefficients );

// =================================================================================================
//     Ranking
// =================================================================================================

/**
 * @brief Return the ranking of the given values, using Standard competition ranking
 * ("1224" ranking).
 *
 * See https://en.wikipedia.org/wiki/Ranking for details.
 *
 * @see ranking_modified(), ranking_dense(), ranking_ordinal(), ranking_fractional() for other
 * ranking methods.
 */
std::vector<size_t> ranking_standard( std::vector<double> const& vec );

/**
 * @brief Return the ranking of the given values, using Modified competition ranking
 * ("1334" ranking).
 *
 * See https://en.wikipedia.org/wiki/Ranking for details.
 *
 * @see ranking_standard(), ranking_dense(), ranking_ordinal(), ranking_fractional() for other
 * ranking methods.
 */
std::vector<size_t> ranking_modified( std::vector<double> const& vec );

/**
 * @brief Return the ranking of the given values, using Dense ranking ("1223" ranking).
 *
 * See https://en.wikipedia.org/wiki/Ranking for details.
 *
 * @see ranking_standard(), ranking_modified(), ranking_ordinal(), ranking_fractional() for other
 * ranking methods.
 */
std::vector<size_t> ranking_dense( std::vector<double> const& vec );

/**
 * @brief Return the ranking of the given values, using Ordinal ranking ("1234" ranking).
 *
 * See https://en.wikipedia.org/wiki/Ranking for details.
 *
 * @see ranking_standard(), ranking_modified(), ranking_dense(), ranking_fractional() for other
 * ranking methods.
 */
std::vector<size_t> ranking_ordinal( std::vector<double> const& vec );

/**
 * @brief Return the ranking of the given values, using Fractional ranking ("1 2.5 2.5 4" ranking).
 *
 * See https://en.wikipedia.org/wiki/Ranking for details. This is the only raking method that
 * returns float values instead of integer values.
 *
 * @see ranking_standard(), ranking_modified(), ranking_dense(), ranking_ordinal() for other
 * ranking methods.
 */
std::vector<double> ranking_fractional( std::vector<double> const& vec );

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
