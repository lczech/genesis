#ifndef GENESIS_UTILS_TOOLS_COLOR_NAMES_H_
#define GENESIS_UTILS_TOOLS_COLOR_NAMES_H_

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

#include "utils/tools/color.hpp"

#include <array>
#include <utility>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Names
// =================================================================================================

/**
 * @brief Struct that stores a static const map of named colors.
 *
 * The map stores all 140 color names of the HTML web colors. It is mainly used as a simple way to
 * create a color scheme in form of a vector of colors.
 *
 * The data of this struct is used by is_named_color() and get_named_color().
 */
struct ColorNames
{
    static const std::array<std::pair<std::string, Color>, 140> map;
};

bool is_named_color( std::string const& name );
Color get_named_color( std::string const& name );

size_t get_named_color_index( std::string const& name );
Color get_named_color_at( size_t at );

} // namespace utils
} // namespace genesis

#endif // include guard
