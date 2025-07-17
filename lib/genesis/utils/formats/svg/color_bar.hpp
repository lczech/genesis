#ifndef GENESIS_UTILS_FORMATS_SVG_COLOR_BAR_H_
#define GENESIS_UTILS_FORMATS_SVG_COLOR_BAR_H_

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
 * @ingroup utils
 */

#include <genesis/utils/formats/svg/gradient.hpp>
#include <genesis/utils/formats/svg/group.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace genesis {
namespace utils {
namespace color {

class ColorMap;
class ColorNormalization;
class ColorNormalizationDiverging;
class ColorNormalizationLinear;
class ColorNormalizationLogarithmic;

} // namespace color
} // namespace utils
} // namespace genesis

// =================================================================================================
//     Svg Color Bar
// =================================================================================================

namespace genesis {
namespace utils {
namespace formats {

struct SvgColorBarSettings
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    enum class Direction
    {
        kTopToBottom,
        kBottomToTop,
        kLeftToRight,
        kRightToLeft
    };

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    Direction direction = Direction::kBottomToTop;

    double width  = 20;
    double height = 200;
    double line_width = 1;

    bool with_tickmarks    = true;
    bool with_labels       = true;

    size_t num_ticks = 5;
    double text_size = 10.0;

};

std::pair<SvgGradientLinear, SvgGroup> make_svg_color_bar(
    SvgColorBarSettings const& settings,
    genesis::utils::color::ColorMap const& map,
    genesis::utils::color::ColorNormalization const& norm,
    std::string const& id = ""
);

SvgGroup make_svg_color_list(
    genesis::utils::color::ColorMap const& map,
    std::vector<std::string> const& labels
);

SvgGroup make_svg_color_list(
    std::vector<genesis::utils::color::Color> const& colors,
    std::vector<std::string> const& labels
);

} // namespace formats
} // namespace utils
} // namespace genesis

#endif // include guard
