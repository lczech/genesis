#ifndef GENESIS_UTILS_COLOR_HEAT_MAP_H_
#define GENESIS_UTILS_COLOR_HEAT_MAP_H_

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

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/color/color.hpp"
#include "genesis/utils/color/map.hpp"

#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Heat Map Parameters
// =================================================================================================

struct HeatmapParameters
{
    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Type of color normalization to apply.
     *
     * We currently only offer linear, logarithmic, and diverging. Need to refactor in the future
     * to move away from an enum, and offer a more flexible way of providing the designed
     * ColorNormalization.
     *
     * When selecting diverging here, because of how this works as of now, we use mid-point centering.
     * In order to make sure that the mid point is, e.g., at 0, the user hence needs to provide
     * the #min_value and #max_value here, so that that they are symmetrical around 0.
     */
    enum class ColorNorm
    {
        kLinear,
        kLogarithmic,
        kDiverging
    };

    /**
     * @brief Normalize the colors over the whole matrix, or individually per row or column.
     */
    enum class NormalizationRange
    {
        kAll,
        kRow,
        kCol
    };

    // -------------------------------------------------------------------------
    //     Constructors
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor from a ColorMap.
     *
     * Just uses that color map, and leaves the other parameters at their defaults,
     * which should be reasonable in most situations anway.
     */
    HeatmapParameters( ColorMap const& map )
        : color_map( map )
    {}

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    // Color settings. We are currently not providing the normalization here,
    // but instead use an enum for that, because the classes are not super well done,
    // and that would get messy... so instead we introduce more development debt here.
    // What could go wrong?!
    // std::unique_ptr<ColorNormalization> color_norm;

    /**
     * @brief Set the ColorMap with all its properties to use for the heatmap.
     */
    ColorMap color_map;

    /**
     * @brief Select which ColorNormalization to apply to the data.
     */
    ColorNorm color_norm;

    /**
     * @brief Range about which to calculate the min and max value for the normalization.
     *
     * By default, we set the min and max value for the normalization using the whole matrix.
     * Using this, instead values can be normalized per row or per column instead, so that the min
     * and max values of each row/column are used to set up the normalization of ranges.
     * See ColorNormalization.
     *
     * Also note that the settings @p min_value and @p max_value in this class can be used to
     * set fixed values instead, by setting them to the desired values.
     */
    NormalizationRange normalization_range = NormalizationRange::kAll;

    /**
     * @brief Minimum value to use for the color normalization.
     *
     * By default, this is set to `NaN`, meaning that the minimum value for the ColorNormalization
     * is dermined by the values in the matrix, according to NormalizationRange. However, when
     * setting this to a finite value, this is used instead as the min, and not obtained from the
     * data. Same for `max_value`.
     */
    double min_value = std::numeric_limits<double>::quiet_NaN();

    /**
     * @brief Maximum value to use for the color normalization.
     *
     * See @p min_value for details. Same here, but for the max.
     */
    double max_value = std::numeric_limits<double>::quiet_NaN();
};

// =================================================================================================
//     Heat Map Matrix Functions
// =================================================================================================

std::unique_ptr<ColorNormalization> make_heatmap_color_norm(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
);

Matrix<Color> make_heatmap_matrix(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
);

} // namespace utils
} // namespace genesis

#endif // include guard
