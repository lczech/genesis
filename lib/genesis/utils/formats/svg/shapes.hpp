#ifndef GENESIS_UTILS_FORMATS_SVG_SHAPES_H_
#define GENESIS_UTILS_FORMATS_SVG_SHAPES_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/attributes.hpp"

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

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string  id;
    SvgPoint     point_1;
    SvgPoint     point_2;
    SvgStroke    stroke;
    SvgTransform transform;

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

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    SvgPoint  position;
    SvgSize   size;

    SvgStroke stroke;
    SvgFill   fill;

    double rx;
    double ry;

    SvgTransform transform;

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

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    SvgPoint  center;
    double    radius;

    SvgStroke stroke;
    SvgFill   fill;

    SvgTransform transform;

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

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    SvgPoint  center;
    double    rx;
    double    ry;

    SvgStroke stroke;
    SvgFill   fill;

    SvgTransform transform;

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

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    std::vector< SvgPoint > points;

    SvgStroke stroke;
    SvgFill   fill;

    SvgTransform transform;

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
        std::vector< SvgPoint > const& points,
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

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    std::vector< SvgPoint > points;

    SvgStroke stroke;
    SvgFill   fill;

    SvgTransform transform;

};

// =================================================================================================
//     Svg Path
// =================================================================================================

struct SvgPath
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgPath;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgPath(
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    SvgPath(
        std::vector< std::string > const& elements,
        SvgStroke const& stroke = SvgStroke(),
        SvgFill   const& fill   = SvgFill()
    );

    ~SvgPath() = default;

    SvgPath( SvgPath const& ) = default;
    SvgPath( SvgPath&& )      = default;

    SvgPath& operator= ( SvgPath const& ) = default;
    SvgPath& operator= ( SvgPath&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    self_type& add( std::string elem );

    self_type& operator <<( std::string elem );

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    std::vector< std::string > elements;

    SvgStroke stroke;
    SvgFill   fill;

    SvgTransform transform;

};

// =================================================================================================
//     Svg Use
// =================================================================================================

struct SvgUse
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgUse;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgUse(
        std::string const& id,
        SvgPoint const& offset
    )
        : referenced_id( id )
        , offset( offset )
    {}

    SvgUse(
        std::string const& id,
        double offset_x,
        double offset_y
    )
        : referenced_id( id )
        , offset( offset_x, offset_y )
    {}

    ~SvgUse() = default;

    SvgUse( SvgUse const& ) = default;
    SvgUse( SvgUse&& )      = default;

    SvgUse& operator= ( SvgUse const& ) = default;
    SvgUse& operator= ( SvgUse&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    SvgBox bounding_box() const;

    void write(
        std::ostream& out,
        size_t indent = 0,
        SvgDrawingOptions const& options = SvgDrawingOptions()
    ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string  referenced_id;
    SvgPoint     offset;
    SvgTransform transform;

};

} // namespace utils
} // namespace genesis

#endif // include guard
