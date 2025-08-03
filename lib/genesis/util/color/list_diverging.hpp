#ifndef GENESIS_UTIL_COLOR_LIST_DIVERGING_H_
#define GENESIS_UTIL_COLOR_LIST_DIVERGING_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup util
 */

#include <genesis/util/color/color.hpp>

#include <string>
#include <vector>

namespace genesis {
namespace util {
namespace color {

// =================================================================================================
//     Convenience Functions
// =================================================================================================

/**
* @brief Divering color palettes.
*
* List of the diverging color palettes offered by color_list_diverging().
*
* These color palette are adapted from ColorBrewer at https://github.com/axismaps/colorbrewer and
* https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` values.
* The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
* (http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider,
* and published under the [Apache-2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
* See also our @link supplement_acknowledgements_code_reuse_colorbrewer Acknowledgements@endlink.
*
* For further color palettes, see ColorListQualitative, ColorListSequential, and ColorListMisc
* which offer the other types of ColorBrewer lists and some more.
*/
enum class ColorListDiverging
{

    /**
     * @brief Color palette `brbg`.
     *
     * Provides a diverging color palette with brown low, white middle, and blue-green high.
     */
    kBrbg,

    /**
     * @brief Color palette `piyg`.
     *
     * Provides a diverging color palette with pink low, white middle, and yellow-green high.
     */
    kPiyg,

    /**
     * @brief Color palette `prgn`.
     *
     * Provides a diverging color palette with purple low, white middle, and green high.
     */
    kPrgn,

    /**
     * @brief Color palette `puor`.
     *
     * Provides a diverging color palette with orange low, white middle, and purple high.
     */
    kPuor,

    /**
     * @brief Color palette `rdbu`.
     *
     * Provides a diverging color palette with red low, white middle, and blue high.
     */
    kRdbu,

    /**
     * @brief Color palette `rdgy`.
     *
     * Provides a diverging color palette with red low, white middle, and grey high.
     */
    kRdgy,

    /**
     * @brief Color palette `rdylbu`.
     *
     * Provides a diverging color palette with red low, pale yellow middle, and blue high.
     */
    kRdylbu,

    /**
     * @brief Color palette `rdylgn`.
     *
     * Provides a diverging color palette with red low, pale yellow middle, and green high.
     */
    kRdylgn,

    /**
     * @brief Color palette `spectral`.
     *
     * Provides a diverging rainbow color palette with red low, pale yellow middle, and blue high.
     */
    kSpectral
};

/**
 * @brief Get a diverging color palette.
 *
 * See ColorListDiverging for the available ones.
 */
std::vector<Color> const& color_list_diverging( ColorListDiverging palette );

/**
 * @brief Get a diverging color palette by its name.
 *
 * The names used here are the same as in ColorListDiverging, but without the leading `k`,
 * i.e., using their original name (case insensitive).
 * For instance, ColorListDiverging::kSpectral can be obtained here simply as `"spectral"`.
 */
std::vector<Color> const& color_list_diverging( std::string const& palette );

/**
 * @brief Get the list of all available diverging color palettes as strings.
 *
 * See also ColorListDiverging for the corresponding enum.
 */
std::vector<std::string> color_list_diverging_names();

} // namespace color
} // namespace util
} // namespace genesis

#endif // include guard
