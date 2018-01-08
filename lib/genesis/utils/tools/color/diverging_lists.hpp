#ifndef GENESIS_UTILS_TOOLS_COLOR_DIVERGING_LISTS_H_
#define GENESIS_UTILS_TOOLS_COLOR_DIVERGING_LISTS_H_

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
* @brief Spectral color palette.
*
* This ColorBrewer color palette is adapted from https://github.com/axismaps/colorbrewer and
* https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` values.
* The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
* (http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider,
* and published under the [Apache-2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
* See also our @link supplement_acknowledgements_code_reuse_colorbrewer Acknowledgements@endlink.
*/
std::vector<Color> const& color_list_spectral();

// =================================================================================================
//     Convenience Functions
// =================================================================================================

enum class DivergingColorList
{
    kSpectral
};

std::vector<Color> const& diverging_color_list( DivergingColorList palette );
std::vector<Color> const& diverging_color_list( std::string const& palette );

} // namespace utils
} // namespace genesis

#endif // include guard
