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

#include "genesis/utils/tools/color/gradient.hpp"

#include "genesis/utils/tools/color/operators.hpp"
#include "genesis/utils/core/logging.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Gradients
// =================================================================================================

Color interpolate (Color const& color1, Color const& color2, double fraction)
{
    // Helper function that linearily interpolates between two char values.
    auto interpolate_char = []( unsigned char d1, unsigned char d2, double fraction )
    {
        fraction = std::min( std::max( 0.0, fraction ), 1.0 );
        return static_cast<unsigned char>(round( (1.0 - fraction) * d1 + fraction * d2 ));
    };

    unsigned char r = interpolate_char( color1.r(), color2.r(), fraction );
    unsigned char g = interpolate_char( color1.g(), color2.g(), fraction );
    unsigned char b = interpolate_char( color1.b(), color2.b(), fraction );
    return Color(r, g, b);
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
