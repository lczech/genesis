#ifndef GENESIS_UTILS_FORMATS_SVG_SHAPES_H_
#define GENESIS_UTILS_FORMATS_SVG_SHAPES_H_

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

#include "utils/formats/svg/helper.hpp"
#include "utils/formats/svg/attributes.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Line
// =================================================================================================

struct SvgLine
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgLine;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgLine(
        SvgPoint const& point_1,
        SvgPoint const& point_2,
        SvgStroke const& stroke = SvgStroke()
    );

    SvgLine(
        double x1, double y1,
        double x2, double y2,
        SvgStroke const& stroke = SvgStroke()
    );

    ~SvgLine() = default;

    SvgLine( SvgLine const& ) = default;
    SvgLine( SvgLine&& )      = default;

    SvgLine& operator= ( SvgLine const& ) = default;
    SvgLine& operator= ( SvgLine&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void offset( double x, double y );
    SvgBox bounding_box() const;

    void write( std::ostream& out, size_t indent = 0 ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    SvgPoint  point_1;
    SvgPoint  point_2;
    SvgStroke stroke;

};

// =================================================================================================
//     Svg Rect
// =================================================================================================

struct SvgRect
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgRect;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgRect(
        SvgPoint const&  position,
        SvgSize  const&  size,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    SvgRect(
        double x, double y,
        double w, double h,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    ~SvgRect() = default;

    SvgRect( SvgRect const& ) = default;
    SvgRect( SvgRect&& )      = default;

    SvgRect& operator= ( SvgRect const& ) = default;
    SvgRect& operator= ( SvgRect&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void offset( double x, double y );
    SvgBox bounding_box() const;

    void write( std::ostream& out, size_t indent = 0 ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    SvgPoint  position;
    SvgSize   size;

    SvgStroke stroke;
    SvgFill   fill;

    double rx;
    double ry;

};

// =================================================================================================
//     Svg Circle
// =================================================================================================

struct SvgCircle
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgCircle;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgCircle(
        SvgPoint const&  center,
        double           radius,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    SvgCircle(
        double cx, double cy,
        double radius,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    ~SvgCircle() = default;

    SvgCircle( SvgCircle const& ) = default;
    SvgCircle( SvgCircle&& )      = default;

    SvgCircle& operator= ( SvgCircle const& ) = default;
    SvgCircle& operator= ( SvgCircle&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void offset( double x, double y );
    SvgBox bounding_box() const;

    void write( std::ostream& out, size_t indent = 0 ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    SvgPoint  center;
    double    radius;

    SvgStroke stroke;
    SvgFill   fill;

};

// =================================================================================================
//     Svg Ellipse
// =================================================================================================

struct SvgEllipse
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgEllipse;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgEllipse(
        SvgPoint const& center,
        double rx, double ry,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    SvgEllipse(
        double cx, double cy,
        double rx, double ry,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    ~SvgEllipse() = default;

    SvgEllipse( SvgEllipse const& ) = default;
    SvgEllipse( SvgEllipse&& )      = default;

    SvgEllipse& operator= ( SvgEllipse const& ) = default;
    SvgEllipse& operator= ( SvgEllipse&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void offset( double x, double y );
    SvgBox bounding_box() const;

    void write( std::ostream& out, size_t indent = 0 ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    SvgPoint  center;
    double    rx;
    double    ry;

    SvgStroke stroke;
    SvgFill   fill;

};

// =================================================================================================
//     Svg Polyline
// =================================================================================================

struct SvgPolyline
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgPolyline;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgPolyline(
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    SvgPolyline(
        std::vector< SvgPoint > points,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    ~SvgPolyline() = default;

    SvgPolyline( SvgPolyline const& ) = default;
    SvgPolyline( SvgPolyline&& )      = default;

    SvgPolyline& operator= ( SvgPolyline const& ) = default;
    SvgPolyline& operator= ( SvgPolyline&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    self_type& add( double x, double y );
    self_type& add( SvgPoint p );

    self_type& operator <<( SvgPoint p );

    void offset( double x, double y );
    SvgBox bounding_box() const;

    void write( std::ostream& out, size_t indent = 0 ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::vector< SvgPoint > points;

    SvgStroke stroke;
    SvgFill   fill;

};

// =================================================================================================
//     Svg Polygon
// =================================================================================================

struct SvgPolygon
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgPolygon;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgPolygon(
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    SvgPolygon(
        std::vector< SvgPoint > points,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    ~SvgPolygon() = default;

    SvgPolygon( SvgPolygon const& ) = default;
    SvgPolygon( SvgPolygon&& )      = default;

    SvgPolygon& operator= ( SvgPolygon const& ) = default;
    SvgPolygon& operator= ( SvgPolygon&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    self_type& add( double x, double y );
    self_type& add( SvgPoint p );

    self_type& operator <<( SvgPoint p );

    void offset( double x, double y );
    SvgBox bounding_box() const;

    void write( std::ostream& out, size_t indent = 0 ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::vector< SvgPoint > points;

    SvgStroke stroke;
    SvgFill   fill;

};

} // namespace utils
} // namespace genesis

#endif // include guard
