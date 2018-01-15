/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
    auto col_mean_stddev = matrix_col_mean_stddev( data, 0.0000001 );
    assert( col_mean_stddev.size() == data.cols() );

    // Iterate the matrix.
    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        for( size_t c = 0; c < data.cols(); ++c ) {

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
    auto row_mean_stddev = matrix_row_mean_stddev( data, 0.0000001 );
    assert( row_mean_stddev.size() == data.rows() );

    // Iterate the matrix.
    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
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

// ================================================================================================
//     Mean and Stddev
// ================================================================================================

MeanStddevPair matrix_mean_stddev( Matrix<double> const& data, double epsilon)
{
    return mean_stddev( data.data(), epsilon );
}

std::vector<MeanStddevPair> matrix_col_mean_stddev( Matrix<double> const& data, double epsilon )
{
    auto ret = std::vector<MeanStddevPair>( data.cols(), { 0.0, 0.0 } );

    // Nothing to do. Better stop here or we risk dividing by zero.
    if( data.rows() == 0 ) {
        return ret;
    }

    // Iterate columns.
    #pragma omp parallel for
    for( size_t c = 0; c < data.cols(); ++c ) {
        double mean   = 0.0;
        double stddev = 0.0;

        // Calculate column mean.
        for( size_t r = 0; r < data.rows(); ++r ) {
            mean += data( r, c );
        }
        mean /= static_cast<double>( data.rows() );

        // Calculate column std dev.
        for( size_t r = 0; r < data.rows(); ++r ) {
            stddev += (( data( r, c ) - mean ) * ( data( r, c ) - mean ));
        }
        stddev /= static_cast<double>( data.rows() );
        stddev = std::sqrt(stddev);

        // The following in an inelegant (but usual) way to handle near-zero values,
        // which later would cause a division by zero.
        assert( stddev >= 0.0 );
        if( stddev <= epsilon ){
            stddev = 1.0;
        }

        // Set result entries.
        ret[ c ].mean   = mean;
        ret[ c ].stddev = stddev;
    }

    return ret;
}

std::vector<MeanStddevPair> matrix_row_mean_stddev( Matrix<double> const& data, double epsilon )
{
    auto ret = std::vector<MeanStddevPair>( data.rows(), { 0.0, 0.0 } );

    // Nothing to do. Better stop here or we risk dividing by zero.
    if( data.cols() == 0 ) {
        return ret;
    }

    // Iterate columns.
    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        double mean   = 0.0;
        double stddev = 0.0;

        // Calculate row mean.
        for( size_t c = 0; c < data.cols(); ++c ) {
            mean += data( r, c );
        }
        mean /= static_cast<double>( data.cols() );

        // Calculate column std dev.
        for( size_t c = 0; c < data.cols(); ++c ) {
            stddev += (( data( r, c ) - mean ) * ( data( r, c ) - mean ));
        }
        stddev /= static_cast<double>( data.cols() );
        stddev = std::sqrt(stddev);

        // The following in an inelegant (but usual) way to handle near-zero values,
        // which later would cause a division by zero.
        assert( stddev >= 0.0 );
        if( stddev <= epsilon ){
            stddev = 1.0;
        }

        // Set result entries.
        ret[ r ].mean   = mean;
        ret[ r ].stddev = stddev;
    }

    return ret;
}

// =================================================================================================
//     Quartiles
// =================================================================================================

Quartiles matrix_quartiles(
    Matrix<double> const& data
) {
    // We make an expensive copy... Not nice, but works for now.
    auto cpy = data.data();
    std::sort( cpy.begin(), cpy.end() );
    return quartiles( cpy );
}

Quartiles matrix_row_quartiles(
    Matrix<double> const& data,
    size_t                row
) {
    auto tmp = data.row( row ).to_vector();
    std::sort( tmp.begin(), tmp.end() );

    return quartiles( tmp );
}

std::vector<Quartiles> matrix_row_quartiles(
    Matrix<double> const& data
) {
    auto result = std::vector<Quartiles>( data.rows(), Quartiles() );

    #pragma omp parallel for
    for( size_t r = 0; r < data.rows(); ++r ) {
        result[r] = matrix_row_quartiles( data, r );
    }

    return result;
}

Quartiles matrix_col_quartiles(
    Matrix<double> const& data,
    size_t                col
) {
    auto tmp = data.col( col ).to_vector();
    std::sort( tmp.begin(), tmp.end() );

    return quartiles( tmp );
}

std::vector<Quartiles> matrix_col_quartiles(
    Matrix<double> const& data
) {
    auto result = std::vector<Quartiles>( data.cols(), Quartiles() );

    #pragma omp parallel for
    for( size_t c = 0; c < data.cols(); ++c ) {
        result[c] = matrix_col_quartiles( data, c );
    }

    return result;
}

// ================================================================================================
//     Correlation Matrix
// ================================================================================================

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

// ================================================================================================
//     Covariance Matrix
// ================================================================================================

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

// ================================================================================================
//     Sums of Squares Cross Products Matrix
// ================================================================================================

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

// =================================================================================================
//     Correlation Coefficients
// =================================================================================================

double matrix_col_pearson_correlation_coefficient(
    Matrix<double> const& mat1, size_t col1,
    Matrix<double> const& mat2, size_t col2
) {
    // Checks.
    if( mat1.rows() != mat2.rows() ) {
        throw std::runtime_error( "Matrices need to have same number of rows." );
    }
    if( col1 >= mat1.cols() || col2 >= mat2.cols() ) {
        throw std::runtime_error( "Column indices cannot be bigger then number of columns." );
    }

    auto c1 = mat1.col( col1 );
    auto c2 = mat2.col( col2 );
    return pearson_correlation_coefficient( c1.begin(), c1.end(), c2.begin(), c2.end() );
}

double matrix_row_pearson_correlation_coefficient(
    Matrix<double> const& mat1, size_t row1,
    Matrix<double> const& mat2, size_t row2
) {
    // Checks.
    if( mat1.cols() != mat2.cols() ) {
        throw std::runtime_error( "Matrices need to have same number of columns." );
    }
    if( row1 >= mat1.rows() || row2 >= mat2.rows() ) {
        throw std::runtime_error( "Row indices cannot be bigger then number of rows." );
    }

    auto r1 = mat1.row( row1 );
    auto r2 = mat2.row( row2 );
    return pearson_correlation_coefficient( r1.begin(), r1.end(), r2.begin(), r2.end() );
}

double matrix_col_spearmans_rank_correlation_coefficient(
    Matrix<double> const& mat1, size_t col1,
    Matrix<double> const& mat2, size_t col2
) {
    if( mat1.rows() != mat2.rows() ) {
        throw std::runtime_error( "Matrices need to have same number of rows." );
    }
    if( col1 >= mat1.cols() || col2 >= mat2.cols() ) {
        throw std::runtime_error( "Column indices cannot be bigger then number of columns." );
    }

    auto c1 = mat1.col( col1 );
    auto c2 = mat2.col( col2 );
    return spearmans_rank_correlation_coefficient( c1.begin(), c1.end(), c2.begin(), c2.end() );
}

double matrix_row_spearmans_rank_correlation_coefficient(
    Matrix<double> const& mat1, size_t row1,
    Matrix<double> const& mat2, size_t row2
) {
    // Checks.
    if( mat1.cols() != mat2.cols() ) {
        throw std::runtime_error( "Matrices need to have same number of columns." );
    }
    if( row1 >= mat1.rows() || row2 >= mat2.rows() ) {
        throw std::runtime_error( "Row indices cannot be bigger then number of rows." );
    }

    auto r1 = mat1.row( row1 );
    auto r2 = mat2.row( row2 );
    return spearmans_rank_correlation_coefficient( r1.begin(), r1.end(), r2.begin(), r2.end() );
}

} // namespace utils
} // namespace genesis
