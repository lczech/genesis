#ifndef GENESIS_UTILS_FORMATS_XML_HELPER_H_
#define GENESIS_UTILS_FORMATS_XML_HELPER_H_

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

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Xml Helper Functions
// =================================================================================================

std::string xml_escape(   std::string const& content );
std::string xml_deescape( std::string const& content );

std::string xml_comment( std::string const& content );

template< typename T >
std::string xml_attribute(
    std::string const& name,
    T const&           value
) {
    std::stringstream ss;
    ss << " " << name << "=\"" << value << "\"";
    return ss.str();
}

} // namespace utils
} // namespace genesis

#endif // include guard
