#ifndef GENESIS_UTILS_FORMATS_SVG_ATTRIBUTES_H_
#define GENESIS_UTILS_FORMATS_SVG_ATTRIBUTES_H_

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

#include "utils/tools/color.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Stroke
// =================================================================================================

struct SvgStroke
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgStroke;

    enum class Type
    {
        kColor,
        kNone,
        kOmit
    };

    enum class LineCap
    {
        kOmit,
        kButt,
        kSquare,
        kRound
    };

    enum class LineJoin
    {
        kOmit,
        kMiter,
        kRound,
        kBevel
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit SvgStroke( Type type = Type::kColor );
    explicit SvgStroke( Color color, double width = 1.0 );

    ~SvgStroke() = default;

    SvgStroke( SvgStroke const& ) = default;
    SvgStroke( SvgStroke&& )      = default;

    SvgStroke& operator= ( SvgStroke const& ) = default;
    SvgStroke& operator= ( SvgStroke&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    Type type;

    Color  color;
    double opacity;

    double      width;
    std::string width_unit;

    LineCap  line_cap;
    LineJoin line_join;
    double   miterlimit;

    std::vector< double > dash_array;
    double                dash_offset;

};

// =================================================================================================
//     Svg Fill
// =================================================================================================

struct SvgFill
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgFill;

    enum class Type
    {
        kColor,
        kNone,
        kOmit
    };

    enum class Rule
    {
        kNone,
        kNonZero,
        kEvenOdd
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit SvgFill( Type type = Type::kColor );
    explicit SvgFill( Color color, double opacity = 1.0 );

    ~SvgFill() = default;

    SvgFill( SvgFill const& ) = default;
    SvgFill( SvgFill&& )      = default;

    SvgFill& operator= ( SvgFill const& ) = default;
    SvgFill& operator= ( SvgFill&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    Type type;

    Color  color;
    double opacity;
    Rule   rule;

};

// =================================================================================================
//     Svg Font
// =================================================================================================

struct SvgFont
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgFont;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit SvgFont( double size = 10, std::string const& family = "Verdana" );

    ~SvgFont() = default;

    SvgFont( SvgFont const& ) = default;
    SvgFont( SvgFont&& )      = default;

    SvgFont& operator= ( SvgFont const& ) = default;
    SvgFont& operator= ( SvgFont&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    double      size;
    std::string family;

};

} // namespace utils
} // namespace genesis

#endif // include guard
