#ifndef GENESIS_UTILS_FORMAT_SVG_MATRIX_H_
#define GENESIS_UTILS_FORMAT_SVG_MATRIX_H_

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

#include <genesis/utils/format/svg/text.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/container/matrix.hpp>

#include <string>
#include <vector>

namespace genesis {
namespace utils {
namespace formats {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

struct SvgGroup;

// =================================================================================================
//     Svg Matrix
// =================================================================================================

struct SvgMatrixSettings
{
public:

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    double pixel_width  = 10.0;
    double pixel_height = 10.0;

    double width_overlap  = 1.0;
    double height_overlap = 1.0;

    genesis::utils::formats::SvgText label_template;

    /**
     * @brief Rotation of the column labels.
     *
     * Default is -90.0 degrees, that is, vertically upwards.
     * 0.0 degrees corresponds to normal horizontal text.
     */
    double column_label_rotation = -90.0;

};

SvgGroup make_svg_matrix(
    genesis::utils::containers::Matrix<genesis::utils::color::Color> const& mat,
    SvgMatrixSettings settings,
    std::vector<std::string> const& row_labels,
    std::vector<std::string> const& col_labels
);

SvgGroup make_svg_matrix(
    genesis::utils::containers::Matrix<genesis::utils::color::Color> const& mat,
    SvgMatrixSettings settings,
    std::vector<std::string> const& row_labels
);

SvgGroup make_svg_matrix(
    genesis::utils::containers::Matrix<genesis::utils::color::Color> const& mat,
    SvgMatrixSettings settings = {}
);

} // namespace formats
} // namespace utils
} // namespace genesis

#endif // include guard
