#ifndef GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_
#define GENESIS_UTILS_TOOLS_COLOR_OPERATORS_H_

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
 * @brief Color operators.
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Color;

// =================================================================================================
//     Color Conversion
// =================================================================================================

Color color_from_doubles (double r, double g, double b);

Color color_from_hex( std::string h, std::string prefix = "#" );

std::string color_to_hex( const Color& c, std::string prefix = "#", bool uppercase = false );

// =================================================================================================
//     Color Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const Color& h);

} // namespace utils
} // namespace genesis

#endif // include guard
