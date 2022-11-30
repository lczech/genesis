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

#include "genesis/utils/formats/svg/shapes.hpp"

#include "genesis/utils/formats/svg/document.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/text/string.hpp"

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

SvgBox SvgLine::bounding_box() const
{
    // For a line, we do not want to just apply the transformations on the whole box,
    // as this might give a way too large box under rotation. Instead, we rotate both
    // points individually, and then compute the box from that.
    auto const t1 = transform.apply( point_1 );
    auto const t2 = transform.apply( point_2 );
    return {
        SvgPoint( std::min( t1.x, point_2.x ), std::min( t1.y, point_2.y )),
        SvgPoint( std::max( t2.x, point_2.x ), std::max( t2.y, point_2.y ))
    };

    // Without applying transformations:
    // return {
    //     SvgPoint( std::min( point_1.x, point_2.x ), std::min( point_1.y, point_2.y )),
    //     SvgPoint( std::max( point_1.x, point_2.x ), std::max( point_1.y, point_2.y ))
    // };
}

void SvgLine::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<line";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << svg_attribute( "x1", point_1.x + options.offset_x );
    out << svg_attribute( "y1", point_1.y + options.offset_y );
    out << svg_attribute( "x2", point_2.x + options.offset_x );
    out << svg_attribute( "y2", point_2.y + options.offset_y );

    stroke.write( out );
    transform.write( out );
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

SvgBox SvgRect::bounding_box() const
{
    return transform.apply( SvgBox{ position, size.width, size.height });
}

void SvgRect::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<rect";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << svg_attribute( "x", position.x + options.offset_x );
    out << svg_attribute( "y", position.y + options.offset_y );
    out << svg_attribute( "width",  size.width );
    out << svg_attribute( "height", size.height );

    if( rx != 0.0 || ry != 0.0 ) {
        out << svg_attribute( "rx", rx );
        out << svg_attribute( "ry", ry );
    }

    stroke.write( out );
    fill.write( out );
    transform.write( out );
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

SvgBox SvgCircle::bounding_box() const
{
    // Computing the transformations of a circle is tricky... Not bothering with that for now,
    // and instead just compute the transformed large box that definitly fits it.
    return transform.apply( SvgBox{
        SvgPoint( center.x - radius, center.y - radius ),
        SvgPoint( center.x + radius, center.y + radius )
    });
}

void SvgCircle::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<circle";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << svg_attribute( "cx", center.x + options.offset_x );
    out << svg_attribute( "cy", center.y + options.offset_y );
    out << svg_attribute( "r",  radius );

    stroke.write( out );
    fill.write( out );
    transform.write( out );
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

SvgBox SvgEllipse::bounding_box() const
{
    // Same as for the circle, not bothering with the complex transformations as of now...
    return transform.apply( SvgBox{
        SvgPoint( center.x - rx, center.y - ry ),
        SvgPoint( center.x + rx, center.y + ry )
    });
}

void SvgEllipse::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<ellipse";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << svg_attribute( "cx", center.x + options.offset_x );
    out << svg_attribute( "cy", center.y + options.offset_y );
    out << svg_attribute( "rx", rx );
    out << svg_attribute( "ry", ry );

    stroke.write( out );
    fill.write( out );
    transform.write( out );
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

SvgBox SvgPolyline::bounding_box() const
{
    return svg_bounding_box( points, transform );
}

void SvgPolyline::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<polyline";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << " points=\"";
    for( size_t i = 0; i < points.size(); ++i ) {
        if( i > 0 ) {
            out << " ";
        }
        out << ( points[i].x + options.offset_x ) << ",";
        out << ( points[i].y + options.offset_y );
    }
    out << "\"";

    stroke.write( out );
    fill.write( out );
    transform.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Polygon
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
    std::vector< SvgPoint > const& points,
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

SvgBox SvgPolygon::bounding_box() const
{
    return svg_bounding_box( points, transform );
}

void SvgPolygon::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<polygon";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << " points=\"";
    for( size_t i = 0; i < points.size(); ++i ) {
        if( i > 0 ) {
            out << " ";
        }
        out << ( points[i].x + options.offset_x ) << ",";
        out << ( points[i].y + options.offset_y );
    }
    out << "\"";

    stroke.write( out );
    fill.write( out );
    transform.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Path
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgPath::SvgPath(
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : stroke( stroke )
    , fill( fill )
{}

SvgPath::SvgPath(
    std::vector< std::string > const& elements,
    SvgStroke const& stroke,
    SvgFill   const& fill
)
    : elements(elements)
    , stroke( stroke )
    , fill( fill )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

SvgPath& SvgPath::add( std::string elem )
{
    elements.push_back( elem );
    return *this;
}

SvgPath& SvgPath::operator <<( std::string elem )
{
    return add( elem );
}

SvgBox SvgPath::bounding_box() const
{
    // TODO
    return {};
}

void SvgPath::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    (void) options;

    out << repeat( SvgDocument::indentation_string, indent );
    out << "<path";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << " d=\"";
    for( size_t i = 0; i < elements.size(); ++i ) {
        if( i > 0 ) {
            out << " ";
        }
        out << elements[i];
    }
    out << "\"";

    stroke.write( out );
    fill.write( out );
    transform.write( out );
    out << " />\n";
}

// =================================================================================================
//     Svg Use
// =================================================================================================

SvgBox SvgUse::bounding_box() const
{
    // Computing the bounding box here is super involved, as the object could have its own
    // transformations first. Then, its bounding box might already be oversized due to how we
    // apply transformations on bounding boxes, and then we add the offset and apply the
    // transformations of this SvgUse object here afterwards, potentally overscaling again...
    // But that's the best that we can do for now. Good enough.
    auto ob = object->bounding_box();
    ob.top_left.x += offset.x;
    ob.top_left.y += offset.y;
    ob.bottom_right.x += offset.x;
    ob.bottom_right.y += offset.y;
    return transform.apply( ob );
}

void SvgUse::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<use";
    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }
    out << svg_attribute( "xlink:href", "#" + object->id() );
    out << svg_attribute( "x", offset.x + options.offset_x );
    out << svg_attribute( "y", offset.y + options.offset_y );
    transform.write( out );
    out << " />\n";
}

} // namespace utils
} // namespace genesis
