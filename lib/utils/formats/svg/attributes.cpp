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

#include "utils/formats/svg/attributes.hpp"

#include "utils/formats/svg/helper.hpp"
#include "utils/tools/color/operators.hpp"
#include "utils/text/string.hpp"

#include <ostream>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Stroke
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgStroke::SvgStroke( SvgStroke::Type type )
    : type( type )
    , color()
    , opacity( 1.0 )
    , width( 1.0 )
    , width_unit()
    , line_cap( LineCap::kOmit )
    , line_join( LineJoin::kOmit )
    , miterlimit( 1.0 )
    , dash_array()
    , dash_offset( 0.0 )
{}

SvgStroke::SvgStroke( Color color_value, double width_value )
    : SvgStroke( Type::kColor )
{
    color = color_value;
    width = width_value;
}

// -------------------------------------------------------------
//     Drawing Function
// -------------------------------------------------------------

void SvgStroke::write( std::ostream& out ) const
{
    // Treat special cases.
    if( type == Type::kOmit ) {
        return;
    }
    if( type == Type::kNone ) {
        out << svg_attribute( "stroke", "none" );
        return;
    }

    out << svg_attribute( "stroke", color_to_hex( color ) );
    out << svg_attribute( "stroke-opacity", opacity );
    out << svg_attribute( "stroke-width", width, width_unit );

    switch( line_cap ) {
        case LineCap::kOmit:
            break;
        case LineCap::kButt:
            out << svg_attribute( "stroke-linecap", "butt" );
            break;
        case LineCap::kSquare:
            out << svg_attribute( "stroke-linecap", "square" );
            break;
        case LineCap::kRound:
            out << svg_attribute( "stroke-linecap", "round" );
            break;
    }

    switch( line_join ) {
        case LineJoin::kOmit:
            break;
        case LineJoin::kMiter:
            out << svg_attribute( "stroke-linejoin", "miter" );
            out << svg_attribute( "stroke-miterlimit", miterlimit );
            break;
        case LineJoin::kRound:
            out << svg_attribute( "stroke-linejoin", "round" );
            break;
        case LineJoin::kBevel:
            out << svg_attribute( "stroke-linejoin", "bevel" );
            break;
    }

    if( dash_array.size() > 0 ) {
        out << svg_attribute( "stroke-dasharray", join( dash_array, " " ));
        out << svg_attribute( "stroke-dashoffset", dash_offset );
    }
}

// =================================================================================================
//     Svg Fill
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgFill::SvgFill( SvgFill::Type type )
    : type( type )
    , color()
    , opacity( 1.0 )
    , rule( Rule::kNone )
{}

SvgFill::SvgFill( Color color, double opacity )
    : type( SvgFill::Type::kColor )
    , color( color )
    , opacity( opacity )
    , rule( Rule::kNone )
{}

// -------------------------------------------------------------
//     Drawing Function
// -------------------------------------------------------------

void SvgFill::write( std::ostream& out ) const
{
    // Treat special cases.
    if( type == Type::kOmit ) {
        return;
    }
    if( type == Type::kNone ) {
        out << svg_attribute( "fill", "none" );
        return;
    }

    out << svg_attribute( "fill", color_to_hex( color ) );
    out << svg_attribute( "fill-opacity", opacity );

    switch( rule ) {
        case Rule::kNone:
            break;
        case Rule::kNonZero:
            out << svg_attribute( "fill-rule", "nonzero" );
            break;
        case Rule::kEvenOdd:
            out << svg_attribute( "fill-rule", "evenodd" );
            break;
    }
}

// =================================================================================================
//     Svg Font
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgFont::SvgFont( double size_value, std::string const& family_value )
    : size( size_value )
    , family( family_value )
{}

// -------------------------------------------------------------
//     Drawing Function
// -------------------------------------------------------------

void SvgFont::write( std::ostream& out ) const
{
    out << svg_attribute( "font-size", size );
    out << svg_attribute( "font-family", family );
}

} // namespace utils
} // namespace genesis
