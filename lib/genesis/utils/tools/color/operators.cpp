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
 * @brief Color operators.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/color/operators.hpp"

#include <assert.h>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "genesis/utils/tools/color.hpp"

#include "genesis/utils/core/logging.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Conversion
// =================================================================================================

/**
 * @brief Create a Color given three doubles in the range [0.0, 1.0] for each of the components
 * red, green and blue.
 */
Color color_from_doubles (double r, double g, double b)
{
    auto convert = [] (double v) {
        return static_cast<unsigned char>( round(std::min(std::max(0.0, v), 1.0) * 255.0 ) );
    };
    return Color(convert(r), convert(g), convert(b));
}

/**
 * @brief Create a Color given a hex color string in the format "#0033ff".
 *
 * The hash sign in the beginning can be replaced by any given prefix.
 * If the string is not correctly formatted, an std::invalid_argument exception is thrown.
 */
Color color_from_hex( std::string h, std::string prefix )
{
    // Check for correct prefix and trim it.
    if (h.substr(0, prefix.size()) != prefix) {
        throw std::invalid_argument("String does not start with given prefix.");
    }
    h = h.substr(prefix.size());

    // Check for correct input size. We also need to check for incorrect chars, as std::stoul (used
    // later for the actual conversion) might just end parsing instead of throwing...
    if (h.size() != 6 || h.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos) {
        throw std::invalid_argument("Expects string with six hexadecimal digits.");
    }

    // Take a position in h and parse it into a char.
    auto hex_parse = [&h] (size_t pos) {
        // We select the two-digit substring at the given position,
        // which resolves to one of the three two-hex-digit color substrings in h.
        auto s = h.substr(pos * 2, 2);
        auto v = std::stoul(s, nullptr, 16);

        // We just fed two chars with a hex value into the conversion, there cannot be a result out
        // of char range from this.
        assert(v < 256);
        return static_cast<unsigned char>(v);
    };

    return Color(hex_parse(0), hex_parse(1), hex_parse(2));
}

/**
 * @brief Return a hex string representation of a Color in the format "#0033ff".
 *
 * The hash sign in the beginning can be replaced by any given prefix.
 * If uppercase is set to true, any outputted alphabetical chars (between A and F for hex strings)
 * will be uppercase.
 */
std::string color_to_hex( const Color& c , std::string prefix, bool uppercase )
{
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
    hex_print(c.r());
    hex_print(c.g());
    hex_print(c.b());

    return stream.str();
}

// =================================================================================================
//     Color Operators
// =================================================================================================

/**
 * @brief Write a textual representation of the Color the a stream, in the format "(r, g, b)".
 */
std::ostream& operator<< (std::ostream& os, const Color& color)
{
    os << "("  << std::to_string(color.r())
       << ", " << std::to_string(color.g())
       << ", " << std::to_string(color.b()) << ")";
    return os;
}

} // namespace utils
} // namespace genesis
