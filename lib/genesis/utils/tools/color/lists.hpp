#ifndef GENESIS_UTILS_TOOLS_COLOR_LISTS_H_
#define GENESIS_UTILS_TOOLS_COLOR_LISTS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

std::vector<Color> const& color_list_magma();
std::vector<Color> const& color_list_inferno();
std::vector<Color> const& color_list_plasma();
std::vector<Color> const& color_list_viridis();
std::vector<Color> const& color_list_spectral();

// =================================================================================================
//     Convenience Functions
// =================================================================================================

enum class ColorList
{
    kMagma,
    kInferno,
    kPlasma,
    kViridis,
    kSpectral
};

std::vector<Color> const& color_list( ColorList          palette );
std::vector<Color> const& color_list( std::string const& palette );

} // namespace utils
} // namespace genesis

#endif // include guard
