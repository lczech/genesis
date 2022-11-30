#ifndef GENESIS_UTILS_FORMATS_SVG_HELPER_H_
#define GENESIS_UTILS_FORMATS_SVG_HELPER_H_

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

#include "genesis/utils/io/base64.hpp"
#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

struct SvgTransform;

// =================================================================================================
//     Svg Point
// =================================================================================================

struct SvgPoint
{
    explicit SvgPoint( double x = 0.0, double y = 0.0 )
        : x(x)
        , y(y)
    {}

    SvgPoint operator+ ( SvgPoint const& other ) const
    {
        return SvgPoint( x + other.x, y + other.y );
    }

    double x;
    double y;
};

// =================================================================================================
//     Svg Size
// =================================================================================================

struct SvgSize
{
    explicit SvgSize( double width = 0.0, double height = 0.0 )
        : width(width)
        , height(height)
    {}

    double width;
    double height;
};

// =================================================================================================
//     Svg Margin
// =================================================================================================

struct SvgMargin
{
    explicit SvgMargin()
        : SvgMargin( 0.0, 0.0, 0.0, 0.0 )
    {}

    explicit SvgMargin( double all )
        : SvgMargin( all, all, all, all )
    {}

    explicit SvgMargin( double top_bottom, double right_left )
        : SvgMargin( top_bottom, right_left, top_bottom, right_left )
    {}

    explicit SvgMargin( double top, double right_left, double bottom )
        : SvgMargin( top, right_left, bottom, right_left )
    {}

    explicit SvgMargin( double top, double right, double bottom, double left )
        : top(top)
        , right(right)
        , bottom(bottom)
        , left(left)
    {}

    double top;
    double right;
    double bottom;
    double left;
};

// =================================================================================================
//     Svg (Bounding) Box
// =================================================================================================

struct SvgBox
{
    SvgBox() = default;

    SvgBox( SvgPoint top_left, SvgPoint bottom_right )
        : top_left(top_left)
        , bottom_right(bottom_right)
    {
        // Make it fool-proof.
        if( bottom_right.x < top_left.x ) {
            std::swap( bottom_right.x, top_left.x );
        }
        if( bottom_right.y < top_left.y ) {
            std::swap( bottom_right.y, top_left.y );
        }
    }

    SvgBox( SvgPoint top_left_v, double width, double height )
        : top_left(top_left_v)
        , bottom_right( top_left.x + width, top_left.y + height )
    {
        // Allow negative size.
        if( width < 0.0 ) {
            std::swap( top_left.x, bottom_right.x );
        }
        if( height < 0.0 ) {
            std::swap( top_left.y, bottom_right.y );
        }
    }

    double width() const
    {
        return bottom_right.x - top_left.x;
    }

    double height() const
    {
        return bottom_right.y - top_left.y;
    }

    SvgSize size() const
    {
        return SvgSize( width(), height() );
    }

    bool empty() const
    {
        // If everything is zero, this either is an uninitialized default box, or we are at the
        // big bang, where everything is condensed into one point. Either way, this means there
        // is no bounding box, as the singularity does not have a dimension.
        return top_left.x     == 0.0 && top_left.y     == 0.0 &&
               bottom_right.x == 0.0 && bottom_right.y == 0.0;
    }

    static SvgBox combine( SvgBox const& lhs, SvgBox const& rhs )
    {
        // If any of the two boxes is empty, return the other one.
        // (If both are empty, this also returns an empty box.)
        if( lhs.empty() ) {
            return rhs;
        }
        if( rhs.empty() ) {
            return lhs;
        }

        return {
            SvgPoint(
                std::min( lhs.top_left.x, rhs.top_left.x ),
                std::min( lhs.top_left.y, rhs.top_left.y )
            ),
            SvgPoint(
                std::max( lhs.bottom_right.x, rhs.bottom_right.x ),
                std::max( lhs.bottom_right.y, rhs.bottom_right.y )
            )
        };
    }

    SvgPoint top_left;
    SvgPoint bottom_right;

};

// =================================================================================================
//     Svg Drawing Options
// =================================================================================================

struct SvgDrawingOptions
{
    double offset_x = 0.0;
    double offset_y = 0.0;
};

// =================================================================================================
//     Svg Helper Functions
// =================================================================================================

inline std::string svg_comment( std::string const& content )
{
    return "<!-- " + content + " -->";
}

template< typename T >
std::string svg_attribute(
    std::string const& name,
    T const&           value,
    std::string const& unit = ""
) {
    std::stringstream ss;
    ss << " " << name << "=\"" << value << unit << "\"";
    return ss.str();
}

inline std::string svg_data_uri(
    std::string const& media_type,
    std::string const& content,
    bool encode_base64 = false
) {
    return "data:" + media_type + (
        encode_base64
        ? ( ";base64," + base64_encode( content ))
        : ( "," + content )
    );
}

/**
 * @brief Create an arc to use in an SvgPath.
 *
 * Example:
 *
 *     SvgPath(
 *         { svg_arc( 0, 0, 10, start_a, end_a ) },
 *         stroke
 *     );
 *
 * The angles are measured in radians, and always drawn clockwise from start to end.
 * If @p wedge is given, the path is drawn including segments to the circle center;
 * otherwise, the path position is moved to the beginning of the arc first, and left at its end.
 */
std::string svg_arc(
    double center_x, double center_y, double radius,
    double start_angle, double end_angle,
    bool wedge = false
);

/**
 * @brief Compute the bounding box of a set of points.
 */
SvgBox svg_bounding_box(
    std::vector<SvgPoint> const& points
);

/**
 * @brief Compute the bounding box of a set of points, including their transformation.
 */
SvgBox svg_bounding_box(
    std::vector<SvgPoint> const& points,
    SvgTransform const& transform
);

// template< typename T >
// std::string svg_style(
//     std::string const& name,
//     T const&           value,
//     std::string const& unit = ""
// ) {
//     std::stringstream ss;
//     ss << name << ":\"" << value << unit << "\" ";
//     return ss.str();
// }

} // namespace utils
} // namespace genesis

#endif // include guard
