/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/tools/color.hpp"

#include "genesis/utils/tools/color/functions.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

Color::Color( double r, double g, double b, double a )
    : r_( r ), g_( g ), b_( b ), a_( a )
{
    if( r < 0.0 || r > 1.0 || g < 0.0 || g > 1.0 || b < 0.0 || b > 1.0 || a < 0.0 || a > 1.0 ) {
        throw std::invalid_argument(
            "Color can only be constructed with values in range [ 0.0, 1.0 ]."
        );
    }
}

// =================================================================================================
//     Factories
// =================================================================================================

Color Color::from_bytes( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
    return color_from_bytes( r, g, b, a );
}

Color Color::from_hex( std::string const& hex_color, std::string const& prefix )
{
    return color_from_hex( hex_color, prefix );
}

// =================================================================================================
//     Modificators
// =================================================================================================

void Color::r (double value)
{
    if( value < 0.0 || value > 1.0 ) {
        throw std::invalid_argument(
            "Color can only be used with values in range [ 0.0, 1.0 ]."
        );
    }
    r_ = value;
}

void Color::g (double value)
{
    if( value < 0.0 || value > 1.0 ) {
        throw std::invalid_argument(
            "Color can only be used with values in range [ 0.0, 1.0 ]."
        );
    }
    g_ = value;
}

void Color::b (double value)
{
    if( value < 0.0 || value > 1.0 ) {
        throw std::invalid_argument(
            "Color can only be used with values in range [ 0.0, 1.0 ]."
        );
    }
    b_ = value;
}

void Color::a (double value)
{
    if( value < 0.0 || value > 1.0 ) {
        throw std::invalid_argument(
            "Color can only be used with values in range [ 0.0, 1.0 ]."
        );
    }
    a_ = value;
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

unsigned char Color::to_byte_( double v )
{
    assert( 0.0 <= v && v <= 1.0 );
    return static_cast<unsigned char>( std::round( 255.0 * v ));
}

double Color::from_byte_( unsigned char v )
{
    return static_cast<double>( v ) / 255.0;
}

} // namespace utils
} // namespace genesis
