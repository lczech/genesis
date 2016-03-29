/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Implementationb of Color gradient functions.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <stdexcept>
#include <string>

#include "utils/tools/color/gradient.hpp"

#include "utils/tools/color/operators.hpp"
#include "utils/core/logging.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Gradients
// =================================================================================================

/**
 * @brief Helper function that linearily interpolates between two char values.
 */
unsigned char interpolate (unsigned char d1, unsigned char d2, double fraction)
{
    fraction = std::min( std::max( 0.0, fraction ), 1.0 );
    return static_cast<unsigned char>(round( (1.0 - fraction) * d1 + fraction * d2 ));
}

/**
 * @brief Helper function that linearily interpolates between two Colors.
 */
Color interpolate (Color color1, Color color2, double fraction)
{
    unsigned char r = interpolate(color1.r(), color2.r(), fraction);
    unsigned char g = interpolate(color1.g(), color2.g(), fraction);
    unsigned char b = interpolate(color1.b(), color2.b(), fraction);
    return Color(r, g, b);
}

/**
 * @brief Returns a Color that is created using a color gradient.
 *
 * The function takes a color gradient in form of a map from double to Color.
 * The keys in the map are offset positions between 0.0 and 1.0. The keys 0.0 and 1.0 need to
 * be the lowest and highest keys in the map, respectively. Otherwise, an `std::invalid_argument`
 * is thrown. An arbitrary number of keys in between these boundaries is allowed.
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
 * This map defines the same gradient that is used for `heat_gradient`.
 *
 * The second parameter of this function then takes the position (again, between 0.0 and 1.0) at
 * which the gradient is evaluated. Values outside of the interval `[0.0, 1.0]` are set to the
 * closest interval border value (so, either 0.0 or 1.0).
 *
 * For example, given the range map above, a percentage value of 0.0 gives green; 0.5 gives yellow;
 * 1.0 gives red; values in between are interpolated accordingly.
 */
Color gradient ( std::map<double, Color> const& ranges, double percentage )
{
    // Check range sanity.
    if( ranges.size() < 2 ) {
        throw std::invalid_argument("Gradient range needs to contain at least two colors.");
    }
    if( ranges.begin()->first != 0.0 ) {
        throw std::invalid_argument("Gradient range needs to start with key value 0.0.");
    }
    if( ranges.rbegin()->first != 1.0 ) {
        throw std::invalid_argument("Gradient range needs to end with key value 1.0.");
    }

    // Ensure the correct interval.
    percentage = std::min( std::max( 0.0, percentage ), 1.0 );

    // Set hi_bound to the next bigger item in ranges that comes after the percentage position.
    // lo_bound then is the one before it. Now, the range between them includes percentage.
    // In case percentage is exactly equal to a value in ranges, it will be stored in lo_bound.
    auto hi_bound = ranges.upper_bound( percentage );
    auto lo_bound = std::prev( hi_bound );
    assert( lo_bound != ranges.end() );

    if( hi_bound == ranges.end() ) {
        // This is the boundary case that occurs when percentage is 1.0.
        // Assert the case and return the last color of the range.
        assert( percentage == 1.0 );
        return ranges.rbegin()->second;
    }

    // Adjust percentage to the new interval between lo and hi, and return the interpolated color.
    percentage = ( percentage - lo_bound->first ) / ( hi_bound->first - lo_bound->first );
    return interpolate( lo_bound->second, hi_bound->second, percentage );
}

/**
 * @brief Returns a Color that represents a heat gradient for a percentage value.
 *
 * Given a percentage value in the range of `[0.0, 1.0]`, the functions represents the heat of
 * that value. For 0.0, the heat gradient value is green, for 0.5 yellow and for 1.0 red.
 * The values in between are interpolated accordingly.
 */
Color heat_gradient (double percentage)
{
    Color red    {255,   0, 0};
    Color yellow {255, 255, 0};
    Color green  {  0, 255, 0};

    percentage = std::min(std::max(0.0, percentage), 1.0);

    if (percentage < 0.5) {
        return interpolate(green, yellow, percentage / 0.5);
    }
    return interpolate(yellow, red, (percentage - 0.5) / 0.5);
}

} // namespace utils
} // namespace genesis
