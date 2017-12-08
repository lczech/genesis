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
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/statistics.hpp"
#include "genesis/utils/core/algorithm.hpp"

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
//     General Matrix Operators
// =================================================================================================

/**
 * @brief Transpose a Matrix.
 */
template <typename T>
Matrix<T> matrix_transpose( Matrix<T> const& data )
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
 * @brief Return whether a Matrix is symmetric, i.e., whether it is square and
 * `m[ i, j ] == m[ j, i ]` holds for all entries.
 */
template <typename T>
bool matrix_is_symmetric( Matrix<T> const& data )
{
    if( data.rows() != data.cols() ) {
        return false;
    }

    // We only need to check the upper triangle, and compare it to the lower triangle.
    // Also, we use early stopping. Of course we do. Who wouldn't.
    for( size_t i = 0; i < data.rows(); ++i ) {
        for( size_t j = i + 1; j < data.cols(); ++j ) {
            if( data( i, j ) != data( j, i ) ) {
                return false;
            }
        }
    }

    return true;
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

/**
 * @brief Print a Matrix to an out stream.
 * See print( Matrix<T> const&, size_t, size_t ) for details.
 */
template <typename T>
void print( std::ostream& out, Matrix<T> const& matrix, size_t rows = 10, size_t cols = 10 )
{
    // If the user does not want limits, or uses wrong ones, just use everything!
    if( rows == 0 || rows >= matrix.rows() ) {
        rows = matrix.rows();
    }
    if( cols == 0 || cols >= matrix.cols() ) {
        cols = matrix.cols();
    }

    // Print as many rows and cols as wanted.
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            out << matrix(i, j);
            if (j < matrix.cols() - 1) {
                out << " ";
            }
        }
        if( cols < matrix.cols() ) {
            out << " ...";
        }
        out << "\n";
    }
    if( rows < matrix.rows() ) {
        out << "...\n";
    }
}

/**
 * @brief Print a Matrix to a `std::string`.
 *
 * If @p rows == 0, all rows are printed. Otherwise, only the given number of rows is printed,
 * followed by an ellipsis (`...`). The same applies to the columns, using @p cols.
 *
 * See also print( std::ostream&, Matrix<T> const&, size_t, size_t ).
 */
template <typename T>
std::string print( Matrix<T> const& matrix, size_t rows = 10, size_t cols = 10 )
{
    std::ostringstream out;
    print( out, matrix, rows, cols );
    return out.str();
}

// =================================================================================================
//     Swapping and Sorting
// =================================================================================================

/**
 * @brief Swap (interchange) two rows of a Matrix, given their indices.
 */
template <typename T>
void matrix_swap_rows( Matrix<T>& data, size_t row_a, size_t row_b )
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
void matrix_swap_cols( Matrix<T>& data, size_t col_a, size_t col_b )
{
    if( col_a >= data.rows() || col_b >= data.rows() ) {
        throw std::invalid_argument( "Invalid column index for swap_cols()." );
    }

    using std::swap;
    for( size_t r = 0; r < data.rows(); ++r ) {
        swap( data( r, col_a ), data( r, col_b ) );
    }
}

/**
 * @brief Sort rows and columns of a Matrix by the sum or the rows.
 *
 * This is an operation for square matrices: It takes the row sum as sorting criterion, and
 * sorts the rows and the columns according to that order. This is mostly useful for symmetric
 * matrices, where `m[ i, j ] == m[ j, i ]`. In those cases, the sorting is stable with respect
 * to the symmetry.
 *
 * See also matrix_sort_by_col_sum_symmetric(). In case of symmetric matrices, it should yield
 * the same Matrix.
 */
template< typename T >
Matrix<T> matrix_sort_by_row_sum_symmetric( Matrix<T> const& data )
{
    if( data.rows() != data.cols() ) {
        throw std::runtime_error( "Symmetric sort only works on square matrices." );
    }

    auto result = Matrix<T>( data.rows(), data.cols() );
    auto row_sums = matrix_row_sums( data );
    auto si = sort_indices( row_sums.begin(), row_sums.end() );

    for( size_t i = 0; i < data.rows(); ++i ) {
        for( size_t j = 0; j < data.cols(); ++j ) {
            result( i, j ) = data( si[i], si[j] );
        }
    }

    return result;
}

/**
 * @brief Sort rows and columns of a Matrix by the sum or the columns.
 *
 * This is an operation for square matrices: It takes the row sum as sorting criterion, and
 * sorts the rows and the columns according to that order. This is mostly useful for symmetric
 * matrices, where `m[ i, j ] == m[ j, i ]`. In those cases, the sorting is stable with respect
 * to the symmetry.
 *
 * See also matrix_sort_by_col_sum_symmetric(). In case of symmetric matrices, it should yield
 * the same Matrix.
 */
template< typename T >
Matrix<T> matrix_sort_by_col_sum_symmetric( Matrix<T> const& data )
{
    if( data.rows() != data.cols() ) {
        throw std::runtime_error( "Symmetric sort only works on square matrices." );
    }

    auto result = Matrix<T>( data.rows(), data.cols() );
    auto col_sums = matrix_col_sums( data );
    auto si = sort_indices( col_sums.begin(), col_sums.end() );

    for( size_t i = 0; i < data.rows(); ++i ) {
        for( size_t j = 0; j < data.cols(); ++j ) {
            result( i, j ) = data( si[i], si[j] );
        }
    }

    return result;
}

// =================================================================================================
//     Matrix Addition
// =================================================================================================

/**
 * @brief Calculate the element-wise sum of two @link Matrix Matrices@endlink.
 *
 * The two matrices need to have the same dimensions.
 */
template< typename T = double, typename A = double, typename B = double >
Matrix<T> matrix_addition( Matrix<A> const& a, Matrix<B> const& b )
{
    if( a.rows() != b.rows() || a.cols() != b.cols() ) {
        throw std::runtime_error( "Cannot add matrices with different dimensions." );
    }

    auto result = Matrix<T>( a.rows(), a.cols() );
    for( size_t r = 0; r < a.rows(); ++r ) {
        for( size_t c = 0; c < a.cols(); ++c ) {
            result( r, c ) = a( r, c ) + b( r, c );
        }
    }
    return result;
}

/**
 * @brief Calculate the element-wise sum of a Matrix and a scalar.
 */
template< typename T = double, typename A = double, typename B = double >
Matrix<T> matrix_addition( Matrix<A> const& matrix, B const& scalar )
{
    auto result = Matrix<T>( matrix.rows(), matrix.cols() );
    for( size_t r = 0; r < matrix.rows(); ++r ) {
        for( size_t c = 0; c < matrix.cols(); ++c ) {
            result( r, c ) = matrix( r, c ) + scalar;
        }
    }
    return result;
}

/**
 * @brief Calculate the element-wise difference of two @link Matrix Matrices@endlink.
 *
 * The two matrices need to have the same dimensions.
 */
template< typename T = double, typename A = double, typename B = double >
Matrix<T> matrix_subtraction( Matrix<A> const& a, Matrix<B> const& b )
{
    if( a.rows() != b.rows() || a.cols() != b.cols() ) {
        throw std::runtime_error( "Cannot add matrices with different dimensions." );
    }

    auto result = Matrix<T>( a.rows(), a.cols() );
    for( size_t r = 0; r < a.rows(); ++r ) {
        for( size_t c = 0; c < a.cols(); ++c ) {
            result( r, c ) = a( r, c ) - b( r, c );
        }
    }
    return result;
}

// =================================================================================================
//     Matrix Multiplication
// =================================================================================================

/**
 * @brief Calculate the product of two @link Matrix Matrices@endlink.
 *
 * The two matrices need to have fitting dimensions, i.e., `a[ l, m ] x b[ m, n ]`, which results
 * in a Matrix of dimensions `r[ l, n ]`.
 */
template< typename T = double, typename A = double, typename B = double >
Matrix<T> matrix_multiplication( Matrix<A> const& a, Matrix<B> const& b )
{
    if( a.cols() != b.rows() ) {
        throw std::runtime_error( "Cannot multiply matrices if a.cols() != b.rows()." );
    }

    // Simple and naive. Fast enough for the few occasions were we need this.
    // If Genesis at some point starts to need more elaborate matrix operations, it might be
    // worth including some proper library for this.
    auto result = Matrix<T>( a.rows(), b.cols() );
    for( size_t r = 0; r < a.rows(); ++r ) {
        for( size_t c = 0; c < b.cols(); ++c ) {
            result( r, c ) = 0.0;
            for( size_t j = 0; j < a.cols(); ++j ) {
                result( r, c ) += a( r, j ) * b( j, c );
            }
        }
    }

    return result;
}

/**
 * @brief Calculate the product of a `vector` @p a with a @link Matrix Matrices@endlink @p b,
 * as if the vector was a Matrix with only one row.
 *
 * Thus, the two arguments need to have fitting dimensions, i.e., `a[ m ] x b[ m, n ]`. The
 * resulting vector has size `n`, which represent the columns of the result Matrix.
 */
template< typename T = double, typename A = double, typename B = double >
std::vector<T> matrix_multiplication( std::vector<A> const& a, Matrix<B> const& b )
{
    if( a.size() != b.rows() ) {
        throw std::runtime_error( "Cannot multiply vector with matrix if a.size() != b.rows()." );
    }

    auto result = std::vector<T>( b.cols(), 0.0 );
    for( size_t c = 0; c < b.cols(); ++c ) {
        for( size_t j = 0; j < a.size(); ++j ) {
            result[ c ] += a[ j ] * b( j, c );
        }
    }

    return result;
}

/**
 * @brief Calculate the product of a @link Matrix Matrices@endlink @p a with a `vector` @p b,
 * as if the vector was a Matrix with only one column.
 *
 * Thus, the two arguments need to have fitting dimensions, i.e., `a[ l, m ] x b[ m ]`. The
 * resulting vector has size `l`, which represent the rows of the result Matrix.
 */
template< typename T = double, typename A = double, typename B = double >
std::vector<T> matrix_multiplication( Matrix<A> const& a, std::vector<B> const& b )
{
    if( a.cols() != b.size() ) {
        throw std::runtime_error( "Cannot multiply matrix with vector if a.cols() != b.size()." );
    }

    auto result = std::vector<T>( a.rows(), 0.0 );
    for( size_t r = 0; r < a.rows(); ++r ) {
        for( size_t j = 0; j < a.cols(); ++j ) {
            result[ r ] += a( r, j ) * b[ j ];
        }
    }

    return result;
}

/**
 * @brief Calculate the element-wise multiplication of a Matrix and a scalar.
 */
template< typename T = double, typename A = double, typename B = double >
Matrix<T> matrix_multiplication( Matrix<A> const& matrix, B const& scalar )
{
    auto result = Matrix<T>( matrix.rows(), matrix.cols() );
    for( size_t r = 0; r < matrix.rows(); ++r ) {
        for( size_t c = 0; c < matrix.cols(); ++c ) {
            result( r, c ) = matrix( r, c ) * scalar;
        }
    }
    return result;
}

} // namespace utils
} // namespace genesis

#endif // include guard
