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

#include "genesis/utils/formats/svg/text.hpp"

#include "genesis/utils/formats/svg/document.hpp"
#include "genesis/utils/formats/xml/helper.hpp"
#include "genesis/utils/text/string.hpp"

#include <ostream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Text
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgText::SvgText(
    std::string const& text,
    SvgPoint const&    position,
    SvgFont const&     font,
    SvgFill const&     fill,
    SvgStroke const&   stroke
)
    : text(text)
    , position(position)
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

void SvgText::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<text";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << svg_attribute( "x", position.x + options.offset_x );
    out << svg_attribute( "y", position.y + options.offset_y );

    font.write( out );
    fill.write( out );
    stroke.write( out );

    if( anchor != Anchor::kNone ) {
        out << anchor_to_string( anchor );
    }
    if( dominant_baseline != DominantBaseline::kNone ) {
        out << dominant_baseline_to_string( dominant_baseline );
    }
    if( alignment_baseline != AlignmentBaseline::kNone ) {
        out << alignment_baseline_to_string( alignment_baseline );
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
    transform.write( out );

    out << ">";
    out << xml_escape( text );

    // if( alignment_baseline == AlignmentBaseline::kNone ) {
        // out << xml_escape( text );
    // } else {
    //     out << "<tspan";
    //     out << alignment_baseline_to_string( alignment_baseline );
    //     out << ">";
    //     out << xml_escape( text );
    //     out << "</tspan>";
    // }

    out << "</text>\n";
}

// -------------------------------------------------------------
//     Helper Functions
// -------------------------------------------------------------

std::string SvgText::anchor_to_string( SvgText::Anchor value )
{
    switch( value ) {
        case Anchor::kNone:
            return std::string();
        case Anchor::kStart:
            return svg_attribute( "text-anchor", "start" );
        case Anchor::kMiddle:
            return svg_attribute( "text-anchor", "middle" );
        case Anchor::kEnd:
            return svg_attribute( "text-anchor", "end" );
        default:
            throw std::invalid_argument( "Invalid Svg attribute Anchor for Svg Text Element." );
    }
}

std::string SvgText::dominant_baseline_to_string( SvgText::DominantBaseline value )
{
    switch( value ) {
        case DominantBaseline::kNone:
            return std::string();
        case DominantBaseline::kAuto:
            return svg_attribute( "dominant-baseline", "auto" );
        case DominantBaseline::kUseScript:
            return svg_attribute( "dominant-baseline", "use-script" );
        case DominantBaseline::kNoChange:
            return svg_attribute( "dominant-baseline", "no-change" );
        case DominantBaseline::kResetSize:
            return svg_attribute( "dominant-baseline", "reset-size" );
        case DominantBaseline::kIdeographic:
            return svg_attribute( "dominant-baseline", "ideographic" );
        case DominantBaseline::kAlphabetic:
            return svg_attribute( "dominant-baseline", "alphabetic" );
        case DominantBaseline::kHanging:
            return svg_attribute( "dominant-baseline", "hanging" );
        case DominantBaseline::kMathematical:
            return svg_attribute( "dominant-baseline", "mathematical" );
        case DominantBaseline::kCentral:
            return svg_attribute( "dominant-baseline", "central" );
        case DominantBaseline::kMiddle:
            return svg_attribute( "dominant-baseline", "middle" );
        case DominantBaseline::kTextAfterEdge:
            return svg_attribute( "dominant-baseline", "text-after-edge" );
        case DominantBaseline::kTextBeforeEdge:
            return svg_attribute( "dominant-baseline", "text-before-edge" );
        case DominantBaseline::kInherit:
            return svg_attribute( "dominant-baseline", "inherit" );
        default:
            throw std::invalid_argument(
                "Invalid Svg attribute Dominant Baseline for Svg Text Element."
            );
    }
}

std::string SvgText::alignment_baseline_to_string( SvgText::AlignmentBaseline value )
{
    switch( value ) {
        case AlignmentBaseline::kNone:
            return std::string();
        case AlignmentBaseline::kAuto:
            return svg_attribute( "alignment-baseline", "auto" );
        case AlignmentBaseline::kBaseline:
            return svg_attribute( "alignment-baseline", "baseline" );
        case AlignmentBaseline::kBeforeEdge:
            return svg_attribute( "alignment-baseline", "before-edge" );
        case AlignmentBaseline::kTextBeforeEdge:
            return svg_attribute( "alignment-baseline", "text-before-edge" );
        case AlignmentBaseline::kMiddle:
            return svg_attribute( "alignment-baseline", "middle" );
        case AlignmentBaseline::kCentral:
            return svg_attribute( "alignment-baseline", "central" );
        case AlignmentBaseline::kAfterEdge:
            return svg_attribute( "alignment-baseline", "after-edge" );
        case AlignmentBaseline::kTextAfterEdge:
            return svg_attribute( "alignment-baseline", "text-after-edge" );
        case AlignmentBaseline::kIdeographic:
            return svg_attribute( "alignment-baseline", "ideographic" );
        case AlignmentBaseline::kAlphabetic:
            return svg_attribute( "alignment-baseline", "alphabetic" );
        case AlignmentBaseline::kHanging:
            return svg_attribute( "alignment-baseline", "hanging" );
        case AlignmentBaseline::kMathematical:
            return svg_attribute( "alignment-baseline", "mathematical" );
        case AlignmentBaseline::kInherit:
            return svg_attribute( "alignment-baseline", "inherit" );
        default:
            throw std::invalid_argument(
                "Invalid Svg attribute Alignment Baseline for Svg Text Element."
            );
    }
}

} // namespace utils
} // namespace genesis
