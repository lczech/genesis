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

#include "utils/math/matrix/operators.hpp"

#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Standard Scale
// ================================================================================================

Matrix<double> matrix_multiplication( Matrix<double> const& a, Matrix<double> const& b)
{
    if( a.cols() != b.rows() ) {
        throw std::runtime_error( "Cannot multiply matrices if a.cols() != b.rows()." );
    }

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
