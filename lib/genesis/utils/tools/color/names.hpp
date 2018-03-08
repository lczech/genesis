#ifndef GENESIS_UTILS_TOOLS_COLOR_NAMES_H_
#define GENESIS_UTILS_TOOLS_COLOR_NAMES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/tools/color.hpp"

#include <utility>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Names
// =================================================================================================

/**
 * @brief Return `true` if the name represents one of the named colors offered by genesis,
 * which is (currently) a shortcut for is_web_color_name() and is_xkcd_color_name().
 */
bool is_color_name( std::string const& name );

/**
 * @brief Return the color represented by the given name,
 * which is (currently) a shortcut for color_from_name_web() and color_from_name_xkcd().
 */
Color color_from_name( std::string const& name );

// =================================================================================================
//     Color Names Web
// =================================================================================================

std::vector<Color> color_palette_web();

/**
 * @brief Return true iff the given name is a named web color.
 *
 * Names are filtered so that spaces, underscores and the letter case are ignored.
 */
bool is_web_color_name( std::string const& name );

/**
 * @brief Retrieve a named web color by name.
 *
 * Names are filtered so that spaces, underscores and the letter case are ignored.
 * If the color name does not exist, an `std::invalid_argument` exception is thrown.
 */
Color color_from_name_web( std::string const& name );

// =================================================================================================
//     Color Names xkcd
// =================================================================================================

std::vector<Color> color_palette_xkcd();

/**
 * @brief Return true iff the given name is a named xkcd color.
 *
 * Names are filtered so that spaces, underscores and the letter case are ignored.
 * See color_from_name_xkcd() for details on this color list.
 */
bool is_xkcd_color_name( std::string const& name );

/**
 * @brief Retrieve a named xkcd color by name.
 *
 * Names are filtered so that spaces, underscores and the letter case are ignored.
 * If the color name does not exist, an `std::invalid_argument` exception is thrown.
 *
 * The colors are taken from an [https://xkcd.com/](xkcd) color survey.
 * They were published under Public Domain, http://creativecommons.org/publicdomain/zero/1.0/.
 * See https://xkcd.com/color/rgb/ and https://blog.xkcd.com/2010/05/03/color-survey-results/
 * for details. See also our
 * @link supplement_acknowledgements_code_reuse_xkcd_color_names Acknowledgements@endlink.
 */
Color color_from_name_xkcd( std::string const& name );

} // namespace utils
} // namespace genesis

#endif // include guard
