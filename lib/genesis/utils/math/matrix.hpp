#ifndef GENESIS_UTILS_MATH_MATRIX_H_
#define GENESIS_UTILS_MATH_MATRIX_H_

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

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/math/statistics.hpp"

#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Min Max
// =================================================================================================

/**
 * @brief Calculate the min and max values of a Matrix.
 *
 * See also matrix_col_minmax() and matrix_row_minmax().
 */
template< typename T >
MinMaxPair<T> matrix_minmax( Matrix<T> const& data )
{
    // Edge case
    if( data.rows() == 0 || data.cols() == 0 ) {
        return { 0.0, 0.0 };
    }

    // Init with first element
    auto ret = MinMaxPair<T>{ data( 0, 0 ), data( 0, 0 ) };

    // Iterate
    for( auto const& e : data ) {
        ret.min = std::min( ret.min, e );
        ret.max = std::max( ret.max, e );
    }

    return ret;
}

/**
 * @brief Calculate the column-wise min and max values of a Matrix.
 *
 * See also matrix_row_minmax() and matrix_minmax().
 */
template< typename T >
std::vector<MinMaxPair<T>> matrix_col_minmax( Matrix<T> const& data )
{
    auto ret = std::vector<MinMaxPair<T>>( data.cols(), { 0.0, 0.0 } );

    // Nothing to do.
    if( data.rows() == 0 ) {
        return ret;
    }

    // Init with the first row.
    for( size_t c = 0; c < data.cols(); ++c ) {
        ret[ c ].min = data( 0, c );
        ret[ c ].max = data( 0, c );
    }

    // Now go through all other rows.
    // Our Matrix is row-major, so this way we make best use of the cache.
    for( size_t r = 1; r < data.rows(); ++r ) {
        for( size_t c = 0; c < data.cols(); ++c ) {

            // Find min and max of the column.
            ret[ c ].min = std::min( ret[ c ].min, data( r, c ) );
            ret[ c ].max = std::max( ret[ c ].max, data( r, c ) );
        }
    }

    return ret;
}

/**
* @brief Calculate the row-wise min and max values of a Matrix.
*
* See also matrix_col_minmax() and matrix_minmax().
*/
template< typename T >
std::vector<MinMaxPair<T>> matrix_row_minmax( Matrix<T> const& data )
{
    auto ret = std::vector<MinMaxPair<T>>( data.rows(), { 0.0, 0.0 } );

    // Nothing to do.
    if( data.cols() == 0 ) {
        return ret;
    }

    for( size_t r = 0; r < data.rows(); ++r ) {
        // Init with the first col.
        ret[ r ].min = data( r, 0 );
        ret[ r ].max = data( r, 0 );

        // Go through all other cols.
        for( size_t c = 1; c < data.cols(); ++c ) {
            ret[ r ].min = std::min( ret[ r ].min, data( r, c ) );
            ret[ r ].max = std::max( ret[ r ].max, data( r, c ) );
        }
    }

    return ret;
}

/**
 * @brief Calculate the sum of all elements in a Matrix.
 *
 * See also matrix_col_sums() and matrix_row_sums().
 */
template< typename T >
T matrix_sum( Matrix<T> const& data )
{
    // Get row sums.
    auto sum = T{};
    for( auto const& e : data ) {
        sum += e;
    }
    return sum;
}

/**
 * @brief Calculate the sum of each row and return the result as a vector.
 *
 * See also matrix_col_sums() and matrix_sum().
 */
template< typename T >
std::vector<T> matrix_row_sums( Matrix<T> const& data )
{
    // Get row sums.
    auto row_sums = std::vector<T>( data.rows(), T{} );
    for( size_t i = 0; i < data.rows(); ++i ) {
        for( size_t j = 0; j < data.cols(); ++j ) {
            row_sums[ i ] += data( i, j );
        }
    }
    return row_sums;
}

/**
 * @brief Calculate the sum of each column and return the result as a vector.
 *
 * See also matrix_row_sums() and matrix_sum().
 */
template< typename T >
std::vector<T> matrix_col_sums( Matrix<T> const& data )
{
    // Get col sums.
    auto col_sums = std::vector<T>( data.cols(), T{} );
    for( size_t i = 0; i < data.rows(); ++i ) {
        for( size_t j = 0; j < data.cols(); ++j ) {
            col_sums[ j ] += data( i, j );
        }
    }
    return col_sums;
}

// =================================================================================================
//     Normalization and Standardization
// =================================================================================================

/**
 * @brief Normalize the columns of a Matrix so that all values are in the range `[ 0.0, 1.0 ]`.
 *
 * The Matrix is manipulated inline. For each column, the new values of the Matrix are calculated
 * as \f$ x_{new} = \frac{ x - x_{min} }{ x_{max} - x_{min} } \f$.
 *
 * The function returns a vector containing the `min` and `max` values of the columns
 * before normalization, see matrix_col_minmax().
 *
 * @param data        Input data Matrix; normalization is done inline.
 * @return            Vector containing the min and max values for each column before normalization.
 */
std::vector<MinMaxPair<double>> normalize_cols( Matrix<double>& data );

/**
 * @brief Normalize the rows of a Matrix so that all values are in the range `[ 0.0, 1.0 ]`.
 *
 * The Matrix is manipulated inline. For each row, the new values of the Matrix are calculated
 * as \f$ x_{new} = \frac{ x - x_{min} }{ x_{max} - x_{min} } \f$.
 *
 * The function returns a vector containing the `min` and `max` values of the rows
 * before normalization, see matrix_row_minmax().
 *
 * @param data        Input data Matrix; normalization is done inline.
 * @return            Vector containing the min and max values for each row before normalization.
 */
std::vector<MinMaxPair<double>> normalize_rows( Matrix<double>& data );

/**
 * @brief Standardize the columns of a Matrix by subtracting the mean and scaling to unit variance.
 *
 * The function performs a column-wise z-transformation on the given data.
 *
 * The function returns a vector containing the mean and standard deviation of the columns
 * before standardization, see MeanStddevPair. If both `scale_means` and `scale_std` are
 * set to `false`, the original data is not manipulated, while the returned vector still contains
 * the means and standard deviations. See also matrix_col_mean_stddev().
 *
 * @param data        Input data Matrix; standardizing is done inline.
 * @param scale_means If `true` (default), center the data by subtracting the mean per column.
 * @param scale_std   If `true` (default), scale the data to unit variance (or equivalently,
 *                    unit standard deviation) per column.
 * @return            Vector containing the mean and standard deviation for each column before
 *                    standardization.
 */
std::vector<MeanStddevPair> standardize_cols(
    Matrix<double>& data,
    bool            scale_means = true,
    bool            scale_std = true
);

/**
 * @brief Standardize the rows of a Matrix by subtracting the mean and scaling to unit variance.
 *
 * The function performs a row-wise z-transformation on the given data.
 *
 * The function returns a vector containing the mean and standard deviation of the rows
 * before standardization, see MeanStddevPair. If both `scale_means` and `scale_std` are
 * set to `false`, the original data is not manipulated, while the returned vector still contains
 * the means and standard deviations. See also matrix_row_mean_stddev().
 *
 * @param data        Input data Matrix; standardizing is done inline.
 * @param scale_means If `true` (default), center the data by subtracting the mean per column.
 * @param scale_std   If `true` (default), scale the data to unit variance (or equivalently,
 *                    unit standard deviation) per column.
 * @return            Vector containing the mean and standard deviation for each column before
 *                    standardization.
 */
std::vector<MeanStddevPair> standardize_rows(
    Matrix<double>& data,
    bool            scale_means = true,
    bool            scale_std = true
);

// =================================================================================================
//     Mean and Stddev
// =================================================================================================

/**
 * @brief Calcualte the mean and standard deviation of all elements in a Matrix.
 *
 * If the resulting standard deviation is below the given @p epsilon (e.g, `0.0000001`), it is
 * "corrected" to be `1.0` instead. This is an inelegant (but usual) way to handle near-zero values,
 * which for some use cases would cause problems like a division by zero later on.
 * By default, @p epsilon is `-1.0`, which deactivates this check - a standard deviation can never
 * be below `0.0`.
 *
 * See also matrix_row_mean_stddev() and matrix_col_mean_stddev().
 */
MeanStddevPair matrix_mean_stddev(
    Matrix<double> const& data,
    double epsilon = -1.0
);

/**
 * @brief Calcualte the column-wise mean and standard deviation of a Matrix.
 *
 * If the resulting standard deviation is below the given @p epsilon (e.g, `0.0000001`), it is
 * "corrected" to be `1.0` instead. This is an inelegant (but usual) way to handle near-zero values,
 * which for some use cases would cause problems like a division by zero later on.
 * By default, @p epsilon is `-1.0`, which deactivates this check - a standard deviation can never
 * be below `0.0`.
 *
 * See also matrix_row_mean_stddev() and matrix_mean_stddev().
 */
std::vector<MeanStddevPair> matrix_col_mean_stddev(
    Matrix<double> const& data,
    double epsilon = -1.0
);

/**
 * @brief Calcualte the row-wise mean and standard deviation of a Matrix.
 *
 * If the resulting standard deviation is below the given @p epsilon (e.g, `0.0000001`), it is
 * "corrected" to be `1.0` instead. This is an inelegant (but usual) way to handle near-zero values,
 * which for some use cases would cause problems like a division by zero later on.
 * By default, @p epsilon is `-1.0`, which deactivates this check - a standard deviation can never
 * be below `0.0`.
 *
 * See also matrix_col_mean_stddev() and matrix_mean_stddev().
 */
std::vector<MeanStddevPair> matrix_row_mean_stddev(
    Matrix<double> const& data,
    double epsilon = -1.0
);

// =================================================================================================
//     Quartiles
// =================================================================================================

/**
 * @brief Calculate the Quartiles of the elmements in Matrix of `double`.
 */
Quartiles matrix_quartiles(
    Matrix<double> const& data
);

Quartiles matrix_row_quartiles(
    Matrix<double> const& data,
    size_t                row
);

std::vector<Quartiles> matrix_row_quartiles(
    Matrix<double> const& data
);

Quartiles matrix_col_quartiles(
    Matrix<double> const& data,
    size_t                col
);

std::vector<Quartiles> matrix_col_quartiles(
    Matrix<double> const& data
);

// =================================================================================================
//     Correlation and Covariance
// =================================================================================================

/**
 * @brief Calculate the correlation Matrix of a given `data` Matrix.
 *
 * This is done by standardizing the mean and variance of the given `data` and then calculating the
 * sums_of_squares_and_cross_products_matrix().
 */
Matrix<double> correlation_matrix( Matrix<double> const& data );

/**
 * @brief Calculate the covariance Matrix of a given `data` Matrix.
 *
 * This is done by standardizing the mean of the given `data` and then calculating the
 * sums_of_squares_and_cross_products_matrix().
 */
Matrix<double> covariance_matrix( Matrix<double> const& data );

/**
 * @brief Calculate the Sums of Squares and Cross Products Matrix (SSCP Matrix).
 */
Matrix<double> sums_of_squares_and_cross_products_matrix( Matrix<double> const& data );

// =================================================================================================
//     Correlation Coefficients
// =================================================================================================

/**
 * @brief Calculate the Pearson Correlation Coefficient between two columns of two
 * @link Matrix Matrices@endlink.
 *
 * Both Matrices need to have the same number of rows. Then, the function calculates the PCC
 * between column @p col1 of Matrix @p mat1 and column @p col2 of Matrix @p mat2.
 */
double matrix_col_pearson_correlation_coefficient(
    Matrix<double> const& mat1, size_t col1,
    Matrix<double> const& mat2, size_t col2
);

/**
 * @brief Calculate the Pearson Correlation Coefficient between two row of two
 * @link Matrix Matrices@endlink.
 *
 * Both Matrices need to have the same number of columns. Then, the function calculates the PCC
 * between row @p row1 of Matrix @p mat1 and row @p row2 of Matrix @p mat2.
 */
double matrix_row_pearson_correlation_coefficient(
    Matrix<double> const& mat1, size_t row1,
    Matrix<double> const& mat2, size_t row2
);

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between two columns of two
 * @link Matrix Matrices@endlink.
 *
 * Both Matrices need to have the same number of rows. Then, the function calculates Spearman's Rho
 * between column @p col1 of Matrix @p mat1 and column @p col2 of Matrix @p mat2.
 */
double matrix_col_spearmans_rank_correlation_coefficient(
    Matrix<double> const& mat1, size_t col1,
    Matrix<double> const& mat2, size_t col2
);

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between two row of two
 * @link Matrix Matrices@endlink.
 *
 * Both Matrices need to have the same number of columns. Then, the function calculates Spearman's
 * Rho between row @p row1 of Matrix @p mat1 and row @p row2 of Matrix @p mat2.
 */
double matrix_row_spearmans_rank_correlation_coefficient(
    Matrix<double> const& mat1, size_t row1,
    Matrix<double> const& mat2, size_t row2
);

// =================================================================================================
//     Sorting
// =================================================================================================

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
 * See also matrix_sort_by_row_sum_symmetric(). In case of symmetric matrices, it should yield
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

/**
 * @brief Sort a Matrix so that the highest entries are on the diagonal.
 *
 * The Matrix is sorted by moving the row and col with the highest entry to `[ 0, 0 ]`,
 * and then applying this operation to the rest of the matrix (that is, excluding the first row
 * and col) iteratievly.
 * The Matrix does not have to be symmetrical, but sorting keeps rows and columns intact.
 */
template< typename T >
Matrix<T> matrix_sort_diagonal_symmetric( Matrix<T> const& data )
{
    if( data.rows() != data.cols() ) {
        throw std::runtime_error( "Symmetric sort only works on square matrices." );
    }

    // Find the row and col that contains the max element in the rest of the matrix,
    // that is, excluding the first rows and cols according to start.
    auto find_max = []( Matrix<T> const& mat, size_t start ){
        size_t max_r = start;
        size_t max_c = start;
        for( size_t r = start; r < mat.rows(); ++r ) {
            for( size_t c = start; c < mat.cols(); ++c ) {
                if( mat(r, c) > mat( max_r, max_c ) ) {
                    max_r = r;
                    max_c = c;
                }
            }
        }
        return std::make_pair( max_r, max_c );
    };

    // Sort by swapping rows and cols.
    auto mat = data;
    assert( mat.rows() == mat.cols() );
    for( size_t i = 0; i < mat.rows(); ++i ) {
        auto const max_pair = find_max( mat, i );
        matrix_swap_rows( mat, i, max_pair.first );
        matrix_swap_cols( mat, i, max_pair.second );
    }
    return mat;
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
