/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/formats/svg/pie_chart.hpp"

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/color/color.hpp"

#include <cassert>
#include <cstdlib>
#include <numeric>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Pie Chart
// =================================================================================================

SvgGroup make_svg_pie_chart(
    std::vector<double> const& values,
    std::vector<Color> const& colors,
    double radius,
    double start_angle,
    bool clockwise
) {

    // Edge cases.
    if( values.size() == 0 ) {
        throw std::runtime_error( "No values given to make svg pie chart" );
    }
    if( colors.size() == 0 ) {
        throw std::runtime_error( "No colors given to make svg pie chart" );
    }
    if( colors.size() < values.size() ) {
        throw std::runtime_error(
            "No enough colors given to make svg pie chart. Given " +
            std::to_string( colors.size() ) + " colors, but " +
            std::to_string( values.size() ) + " values."
        );
    }
    auto const all_good = std::all_of( values.cbegin(), values.cend(), []( double v ){
        return std::isfinite(v) && v >= 0.0;
    });
    if( ! all_good ) {
        throw std::runtime_error(
            "Invalid negative or non-finite values given to make svg pie chart"
        );
    }

    // Prepare result and helper values.
    SvgGroup result;
    double const total = std::accumulate( values.begin(), values.end(), 0.0 );
    double const dir = clockwise ? 1.0 : -1.0;

    // Fill the chart. We keep a running sum (in radians) of how much pie we have filled.
    double sum = 0.0;
    for( size_t i = 0; i < values.size(); ++i ) {
        if( values[i] == 0.0 ) {
            continue;
        }

        // We compute the angle that the wedge spans, and where it starts and ends.
        // If we are going counter clockwise, we have to swap the positions, due to
        // how our svg_arc helper function takes its arguments (as it always paints clockwise).
        auto const angle = 2.0 * utils::PI * values[i] / total;
        auto start_a = start_angle + dir * sum;
        auto end_a = start_angle + dir * ( sum + angle );
        if( ! clockwise ) {
            std::swap( start_a, end_a );
        }
        sum += angle;

        // We create a pie segment as a path, moving to the origin, then an arc with the
        // specified angles, then back to the origin. The svg_arc function takes care of that.
        assert( i < colors.size() );
        result << SvgPath(
            {{ svg_arc( 0, 0, radius, start_a, end_a, true ) }},
            SvgStroke( SvgStroke::Type::kNone ),
            SvgFill( colors[ i ] )
        );
    }

    return result;
}

} // namespace utils
} // namespace genesis
