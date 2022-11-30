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
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <ostream>
#include <sstream>
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
    // We collect all points that are part of the path, and transform later.
    // Could be done more mem efficient by doing the transforms immediately,
    // but well... easier that way for now.
    std::vector<SvgPoint> points;

    // Helper functions to read a single value and a pair of values, e.g., a coordinate.
    // We check that these are values, and not new commands. In the check, we access the first
    // character of the current list element, which is valid, as our split function
    // does not output empty elements.
    auto read_val_ = []( std::vector<std::string> const& list, size_t& i, double& val )
    {
        if( i >= list.size() ) {
            return false;
        }
        assert( ! list[i].empty() );
        if( is_alpha( list[i][0] ) ) {
            return false;
        }
        val = convert_to_double( list[i + 0] );
        ++i;
        return true;
    };
    auto read_coord_pair_ = []( std::vector<std::string> const& list, size_t& i, SvgPoint& coord )
    {
        if( i + 1 >= list.size() ) {
            return false;
        }
        assert( ! list[i].empty() );
        if( is_alpha( list[i][0] ) ) {
            return false;
        }
        coord.x = convert_to_double( list[i + 0] );
        coord.y = convert_to_double( list[i + 1] );
        i += 2;
        return true;
    };

    bool start = true;
    SvgPoint cur;
    for( auto const& elem : elements ) {
        auto const list = split( elem, " \t," );

        // We expect the commands to be separated from their values... That is not according
        // to the svg standard, where there does not need to be a delimiter, but it works for now,
        // where we have control over the path commands that we use.
        for( size_t i = 0; i < list.size(); ) {
            auto const& tok = list[i];
            if( start && tok != "M" && tok != "m" ) {
                throw std::invalid_argument( "SvgPath has to start with an M or m command." );
            }
            start = false;

            // Start the token processing at the first value. Easier for all below code.
            // We also define a coord helper, and some others to fill with data.
            ++i;
            double val;
            SvgPoint coord;

            // moveto and lineto
            // https://svgwg.org/svg2-draft/paths.html#PathDataMovetoCommands
            // https://svgwg.org/svg2-draft/paths.html#PathDataLinetoCommands
            if( tok == "M" || tok == "L" || tok == "m" || tok == "l" ) {
                while( read_coord_pair_( list, i, coord )) {
                    if( tok == "M" || tok == "L" ) {
                        cur = coord;
                    }
                    if( tok == "m" || tok == "l" ) {
                        cur = cur + coord;
                    }
                    points.push_back( cur );
                }
                continue;
            }

            // closepath
            // https://svgwg.org/svg2-draft/paths.html#PathDataClosePathCommand
            if( tok == "Z" || tok == "z" ) {
                continue;
            }

            // lineto h and v
            // https://svgwg.org/svg2-draft/paths.html#PathDataLinetoCommands
            if( tok == "H" || tok == "h" || tok == "V" || tok == "v" ) {
                while( read_val_( list, i, val )) {
                    if( tok == "H" ) {
                        cur.x = val;
                    }
                    if( tok == "h" ) {
                        cur.x += val;
                    }
                    if( tok == "V" ) {
                        cur.y = val;
                    }
                    if( tok == "v" ) {
                        cur.y += val;
                    }
                    points.push_back( cur );
                }
                continue;
            }

            // cubic and quadratic bezier
            // https://svgwg.org/svg2-draft/paths.html#PathDataCubicBezierCommands
            // https://svgwg.org/svg2-draft/paths.html#PathDataQuadraticBezierCommands
            if(
                tok == "C" || tok == "c" || tok == "S" || tok == "s" ||
                tok == "Q" || tok == "q" || tok == "T" || tok == "t"
            ) {
                while( read_coord_pair_( list, i, coord )) {
                    // We read extra coordinates as needed. Only the last pair is what we want.
                    bool good = true;
                    if(
                        tok == "C" || tok == "c" ||
                        tok == "S" || tok == "s" ||
                        tok == "Q" || tok == "q"
                    ) {
                        good &= read_coord_pair_( list, i, coord );
                    }
                    if( tok == "C" || tok == "c" ) {
                        good &= read_coord_pair_( list, i, coord );
                    }
                    if( ! good ) {
                        throw std::runtime_error( "Invalid SvgPath Bezier command." );
                    }

                    // Turn them into our current coordinate as needed, and store it.
                    if( tok == "C" || tok == "S" || tok == "Q" || tok == "T" ) {
                        cur = coord;
                    }
                    if( tok == "c" || tok == "s" || tok == "q" || tok == "t" ) {
                        cur = cur + coord;
                    }
                    points.push_back( cur );
                }
                continue;
            }

            // elliptical arc curve
            // https://svgwg.org/svg2-draft/paths.html#PathDataEllipticalArcCommands
            if( tok == "A" || tok == "a" ) {
                while( read_coord_pair_( list, i, coord )) {
                    // We simplify our code here a bit, and read the flags as doubles...
                    // we ignore them anyway, so that should work.
                    bool good = true;
                    good &= read_val_( list, i, val );
                    good &= read_coord_pair_( list, i, coord );
                    good &= read_coord_pair_( list, i, coord );
                    if( ! good ) {
                        throw std::runtime_error( "Invalid SvgPath elliptical arc curve command." );
                    }

                    // Turn them into our current coordinate as needed, and store it.
                    if( tok == "A" ) {
                        cur = coord;
                    }
                    if( tok == "a" ) {
                        cur = cur + coord;
                    }
                    points.push_back( cur );
                }
                continue;
            }

            // Reaching here means we did not find the token.
            throw std::runtime_error( "Invalid SvgPath command '" + tok + "'." );
        }
    }

    return svg_bounding_box( points, transform );
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
