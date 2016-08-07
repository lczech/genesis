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

#include "utils/formats/svg/text.hpp"

#include "utils/formats/svg/document.hpp"
#include "utils/formats/xml/helper.hpp"
#include "utils/text/string.hpp"

#include <ostream>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Text
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgText::SvgText(
    SvgPoint const&    position,
    std::string const& text,
    SvgFont const&     font,
    SvgFill const&     fill,
    SvgStroke const&   stroke
)
    : position(position)
    , text(text)
    , font(font)
    , fill(fill)
    , stroke(stroke)
    , kerning( 0.0 )
    , letter_spacing( 0.0 )
    , word_spacing( 0.0 )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

void SvgText::offset( double x, double y )
{
    position.x += x;
    position.y += y;
}

SvgBox SvgText::bounding_box() const
{
    // Resuting box positions.
    double t, b, l, r;

    // Dimensions.
    auto fs = font.size;
    double width  = text.size() * fs / 1.8;
    double height = fs * 1.2;

    // Horizontal.
    if( anchor == Anchor::kEnd ) {
        l = position.x - width;

    } else if( anchor == Anchor::kMiddle ) {
        l = position.x - width / 2.0;

    } else {
        l = position.x;
    }
    r = l + width;

    // Vertical.
    // Allow for letters below the ground line.
    b = position.y + height * 0.3;
    t = b - height;

    return { SvgPoint( l, t ), SvgPoint( r, b ) };
}

void SvgText::write( std::ostream& out, size_t indent ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<text";

    out << svg_attribute( "x", position.x );
    out << svg_attribute( "y", position.y );

    font.write( out );
    fill.write( out );
    stroke.write( out );

    switch( anchor ) {
        case Anchor::kNone:
            break;
        case Anchor::kStart:
            out << svg_attribute( "text-anchor", "start" );
            break;
        case Anchor::kMiddle:
            out << svg_attribute( "text-anchor", "middle" );
            break;
        case Anchor::kEnd:
            out << svg_attribute( "text-anchor", "end" );
            break;
    }

    if( kerning != 0.0 ) {
        out << svg_attribute( "kerning", kerning );
    }
    if( letter_spacing != 0.0 ) {
        out << svg_attribute( "letter-spacing", letter_spacing );
    }
    if( word_spacing != 0.0 ) {
        out << svg_attribute( "word-spacing", word_spacing );
    }

    if( dx != "" ) {
        out << svg_attribute( "dx", dx );
    }
    if( dy != "" ) {
        out << svg_attribute( "dy", dy );
    }

    out << ">";
    out << xml_escape( text );
    out << "</text>\n";
}

} // namespace utils
} // namespace genesis
