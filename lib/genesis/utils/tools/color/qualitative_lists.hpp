#ifndef GENESIS_UTILS_TOOLS_COLOR_QUALITATIVE_LISTS_H_
#define GENESIS_UTILS_TOOLS_COLOR_QUALITATIVE_LISTS_H_

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

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Lists
// =================================================================================================

/**
 * @brief Color palette `accent`.
 *
 * Provides a color palette with 8 colors, 4 pale and 4 saturated,
 * for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_accent();

/**
 * @brief Color palette `dark2`.
 *
 * Provides a color palette with 8 dark colors based on color_list_set2()
 * for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_dark2();

/**
 * @brief Color palette `paired`.
 *
 * Provides a color palette with 12 colors in 6 pairs for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_paired();

/**
 * @brief Color palette `pastel1`.
 *
 * Provides a color palette with 9 colors based on color_list_set1()
 * for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_pastel1();

/**
 * @brief Color palette `pastel2`.
 *
 * Provides a color palette with 9 colors based on color_list_set2()
 * for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_pastel2();

/**
 * @brief Color palette `set1`.
 *
 * Provides a color palette with 9 easy-to-name colors for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_set1();

/**
 * @brief Color palette `set2`.
 *
 * Provides a color palette with 8 hard-to-name colors for use with qualitative/categorical data.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_set2();

/**
 * @brief Color palette `set3`.
 *
 * Provides a color palette with 12 colors for use with qualitative/categorical data.
 * The colors are more saturated than the Pastels but less so than set2().
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_set3();

// =================================================================================================
//     Convenience Functions
// =================================================================================================

enum class QualitativeColorList
{
    kAccent,
    kDark2,
    kPaired,
    kPastel1,
    kPastel2,
    kSet1,
    kSet2,
    kSet3
};

std::vector<Color> const& qualitative_color_list( QualitativeColorList palette );
std::vector<Color> const& qualitative_color_list( std::string const& palette );

std::vector<std::string> qualitative_color_list_names();

} // namespace utils
} // namespace genesis

#endif // include guard
