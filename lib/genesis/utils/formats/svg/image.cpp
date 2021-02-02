/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/utils/formats/svg/image.hpp"

#include "genesis/utils/formats/svg/document.hpp"
#include "genesis/utils/text/string.hpp"

#include <ostream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Image
// =================================================================================================

// -------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------

SvgImage::SvgImage(
    std::string const& href
)
    : href( href )
{}

SvgImage::SvgImage(
    std::string const& href,
    SvgPoint const&    position
)
    : href( href )
    , position( position )
{}

SvgImage::SvgImage(
    std::string const& href,
    SvgPoint const&    position,
    SvgSize const&     size
)
    : href( href )
    , position( position )
    , size( size )
{}

SvgImage::SvgImage(
    std::string const& href,
    double x, double y,
    double w, double h
)
    : href( href )
    , position( SvgPoint( x, y ) )
    , size( SvgSize( w, h ) )
{}

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

SvgBox SvgImage::bounding_box() const
{
    return SvgBox( position, size.width, size.height );
}

void SvgImage::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    (void) options;

    out << repeat( SvgDocument::indentation_string, indent );
    out << "<image";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }

    out << svg_attribute( "x", position.x );
    out << svg_attribute( "y", position.y );

    if( size.width != 0.0 || size.height != 0.0 ) {
        out << svg_attribute( "width",  size.width  );
        out << svg_attribute( "height", size.height );
    }

    out << svg_attribute( "xlink:href", href );

    transform.write( out );

    switch( rendering ) {
        case ImageRendering::kNone: {
            // Nothing to do.
            break;
        }
        case ImageRendering::kAuto: {
            out << svg_attribute( "style", "image-rendering:auto" );
            // out << svg_attribute( "image-rendering", "auto" );
            break;
        }
        case ImageRendering::kOptimizeSpeed: {
            out << svg_attribute( "style", "image-rendering:optimizeSpeed" );
            // out << svg_attribute( "image-rendering", "optimizeSpeed" );
            break;
        }
        case ImageRendering::kOptimizeQuality: {
            out << svg_attribute( "style", "image-rendering:optimizeQuality" );
            // out << svg_attribute( "image-rendering", "optimizeQuality" );
            break;
        }
        case ImageRendering::kSmooth: {
            out << svg_attribute( "style", "image-rendering:smooth" );
            // out << svg_attribute( "image-rendering", "smooth" );
            break;
        }
        case ImageRendering::kCrispEdges: {
            out << svg_attribute( "style", "image-rendering:crisp-edges" );
            // out << svg_attribute( "image-rendering", "crisp-edges" );
            break;
        }
        case ImageRendering::kPixelated: {
            out << svg_attribute( "style", "image-rendering:pixelated" );
            // out << svg_attribute( "image-rendering", "pixelated" );
            break;
        }
        case ImageRendering::kInherit: {
            out << svg_attribute( "style", "image-rendering:inherit" );
            // out << svg_attribute( "image-rendering", "inherit" );
            break;
        }
        case ImageRendering::kInitial: {
            out << svg_attribute( "style", "image-rendering:initial" );
            // out << svg_attribute( "image-rendering", "initial" );
            break;
        }
        case ImageRendering::kUnset: {
            out << svg_attribute( "style", "image-rendering:unset" );
            // out << svg_attribute( "image-rendering", "unset" );
            break;
        }
    }

    out << " />\n";
}

} // namespace utils
} // namespace genesis
