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

#include "genesis/utils/containers/matrix/operators.hpp"

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

} // namespace utils
} // namespace genesis
