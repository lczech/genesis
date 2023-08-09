#ifndef GENESIS_UTILS_COLOR_LIST_MISC_H_
#define GENESIS_UTILS_COLOR_LIST_MISC_H_

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
 * @brief Color palette from the the Nextstrain build for novel coronavirus (nCoV) at
 * https://nextstrain.org/ncov
 *
 * The palette starts at purple and blue, progresses via green and yellow, towards orange and red.
 * This function returns a palette with 256 entries.
 * The original palette also has a reduced set of colors when using < 16 entries. We made these available
 * via the @link color_list_nextstrain( size_t n ) color_list_nextstrain( size_t n )@endlink
 * function, see there for details.
 *
 * This color palette is adapted from the Nextstrain build for novel coronavirus (nCoV) at
 * https://nextstrain.org/ncov, using their color scheme
 * https://github.com/nextstrain/ncov/blob/master/config/color_schemes.tsv
 * and converting parts of it to RGB `double` values.
 * The repository at https://github.com/nextstrain/ncov is published under the
 * [MIT license](https://opensource.org/licenses/MIT).
 * See also our @link supplement_acknowledgements_code_reuse_nextstrain Acknowledgements@endlink.
 */
std::vector<Color> const& color_list_nextstrain();

/**
 * @brief Color palette from the the Nextstrain build for novel coronavirus (nCoV) at
 * https://nextstrain.org/ncov
 *
 * The original palette has a reduced set of colors when using < 16 entries. This function makes
 * these reduced color sets available when using `n < 16`. All values above that use the full
 * palette (with 256) and interpolate to the requested number of entries. NB: The original palette
 * continues to 500 colors as well, but the differences between adjacent entries are diminishing,
 * so that our approach of interpolation when requesting a palette with more than 256 entries
 * yields highly similar entries. Also, who needs a palette with that many entries anyway?!
 *
 * This color palette is adapted from the Nextstrain build for novel coronavirus (nCoV) at
 * https://nextstrain.org/ncov, using their color scheme
 * https://github.com/nextstrain/ncov/blob/master/config/color_schemes.tsv
 * and converting parts of it to RGB `double` values.
 * The repository at https://github.com/nextstrain/ncov is published under the
 * [MIT license](https://opensource.org/licenses/MIT).
 * See also our @link supplement_acknowledgements_code_reuse_nextstrain Acknowledgements@endlink.
 */
std::vector<Color> color_list_nextstrain( size_t n );

// =================================================================================================
//     Convenience Functions
// =================================================================================================

enum class ColorListMisc
{
    kNextstrain
};

std::vector<Color> const& color_list_misc( ColorListMisc palette );
std::vector<Color> const& color_list_misc( std::string const& palette );

std::vector<std::string> color_list_misc_names();

} // namespace utils
} // namespace genesis

#endif // include guard
