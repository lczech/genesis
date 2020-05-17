/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/formats/xml/writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/formats/xml/document.hpp"
#include "genesis/utils/formats/xml/helper.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Printing
// =================================================================================================

void XmlWriter::write( XmlDocument const& document, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    auto& os = target->ostream();

    if( !document.xml_tag.empty() || !document.declarations.empty() ) {
        os << "<?" << document.xml_tag << print_attributes_list_(document.declarations) << "?>\n";
    }
    print_element_( &document, 0, os );
    os << "\n";
}

// =================================================================================================
//     Internal
// =================================================================================================

/**
 * @brief Print an XML comment.
 */
void XmlWriter::print_comment_( XmlComment const* value, std::ostream& os ) const
{
    os << xml_comment( value->content );
}

/**
 * @brief Print an XML markup (simple text).
 */
void XmlWriter::print_markup_( XmlMarkup const* value, std::ostream& os ) const
{
    os << xml_escape(value->content);
}

/**
 * @brief Print an XML element.
 */
void XmlWriter::print_element_( XmlElement const* value, const int indent_level, std::ostream& os ) const
{
    // Prepare indention and opening tag.
    std::string in0 (indent_level * indent, ' ');
    os << in0 << "<" << value->tag << print_attributes_list_(value->attributes);

    // If it's an empty element, close it, and we are done.
    if (value->content.size() == 0) {
        os << " />";
        return;
    }

    // If the element only contains a single markup, don't add new lines. However, if it contains
    // more data, put each element in a new line.
    os << ">";
    if (value->content.size() == 1 && value->content[0]->is_markup()) {
        print_markup_( xml_value_to_markup(value->content[0].get()), os );
    } else {
        std::string in1 ((indent_level + 1) * indent, ' ');
        os << "\n";

        for (auto& v : value->content) {
            if (v->is_comment()) {
                os << in1;
                print_comment_( xml_value_to_comment(v.get()), os );
            } else if (v->is_markup()) {
                os << in1;
                print_markup_( xml_value_to_markup(v.get()), os );
            } else if (v->is_element()) {
                print_element_( xml_value_to_element(v.get()), indent_level + 1, os );
            } else {
                // there are no other cases
                assert(0);
            }
            os << "\n";
        }
        os << in0;
    }

    os << "</" << value->tag << ">";
}

/**
 * @brief Print a list of XML attributes.
 */
std::string XmlWriter::print_attributes_list_ (StringMapType attr) const
{
    std::string xml;
    for (auto pair : attr) {
        xml += xml_attribute( pair.first, pair.second );
        // xml += " " + pair.first + "=\"" + pair.second + "\"";
    }
    return xml;
}

} // namespace utils
} // namespace genesis
