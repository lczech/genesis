#ifndef GENESIS_UTILS_FORMATS_SVG_AXIS_H_
#define GENESIS_UTILS_FORMATS_SVG_AXIS_H_

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

#include "genesis/utils/formats/svg/group.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Axis
// =================================================================================================

struct SvgAxisSettings
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    enum class Position
    {
        kLeft,
        kRight,
        kTop,
        kBottom
    };

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    /**
     * @brief Position of the axis, relative to the plot that it is describing.
     */
    Position position = Position::kBottom;

    /**
     * @brief Length of the axis.
     *
     * We always start drawing from 0.0 into the mathematically positive axis direction (left to
     * right, bottom to top), and draw an axis line of the specified length.
     */
    double length = 1.0;

    /**
     * @brief Offset from the origin for the axis.
     *
     * Always positive - for a left axis, this moves the axis towars the left, for a top axis towards
     * the top, etc.
     */
    double offset = 10.0;

    /**
     * @brief Length of the tick lines drawn from the axis to mark the tick labels.
     */
    double tick_size = 8.0;

    /**
     * @brief Size of the text of the tick labels.
     */
    double label_text_size = 10.0;

    /**
     * @brief Size of the text of the axis name.
     */
    double axis_text_size = 10.0;
};

/**
 * @brief Simple helper to make an axis.
 *
 * The helper currently only draws lines and puts the @p labels alongside. The given @p labels
 * have already to be pre-computed, with their position (`first` of the map) being their relative
 * position in `[ 0.0, 1.0 ]` along the axis.
 */
SvgGroup make_svg_axis(
    SvgAxisSettings const& settings,
    std::map<double, std::string> const& labels,
    std::string const& name = ""
);

/**
 * @brief Simple helper to make an axis.
 *
 * This overload of the function takes the result of the Tickmarks class when creating labels,
 * and forwards it to the other overload that takes string labeles instead, for convenience.
 * When using @p round_labels (default `true`), `std::round` is called on the label values first,
 * in order to print integer numbers.
 */
SvgGroup make_svg_axis(
    SvgAxisSettings const& settings,
    std::vector<Tickmarks::LabeledTick> const& labels,
    std::string const& name = "",
    bool round_labels = true
);

} // namespace utils
} // namespace genesis

#endif // include guard
