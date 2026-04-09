#ifndef GENESIS_UTIL_FORMAT_SVG_PIE_CHART_H_
#define GENESIS_UTIL_FORMAT_SVG_PIE_CHART_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @ingroup util
 */

#include <genesis/util/format/svg/attribute.hpp>
#include <genesis/util/format/svg/group.hpp>
#include <genesis/util/format/svg/shape.hpp>

#include <genesis/util/math/common.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace genesis {
namespace util {
namespace color {

class Color;
class ColorMap;

} // namespace color
} // namespace util
} // namespace genesis

// =================================================================================================
//     Svg Pie Chart
// =================================================================================================

namespace genesis {
namespace util {
namespace format {

/**
 * @brief Make a pie chart in SVG.
 *
 * The given @p values are automatically re-scaled to fill the circle.
 * If there are more @p colors than @p values, only the first ones needed are used;
 * if given @p colors have fewer entries thatn there are @p values, an exception is thrown.
 *
 * The @p radius of the char defaults to 1.0, and the @p start_angle to `-pi/2`, so that the chart
 * starts from the top (most typical). By default, we also draw the values @p clockwise
 * (that is, reversed form the mathematical direction).
 */
SvgGroup make_svg_pie_chart(
    std::vector<double> const& values,
    std::vector<genesis::util::color::Color> const& colors,
    double radius = 1.0,
    double start_angle = - genesis::util::math::PI / 2.0,
    bool clockwise = true
);

} // namespace format
} // namespace util
} // namespace genesis

#endif // include guard
