/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "utils/math/matrix/statistics.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Normalize
// ================================================================================================

void normalize( Matrix<double>& data )
{
    // Nothing to do.
    if( data.rows() == 0 ) {
        return;
    }

    // Iterate columns.
    for( size_t c = 0; c < data.cols(); ++c ) {

        // Find min and max of the column.
        double min = data( 0, c );
        double max = data( 0, c );
        for( size_t r = 1; r < data.rows(); ++r ) {
            min = std::min( min, data( r, c ) );
            max = std::max( max, data( r, c ) );
        }

        // Adjust column values.
        double diff = max - min;
        for( size_t r = 0; r < data.rows(); ++r ) {
            data( r, c ) = ( data( r, c ) - min ) / diff;
        }
    }
}

// ================================================================================================
//     Standardize
// ================================================================================================

void standardize( Matrix<double>& data, bool scale_means, bool scale_std )
{
    // Nothing to do.
    if( ! scale_means && ! scale_std ) {
        return;
    }
    if( data.rows() == 0 ) {
        return;
    }

    // Minimum dev that we allow (necessary for numerical reasons).
    double const eps = 0.0000001;

    // Iterate columns.
    for( size_t c = 0; c < data.cols(); ++c ) {
        double mean   = 0.0;
        double stddev = 0.0;

        // Calculate column mean.
        for( size_t r = 0; r < data.rows(); ++r ) {
            mean += data( r, c );
        }
        mean /= static_cast<double>( data.rows() );

        // Calculate column std dev, if needed.
        if( scale_std ) {
            for( size_t r = 0; r < data.rows(); ++r ) {
                stddev += (( data( r, c ) - mean ) * ( data( r, c ) - mean ));
            }
            stddev /= static_cast<double>( data.rows() );
            stddev = sqrt(stddev);

            // The following in an inelegant (but usual) way to handle near-zero values,
            // which later would cause a division by zero.
            if( stddev <= eps ){
                stddev = 1.0;
            }
        }

        // Manipulate the data.
        for( size_t r = 0; r < data.rows(); ++r ) {

            // Subtract mean (i.e., center data).
            data( r, c ) -= mean;

            // Scale to unit variance, if needed.
            if( scale_std ) {
                assert( stddev > 0.0 );
                data( r, c ) /= stddev;
                // data( r, c ) /= stddev * sqrt( static_cast<double>( data.rows() ) );
            }

            // If we do not want to center the data, move it back.
            if( ! scale_means ) {
                data( r, c ) += mean;
            }
        }
    }
}

// ================================================================================================
//     Correlation Matrix
// ================================================================================================

Matrix<double> correlation_matrix( Matrix<double> const& data )
{
    // Standardize mean and variance.
    auto stddata = data;
    standardize( stddata, true, true );

    // Calculate matrix.
    return sums_of_squares_and_cross_products_matrix( stddata );
}

// ================================================================================================
//     Covariance Matrix
// ================================================================================================

Matrix<double> covariance_matrix( Matrix<double> const& data )
{
    // Standardize mean, but not the variance.
    auto stddata = data;
    standardize( stddata, true, false );

    // Calculate matrix.
    return sums_of_squares_and_cross_products_matrix( stddata );
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

} // namespace utils
} // namespace genesis
