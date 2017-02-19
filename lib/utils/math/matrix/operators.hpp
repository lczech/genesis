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

#include "utils/math/matrix.hpp"

namespace genesis {
namespace utils {

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
