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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/formats/svg/shapes.hpp"

#include "utils/formats/svg/document.hpp"
#include "utils/text/string.hpp"

#include <algorithm>
#include <ostream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Line
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgLine::SvgLine( SvgPoint const& p1, SvgPoint const& p2, SvgStroke const& stroke )
    : point_1( p1 )
    , point_2( p2 )
    , stroke( stroke )
{}

SvgLine::SvgLine( double x1, double y1, double x2, double y2, SvgStroke const& stroke )
    : SvgLine( SvgPoint( x1, y1 ), SvgPoint( x2, y2 ), stroke )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

void SvgLine::offset( double x, double y )
{
    point_1.x += x;
    point_1.y += y;
    point_2.x += x;
    point_2.y += y;
}

SvgBox SvgLine::bounding_box() const
{
    return {
        SvgPoint( std::min( point_1.x, point_2.x ), std::min( point_1.y, point_2.y )),
        SvgPoint( std::max( point_1.x, point_2.x ), std::max( point_1.y, point_2.y ))
    };
}

void SvgLine::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<line";

    out << svg_attribute( "x1", point_1.x );
    out << svg_attribute( "y1", point_1.y );
    out << svg_attribute( "x2", point_2.x );
    out << svg_attribute( "y2", point_2.y );

    stroke.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Rect
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgRect::SvgRect(
    SvgPoint const&  position,
    SvgSize  const&  size,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : position( position )
    , size( size )
    , stroke( stroke )
    , fill( fill )
    , rx( 0.0 )
    , ry( 0.0 )
{}

SvgRect::SvgRect(
    double x, double y,
    double w, double h,
    SvgStroke const& stroke ,
    SvgFill   const& fill
)
    : SvgRect( SvgPoint( x, y ), SvgSize( w, h ), stroke, fill )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

void SvgRect::offset( double x, double y )
{
    position.x += x;
    position.y += y;
}

SvgBox SvgRect::bounding_box() const
{
    return { position, size.width, size.height };
}

void SvgRect::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<rect";

    out << svg_attribute( "x", position.x );
    out << svg_attribute( "y", position.y );
    out << svg_attribute( "width",  size.width );
    out << svg_attribute( "height", size.height );

    if( rx != 0.0 || ry != 0.0 ) {
        out << svg_attribute( "rx", rx );
        out << svg_attribute( "ry", ry );
    }

    stroke.write( out );
    fill.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Circle
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgCircle::SvgCircle(
    SvgPoint const&  center,
    double           radius,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : center( center )
    , radius( radius )
    , stroke( stroke )
    , fill( fill )
{}

SvgCircle::SvgCircle(
    double cx, double cy,
    double radius,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : SvgCircle( SvgPoint( cx, cy ), radius, stroke, fill )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

void SvgCircle::offset( double x, double y )
{
    center.x += x;
    center.y += y;
}

SvgBox SvgCircle::bounding_box() const
{
    return {
        SvgPoint( center.x - radius, center.y - radius ),
        SvgPoint( center.x + radius, center.y + radius )
    };
}

void SvgCircle::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<circle";

    out << svg_attribute( "cx", center.x );
    out << svg_attribute( "cy", center.y );
    out << svg_attribute( "r",  radius );

    stroke.write( out );
    fill.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Ellipse
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgEllipse::SvgEllipse(
    SvgPoint const& center,
    double rx, double ry,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : center( center )
    , rx( rx )
    , ry( ry )
    , stroke( stroke )
    , fill( fill )
{}

SvgEllipse::SvgEllipse(
    double cx, double cy,
    double rx, double ry,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : SvgEllipse( SvgPoint( cx, cy ), rx, ry, stroke, fill )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

void SvgEllipse::offset( double x, double y )
{
    center.x += x;
    center.y += y;
}

SvgBox SvgEllipse::bounding_box() const
{
    return {
        SvgPoint( center.x - rx, center.y - ry ),
        SvgPoint( center.x + rx, center.y + ry )
    };
}

void SvgEllipse::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<ellipse";

    out << svg_attribute( "cx", center.x );
    out << svg_attribute( "cy", center.y );
    out << svg_attribute( "rx", rx );
    out << svg_attribute( "ry", ry );

    stroke.write( out );
    fill.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Polyline
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgPolyline::SvgPolyline(
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : stroke( stroke )
    , fill( fill )
{}

SvgPolyline::SvgPolyline(
    std::vector< SvgPoint > points,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : points( points )
    , stroke( stroke )
    , fill( fill )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

SvgPolyline& SvgPolyline::add( double x, double y )
{
    return add( SvgPoint( x, y ));
}

SvgPolyline& SvgPolyline::add( SvgPoint p )
{
    points.push_back( p );
    return *this;
}

SvgPolyline& SvgPolyline::operator <<( SvgPoint p )
{
    return add( p );
}

void SvgPolyline::offset( double x, double y )
{
    for( auto& point : points ) {
        point.x += x;
        point.y += y;
    }
}

SvgBox SvgPolyline::bounding_box() const
{
    if( points.size() == 0 ) {
        return {};
        // throw std::runtime_error(
        //     "Cannot calculate bounding box of Polyline without any points."
        // );
    }

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

void SvgPolyline::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<polyline";

    out << " points=\"";
    for( size_t i = 0; i < points.size(); ++i ) {
        if( i > 0 ) {
            out << " ";
        }
        out << points[i].x << "," << points[i].y;
    }
    out << "\"";

    stroke.write( out );
    fill.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Polyline
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgPolygon::SvgPolygon(
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : stroke( stroke )
    , fill( fill )
{}

SvgPolygon::SvgPolygon(
    std::vector< SvgPoint > points,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : points( points )
    , stroke( stroke )
    , fill( fill )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

SvgPolygon& SvgPolygon::add( double x, double y )
{
    return add( SvgPoint( x, y ));
}

SvgPolygon& SvgPolygon::add( SvgPoint p )
{
    points.push_back( p );
    return *this;
}

SvgPolygon& SvgPolygon::operator <<( SvgPoint p )
{
    return add( p );
}

void SvgPolygon::offset( double x, double y )
{
    for( auto& point : points ) {
        point.x += x;
        point.y += y;
    }
}

SvgBox SvgPolygon::bounding_box() const
{
    if( points.size() == 0 ) {
        return {};
        // throw std::runtime_error(
        //     "Cannot calculate bounding box of Polygon without any points."
        // );
    }

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

void SvgPolygon::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<polygon";

    out << " points=\"";
    for( size_t i = 0; i < points.size(); ++i ) {
        if( i > 0 ) {
            out << " ";
        }
        out << points[i].x << "," << points[i].y;
    }
    out << "\"";

    stroke.write( out );
    fill.write( out );
    out << " />\n";
}

} // namespace utils
} // namespace genesis
