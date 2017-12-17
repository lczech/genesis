#ifndef GENESIS_UTILS_FORMATS_SVG_PALETTE_H_
#define GENESIS_UTILS_FORMATS_SVG_PALETTE_H_

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

#include "genesis/utils/formats/svg/gradient.hpp"
#include "genesis/utils/formats/svg/group.hpp"

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/palette.hpp"

#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Color Palette
// =================================================================================================

struct SvgPalette
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgPalette;

    enum class Direction
    {
        kTopToBottom,
        kBottomToTop,
        kLeftToRight,
        kRightToLeft
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgPalette() = default;
    ~SvgPalette() = default;

    SvgPalette( SvgPalette const& ) = default;
    SvgPalette( SvgPalette&& )      = default;

    SvgPalette& operator= ( SvgPalette const& ) = default;
    SvgPalette& operator= ( SvgPalette&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    std::pair<SvgGradientLinear, SvgGroup> make() const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    ColorPalette palette;
    Direction direction = Direction::kBottomToTop;

    double width  = 20;
    double height = 200;

    bool with_tickmarks    = true;
    bool with_labels       = true;
    bool diverging_palette = false;

    size_t num_ticks = 5;

};

} // namespace utils
} // namespace genesis

#endif // include guard
