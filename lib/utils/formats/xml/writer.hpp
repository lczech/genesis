#ifndef GENESIS_UTILS_FORMATS_XML_WRITER_H_
#define GENESIS_UTILS_FORMATS_XML_WRITER_H_

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

#include <string>
#include <unordered_map>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class XmlComment;
class XmlMarkup;
class XmlElement;
class XmlDocument;
\
// =================================================================================================
//     Xml Writer
// =================================================================================================

/**
 * @brief Write Xml data.
 */
class XmlWriter
{
    typedef std::unordered_map<std::string, std::string> StringMapType;

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    void        to_file   ( const XmlDocument& document, const std::string& filename);
    void        to_string ( const XmlDocument& document,       std::string& output);
    std::string to_string ( const XmlDocument& document);

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:
    void print_comment (std::string& xml, const XmlComment* value);
    void print_markup  (std::string& xml, const XmlMarkup*  value);
    void print_element (std::string& xml, const XmlElement* value, const int indent_level);

    std::string print_attributes_list (StringMapType attr);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

public:

    /**
     * @brief Indent used for printing the XML elements.
     */
    int indent = 4;
};

} // namespace utils
} // namespace genesis

#endif // include guard
