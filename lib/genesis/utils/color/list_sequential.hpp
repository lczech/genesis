#ifndef GENESIS_UTILS_COLOR_LIST_SEQUENTIAL_H_
#define GENESIS_UTILS_COLOR_LIST_SEQUENTIAL_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @ingroup utils
 */

#include "genesis/utils/color/color.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Lists
// =================================================================================================

/**
 * @brief Color Palette `bupubk`.
 *
 * Provides a sequential blue color palette of light blue, purple and black.
 * Particularly useful for visualizing trees, as the light blue is still visible when
 * used to color branches.
 */
std::vector<Color> const& color_list_bupubk();

/**
 * @brief Color palette `blues`.
 *
 * Provides a sequential blue color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_blues();

/**
 * @brief Color palette `bugn`.
 *
 * Provides a sequential blue-green color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_bugn();

/**
 * @brief Color palette `bupu`.
 *
 * Provides a sequential blue-purple color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_bupu();

/**
 * @brief Color palette `gnbu`.
 *
 * Provides a sequential green-blue color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_gnbu();

/**
 * @brief Color palette `greens`.
 *
 * Provides a sequential green color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_greens();

/**
 * @brief Color palette `greys`.
 *
 * Provides a sequential grey color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_greys();

/**
 * @brief Color palette `oranges`.
 *
 * Provides a sequential orange color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_oranges();

/**
 * @brief Color palette `orrd`.
 *
 * Provides a sequential orange-red color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_orrd();

/**
 * @brief Color palette `pubu`.
 *
 * Provides a sequential purple-blue color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_pubu();

/**
 * @brief Color palette `pubugn`.
 *
 * Provides a sequential purple-blue-green color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_pubugn();

/**
 * @brief Color palette `purd`.
 *
 * Provides a sequential purple-red color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_purd();

/**
 * @brief Color palette `purples`.
 *
 * Provides a sequential purple color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_purples();

/**
 * @brief Color palette `rdpu`.
 *
 * Provides a sequential red-purple color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_rdpu();

/**
 * @brief Color palette `reds`.
 *
 * Provides a sequential red color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_reds();

/**
 * @brief Color palette `ylgn`.
 *
 * Provides a sequential yellow-green color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_ylgn();

/**
 * @brief Color palette `ylgnbu`.
 *
 * Provides a sequential yellow-green-blue color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_ylgnbu();

/**
 * @brief Color palette `ylorbr`.
 *
 * Provides a sequential yellow-orange-brown color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_ylorbr();

/**
 * @brief Color palette `ylorrd`.
 *
 * Provides a sequential yellow-orange-red color palette of increasing saturation.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_ylorrd();

/**
 * @brief Color palette `heat`.
 *
 * Provides a sequential heat color palette from white via yellow to orange and red.
 *
 * @copydetails color_list_spectral()
 */
std::vector<Color> const& color_list_heat();

/**
 * @brief Color palette `magma`.
 *
 * @copydetails color_list_viridis()
 */
std::vector<Color> const& color_list_magma();

/**
* @brief Color palette `inferno`.
*
* @copydetails color_list_viridis()
*/
std::vector<Color> const& color_list_inferno();

/**
* @brief Color palette `plasma`.
*
* @copydetails color_list_viridis()
*/
std::vector<Color> const& color_list_plasma();

/**
* @brief Color palette `viridis`.
*
* This Matplotlib color palette is adapted from https://github.com/BIDS/colormap.
* New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt, and Eric Firing.
* The colormaps are released under the CC0 license / public domain dedication.
* See https://creativecommons.org/publicdomain/zero/1.0/ for the license. See also our
* @link supplement_acknowledgements_code_reuse_matplotlib_color_maps Acknowledgements@endlink.
*/
std::vector<Color> const& color_list_viridis();

// =================================================================================================
//     Convenience Functions
// =================================================================================================

enum class ColorListSequential
{
    kBupubk,
    kBlues,
    kBugn,
    kBupu,
    kGnbu,
    kGreens,
    kGreys,
    kOranges,
    kOrrd,
    kPubu,
    kPubugn,
    kPurd,
    kPurples,
    kRdpu,
    kReds,
    kYlgn,
    kYlgnbu,
    kYlorbr,
    kYlorrd,
    kHeat,
    kMagma,
    kInferno,
    kPlasma,
    kViridis
};

std::vector<Color> const& color_list_sequential( ColorListSequential palette );
std::vector<Color> const& color_list_sequential( std::string const&  palette );

std::vector<std::string> color_list_sequential_names();

} // namespace utils
} // namespace genesis

#endif // include guard
