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

#include "genesis/utils/formats/svg/group.hpp"

#include "genesis/utils/formats/svg/document.hpp"
#include "genesis/utils/text/string.hpp"

#include <ostream>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Group
// =================================================================================================

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

SvgBox SvgGroup::bounding_box() const
{
    // Get bounding box of all elements and the dimensions of the document.
    SvgBox bbox;
    for( auto const& elem : content_ ) {
        bbox = SvgBox::combine( bbox, elem.bounding_box() );
    }
    return bbox;
}

void SvgGroup::write( std::ostream& out, size_t indent, SvgDrawingOptions const& options ) const
{
    out << repeat( SvgDocument::indentation_string, indent );
    out << "<g";

    if( ! id.empty() ) {
        out << svg_attribute( "id", id );
    }
    transform.write( out );

    out << ">\n";

    // Print content.
    for( auto const& elem : content_ ) {
        elem.write( out, indent + 1, options );
    }

    out << repeat( SvgDocument::indentation_string, indent );
    out << "</g>\n";
}

SvgGroup& SvgGroup::add( SvgObject const& object )
{
    content_.push_back( object );
    return *this;
}

SvgGroup& SvgGroup::add( SvgObject&& object )
{
    content_.push_back( std::move( object ));
    return *this;
}

SvgGroup& SvgGroup::operator <<( SvgObject const& object )
{
    return add( object );
}

SvgGroup& SvgGroup::operator <<( SvgObject&& object )
{
    return add( std::move( object ));
}

} // namespace utils
} // namespace genesis
