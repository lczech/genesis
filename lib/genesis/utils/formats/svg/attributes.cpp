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

#include "genesis/utils/formats/svg/attributes.hpp"

#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/tools/color/operators.hpp"
#include "genesis/utils/text/string.hpp"

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

SvgStroke::SvgStroke( std::string gradient_id_value )
    : SvgStroke( Type::kGradient )
{
    gradient_id = gradient_id_value;
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
    if( type == Type::kGradient ) {
        out << svg_attribute( "stroke", "url(#" + gradient_id + ");" );
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

SvgFill::SvgFill( std::string gradient_id_value )
    : SvgFill( Type::kGradient )
{
    gradient_id = gradient_id_value;
}

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
    if( type == Type::kGradient ) {
        out << svg_attribute( "fill", "url(#" + gradient_id + ");" );
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

// =================================================================================================
//     Svg Transformation
// =================================================================================================

// -------------------------------------------------------------------------
//     Subclass Translate
// -------------------------------------------------------------------------

void SvgTransform::Translate::write( std::ostream &out ) const
{
    if( tx != 0.0 || ty != 0.0 ) {
        out << "translate( " << tx << ", " << ty << " )";
    }
}

// -------------------------------------------------------------------------
//     Subclass Rotate
// -------------------------------------------------------------------------

void SvgTransform::Rotate::write( std::ostream &out ) const
{
    if( a != 0.0 ) {
        if( cx != 0.0 || cy != 0.0 ) {
            out << "rotate( " << a << ", " << cx << ", " << cy << " )";
        } else {
            out << "rotate( " << a << " )";
        }
    }
}

// -------------------------------------------------------------------------
//     Subclass Scale
// -------------------------------------------------------------------------

void SvgTransform::Scale::write( std::ostream &out ) const
{
    if( sx != 1.0 || sy != 1.0 ) {
        if( sx == sy ) {
            out << "scale( " << sx << " )";
        } else {
            out << "scale( " << sx << ", " << sy << " )";
        }
    }
}

// -------------------------------------------------------------------------
//     Subclass Skew
// -------------------------------------------------------------------------

void SvgTransform::Skew::write( std::ostream &out ) const
{
    if( ax != 0.0 ) {
        out << "skewX( " << ax << " )";
    }
    if( ax != 0.0 && ay != 0.0 ) {
        out << " ";
    }
    if( ay != 0.0 ) {
        out << "skewY( " << ay << " )";
    }
}

// -------------------------------------------------------------------------
//     Subclass Matrix
// -------------------------------------------------------------------------

void SvgTransform::Matrix::write( std::ostream &out ) const
{
    if( a != 1.0 || b != 0.0 || c != 0.0 || d != 1.0 || e != 0.0 || f != 0.0 ) {
        out << "matrix( " << a << ", " << b << ", " << c << ", ";
        out               << d << ", " << e << ", " << f << " )";
    }
}

// -------------------------------------------------------------------------
//     SvgTransform Main Class
// -------------------------------------------------------------------------

void SvgTransform::append( Transformation&& t )
{
    transformations.push_back( std::move( t ));
}

void SvgTransform::append( Transformation const& t )
{
    transformations.push_back( t );
}

void SvgTransform::write( std::ostream& out ) const
{
    if( ! transformations.empty() ) {
        out << " transform=\"";
        for( auto const& t : transformations ) {
            if( &t != &transformations[0] ) {
                out << " ";
            }
            t.write( out );
        }
        out << "\"";
    }
}

} // namespace utils
} // namespace genesis
