#ifndef GENESIS_UTILS_FORMATS_XML_WRITER_H_
#define GENESIS_UTILS_FORMATS_XML_WRITER_H_

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

#include "genesis/utils/io/output_target.hpp"

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

    void write( XmlDocument const& document, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:

    void print_comment_ ( XmlComment const* value, std::ostream& os ) const;
    void print_markup_  ( XmlMarkup const*  value, std::ostream& os ) const;
    void print_element_ ( XmlElement const* value, const int indent_level, std::ostream& os ) const;

    std::string print_attributes_list_( StringMapType attr ) const;

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
