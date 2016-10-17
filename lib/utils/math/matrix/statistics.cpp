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

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Standard Scale
// ================================================================================================

void standard_scale( Matrix<double>& data, bool scale_means, bool scale_std )
{
    // Nothing to do.
    if( ! scale_means && ! scale_std ) {
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
                data( r, c ) /= stddev * sqrt( static_cast<double>( data.rows() ) );
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
    // Init the result matrix.
    auto cormat = Matrix<double>( data.cols(), data.cols() );

    // Standardize mean and variance.
    auto stddata = data;
    standard_scale( stddata );

    // Calculate the correlation matrix.
    for( size_t c1 = 0; c1 < stddata.cols(); ++c1 ) {
        // cormat( c1, c1 ) = 1.0;

        for( size_t c2 = c1; c2 < stddata.cols(); ++c2 ) {
            cormat( c1, c2 ) = 0.0;
            for( size_t r = 0; r < stddata.rows(); ++r ) {
                cormat( c1, c2 ) += stddata( r, c1 ) * stddata( r, c2 );
            }
            cormat( c2, c1 ) = cormat( c1, c2 );
        }
    }

    return cormat;
}

// ================================================================================================
//     Covariance Matrix
// ================================================================================================

Matrix<double> covariance_matrix( Matrix<double> const& data )
{
    // Init the result matrix.
    auto covmat = Matrix<double>( data.cols(), data.cols() );

    // Standardize mean, but not the variance.
    auto stddata = data;
    standard_scale( stddata, true, false );

    // Calculate the covariance matrix.
    for( size_t c1 = 0; c1 < stddata.cols(); ++c1 ) {
        for( size_t c2 = c1; c2 < stddata.cols(); ++c2 ) {
            covmat( c1, c2 ) = 0.0;
            for( size_t r = 0; r < stddata.rows(); ++r ) {
                covmat( c1, c2 ) += stddata( r, c1 ) * stddata( r, c2 );
            }
            covmat( c2, c1 ) = covmat( c1, c2 );
        }
    }

    return covmat;
}

// ================================================================================================
//     Sums of Squares Cross Products Matrix
// ================================================================================================

Matrix<double> sums_of_squares_cross_products_matrix( Matrix<double> const& data )
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
