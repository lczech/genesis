#ifndef GENESIS_UTILS_MATH_MATRIX_STATISTICS_H_
#define GENESIS_UTILS_MATH_MATRIX_STATISTICS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Matrix statstic functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/common.hpp"
#include "utils/math/matrix.hpp"

#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     Structs for Storing Results
// ================================================================================================

/**
 * @brief Helper structure returned from standardize( Matrix<double>& data ) that stores the
 * mean and standard deviation for a column of the original data.
 */
struct MatrixStandardizeData
{
    double mean;
    double stddev;
};

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
std::vector<MinMaxPair<double>> normalize( Matrix<double>& data );

/**
 * @brief Standardize the columns of a Matrix by subtracting the mean and scaling to unit variance.
 *
 * The function performs a z-transformation on the given data.
 *
 * The function returns a vector containing the mean and standard deviation of the columns
 * before standardization, see MatrixStandardizeData. If both `scale_means` and `scale_std` are
 * set to `false`, the original data is not manipulated, while the returned vector still contains
 * the means and standard deviations.
 *
 * @param data        Input data Matrix; standardizing is done inline.
 * @param scale_means If `true` (default), center the data by subtracting the mean per column.
 * @param scale_std   If `true` (default), scale the data to unit variance (or equivalently,
 *                    unit standard deviation) per column.
 * @return            Vector containing the mean and standard deviation for each column before
 *                    standardization.
 */
std::vector<MatrixStandardizeData> standardize(
    Matrix<double>& data,
    bool            scale_means = true,
    bool            scale_std = true
);

// =================================================================================================
//     Min Max
// =================================================================================================

/**
 * @brief Calculate the column-wise min and max values of a Matrix.
 *
 * See also matrix_row_minmax().
 */
std::vector<MinMaxPair<double>> matrix_col_minmax( Matrix<double> const& data );

/**
* @brief Calculate the row-wise min and max values of a Matrix.
*
* See also matrix_col_minmax().
*/
std::vector<MinMaxPair<double>> matrix_row_minmax( Matrix<double> const& data );

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

} // namespace utils
} // namespace genesis

#endif // include guard
