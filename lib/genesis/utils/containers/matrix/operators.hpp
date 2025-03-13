#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_OPERATORS_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_OPERATORS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief Matrix operators.
 *
 * @file
 * @ingroup utils
 */

#include <cassert>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/io/deserializer.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/io/serializer.hpp"

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
Matrix<T> transpose( Matrix<T> const& mat )
{
    auto res = Matrix<T>( mat.cols(), mat.rows() );
    for( size_t r = 0; r < mat.rows(); ++r ) {
        for( size_t c = 0; c < mat.cols(); ++c ) {
            res( c, r ) = mat( r, c );
        }
    }
    return res;
}

/**
 * @brief Transpose a Matrix inplace, without allocating a new Matrix.
 *
 * Only needs additional storage for 1 bit per element of the matrix,
 * compared to the full reallocation of the transpose() function.
 * It is however somewhat slower (4-5x more time in our debug build).
 */
template <typename T>
void transpose_inplace( Matrix<T>& mat )
{
    // We use a follow-the-cycles implementation inspired by https://stackoverflow.com/a/9320349
    // That description seems to use a flipped notation by expecting n x m matrix.

    // Need additional storage to see which entries have been cycled already.
    std::vector<bool> visited( mat.data().size() );
    size_t const div = mat.data().size() - 1;

    // Cycle through the matrix until we have moved every entry to its destination.
    size_t cycle = 0;
    while( ++cycle < mat.data().size() ) {
        if( visited[cycle] ){
            continue;
        }
        size_t cur = cycle;
        do {
            cur = cur == div ? div : (mat.rows() * cur) % div;
            std::swap( mat.data_[cur], mat.data_[cycle]);
            visited[cur] = true;
        } while( cur != cycle );
    }

    // Finally we need to update the dimensions of the matrix.
    std::swap( mat.rows_, mat.cols_ );
}

/**
 * @brief Return whether a Matrix is a square matrix, that is, whether its number of rows and number
 * of columns are idetical.
 */
template <typename T>
bool is_square( Matrix<T> const& data )
{
    return data.rows() == data.cols();
}

/**
 * @brief Return whether a Matrix is symmetric, i.e., whether it is square and
 * `m[ i, j ] == m[ j, i ]` holds for all entries.
 */
template <typename T>
bool is_symmetric( Matrix<T> const& data )
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

// =================================================================================================
//     Input Output
// =================================================================================================

/**
 * @brief Print the elements of a Matrix to a stream, using `operator <<` for each element.
 */
template<typename T>
std::ostream& operator<< ( std::ostream& os, const Matrix<T>& matrix )
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
 * @brief Template specialization for `signed char`, in order to print nicely.
 */
template<>
std::ostream& operator<< (std::ostream& os, const Matrix<signed char>& matrix);

/**
 * @brief Template specialization for `unsigned char`, in order to print nicely.
 */
template<>
std::ostream& operator<< (std::ostream& os, const Matrix<unsigned char>& matrix);

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

            // We need some special printing for int char types...
            if( std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value ) {
                out << static_cast<int>( matrix( i, j ));
            } else {
                out << matrix(i, j);
            }
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

template<typename T>
Serializer& operator<<( Serializer& serializer, Matrix<T> const& mat )
{
    // We write the number of rows and cols first.
    // Then, the data serialization will additionally store the size of the underlying vector
    // that is used in the Matrix, which is a bit of overhead, but we live with that for now.
    serializer << mat.rows();
    serializer << mat.cols();
    serializer << mat.data();
    return serializer;
}

template<typename T>
Deserializer& operator>>( Deserializer& deserializer, Matrix<T>& mat )
{
    // This funciton is a friend of the Matrix class, so that we can write to its data directly.
    // Otherwise, we'd need special constructors etc, which is a bit cumbersome.
    deserializer >> mat.rows_;
    deserializer >> mat.cols_;
    deserializer >> mat.data_;

    // Check that the input was actually valid.
    if( mat.rows_ * mat.cols_ != mat.data_.size() ) {
        throw std::invalid_argument(
            "Cannot deserialize Matrix of expected size " + std::to_string( mat.rows_ ) +
            " * " + std::to_string( mat.cols_ ) + " = " + std::to_string( mat.rows_ * mat.cols_ ) +
            " with data of size " + std::to_string( mat.data_.size() )
        );
    }
    return deserializer;
}

// =================================================================================================
//     Swapping
// =================================================================================================

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

} // namespace utils
} // namespace genesis

#endif // include guard
