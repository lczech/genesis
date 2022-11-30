/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/utils/formats/svg/helper.hpp"

#include "genesis/utils/formats/svg/attributes.hpp"

#include <algorithm>
#include <limits>
#include <ostream>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Helper Functions
// =================================================================================================

// -------------------------------------------------------------
//     svg_arc
// -------------------------------------------------------------

std::string svg_arc(
    double center_x, double center_y, double radius,
    double start_angle, double end_angle,
    bool wedge
) {
    std::string large_arc;
    if( start_angle > end_angle ) {
        large_arc = ( end_angle - start_angle <= utils::PI ? "1" : "0" );
    } else {
        large_arc = ( end_angle - start_angle <= utils::PI ? "0" : "1" );
    }

    double start_x = center_x + ( radius * std::cos( end_angle ));
    double start_y = center_y + ( radius * std::sin( end_angle ));
    double end_x   = center_x + ( radius * std::cos( start_angle ));
    double end_y   = center_y + ( radius * std::sin( start_angle ));

    std::ostringstream os;
    if( wedge ) {
        os << "M " << center_x << " " << center_y << " ";
        os << "L " << start_x << " " << start_y << " ";
    } else {
        os << "M " << start_x << " " << start_y << " ";
    }
    os << "A " << radius << " " << radius << " " << 0 << " " << large_arc << " " << 0 << " ";
    os << end_x << " " << end_y;
    if( wedge ) {
        os << "L " << center_x << " " << center_y << " ";
    }
    return os.str();
}

// -------------------------------------------------------------
//     svg_bounding_box
// -------------------------------------------------------------

SvgBox svg_bounding_box(
    std::vector<SvgPoint> const& points
) {
    if( points.size() == 0 ) {
        return {};
        // throw std::runtime_error(
        //     "Cannot calculate bounding box of Polyline without any points."
        // );
    }

    // Without transformations: Get the box around the points.
    auto minmax_x = std::minmax_element(
        points.begin(), points.end(),
        []( SvgPoint lhs, SvgPoint rhs ){ return lhs.x < rhs.x; }
    );
    auto minmax_y = std::minmax_element(
        points.begin(), points.end(),
        []( SvgPoint lhs, SvgPoint rhs ){ return lhs.y < rhs.y; }
    );
    return {
        SvgPoint( minmax_x.first->x,  minmax_y.first->y ),
        SvgPoint( minmax_x.second->x, minmax_y.second->y )
    };
}

SvgBox svg_bounding_box(
    std::vector<SvgPoint> const& points,
    SvgTransform const& transform
) {
    if( points.size() == 0 ) {
        return {};
        // throw std::runtime_error(
        //     "Cannot calculate bounding box of Polyline without any points."
        // );
    }

    // Init the boundaries.
    auto min_x =   std::numeric_limits<double>::infinity();
    auto max_x = - std::numeric_limits<double>::infinity();
    auto min_y =   std::numeric_limits<double>::infinity();
    auto max_y = - std::numeric_limits<double>::infinity();

    // Transform all points individually, and capture their coordinates.
    for( auto const& p : points ) {
        auto const tr_point = transform.apply( p );
        min_x = std::min( min_x, tr_point.x );
        max_x = std::max( max_x, tr_point.x );
        min_y = std::min( min_y, tr_point.y );
        max_y = std::max( max_y, tr_point.y );
    }
    return {
        SvgPoint( min_x, min_y ),
        SvgPoint( max_x, max_y )
    };
}

} // namespace utils
} // namespace genesis
