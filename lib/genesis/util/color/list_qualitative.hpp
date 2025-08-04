#ifndef GENESIS_UTIL_COLOR_LIST_QUALITATIVE_H_
#define GENESIS_UTIL_COLOR_LIST_QUALITATIVE_H_

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
 * @brief Qualitative color palettes.
 *
 * List of the qualitative color palettes offered by color_list_qualitative(). A list of all color
* palettes offered here by their name can also be obtained from color_list_qualitative_names().
 *
 * These color palette are adapted from ColorBrewer at https://github.com/axismaps/colorbrewer and
 * https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` values.
 * The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
 * (http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider,
 * and published under the [Apache-2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
 * See also our @link supplement_acknowledgements_code_reuse_colorbrewer Acknowledgements@endlink.
 *
 * For further color palettes, see ColorListDiverging, ColorListQualitative, and ColorListMisc
 * which offer the other types of ColorBrewer lists and some more.
 */
enum class ColorListQualitative
{
    /**
     * @brief Color palette `accent`.
     *
     * Provides a color palette with 8 colors, 4 pale and 4 saturated,
     * for use with qualitative/categorical data.
     */
    kAccent,

    /**
     * @brief Color palette `dark2`.
     *
     * Provides a color palette with 8 dark colors based on color_list_set2()
     * for use with qualitative/categorical data.
     */
    kDark2,

    /**
     * @brief Color palette `paired`.
     *
     * Provides a color palette with 12 colors in 6 pairs for use with qualitative/categorical data.
     */
    kPaired,

    /**
     * @brief Color palette `pastel1`.
     *
     * Provides a color palette with 9 colors based on color_list_set1()
     * for use with qualitative/categorical data.
     */
    kPastel1,

    /**
     * @brief Color palette `pastel2`.
     *
     * Provides a color palette with 9 colors based on color_list_set2()
     * for use with qualitative/categorical data.
     */
    kPastel2,

    /**
     * @brief Color palette `set1`.
     *
     * Provides a color palette with 9 easy-to-name colors for use with qualitative/categorical data.
     */
    kSet1,

    /**
     * @brief Color palette `set2`.
     *
     * Provides a color palette with 8 hard-to-name colors for use with qualitative/categorical data.
     */
    kSet2,

    /**
     * @brief Color palette `set3`.
     *
     * Provides a color palette with 12 colors for use with qualitative/categorical data.
     * The colors are more saturated than the Pastels but less so than set2().
     */
    kSet3
};

/**
 * @brief Get a qualitative color palette.
 *
 * See ColorListQualitative for the available ones.
 */
std::vector<Color> const& color_list_qualitative( ColorListQualitative palette );

/**
 * @brief Get a qualitative color palette by its name.
 *
 * The names used here are the same as in ColorListQualitative, but without the leading `k`,
 * i.e., using their original name (case insensitive and ignoring all non-alnum characters).
 * A list of all color palettes offered here by their name can also be obtained from
 * color_list_qualitative_names().
 * For instance, ColorListQualitative::kAccent can be obtained here simply as `"accent"`.
 */
std::vector<Color> const& color_list_qualitative( std::string const& palette );

/**
 * @brief Get the list of all available qualitative color palettes as strings.
 *
 * See also ColorListQualitative for the corresponding enum. Use the color list names provided here
 * as arguments for color_list_qualitative( std::string const& ).
 */
std::vector<std::string> color_list_qualitative_names();

} // namespace color
} // namespace util
} // namespace genesis

#endif // include guard
