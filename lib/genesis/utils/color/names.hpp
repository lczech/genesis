#ifndef GENESIS_UTILS_COLOR_NAMES_H_
#define GENESIS_UTILS_COLOR_NAMES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/color/color.hpp"

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
 * which is (currently) a shortcut for is_color_name_web() and is_color_name_xkcd().
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
 * Names are compared only by their alnum chars, and the letter case is ignored.
 */
bool is_color_name_web( std::string const& name );

/**
 * @brief Retrieve a named web color by name.
 *
 * Names are compared only by their alnum chars, and the letter case is ignored.
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
 * Names are compared only by their alnum chars, and the letter case is ignored.
 * See color_from_name_xkcd() for details on this color list.
 */
bool is_color_name_xkcd( std::string const& name );

/**
 * @brief Retrieve a named xkcd color by name.
 *
 * Names are compared only by their alnum chars, and the letter case is ignored.
 * If the color name does not exist, an `std::invalid_argument` exception is thrown.
 *
 * The colors are taken from an [https://xkcd.com/](xkcd) color survey.
 * They were published under Public Domain, http://creativecommons.org/publicdomain/zero/1.0/.
 * See https://xkcd.com/color/rgb/ and https://blog.xkcd.com/2010/05/03/color-survey-results/
 * for details. See also our
 * @link supplement_acknowledgements_code_reuse_xkcd_color_names Acknowledgements@endlink.
 */
Color color_from_name_xkcd( std::string const& name );

// =================================================================================================
//     Color Names LEGO
// =================================================================================================

std::vector<Color> color_palette_lego();

/**
 * @brief Return true iff the given name is a named LEGO color.
 *
 * Names are compared only by their alnum chars, and the letter case is ignored.
 * See color_from_name_lego() for details on this color list.
 */
bool is_color_name_lego( std::string const& name );

/**
 * @brief Retrieve a named LEGO color by name.
 *
 * Names are compared only by their alnum chars, and the letter case is ignored.
 * If the color name does not exist, an `std::invalid_argument` exception is thrown.
 *
 * The colors are taken from the `colors.csv.gz` table of the LEGO Catalog Database at
 * https://rebrickable.com/downloads/ on 2024-06-17. They were published with the note
 *
 *  > You can use these files for any purpose.
 *
 * which we interpret as the table being in the public domain.
 */
Color color_from_name_lego( std::string const& name );

} // namespace utils
} // namespace genesis

#endif // include guard
