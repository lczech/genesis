#ifndef GENESIS_UTILS_TOOLS_COLOR_FUNCTIONS_H_
#define GENESIS_UTILS_TOOLS_COLOR_FUNCTIONS_H_

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

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Color;

// =================================================================================================
//     Color Conversion
// =================================================================================================

/**
 * @brief Create a Color given three or four values in the range `[ 0, 255 ]` for each
 * of the components red, green and blue, and optionally alpha.
 */
Color color_from_bytes( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255 );

/**
 * @brief Create a Color given a hex color string in the format "#003366[ff]".
 *
 * The hash sign in the beginning can be replaced by any given prefix.
 * If the string is not correctly formatted, an std::invalid_argument exception is thrown.
 * If the string contains only RGB, alpha is set to `1.0`.
 */
Color color_from_hex( std::string const& hex_color, std::string const& prefix = "#" );

/**
 * @brief Return a hex string representation of a Color in the format "#003366[ff]".
 *
 * The hash sign in the beginning can be replaced by any given @p prefix.
 * If @p uppercase is set to `true`, any outputted alphabetical chars
 * (that is, A to F for hex strings) are uppercase.
 * if @p with_alpha is set to `true`, two additional hex digits are printed for the alpha channel.
 */
std::string color_to_hex(
    Color const& c,
    std::string const& prefix = "#",
    bool uppercase = false,
    bool with_alpha = false
);

// =================================================================================================
//     Color Operators
// =================================================================================================

/**
 * @brief Write a textual representation of the Color the a stream, in the format "(r, g, b, a)".
 */
std::ostream& operator<< ( std::ostream& os, Color const& h );

/**
 * @brief Resolve a string representing a color.
 *
 * The string can either be a hex color as accepted by color_to_hex(), e.g., "#003366[ff]",
 * or one of the named colors, see is_color_name() and color_from_name().
 */
Color resolve_color_string( std::string const& color_str );

// =================================================================================================
//     Color Gradients
// =================================================================================================

/**
 * @brief Linearily interpolate between two Color%s.
 */
Color interpolate (Color const& color1, Color const& color2, double fraction);

/**
 * @brief Returns a Color that is created using a color gradient.
 *
 * The function takes a color gradient in form of a map from double to Color.
 * The keys in the map are offset positions (stops) with specific Color%s assigned to them.
 * An arbitrary number of keys in between these boundaries is allowed.
 *
 * Each key is associated with a Color value, which indicates the color at that position.
 *
 * Example:
 *
 *     // Prepare gradient ranges.
 *     auto ranges = std::map<double, Color>();
 *     ranges[ 0.0 ] = Color( 0.0, 1.0, 0.0 );
 *     ranges[ 0.5 ] = Color( 1.0, 1.0, 0.0 );
 *     ranges[ 1.0 ] = Color( 1.0, 0.0, 0.0 );
 *
 *     // Get color at 30% of the green-yellow-red gradient.
 *     Color c = gradient( ranges, 0.3 );
 *
 * This map defines the same gradient that is used for `heat_gradient`. The range boundaries
 * do not have to be `[ 0.0, 1.0 ]`. They can be set to any range that is needed to reflect the
 * range of possible values.
 *
 * The second parameter of this function then takes the position at which the gradient is evaluated.
 * Values outside of the interval that is defined by the range are set to the closest interval
 * border value.
 *
 * For example, given the range map above, a value of 0.0 gives green; 0.5 gives yellow;
 * 1.0 gives red; values in between are interpolated linearily.
 */
Color gradient ( std::map<double, Color> const& ranges, double value );

/**
 * @brief Return a Color that represents a heat gradient for a given percentage value.
 *
 * Given a percentage value in the range of `[ 0.0, 1.0] `, the functions represents the heat of
 * that value. For 0.0, the heat gradient value is green, for 0.5 yellow and for 1.0 red.
 * The values in between are interpolated linearily.
 */
Color heat_gradient (double percentage);

} // namespace utils
} // namespace genesis

#endif // include guard
