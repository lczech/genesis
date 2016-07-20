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

#include "utils/formats/svg/document.hpp"

#include "utils/formats/svg/helper.hpp"

#include "utils/core/genesis.hpp"
#include "utils/core/options.hpp"
#include "utils/text/string.hpp"
#include "utils/tools/date_time.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Document
// =================================================================================================

// -------------------------------------------------------------
//     Static Members
// -------------------------------------------------------------

std::string SvgDocument::indentation_string = "    ";

// -------------------------------------------------------------
//     Members
// -------------------------------------------------------------

/**
 * @brief Write the SvgDocument to an output stream.
 */
void SvgDocument::to_svg_document( std::ostream& out ) const
{
    // SVG header.
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" ";
    out << "xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";

    // Some metadata.
    out << svg_comment(
        "Created with genesis " + genesis_version() + " (" + genesis_url() + ") " +
        "on " + current_date() + " at " + current_time()
    );
    if( Options::get().command_line_string() != "" ) {
        out << svg_comment( "Program invocation: " + Options::get().command_line_string() );
    }

    // Content.
    for( auto const& elem : content_ ) {
        elem.to_svg_element( out, 1 );
    }

    // Finish.
    out << "</svg>\n";
}

/**
 * @brief Add an SvgObject to the document.
 *
 * Returns the SvgDocument in order to allow for a fluent interface.
 */
SvgDocument& SvgDocument::add( SvgObject const& object )
{
    content_.push_back( object );
    return *this;
}

/**
 * @brief Shortcut operator for add(), which allows an even more fluent interface.
 */
SvgDocument& SvgDocument::operator <<( SvgObject const& object )
{
    return add( object );
}

} // namespace utils
} // namespace genesis
