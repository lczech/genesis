#ifndef GENESIS_UTILS_MATH_DISTANCE_H_
#define GENESIS_UTILS_MATH_DISTANCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/containers/matrix.hpp"
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
//     Norms
// =================================================================================================

/**
 * @brief Calculate the p-norm of a range of numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element. The parameter @p p has to be >= 1.0.
 * In order to get the maximum norm (or infinity norm), @p p can also be set to positive infinity,
 * that is, \c std::numeric_limits<double>::infinity(). Default is `p == 2.0`, which is the Euclidean
 * norm.
 *
 * @see euclidean_norm(), manhattan_norm(), and maximum_norm() for special cases,
 * which simply call this function with a fixed @p p, in order to make code more expressive.
 * @see aitchison_norm() for another type of norm.
 */
template <class ForwardIterator>
double p_norm( ForwardIterator first, ForwardIterator last, double p = 2.0 )
{
    // Validity. We allow positive inifity.
    if( p < 1.0 || ( ! std::isfinite( p ) && ! std::isinf( p ))) {
        throw std::runtime_error( "Cannot calculate p-norm with p < 1.0" );
    }
    assert( p >= 1.0 );
    assert( std::isfinite( p ) || std::isinf( p ));

    double sum = 0.0;
    size_t cnt = 0;

    // Add vector elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            if( std::isfinite( p )) {
                sum += std::pow( std::abs( *it ), p );
            } else {
                sum = std::max( sum, std::abs( *it ));
            }
            ++cnt;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }

    // Return the result.
    assert( cnt > 0 );
    if( std::isfinite( p )) {
        return std::pow( sum, 1.0 / p );
    } else {
        return sum;
    }

    // Make old compilers happy.
    return 0.0;
}

/**
 * @brief Calculate the p-norm of a `std::vector` of `double` elements.
 *
 * @see p_norm( ForwardIterator, ForwardIterator, double ) for details.
 * @see aitchison_norm() for another type of norm.
 */
inline double p_norm( std::vector<double> const& vec, double p = 2.0 )
{
    return p_norm( vec.begin(), vec.end(), p );
}

/**
 * @brief Calculate the Manhattan norm (L1 norm) of a range of numbers.
 *
 * The function is a more expressive version of p_norm( ForwardIterator, ForwardIterator, double )
 * with `p == 1.0`. See there for details.
 */
template <class ForwardIterator>
double manhattan_norm( ForwardIterator first, ForwardIterator last )
{
    return p_norm( first, last, 1.0 );
}

/**
 * @brief Calculate the Manhattan norm (L1 norm) of a `std::vector` of `double` elements.
 *
 * The function is a more expressive version of p_norm( std::vector<double> const&, double )
 * with `p == 1.0`. See there for details.
 */
inline double manhattan_norm( std::vector<double> const& vec )
{
    return p_norm( vec.begin(), vec.end(), 1.0 );
}

/**
 * @brief Calculate the Euclidean norm (L2 norm) of a range of numbers.
 *
 * The function is a more expressive version of p_norm( ForwardIterator, ForwardIterator, double )
 * with `p == 2.0`. See there for details.
 */
template <class ForwardIterator>
double euclidean_norm( ForwardIterator first, ForwardIterator last )
{
    return p_norm( first, last, 2.0 );
}

/**
 * @brief Calculate the Euclidean norm (L2 norm) of a `std::vector` of `double` elements.
 *
 * The function is a more expressive version of p_norm( std::vector<double> const&, double )
 * with `p == 2.0`. See there for details.
 */
inline double euclidean_norm( std::vector<double> const& vec )
{
    return p_norm( vec.begin(), vec.end(), 2.0 );
}

/**
 * @brief Calculate the Maximum norm (infinity norm) of a range of numbers.
 *
 * The function is a more expressive version of p_norm( ForwardIterator, ForwardIterator, double )
 * with \c p == std::numeric_limits<double>::infinity().
 * See there for details.
 */
template <class ForwardIterator>
double maximum_norm( ForwardIterator first, ForwardIterator last )
{
    return p_norm( first, last, std::numeric_limits<double>::infinity() );
}

/**
 * @brief Calculate the Maximum norm (infinity norm) of a `std::vector` of `double` elements.
 *
 * The function is a more expressive version of p_norm( std::vector<double> const&, double )
 * with \c p == std::numeric_limits<double>::infinity().
 * See there for details.
 */
inline double maximum_norm( std::vector<double> const& vec )
{
    return p_norm( vec.begin(), vec.end(), std::numeric_limits<double>::infinity() );
}

/**
 * @brief Calculate the Aitchison norm of a range of positive numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 *
 * Following [1], the Aitchison norm \f$ \| x \|_a \f$ of a vector \f$ x \f$ with \f$ s \f$ elements
 * is caluclated as
 *
 * \f$ \| x \|_a = \sqrt{ \frac{1}{2s} \sum_{j=1}^{s} \sum_{k=1}^{s} \left( \ln{ \frac{x_j}{x_k} } \right)^2 } \f$
 *
 * That is, the calculation is in \f$ \mathcal{O}( s^2 ) \f$.
 *
 * > [1] V. Pawlowsky-Glahn, J. J. Egozcue, and R. Tolosana-Delgado,
 * > "Modelling and Analysis of Compositional Data".
 * > Chichester, UK: John Wiley & Sons, Ltd, 2015.
 * > https://onlinelibrary.wiley.com/doi/book/10.1002/9781119003144
 *
 * @see p_norm(), euclidean_norm(), manhattan_norm(), and maximum_norm() for some standard norms.
 */
template <class ForwardIterator>
double aitchison_norm( ForwardIterator first, ForwardIterator last )
{
    double sum = 0.0;
    size_t cnt = 0;

    // Outer loop.
    auto it_out = first;
    while( it_out != last ) {
        if( std::isfinite( *it_out ) ) {

            if( *it_out <= 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate Aitchison norm of non-positive values."
                );
            }

            // Inner loop.
            auto it_in = first;
            while( it_in != last ) {
                if( std::isfinite( *it_in ) ) {
                    auto const ln = std::log( *it_out / *it_in );
                    sum += ln * ln;
                }
                ++it_in;
            }

            ++cnt;
        }
        ++it_out;
    }

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }

    // Return the result.
    assert( cnt > 0 );
    return std::sqrt( sum / ( 2.0 * static_cast<double>( cnt )));
}

/**
 * @brief Calculate the Aitchison norm of a `std::vector` of `double` elements.
 *
 * @see aitchison_norm( ForwardIterator, ForwardIterator ) for details.
 */
inline double aitchison_norm( std::vector<double> const& vec )
{
    return aitchison_norm( vec.begin(), vec.end() );
}

// =================================================================================================
//     Distances
// =================================================================================================

/**
 * @brief Calculate the p-norm distance between two (mathematical) vectors.
 *
 * The iterators @p first_a, @p last_a, @p first_b, and @p last_b need to point to a range of
 * `double` values, with @p last_a and @p last_b being the past-the-end elements. The parameter
 * @p p has to be >= 1.0.
 * In order to get the maximum norm distance (or infinity norm distance), @p p can also be set to
 * positive infinity, that is, \c std::numeric_limits<double>::infinity(). Default is `p == 2.0`,
 * which is the Euclidean distance.
 *
 * See https://en.wikipedia.org/wiki/Distance#Distance_in_Euclidean_space for details.
 *
 * @see euclidean_distance(), manhattan_distance(), and maximum_distance() for special cases,
 * which simply call this function with a fixed @p p, in order to make code more expressive.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double p_norm_distance(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b,
    double p = 2.0
) {
    // Validity. We allow positive inifity.
    if( p < 1.0 || ( ! std::isfinite( p ) && ! std::isinf( p ))) {
        throw std::runtime_error( "Cannot calculate p-norm distance with p < 1.0" );
    }
    assert( p >= 1.0 );
    assert( std::isfinite( p ) || std::isinf( p ));

    double sum = 0.0;
    size_t cnt = 0;

    for_each_finite_pair( first_a, last_a, first_b, last_b, [&]( double val_a, double val_b ){
        if( std::isfinite( p )) {
            sum += std::pow( std::abs( val_a - val_b ), p );
        } else {
            sum = std::max( sum, std::abs( val_a - val_b ) );
        }
        ++cnt;
    });

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }

    // Return the result.
    assert( cnt > 0 );
    if( std::isfinite( p )) {
        return std::pow( sum, 1.0 / p );
    } else {
        return sum;
    }

    // Make old compilers happy.
    return 0.0;
}

/**
 * @brief Calculate the p-norm distance between two vectors of `double` elements.
 *
 * @see p_norm_distance( ForwardIteratorA, ForwardIteratorA, ForwardIteratorB, ForwardIteratorB, double ) for details.
 */
inline double p_norm_distance(
    std::vector<double> const& vec_a, std::vector<double> const& vec_b, double p = 2.0
) {
    return p_norm_distance( vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), p );
}

/**
 * @brief Calculate the Manhattan norm (L1 norm) distance between two (mathematical) vectors.
 *
 * The function is a more expressive version of
 * p_norm_distance( ForwardIteratorA, ForwardIteratorA, ForwardIteratorB, ForwardIteratorB, double )
 * with `p == 1.0`. See there for details.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double manhattan_distance(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b
) {
    return p_norm_distance( first_a, last_a, first_b, last_b, 1.0 );
}

/**
 * @brief Calculate the Manhattan norm (L1 norm) distance between two vectors of `double` elements.
 *
 * The function is a more expressive version of
 * p_norm_distance( std::vector<double> const&, std::vector<double> const&, double )
 * with `p == 1.0`. See there for details.
 */
inline double manhattan_distance(
    std::vector<double> const& vec_a, std::vector<double> const& vec_b
) {
    return p_norm_distance( vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), 1.0 );
}

/**
 * @brief Calculate the Euclidean norm (L2 norm) distance between two (mathematical) vectors.
 *
 * The function is a more expressive version of
 * p_norm_distance( ForwardIteratorA, ForwardIteratorA, ForwardIteratorB, ForwardIteratorB, double )
 * with `p == 2.0`. See there for details.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double euclidean_distance(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b
) {
    return p_norm_distance( first_a, last_a, first_b, last_b, 2.0 );
}

/**
 * @brief Calculate the Euclidean norm (L2 norm) distance between two vectors of `double` elements.
 *
 * The function is a more expressive version of
 * p_norm_distance( std::vector<double> const&, std::vector<double> const&, double )
 * with `p == 2.0`. See there for details.
 */
inline double euclidean_distance(
    std::vector<double> const& vec_a, std::vector<double> const& vec_b
) {
    return p_norm_distance( vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), 2.0 );
}

/**
 * @brief Calculate the Maximum norm (infinity norm) distance between two (mathematical) vectors.
 *
 * The function is a more expressive version of
 * p_norm_distance( ForwardIteratorA, ForwardIteratorA, ForwardIteratorB, ForwardIteratorB, double )
 * with \c p == std::numeric_limits<double>::infinity().
 * See there for details.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double maximum_distance(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b
) {
    return p_norm_distance(
        first_a, last_a, first_b, last_b, std::numeric_limits<double>::infinity()
    );
}

/**
 * @brief Calculate the Maximum norm (infinity norm) distance between two vectors of `double`
 * elements.
 *
 * The function is a more expressive version of
 * p_norm_distance( std::vector<double> const&, std::vector<double> const&, double )
 * with \c p == std::numeric_limits<double>::infinity().
 * See there for details.
 */
inline double maximum_distance(
    std::vector<double> const& vec_a, std::vector<double> const& vec_b
) {
    return p_norm_distance(
        vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(),
        std::numeric_limits<double>::infinity()
    );
}

// =================================================================================================
//     Distances Matrices
// =================================================================================================

/**
 * @brief Calculate the pairwise distance matrix between the rows of a given matrix.
 *
 * The function uses p_norm_distance() to calculate the distances, see there for details.
 * Each row of the matrix is considered a vector with the length of the columns of the matrix.
 * Hence, the resulting quadratic distance matrix has dimensions `r * r`, with `r` being the
 * number of rows of the input matrix.
 *
 * @see manhattan_distance_matrix(), euclidean_distance_matrix(), and maximum_distance_matrix()
 * for specialized versions of this function with a fixed @p p for more expressive code.
 */
Matrix<double> p_norm_distance_matrix( Matrix<double> const& data, double p = 2.0 );

/**
 * @brief Calculate the pairwise manhatten distance matrix between the rows of a given matrix.
 *
 * See p_norm_distance_matrix() for details. This function simply uses a fixed `p == 1.0` for
 * more expressive code.
 */
Matrix<double> manhattan_distance_matrix( Matrix<double> const& data );

/**
 * @brief Calculate the pairwise euclidean distance matrix between the rows of a given matrix.
 *
 * See p_norm_distance_matrix() for details. This function simply uses a fixed `p == 2.0` for
 * more expressive code.
 */
Matrix<double> euclidean_distance_matrix( Matrix<double> const& data );

/**
 * @brief Calculate the pairwise maximum distance matrix between the rows of a given matrix.
 *
 * See p_norm_distance_matrix() for details. This function simply uses a fixed
 * \c p == std::numeric_limits<double>::infinity()
 * more expressive code.
 */
Matrix<double> maximum_distance_matrix( Matrix<double> const& data );

} // namespace utils
} // namespace genesis

#endif // include guard
