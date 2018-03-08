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
 * @brief Color operators and functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/color/functions.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/names.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Conversion
// =================================================================================================

Color color_from_bytes( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
    Color c;
    c.r_byte(r);
    c.g_byte(g);
    c.b_byte(b);
    c.a_byte(a);
    return c;
}

Color color_from_hex( std::string const& hex_color, std::string const& prefix )
{
    // Check for correct prefix and trim it.
    if (hex_color.substr(0, prefix.size()) != prefix) {
        throw std::invalid_argument("String does not start with given prefix.");
    }
    auto const h = utils::trim( hex_color.substr( prefix.size() ));

    // Check for incorrect chars, as std::stoul (used later for the actual conversion)
    // might just end parsing instead of throwing...
    if(  h.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos ) {
        throw std::invalid_argument("Expects string with six or eight hexadecimal digits.");
    }

    // Take a two-byte position in h, range [0-3], and parse the to bytes into a double.
    auto hex_parse = [&h] (size_t pos) {
        // We select the two-digit substring at the given position,
        // which resolves to one of the three two-hex-digit color substrings in h.
        auto s = h.substr(pos * 2, 2);
        auto v = std::stoul(s, nullptr, 16);

        // We just fed two chars with a hex value into the conversion, there cannot be a result out
        // of char range from this.
        assert(v < 256);
        return static_cast<double>(v) / 255.0;
    };

    // Check for correct input size, and convert accordingly.
    if( h.size() == 6 ) {
        return Color( hex_parse(0), hex_parse(1), hex_parse(2) );
    } else if( h.size() == 8 ) {
        return Color( hex_parse(0), hex_parse(1), hex_parse(2), hex_parse(3) );
    } else {
        throw std::invalid_argument("Expects string with six or eight hexadecimal digits.");
    }
}

std::string color_to_hex(
    Color const& c , std::string const& prefix, bool uppercase, bool with_alpha
) {
    std::stringstream stream;
    stream << prefix;

    auto hex_print = [&stream, uppercase] (unsigned char v) {
        if (uppercase) {
            stream << std::setfill ('0') << std::setw(2) << std::hex << std::uppercase
                   << static_cast<int>(v);
        } else {
            stream << std::setfill ('0') << std::setw(2) << std::hex << std::nouppercase
                   << static_cast<int>(v);
        }
    };
    hex_print( c.r_byte() );
    hex_print( c.g_byte() );
    hex_print( c.b_byte() );
    if( with_alpha ) {
        hex_print( c.a_byte() );
    }

    return stream.str();
}

// =================================================================================================
//     Color Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, Color const& color)
{
    os << "( " << std::to_string( color.r() )
       << ", " << std::to_string( color.g() )
       << ", " << std::to_string( color.b() )
       << ", " << std::to_string( color.a() ) << " )";
    return os;
}

Color resolve_color_string( std::string const& color_str )
{
    auto const str = trim( color_str );

    // Check if it is a hex color string.
    if( starts_with( str, "#" ) ) {
        return color_from_hex( str );
    }

    return color_from_name( str );
}

// =================================================================================================
//     Color Gradients
// =================================================================================================

Color interpolate (Color const& color1, Color const& color2, double fraction)
{
    // Helper function that linearily interpolates between two values.
    auto interpolate_values = []( double d1, double d2, double fraction )
    {
        return (1.0 - fraction) * d1 + fraction * d2;
    };

    fraction = std::min( std::max( 0.0, fraction ), 1.0 );
    double r = interpolate_values( color1.r(), color2.r(), fraction );
    double g = interpolate_values( color1.g(), color2.g(), fraction );
    double b = interpolate_values( color1.b(), color2.b(), fraction );
    double a = interpolate_values( color1.a(), color2.a(), fraction );
    return Color( r, g, b, a );
}

Color gradient ( std::map<double, Color> const& ranges, double value )
{
    // Check range sanity.
    if( ranges.size() < 2 ) {
        throw std::invalid_argument("Gradient range needs to contain at least two colors.");
    }

    // Get range boundaries.
    double const min = ranges.begin()->first;
    double const max = ranges.rbegin()->first;

    // Ensure the correct interval.
    value = std::min( std::max( min, value ), max );

    // Set hi_bound to the next bigger item in ranges that comes after the value position.
    // lo_bound then is the one before it. Now, the range between them includes value.
    // In case value is exactly equal to a value in ranges, it will be stored in lo_bound.
    auto hi_bound = ranges.upper_bound( value );
    auto lo_bound = std::prev( hi_bound );
    assert( lo_bound != ranges.end() );

    if( hi_bound == ranges.end() ) {
        // This is the boundary case that occurs when value is max.
        // Assert the case and return the last color of the range.
        assert( value == max );
        return ranges.rbegin()->second;
    }

    // Adjust value to the new interval between lo and hi, and return the interpolated color.
    value = ( value - lo_bound->first ) / ( hi_bound->first - lo_bound->first );
    return interpolate( lo_bound->second, hi_bound->second, value );
}

Color heat_gradient (double percentage)
{
    Color red    { 1.0, 0.0, 0.0 };
    Color yellow { 1.0, 1.0, 0.0 };
    Color green  { 0.0, 1.0, 0.0 };

    percentage = std::min(std::max(0.0, percentage), 1.0);

    if (percentage < 0.5) {
        return interpolate(green, yellow, percentage / 0.5);
    }
    return interpolate(yellow, red, (percentage - 0.5) / 0.5);
}

} // namespace utils
} // namespace genesis
