#ifndef GENESIS_UTILS_TOOLS_COLOR_GRADIENT_H_
#define GENESIS_UTILS_TOOLS_COLOR_GRADIENT_H_

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
 * @brief Header for Color gradient functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/color.hpp"

#include <map>

namespace genesis {
namespace utils {

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
 *     ranges[ 0.0 ] = Color(   0, 255, 0 );
 *     ranges[ 0.5 ] = Color( 255, 255, 0 );
 *     ranges[ 1.0 ] = Color( 255,   0, 0 );
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
 * Given a percentage value in the range of `[0.0, 1.0]`, the functions represents the heat of
 * that value. For 0.0, the heat gradient value is green, for 0.5 yellow and for 1.0 red.
 * The values in between are interpolated linearily.
 */
Color heat_gradient (double percentage);

} // namespace utils
} // namespace genesis

#endif // include guard
