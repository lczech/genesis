#ifndef GENESIS_UTIL_COLOR_LIST_SEQUENTIAL_H_
#define GENESIS_UTIL_COLOR_LIST_SEQUENTIAL_H_

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
 * @brief Sequential color palettes.
 *
 * List of the sequential color palettes offered by color_list_sequential().
 * The available palettes come from difference sources, credited below.
 *
 * Some color palettes are adapted from ColorBrewer at https://github.com/axismaps/colorbrewer and
 * https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` values.
 * The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
 * (http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider,
 * and published under the [Apache-2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
 * See also our @link supplement_acknowledgements_code_reuse_colorbrewer Acknowledgements@endlink.
 *
 * Some color palettes are adapted from Matplotlib at https://github.com/BIDS/colormap.
 * New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt, and Eric Firing.
 * The colormaps are released under the CC0 license / public domain dedication.
 * See https://creativecommons.org/publicdomain/zero/1.0/ for the license. See also our
 * @link supplement_acknowledgements_code_reuse_matplotlib_color_maps Acknowledgements@endlink.
 *
 * For further color palettes, see ColorListQualitative, ColorListSequential, and ColorListMisc
 * which offer the other types of ColorBrewer lists and some more.
 */
enum class ColorListSequential
{

    /**
     * @brief Color Palette `bupubk`.
     *
     * Provides a sequential blue color palette of light blue, purple and black.
     * Particularly useful for visualizing thin lines on white background, as the light blue
     * is still well visible when used to color lines.
     *
     * This is an original color palette of genesis, developed for phylogenetic tree visualization.
     */
    kBupubk,

    /**
     * @brief Color palette `blues`.
     *
     * Provides a sequential blue color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kBlues,

    /**
     * @brief Color palette `bugn`.
     *
     * Provides a sequential blue-green color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kBugn,

    /**
     * @brief Color palette `bupu`.
     *
     * Provides a sequential blue-purple color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kBupu,

    /**
     * @brief Color palette `gnbu`.
     *
     * Provides a sequential green-blue color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kGnbu,

    /**
     * @brief Color palette `greens`.
     *
     * Provides a sequential green color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kGreens,

    /**
     * @brief Color palette `greys`.
     *
     * Provides a sequential grey color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kGreys,

    /**
     * @brief Color palette `oranges`.
     *
     * Provides a sequential orange color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kOranges,

    /**
     * @brief Color palette `orrd`.
     *
     * Provides a sequential orange-red color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kOrrd,

    /**
     * @brief Color palette `pubu`.
     *
     * Provides a sequential purple-blue color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kPubu,

    /**
     * @brief Color palette `pubugn`.
     *
     * Provides a sequential purple-blue-green color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kPubugn,

    /**
     * @brief Color palette `purd`.
     *
     * Provides a sequential purple-red color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kPurd,

    /**
     * @brief Color palette `purples`.
     *
     * Provides a sequential purple color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kPurples,

    /**
     * @brief Color palette `rdpu`.
     *
     * Provides a sequential red-purple color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kRdpu,

    /**
     * @brief Color palette `reds`.
     *
     * Provides a sequential red color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kReds,

    /**
     * @brief Color palette `ylgn`.
     *
     * Provides a sequential yellow-green color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kYlgn,

    /**
     * @brief Color palette `ylgnbu`.
     *
     * Provides a sequential yellow-green-blue color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kYlgnbu,

    /**
     * @brief Color palette `ylorbr`.
     *
     * Provides a sequential yellow-orange-brown color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kYlorbr,

    /**
     * @brief Color palette `ylorrd`.
     *
     * Provides a sequential yellow-orange-red color palette of increasing saturation.
     * It is part of the ColorBrewer palettes.
     */
    kYlorrd,

    /**
     * @brief Color palette `heat`.
     *
     * Provides a sequential heat color palette from white via yellow to orange and red.
     * It is part of the ColorBrewer palettes.
     */
    kHeat,

    /**
     * @brief Color palette `magma`.
     *
     * It is part of the Matplotlib palettes.
     */
    kMagma,

    /**
     * @brief Color palette `inferno`.
     *
     * It is part of the Matplotlib palettes.
     */
    kInferno,

    /**
     * @brief Color palette `plasma`.
     *
     * It is part of the Matplotlib palettes.
     */
    kPlasma,

    /**
     * @brief Color palette `viridis`.
     *
     * It is part of the Matplotlib palettes.
     */
    kViridis
};

/**
 * @brief Get a sequential color palette.
 *
 * See ColorListSequential for the available ones.
 */
std::vector<Color> const& color_list_sequential( ColorListSequential palette );

/**
 * @brief Get a sequential color palette by its name.
 *
 * The names used here are the same as in ColorListSequential, but without the leading `k`,
 * i.e., using their original name (case insensitive).
 * For instance, ColorListSequential::kViridis can be obtained here simply as `"viridis"`.
 */
std::vector<Color> const& color_list_sequential( std::string const&  palette );

/**
 * @brief Get the list of all available sequential color palettes as strings.
 *
 * See also ColorListSequential for the corresponding enum.
 */
std::vector<std::string> color_list_sequential_names();

} // namespace color
} // namespace util
} // namespace genesis

#endif // include guard
