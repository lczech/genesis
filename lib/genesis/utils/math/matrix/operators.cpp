/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/math/matrix/operators.hpp"

#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Helpful Functions
// ================================================================================================

std::pair<size_t, size_t> triangular_indices( size_t k, size_t n )
{
    // Using equations from http://stackoverflow.com/a/27088560/4184258
    // See also https://en.wikipedia.org/wiki/Triangular_number

    size_t const i = n - 2 - std::floor( std::sqrt( 4 * n * ( n - 1 ) - 7 - ( 8 * k )) / 2.0 - 0.5 );
    size_t const j = k + i + 1 - n * ( n - 1 ) / 2 + ( n - i ) * (( n - i ) - 1 ) / 2;
    return { i, j };
}

size_t triangular_index( size_t i, size_t j, size_t n )
{
    size_t const k = ( n * ( n - 1 ) / 2 ) - ( n -  i ) * (( n - i ) - 1 ) / 2 + j - i - 1;
    return k;
}

size_t triangular_size( size_t n )
{
    return ( n * n - n ) / 2;
}

// ================================================================================================
//     Matrix Operators
// ================================================================================================

Matrix<double> matrix_multiplication( Matrix<double> const& a, Matrix<double> const& b)
{
    if( a.cols() != b.rows() ) {
        throw std::runtime_error( "Cannot multiply matrices if a.cols() != b.rows()." );
    }

    // Simple and naive. Fast enough for the few occasions were we need this.
    // If Genesis at some point starts to need more elaborate matrix operations, it might be
    // worth including some proper library for this.
    auto result = Matrix<double>( a.rows(), b.cols() );
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

} // namespace utils
} // namespace genesis
