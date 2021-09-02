#ifndef GENESIS_UTILS_MATH_STATISTICS_H_
#define GENESIS_UTILS_MATH_STATISTICS_H_

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

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/ranking.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Static Assersions
// =================================================================================================

// We need to make sure that doubles and their infinities behave the way we expect!

static_assert(
    std::numeric_limits<double>::is_iec559,
    "IEC 559/IEEE 754 floating-point types required (wrong double type)."
);
static_assert(
    std::numeric_limits<double>::has_infinity,
    "IEC 559/IEEE 754 floating-point types required (does not have infinity)."
);
static_assert(
    std::numeric_limits<double>::has_quiet_NaN,
    "IEC 559/IEEE 754 floating-point types required (does not have quite NaN)."
);
static_assert(
    - std::numeric_limits<double>::infinity() < std::numeric_limits<double>::lowest(),
    "IEC 559/IEEE 754 floating-point types required (infinity is not the lowest value)."
);

// Clang fails to compile the following assertions, because of missing const expr markers
// in their std lib implementation. We hence need to skip those tests for clang :-(
// Hopefully, the above assertions are enough to cover the basics.

#ifndef __clang__

static_assert(
    std::isinf( - std::numeric_limits<double>::infinity() ),
    "IEC 559/IEEE 754 floating-point types required (infinity is not working properly)."
);
static_assert(
    std::isinf( -1 * std::numeric_limits<double>::infinity()),
    "IEC 559/IEEE 754 floating-point types required."
);
static_assert(
    -1 * std::numeric_limits<double>::infinity() < std::numeric_limits<double>::lowest(),
    "IEC 559/IEEE 754 floating-point types required."
);

#endif // __clang__

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
//     Standard Helper Functions
// =================================================================================================

/**
 * @brief Count the number of finite elements in a range of double values.
 *
 * The function iterates a range of values that are expected to be double values.
 * It uses `std::isfinite` as criterion for validity of the values.
 *
 * The return value is a pair containing the number of finite elements as `first`,
 * and the number of total number elements in the range as `second` (the size of the range).
 */
template <class ForwardIterator>
std::pair<size_t, size_t> count_finite_elements( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    size_t valid = 0;
    size_t total = 0;

    // Iterate.
    while( first != last ) {
        if( std::isfinite( *first ) ) {
            ++valid;
        }
        ++total;
        ++first;
    }

    return { valid, total };
}

/**
 * @brief Return the minimum of a range of double values.
 *
 * Only finite values are used in the comparison. If there are no finite values at all
 * in the range, a quite NaN is returned.
 */
template <class ForwardIterator>
double finite_minimum( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    double min = std::numeric_limits<double>::max();
    size_t cnt = 0;

    // Iterate.
    while( first != last ) {
        if( std::isfinite( *first ) ) {
            if( *first < min ) {
                min = *first;
            }
            ++cnt;
        }
        ++first;
    }

    // If there are no valid elements, return nan.
    if( cnt == 0 ) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    return min;
}

/**
 * @brief Return the maximum of a range of double values.
 *
 * Only finite values are used in the comparison. If there are no finite values at all
 * in the range, a quite NaN is returned.
 */
template <class ForwardIterator>
double finite_maximum( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    double max = std::numeric_limits<double>::lowest();
    size_t cnt = 0;

    // Iterate.
    while( first != last ) {
        if( std::isfinite( *first ) ) {
            if( *first > max ) {
                max = *first;
            }
            ++cnt;
        }
        ++first;
    }

    // If there are no valid elements, return nan.
    if( cnt == 0 ) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    return max;
}

/**
 * @brief Return the minimum and the maximum of a range of double values.
 *
 * Only finite values are used in the comparison. If there are no finite values at all
 * in the range, a quite NaN is returned.
 */
template <class ForwardIterator>
MinMaxPair<double> finite_minimum_maximum( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    MinMaxPair<double> result;
    result.min = std::numeric_limits<double>::max();
    result.max = std::numeric_limits<double>::lowest();
    size_t cnt = 0;

    // Iterate.
    while( first != last ) {
        if( std::isfinite( *first ) ) {
            if( *first < result.min ) {
                result.min = *first;
            }
            if( *first > result.max ) {
                result.max = *first;
            }
            ++cnt;
        }
        ++first;
    }

    // If there are no valid elements, return nan.
    if( cnt == 0 ) {
        result.min = std::numeric_limits<double>::quiet_NaN();
        result.max = std::numeric_limits<double>::quiet_NaN();
    }

    return result;
}

// =================================================================================================
//     Normalization and Compositional Data Analysis
// =================================================================================================

/**
 * @brief Calculate the closure of a range of numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element. Then, the closure [1] of the elements is calculated,
 * that is, they are all divided by their total sum. This is used in compositional data analysis.
 * Non-finite elements are ignored, negative elements throw an exception.
 *
 * > [1] J. Aitchison,
 * > "The statistical analysis of compositional data".
 * > Chapman and Hall London, 1986.
 * > https://www.jstor.org/stable/2345821
 */
template <class ForwardIterator>
void closure( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    double sum = 0.0;
    size_t cnt = 0;

    // Sum up elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            if( *it < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate closure of negative numbers."
                );
            }

            sum += *it;
            ++cnt;
        }
        ++it;
    }

    // If there are no valid elements, return.
    if( cnt == 0 ) {
        return;
    }

    // Make the closure.
    it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            *it /= sum;
        }
        ++it;
    }
}

/**
 * @brief Calculate the closure of a `std::vector` of `double` elements.
 *
 * @see closure( ForwardIterator first, ForwardIterator last ) for details.
 */
inline void closure( std::vector<double>& vec )
{
    return closure( vec.begin(), vec.end() );
}

// =================================================================================================
//     Mean Stddev
// =================================================================================================

/**
 * @brief Calculate the arithmetic mean and standard deviation of a range of `double` elements.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the arithmetic mean and standard deviation of all finite elements
 * in the range. If no elements are finite, or if the range is empty, both returned values are `0.0`.
 * Non-finite numbers are ignored.
 *
 * If the resulting standard deviation is below the given @p epsilon (e.g, `0.0000001`), it is
 * "corrected" to be `1.0` instead. This is an inelegant (but usual) way to handle near-zero values,
 * which for some use cases would cause problems like a division by zero later on.
 * By default, @p epsilon is `-1.0`, which deactivates this check.
 *
 * @see mean_stddev( std::vector<double> const&, double epsilon ) for a version for `std::vector`.
 * @see arithmetic_mean() for a function that only calculates the mean, and thus saves the effort
 * of a second iteration over the range.
 */
template <class ForwardIterator>
MeanStddevPair mean_stddev( ForwardIterator first, ForwardIterator last, double epsilon = -1.0 )
{
    // Prepare result.
    MeanStddevPair result;
    result.mean   = 0.0;
    result.stddev = 0.0;
    size_t count  = 0;

    // Sum up elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            result.mean += *it;
            ++count;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( count == 0 ) {
        return result;
    }

    //  Calculate mean.
    result.mean /= static_cast<double>( count );

    // Calculate std dev.
    it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            result.stddev += (( *it - result.mean ) * ( *it - result.mean ));
        }
        ++it;
    }
    assert( count > 0 );
    result.stddev /= static_cast<double>( count );
    result.stddev = std::sqrt( result.stddev );

    // The following in an inelegant (but usual) way to handle near-zero values,
    // which later would cause a division by zero.
    assert( result.stddev >= 0.0 );
    if( result.stddev <= epsilon ){
        result.stddev = 1.0;
    }

    return result;
}

/**
 * @brief Calculate the mean and standard deviation of a `std::vector` of `double` elements.
 *
 * @see mean_stddev( ForwardIterator first, ForwardIterator last, double epsilon ) for details.
 * @see arithmetic_mean() for a function that only calculates the mean, and thus saves the effort
 * of a second iteration over the range.
 */
inline MeanStddevPair mean_stddev( std::vector<double> const& vec, double epsilon = -1.0 )
{
    return mean_stddev( vec.begin(), vec.end(), epsilon );
}

// =================================================================================================
//     Arithmetic Mean
// =================================================================================================

/**
 * @brief Calculate the arithmetic mean of a range of numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the arithmetic mean of all finite elements in the range.
 * If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored.
 *
 * @see arithmetic_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see mean_stddev() for a function that also calculates the standard deviation.
 * @see geometric_mean() for a function that calculates the geometric mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean.
 */
template <class ForwardIterator>
double arithmetic_mean( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    double mean  = 0.0;
    size_t count = 0;

    // Sum up elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            mean += *it;
            ++count;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( count == 0 ) {
        assert( mean == 0.0 );
        return mean;
    }

    //  Calculate mean.
    assert( count > 0 );
    return mean / static_cast<double>( count );
}

/**
 * @brief Calculate the arithmetic mean of a `std::vector` of `double` elements.
 *
 * @see arithmetic_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see mean_stddev() for a function that simultaneously calculates the standard deviation.
 * @see geometric_mean() for a function that calculates the geometric mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 */
inline double arithmetic_mean( std::vector<double> const& vec )
{
    return arithmetic_mean( vec.begin(), vec.end() );
}

/**
 * @brief Calculate the weighted arithmetic mean of a range of `double` values.
 *
 * The iterators @p first_value and @p last_value, as well as @p first_weight and @p last_weight,
 * need to point to ranges of `double` values, with @p last_value and @p last_weight being the
 * past-the-end elements. Both ranges need to have the same size.
 * The function then calculates the weighted arithmetic mean of all finite elements
 * in the range. If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. The weights have to be non-negative.
 *
 * @see weighted_arithmetic_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see arithmetic_mean() for the unweighted version.
 * @see geometric_mean() for a function that calculates the geometric mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 * @see weighted_geometric_mean() for a function that calculates the weighted geometric mean, and
 * @see weighted_harmonic_mean() for a function that calculates the weighted harmonic mean.
 */
template <class ForwardIterator>
double weighted_arithmetic_mean(
    ForwardIterator first_value,  ForwardIterator last_value,
    ForwardIterator first_weight, ForwardIterator last_weight
) {
    double num = 0.0;
    double den = 0.0;
    size_t cnt = 0;

    // Multiply elements.
    for_each_finite_pair(
        first_value, last_value,
        first_weight, last_weight,
        [&]( double value, double weight ){
            if( weight < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate weighted arithmetic mean with negative weights."
                );
            }

            num += weight * value;
            den += weight;
            ++cnt;
        }
    );

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }
    if( den == 0.0 ) {
        throw std::invalid_argument(
            "Cannot calculate weighted arithmetic mean with all weights being 0."
        );
    }

    // Return the result.
    assert( cnt > 0 );
    assert( den > 0.0 );
    return ( num / den );
}

/**
 * @brief Calculate the weighted arithmetic mean of a `std::vector` of `double` elements.
 *
 * @see weighted_arithmetic_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see arithmetic_mean() for the unweighted version.
 * @see geometric_mean() for a function that calculates the geometric mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 * @see weighted_geometric_mean() for a function that calculates the weighted geometric mean, and
 * @see weighted_harmonic_mean() for a function that calculates the weighted harmonic mean.
 */
inline double weighted_arithmetic_mean(
    std::vector<double> const& values,
    std::vector<double> const& weights
) {
    return weighted_arithmetic_mean( values.begin(), values.end(), weights.begin(), weights.end() );
}

// =================================================================================================
//     Geometric Mean
// =================================================================================================

/**
 * @brief Calculate the geometric mean of a range of positive numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the geometric mean of all positive finite elements in the range.
 * If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. If finite non-positive numbers (zero or negative) are found,
 * an exception is thrown.
 *
 * @see geometric_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see weighted_geometric_mean() for a weighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 */
template <class ForwardIterator>
double geometric_mean( ForwardIterator first, ForwardIterator last )
{
    double sum   = 0.0;
    size_t count = 0;

    // Iterate elements. For numeric stability, we use sum of logs instead of products;
    // otherwise, we run into overflows too quickly!
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            if( *it <= 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate geometric mean of non-positive numbers."
                );
            }
            sum += std::log( *it );
            ++count;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( count == 0 ) {
        return 0.0;
    }

    // Return the result.
    assert( count > 0 );
    assert( std::isfinite( sum ));
    return std::exp( sum / static_cast<double>( count ));
}

/**
 * @brief Calculate the geometric mean of a `std::vector` of `double` elements.
 *
 * @see geometric_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 */
inline double geometric_mean( std::vector<double> const& vec )
{
    return geometric_mean( vec.begin(), vec.end() );
}

/**
 * @brief Calculate the weighted geometric mean of a range of positive numbers.
 *
 * The iterators @p first_value and @p last_value, as well as @p first_weight and @p last_weight,
 * need to point to ranges of `double` values, with @p last_value and @p last_weight being the
 * past-the-end elements. Both ranges need to have the same size.
 * The function then calculates the weighted geometric mean of all positive finite elements
 * in the range. If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. If finite non-positive numbers (zero or negative) are found,
 * an exception is thrown. The weights have to be non-negative.
 *
 * For a set of values \f$ v \f$ and a set of weights \f$ w \f$,
 * the weighted geometric mean \f$ g \f$ is calculated following [1]:
 *
 * \f$ g = \exp \left( \frac{ \sum w \cdot \log v }{ \sum w } \right) \f$
 *
 * That is, if all weights are `1.0`, the formula yields the standard geometric mean.
 *
 * > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 *
 * @see weighted_geometric_mean( std::vector<double> const&, std::vector<double> const& ) for a version for `std::vector`.
 * @see geometric_mean() for the unweighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean, and
 * @see weighted_harmonic_mean() for a function that calculates the weighted harmonic mean.
 */
template <class ForwardIterator>
double weighted_geometric_mean(
    ForwardIterator first_value,  ForwardIterator last_value,
    ForwardIterator first_weight, ForwardIterator last_weight
) {
    double num = 0.0;
    double den = 0.0;
    size_t cnt = 0;

    // Multiply elements.
    for_each_finite_pair(
        first_value, last_value,
        first_weight, last_weight,
        [&]( double value, double weight ){
            if( value <= 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate weighted geometric mean of non-positive values."
                );
            }
            if( weight < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate weighted geometric mean with negative weights."
                );
            }

            num += weight * std::log( value );
            den += weight;
            ++cnt;
        }
    );

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }
    if( den == 0.0 ) {
        throw std::invalid_argument(
            "Cannot calculate weighted geometric mean with all weights being 0."
        );
    }

    // Return the result.
    assert( cnt > 0 );
    assert( std::isfinite( num ));
    assert( std::isfinite( den ) && ( den > 0.0 ));
    return std::exp( num / den );
}

/**
 * @brief Calculate the weighted geometric mean of a `std::vector` of `double` elements.
 *
 * @see weighted_geometric_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see geometric_mean() for the unweighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see harmonic_mean() for a function that calculates the harmonic mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean, and
 * @see weighted_harmonic_mean() for a function that calculates the weighted harmonic mean.
 */
inline double weighted_geometric_mean(
    std::vector<double> const& values,
    std::vector<double> const& weights
) {
    return weighted_geometric_mean( values.begin(), values.end(), weights.begin(), weights.end() );
}

// =================================================================================================
//     Harmoic Mean
// =================================================================================================

/**
 * @brief Select a policy on how to treat zeroes in the computation of harmonic_mean()
 * and weighted_harmonic_mean().
 */
enum class HarmonicMeanZeroPolicy
{
    /**
     * @brief Throw an exception when a zero value is encountered in the data.
     */
    kThrow,

    /**
     * @brief Ignore any zero values.
     */
    kIgnore,

    /**
     * @brief If any zero value is encountered in the data, simply return zero as the harmonic mean.
     *
     * This is for example the interpretation of using the harmonic mean to compute the average
     * resistance of a set of resistors in parallel, where one zero-resistance resistor would lead
     * to the whole set having zero resistance.
     */
    kReturnZero,

    /**
     * @brief Apply a zero-value correction.
     *
     * The correction is computed as
     *
     * \f$ \mu_h = \frac{N_T - N_0}{\sum^{N_T - N_0}_{i=1} \frac{1}{x_i}} \times \frac{N_T - N_0} {N_T} \f$
     *
     * where \f$ \mu_h \f$ is harmonic mean, \f$ x_i \f$ are the non-zero values of the data only,
     * \f$ N_T \f$ is the (total) sample size, and \f$ N_0 \f$ is the number of zero values.
     *
     * This follows the EPA (Environmental Protection Agency) program DFLOW, see
     * https://www.epa.gov/ceam/dflow and https://rdrr.io/cran/lmomco/man/harmonic.mean.html
     */
    kCorrection
};

/**
 * @brief Calculate the harmonic mean of a range of positive numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the harmonic mean of all non-negative or positive (depending on the
 * @p zero_policy) finite elements in the range.
 * If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. If finite negative numbers are found, an exception is thrown.
 * Zero values are treated according to the @p zero_policy.
 *
 * @see harmonic_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see weighted_harmonic_mean() for a weighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see geometric_mean() for a function that calculates the geometric mean.
 */
template <class ForwardIterator>
double harmonic_mean(
    ForwardIterator first, ForwardIterator last,
    HarmonicMeanZeroPolicy zero_policy = HarmonicMeanZeroPolicy::kThrow
) {
    // Keep track of the total sum of inverses, the count of how many samples were used in total
    // (this excludes non-finite data points), and the number of zero value found, which is only
    // used with HarmonicMeanZeroPolicy::kCorrection
    double sum    = 0.0;
    size_t count  = 0;
    size_t zeroes = 0;

    // Iterate elements. For numeric stability, we use sum of logs instead of products;
    // otherwise, we run into overflows too quickly!
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            if( *it < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate harmonic mean of negative values."
                );
            }

            if( *it > 0.0 ) {
                sum += 1.0 / static_cast<double>( *it );
                ++count;
            } else {
                assert( *it == 0.0 );
                switch( zero_policy ) {
                    case HarmonicMeanZeroPolicy::kThrow: {
                        throw std::invalid_argument(
                            "Zero value found when calculating harmonic mean."
                        );
                    }
                    case HarmonicMeanZeroPolicy::kIgnore: {
                        // Do nothing.
                        break;
                    }
                    case HarmonicMeanZeroPolicy::kReturnZero: {
                        // If any value is zero, we do not need to finish the iteration.
                        return 0.0;
                    }
                    case HarmonicMeanZeroPolicy::kCorrection: {
                        // Increment both counters, but do not add anything to the sum.
                        ++count;
                        ++zeroes;
                        break;
                    }
                }
            }
        }
        ++it;
    }

    // If there are no valid elements, or all of them are zero, return an all-zero result.
    if( count == 0 || count == zeroes ) {
        return 0.0;
    }
    assert( count > 0 );
    assert( count > zeroes );
    assert( std::isfinite( sum ));

    // Return the result. We always compute the correction,
    // which however does not alter the result if not used.
    auto const correction = static_cast<double>( count - zeroes ) / static_cast<double>( count );
    return correction * static_cast<double>( count - zeroes ) / sum;
}

/**
 * @brief Calculate the harmonic mean of a `std::vector` of `double` elements.
 *
 * @see harmonic_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see geometric_mean() for a function that calculates the geometric mean.
 */
inline double harmonic_mean(
    std::vector<double> const& vec,
    HarmonicMeanZeroPolicy zero_policy = HarmonicMeanZeroPolicy::kThrow
) {
    return harmonic_mean( vec.begin(), vec.end(), zero_policy );
}

/**
 * @brief Calculate the weighted harmonic mean of a range of positive numbers.
 *
 * The iterators @p first_value and @p last_value, as well as @p first_weight and @p last_weight,
 * need to point to ranges of `double` values, with @p last_value and @p last_weight being the
 * past-the-end elements. Both ranges need to have the same size.
 * The function then calculates the weighted harmonic mean of all non-negative or positive
 * (depending on the @p zero_policy) finite elements in the range.
 * If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. If finite negative numbers are found, an exception is thrown.
 * Zero values are treated according to the @p zero_policy. The weights have to be non-negative,
 * and elements with non-finite weights are skipped.
 *
 * For a set of values \f$ v \f$ and a set of weights \f$ w \f$,
 * the weighted harmonic mean \f$ g \f$ is calculated following [1]:
 *
 * \f$ h = \frac{ \sum w }{ \sum \frac{ w }{ v } } \f$
 *
 * That is, if all weights are `1.0`, the formula yields the standard harmonic mean.
 *
 * @see weighted_harmonic_mean( std::vector<double> const&, std::vector<double> const& ) for a version for `std::vector`.
 * @see harmonic_mean() for the unweighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see geometric_mean() for a function that calculates the geometric mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean, and
 * @see weighted_geometric_mean() for a function that calculates the weighted geometric mean.
 */
template <class ForwardIterator>
double weighted_harmonic_mean(
    ForwardIterator first_value,  ForwardIterator last_value,
    ForwardIterator first_weight, ForwardIterator last_weight,
    HarmonicMeanZeroPolicy zero_policy = HarmonicMeanZeroPolicy::kThrow
) {
    // Keep track of the numerator (sum of all weights of positive values) and denominator
    // (sum of weights divided by values) of the summation, as well as the sum of all weights
    // (which can be different from the numerator, if there are zero values), the total number of
    // values used, and the number of zero values found.
    double weights = 0.0;
    double num     = 0.0;
    double den     = 0.0;
    size_t count   = 0;
    size_t zeroes  = 0;

    // Multiply elements, only considering finite ones.
    for_each_finite_pair(
        first_value, last_value,
        first_weight, last_weight,
        [&]( double value, double weight ){
            if( value < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate weighted harmonic mean of negative values."
                );
            }
            if( weight < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate weighted harmonic mean with negative weights."
                );
            }
            if( value > 0.0 ) {
                weights += weight;
                num     += weight;
                den     += weight / static_cast<double>( value );
                ++count;
            } else {
                assert( value == 0.0 );
                switch( zero_policy ) {
                    case HarmonicMeanZeroPolicy::kThrow: {
                        throw std::invalid_argument(
                            "Zero value found when calculating weighted harmonic mean."
                        );
                    }
                    case HarmonicMeanZeroPolicy::kIgnore: {
                        // Do nothing.
                        break;
                    }
                    case HarmonicMeanZeroPolicy::kReturnZero:
                    case HarmonicMeanZeroPolicy::kCorrection:{
                        // Increment the sum of all weights, so that zero values are contributing
                        // to the corrected result according to their weight, and increment
                        // both counters, but do not add anything to the sums.
                        // In case of the return zero policy, we use the zeroes counter as a flag
                        // indicating that we have found a zero value.
                        weights += weight;
                        ++count;
                        ++zeroes;
                        break;
                    }
                }
            }
        }
    );

    // If there are no valid elements, or all of them are zero, return an all-zero result.
    if( count == 0 || count == zeroes ) {
        return 0.0;
    }
    // For the return zero policy, if one of them is zero, return zero.
    if( zero_policy == HarmonicMeanZeroPolicy::kReturnZero && zeroes > 0 ) {
        return 0.0;
    }
    if( num == 0.0 || den == 0.0 ) {
        throw std::invalid_argument(
            "Cannot calculate weighted harmonic mean with all weights being 0."
        );
    }
    if( zeroes == 0 ) {
        (void) zeroes;
        assert( weights == num );
    }
    assert( count > 0 );
    assert( count > zeroes );
    assert( weights >= num );
    assert( std::isfinite( num ) && ( num > 0.0 ));
    assert( std::isfinite( den ) && ( den > 0.0 ));
    assert( std::isfinite( weights ) && ( weights > 0.0 ));

    // Return the result. We always compute the correction,
    // which however does not alter the result if not used.
    auto const correction = num / weights;
    return correction * num / den;
}

/**
 * @brief Calculate the weighted harmonic mean of a `std::vector` of `double` elements.
 *
 * @see weighted_harmonic_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see harmonic_mean() for the unweighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean, and
 * @see geometric_mean() for a function that calculates the geometric mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean, and
 * @see weighted_geometric_mean() for a function that calculates the weighted geometric mean.
 */
inline double weighted_harmonic_mean(
    std::vector<double> const& values,
    std::vector<double> const& weights,
    HarmonicMeanZeroPolicy zero_policy = HarmonicMeanZeroPolicy::kThrow
) {
    return weighted_harmonic_mean(
        values.begin(), values.end(),
        weights.begin(), weights.end(),
        zero_policy
    );
}

// =================================================================================================
//     Median
// =================================================================================================

/**
 * @brief Calculate the median value of a sorted range of `double` values.
 *
 * The iterators are as usual: @p first points to the first element of the range,
 * @p last to the past-the-end element.
 *
 * The median of an odd sized range is its middle element; the median of an even sized range
 * is the arithmetic mean (average) of its two middle elements.
 */
template <class RandomAccessIterator>
double median( RandomAccessIterator first, RandomAccessIterator last )
{
    // Checks.
    if( ! std::is_sorted( first, last )) {
        throw std::runtime_error( "Range has to be sorted for median calculation." );
    }
    auto const size = static_cast<size_t>( std::distance( first, last ));
    if( size == 0 ) {
        return 0.0;
    }

    // Even or odd size? Median is calculated differently.
    if( size % 2 == 0 ) {

        // Get the two middle positions.
        size_t pl = size / 2 - 1;
        size_t pu = size / 2;
        assert( pl < size && pu < size );

        return ( *(first + pl) + *(first + pu) ) / 2.0;

    } else {

        // Int division, rounds down. This is what we want.
        size_t p = size / 2;
        assert( p < size );

        return *(first + p);
    }
}

/**
 * @brief Calculate the median value of a `vector` of `double`.
 *
 * The vector has to be sorted.
 */
inline double median( std::vector<double> const& vec )
{
    return median( vec.begin(), vec.end() );
}

// =================================================================================================
//     Quartiles
// =================================================================================================

/**
 * @brief Calculate the Quartiles of a sorted range of `double` values.
 *
 * The iterators are as usual: @p first points to the first element of the range,
 * @p last to the past-the-end element.
 */
template <class RandomAccessIterator>
Quartiles quartiles( RandomAccessIterator first, RandomAccessIterator last )
{
    // Prepare result.
    Quartiles result;

    // Checks.
    if( ! std::is_sorted( first, last )) {
        throw std::runtime_error( "Range has to be sorted for quartiles calculation." );
    }
    auto const size = static_cast<size_t>( std::distance( first, last ));
    if( size == 0 ) {
        return result;
    }

    // Set min, 50% and max.
    result.q0 = *first;
    result.q2 = median( first, last );
    result.q4 = *(first + size - 1);

    // Even or odd size? Quartiles are calculated differently.
    // This could be done shorter, but this way is more expressive.
    if( size % 2 == 0 ) {

        // Even: Split exaclty in halves.
        result.q1 = median( first, first + size / 2 );
        result.q3 = median( first + size / 2, first + size );

    } else {

        // Odd: Do not include the median value itself.
        result.q1 = median( first, first + size / 2 );
        result.q3 = median( first + size / 2 + 1, first + size );
    }

    return result;
}

/**
 * @brief Calculate the Quartiles of a `vector` of `double`.
 *
 * The vector has to be sorted.
 */
inline Quartiles quartiles( std::vector<double> const& vec )
{
    return quartiles( vec.begin(), vec.end() );
}

// =================================================================================================
//     Dispersion
// =================================================================================================

/**
 * @brief Calculate the index of dispersion.
 *
 * The coefficient of variation (CV), also known as the relative standard deviation (RSD),
 * is defined as the ratio of the standard deviation to the mean.
 * See mean_stddev() to calculate those values.
 * See https://en.wikipedia.org/wiki/Coefficient_of_variation for details.
 */
inline double coefficient_of_variation( MeanStddevPair const& ms )
{
    return ms.stddev / ms.mean;
}

/**
 * @copydoc coefficient_of_variation( MeanStddevPair const& ms )
 */
inline std::vector<double> coefficient_of_variation( std::vector<MeanStddevPair> const& ms )
{
    auto res = std::vector<double>( ms.size() );
    for( size_t i = 0; i < ms.size(); ++i ) {
        res[ i ] = coefficient_of_variation( ms[i] );
    }
    return res;
}

/**
 * @brief Calculate the index of dispersion.
 *
 * The index of dispersion, also known as the dispersion index, coefficient of dispersion,
 * relative variance, variance-to-mean ratio (VMR) or Fano factor, is defined as the ratio of the
 * variance to the mean. Variance is the square of the standard deviation.
 * See mean_stddev() to calculate those values.
 * See https://en.wikipedia.org/wiki/Index_of_dispersion for details.
 */
inline double index_of_dispersion( MeanStddevPair const& ms )
{
    return ms.stddev * ms.stddev / ms.mean;
}

/**
 * @copydoc index_of_dispersion( MeanStddevPair const& ms )
 */
inline std::vector<double> index_of_dispersion( std::vector<MeanStddevPair> const& ms )
{
    auto res = std::vector<double>( ms.size() );
    for( size_t i = 0; i < ms.size(); ++i ) {
        res[ i ] = index_of_dispersion( ms[i] );
    }
    return res;
}

/**
 * @brief Calculate the quartile_coefficient_of_dispersion.
 *
 * The quartile coefficient of dispersion is defined as `( Q3 - Q1 ) / ( Q3 + Q1 )`.
 * See quartiles() to caculate those values.
 * See https://en.wikipedia.org/wiki/Quartile_coefficient_of_dispersion for details.
 */
inline double quartile_coefficient_of_dispersion( Quartiles const& q )
{
    return ( q.q3 - q.q1 ) / ( q.q3 + q.q1 );
}

/**
 * @copydoc quartile_coefficient_of_dispersion( Quartiles const& ms )
 */
inline std::vector<double> quartile_coefficient_of_dispersion( std::vector<Quartiles> const& q )
{
    auto res = std::vector<double>( q.size() );
    for( size_t i = 0; i < q.size(); ++i ) {
        res[ i ] = quartile_coefficient_of_dispersion( q[i] );
    }
    return res;
}

// =================================================================================================
//     Correlation Coefficients
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
    auto ranks_a = ranking_fractional( cleaned.first );
    auto ranks_b = ranking_fractional( cleaned.second );
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
