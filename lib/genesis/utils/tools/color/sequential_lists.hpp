#ifndef GENESIS_UTILS_TOOLS_COLOR_SEQUENTIAL_LISTS_H_
#define GENESIS_UTILS_TOOLS_COLOR_SEQUENTIAL_LISTS_H_

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
 * @brief Magma color palette.
 *
 * This Matplotlib color palette is adapted from https://github.com/BIDS/colormap.
 * New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt.
 * The colormaps are released under the CC0 license / public domain dedication.
 * See https://creativecommons.org/publicdomain/zero/1.0/ for the license. See also our
 * @link supplement_acknowledgements_code_reuse_matplotlib_color_maps Acknowledgements@endlink.
 */
std::vector<Color> const& color_list_magma();

/**
* @brief Inferno color palette.
*
* This Matplotlib color palette is adapted from https://github.com/BIDS/colormap.
* New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt.
* The colormaps are released under the CC0 license / public domain dedication.
* See https://creativecommons.org/publicdomain/zero/1.0/ for the license. See also our
* @link supplement_acknowledgements_code_reuse_matplotlib_color_maps Acknowledgements@endlink.
*/
std::vector<Color> const& color_list_inferno();

/**
* @brief Plasma color palette.
*
* This Matplotlib color palette is adapted from https://github.com/BIDS/colormap.
* New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt.
* The colormaps are released under the CC0 license / public domain dedication.
* See https://creativecommons.org/publicdomain/zero/1.0/ for the license. See also our
* @link supplement_acknowledgements_code_reuse_matplotlib_color_maps Acknowledgements@endlink.
*/
std::vector<Color> const& color_list_plasma();

/**
* @brief Viridis color palette.
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

enum class SequentialColorList
{
    kMagma,
    kInferno,
    kPlasma,
    kViridis
};

std::vector<Color> const& sequential_color_list( SequentialColorList palette );
std::vector<Color> const& sequential_color_list( std::string const&  palette );

} // namespace utils
} // namespace genesis

#endif // include guard
