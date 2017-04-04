#ifndef GENESIS_UTILS_MATH_MATRIX_OPERATORS_H_
#define GENESIS_UTILS_MATH_MATRIX_OPERATORS_H_

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
 * @brief Matrix operators.
 *
 * @file
 * @ingroup utils
 */

#include <ostream>
#include <stdexcept>

#include "genesis/utils/math/matrix.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Helpful Functions
// =================================================================================================

/**
 * @brief Given a linear index in a upper triangular Matrix, find the corresponding Matrix indices.
 *
 * Given an upper triangle Matrix of size `n == 5`
 *
 *     0  k0  k1  k2  k3
 *     0   0  k4  k5  k6
 *     0   0   0  k7  k8
 *     0   0   0   0  k9
 *     0   0   0   0   0
 *
 * and given a @p k (like above), find the corresponding indices `i` and `j` for this @p k,
 * e.g.,
 *
 *     k == 0 --> i == 0, j == 1
 *     k == 5 --> i == 1, j == 3
 *     k == 9 --> i == 3, j == 4
 *     ...
 *
 * Because the calculation involves solving
 * [triangular numbers](https://en.wikipedia.org/wiki/Triangular_number), the function internally
 * operates on `double` values. This is a bit smelly, but seems to work (tested with `n == 100,000`).
 *
 * See also triangular_index() for the opposite function, and triangular_size() to calculate the
 * maximal @p k that will occur in a trian Matrix of a given size @p n.
 *
 * @param k Linear index in the upper triangle of a quadratic Matrix.
 * @param n Size of the quadratic Matrix, i.e., the row/column length.
 * @return  Pair `( i, j )` of the indices for the given `k`.
 */
std::pair<size_t, size_t> triangular_indices( size_t k, size_t n );

/**
 * @brief Given indices @p i and @p j in a quadratic Matrix, find the corresponding linear index.
 *
 * See triangular_indices() for the opposite function, which also explains the details, and
 * triangular_size() to calculate the
 * maximal @p k that will occur in a trian Matrix of a given size @p n.
 *
 * @param i, j Indices of the Matrix for which to calculate the linear index `k`.
 * @param n    Size of the quadratic Matrix, i.e., the row/column length.
 * @return     Linear index `k` for the given @p i and @p j.
 */
size_t triangular_index( size_t i, size_t j, size_t n );

/**
 * @brief Calculate the number of linear indices needed for a triangular Matrix of size @p n.
 *
 * Given an upper triangle Matrix of size `n == 5`
 *
 *     0  k0  k1  k2  k3
 *     0   0  k4  k5  k6
 *     0   0   0  k7  k8
 *     0   0   0   0  k9
 *     0   0   0   0   0
 *
 * we need `10` indices `k == 0..9` to linearly describe the positions in the triangle.
 * This function returns this number of indices for a given @p n.
 *
 * See also triangular_indices() for calculating the Matrix indices `i` and `j` for a given `k`.
 *
 * @param n Size of the quadratic Matrix, i.e., the row/column length.
 * @return  Number of needed linear indices `k` to describe a triangle of the Matrix.
 */
size_t triangular_size( size_t n );

// =================================================================================================
//     Matrix Operators
// =================================================================================================

/**
 * @brief Calculate the product of two @link Matrix Matrices@endlink.
 *
 * The two matrices need to have fitting sized, i.e., `a[ l, m ] x b[ m, n ]`.
 */
Matrix<double> matrix_multiplication( Matrix<double> const& a, Matrix<double> const& b);

/**
 * @brief Transpose a Matrix.
 */
template <typename T>
Matrix<T> transpose( Matrix<T> const& data )
{
    auto res = Matrix<T>( data.cols(), data.rows() );
    for( size_t r = 0; r < data.rows(); ++r ) {
        for( size_t c = 0; c < data.cols(); ++c ) {
            res( c, r ) = data( r, c );
        }
    }
    return res;
}

/**
 * @brief Swap (interchange) two rows of a Matrix, given their indices.
 */
template <typename T>
void swap_rows( Matrix<T>& data, size_t row_a, size_t row_b )
{
    if( row_a >= data.rows() || row_b >= data.rows() ) {
        throw std::invalid_argument( "Invalid row index for swap_rows()." );
    }

    using std::swap;
    for( size_t c = 0; c < data.cols(); ++c ) {
        swap( data( row_a, c ), data( row_b, c ) );
    }
}

/**
 * @brief Swap (interchange) two columns of a Matrix, given their indices.
 */
template <typename T>
void swap_cols( Matrix<T>& data, size_t col_a, size_t col_b )
{
    if( col_a >= data.rows() || col_b >= data.rows() ) {
        throw std::invalid_argument( "Invalid column index for swap_cols()." );
    }

    using std::swap;
    for( size_t r = 0; r < data.rows(); ++r ) {
        swap( data( r, col_a ), data( r, col_b ) );
    }
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const Matrix<T>& matrix)
{
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            os << matrix(i, j);
            if (j < matrix.cols() - 1) {
                os << " ";
            }
        }
        os << "\n";
    }
    return os;
}

} // namespace utils
} // namespace genesis

#endif // include guard
