/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/matrix.hpp"

#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/statistics.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace utils {

// ================================================================================================
//     Normalize
// ================================================================================================

std::vector<MinMaxPair<double>> normalize_cols( Matrix<double>& data )
{
    auto col_minmax = matrix_col_minmax( data );
    assert( col_minmax.size() == data.cols() );

    // Iterate the matrix.
    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        for( size_t c = 0; c < data.cols(); ++c ) {
            // Adjust column values.
            assert( col_minmax[c].max >= col_minmax[c].min );
            double diff = col_minmax[c].max - col_minmax[c].min;
            data( r, c ) = ( data( r, c ) - col_minmax[c].min ) / diff;
        }
    }

    return col_minmax;
}

std::vector<MinMaxPair<double>> normalize_rows( Matrix<double>& data )
{
    auto row_minmax = matrix_row_minmax( data );
    assert( row_minmax.size() == data.rows() );

    // Iterate the matrix.
    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        for( size_t c = 0; c < data.cols(); ++c ) {
            // Adjust row values.
            assert( row_minmax[r].max >= row_minmax[r].min );
            double diff = row_minmax[r].max - row_minmax[r].min;
            data( r, c ) = ( data( r, c ) - row_minmax[r].min ) / diff;
        }
    }

    return row_minmax;
}

// ================================================================================================
//     Standardize
// ================================================================================================

std::vector<MeanStddevPair> standardize_cols(
    Matrix<double>& data,
    bool            scale_means,
    bool            scale_std
) {
    auto col_mean_stddev = std::vector<MeanStddevPair>( data.cols() );

    // Iterate the matrix.
    #pragma omp parallel for
    for( size_t c = 0; c < data.cols(); ++c ) {
        col_mean_stddev[c] = mean_stddev( data.col(c).begin(), data.col(c).end(), 0.0000001 );

        for( size_t r = 0; r < data.rows(); ++r ) {

            // Subtract mean (i.e., center data).
            if( scale_means ) {
                data( r, c ) -= col_mean_stddev[c].mean;
            }

            // Scale to unit variance, if needed.
            if( scale_std ) {
                assert( col_mean_stddev[c].stddev > 0.0 );
                data( r, c ) /= col_mean_stddev[c].stddev;
            }
        }
    }

    return col_mean_stddev;
}

std::vector<MeanStddevPair> standardize_rows(
    Matrix<double>& data,
    bool            scale_means,
    bool            scale_std
) {
    auto row_mean_stddev = std::vector<MeanStddevPair>( data.rows() );

    // Iterate the matrix.
    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        row_mean_stddev[r] = mean_stddev( data.row(r).begin(), data.row(r).end(), 0.0000001 );

        for( size_t c = 0; c < data.cols(); ++c ) {

            // Subtract mean (i.e., center data).
            if( scale_means ) {
                data( r, c ) -= row_mean_stddev[r].mean;
            }

            // Scale to unit variance, if needed.
            if( scale_std ) {
                assert( row_mean_stddev[r].stddev > 0.0 );
                data( r, c ) /= row_mean_stddev[r].stddev;
            }
        }
    }

    return row_mean_stddev;
}

// =================================================================================================
//     Filter Constant Columns
// =================================================================================================

std::vector<size_t> filter_constant_columns( utils::Matrix<double>& data, double epsilon )
{
    // Get the column-wise min and max values.
    auto const col_minmax = utils::matrix_col_minmax( data );

    // Store which columns to keep, by index.
    std::vector<size_t> keep_cols;
    for( size_t c = 0; c < data.cols(); ++c ) {
        // Non finite columns are left out in any case.
        if( ! std::isfinite( col_minmax[c].min ) || ! std::isfinite( col_minmax[c].max )) {
            continue;
        }

        assert( col_minmax[c].min <= col_minmax[c].max );
        if (( col_minmax[c].max - col_minmax[c].min ) > epsilon ) {
            keep_cols.push_back( c );
        }
    }
    assert( keep_cols.size() <= data.cols() );

    // Produce new, filtered matrix.
    auto new_mat = utils::Matrix<double>( data.rows(), keep_cols.size() );

    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        for( size_t i = 0; i < keep_cols.size(); ++i ) {
            new_mat( r, i ) = data( r, keep_cols[i] );
        }
    }

    // Overwrite the matrix.
    data = new_mat;
    return keep_cols;
}

// =================================================================================================
//     Correlation and Covariance
// =================================================================================================

Matrix<double> correlation_matrix( Matrix<double> const& data )
{
    // Standardize mean and variance.
    auto stddata = data;
    standardize_cols( stddata, true, true );

    // Calculate matrix. First build the sum of squares, then normalize.
    auto sscp = sums_of_squares_and_cross_products_matrix( stddata );
    for( auto& elem : sscp ) {
        elem /= static_cast<double>( data.rows() );
    }
    return sscp;
}

Matrix<double> covariance_matrix( Matrix<double> const& data )
{
    // Standardize mean, but not the variance.
    auto stddata = data;
    standardize_cols( stddata, true, false );

    // Calculate matrix. First build the sum of squares, then normalize.
    auto sscp = sums_of_squares_and_cross_products_matrix( stddata );
    for( auto& elem : sscp ) {
        elem /= static_cast<double>( data.rows() );
    }
    return sscp;
}

Matrix<double> sums_of_squares_and_cross_products_matrix( Matrix<double> const& data )
{
    auto mat = Matrix<double>( data.cols(), data.cols() );

    // Calculate the covariance matrix.
    for( size_t c1 = 0; c1 < data.cols(); ++c1 ) {
        for( size_t c2 = c1; c2 < data.cols(); ++c2 ) {
            mat( c1, c2 ) = 0.0;
            for( size_t r = 0; r < data.rows(); ++r ) {
                mat( c1, c2 ) += data( r, c1 ) * data( r, c2 );
            }
            mat( c2, c1 ) = mat( c1, c2 );
        }
    }

    return mat;
}

} // namespace utils
} // namespace genesis
