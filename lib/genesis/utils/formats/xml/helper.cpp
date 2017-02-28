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

#include "genesis/utils/formats/xml/helper.hpp"

#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Xml Helper Functions
// =================================================================================================

/**
 * @brief Escape special XML characters.
 */
std::string xml_escape(   std::string const& txt )
{
    std::string res;
    res = utils::replace_all( txt, "<",  "&lt;"   );
    res = utils::replace_all( res, ">",  "&gt;"   );
    res = utils::replace_all( res, "&",  "&amp;"  );
    res = utils::replace_all( res, "'",  "&apos;" );
    res = utils::replace_all( res, "\"", "&quot;" );
    return res;
}

/**
 * @brief De-escape special XML characters.
 */
std::string xml_deescape( std::string const& txt )
{
    std::string res;
    res = utils::replace_all( txt, "&lt;",   "<"  );
    res = utils::replace_all( res, "&gt;",   ">"  );
    res = utils::replace_all( res, "&amp;",  "&"  );
    res = utils::replace_all( res, "&apos;", "'"  );
    res = utils::replace_all( res, "&quot;", "\"" );
    return res;
}

std::string xml_comment( std::string const& content )
{
    return "<!-- " + content + " -->";
}

} // namespace utils
} // namespace genesis
