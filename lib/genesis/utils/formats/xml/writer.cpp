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

#include "genesis/utils/formats/xml/writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/formats/xml/document.hpp"
#include "genesis/utils/formats/xml/helper.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/text/string.hpp"

#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Write an XML file from an XmlDocument. Return true iff successful.
 *
 * If the file cannot be written to, the function throws an exception. Also, by default, if the file
 * already exists, an exception is thrown.
 * See @link Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink to
 * change this behaviour.
 */
void XmlWriter::to_file( const XmlDocument& document, const std::string& filename )
{
    std::string xml;
    to_string( document, xml );
    utils::file_write(xml, filename);
}

/**
 * @brief Give the XML string representation of a XmlDocument.
 */
void XmlWriter::to_string( const XmlDocument& document, std::string& output )
{
    output = to_string(document);
}

/**
 * @brief Return the XML representation of a XmlDocument.
 */
std::string XmlWriter::to_string( const XmlDocument& document )
{
    std::string res = "";
    if (!document.xml_tag.empty() || !document.declarations.empty()) {
        res = "<?" + document.xml_tag + print_attributes_list(document.declarations) + "?>\n";
    }
    print_element(res, &document, 0);
    return res + "\n";
}

// =================================================================================================
//     Internal
// =================================================================================================

/**
 * @brief Print an XML comment.
 */
void XmlWriter::print_comment (std::string& xml, const XmlComment* value)
{
    xml += xml_comment( value->content );
}

/**
 * @brief Print an XML markup (simple text).
 */
void XmlWriter::print_markup  (std::string& xml, const XmlMarkup*  value)
{
    xml += xml_escape(value->content);
}

/**
 * @brief Print an XML element.
 */
void XmlWriter::print_element (std::string& xml, const XmlElement* value, const int indent_level)
{
    // Prepare indention and opening tag.
    std::string in0 (indent_level * indent, ' ');
    xml += in0 + "<" + value->tag + print_attributes_list(value->attributes);

    // If it's an empty element, close it, and we are done.
    if (value->content.size() == 0) {
        xml += " />";
        return;
    }

    // If the element only contains a single markup, don't add new lines. However, if it contains
    // more data, put each element in a new line.
    xml += ">";
    if (value->content.size() == 1 && value->content[0]->is_markup()) {
        print_markup(xml, xml_value_to_markup(value->content[0].get()));
    } else {
        std::string in1 ((indent_level + 1) * indent, ' ');
        xml += "\n";

        for (auto& v : value->content) {
            if (v->is_comment()) {
                xml += in1;
                print_comment(xml, xml_value_to_comment(v.get()));
            } else if (v->is_markup()) {
                xml += in1;
                print_markup(xml, xml_value_to_markup(v.get()));
            } else if (v->is_element()) {
                print_element(xml, xml_value_to_element(v.get()), indent_level + 1);
            } else {
                // there are no other cases
                assert(0);
            }
            xml += "\n";
        }
        xml += in0;
    }

    xml += "</" + value->tag + ">";
}

/**
 * @brief Print a list of XML attributes.
 */
std::string XmlWriter::print_attributes_list (StringMapType attr)
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
