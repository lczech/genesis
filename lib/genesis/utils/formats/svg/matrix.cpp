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

#include "genesis/utils/formats/svg/matrix.hpp"

#include "genesis/utils/formats/svg/group.hpp"
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/formats/svg/text.hpp"

#include "genesis/utils/color/color.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Matrix
// =================================================================================================

SvgGroup make_svg_matrix(
    Matrix<Color> const& mat,
    SvgMatrixSettings settings,
    std::vector<std::string> const& row_labels,
    std::vector<std::string> const& col_labels
) {
    // Result
    SvgGroup group;

    // Input checks.
    if( ! row_labels.empty() && row_labels.size() != mat.rows() ) {
        throw std::invalid_argument(
            "Svg Matrix drawing expects same number of row labels that the matrix has rows. "
        );
    }
    if( ! col_labels.empty() && col_labels.size() != mat.cols() ) {
        throw std::invalid_argument(
            "Svg Matrix drawing expects same number of col labels that the matrix has cols. "
        );
    }

    // Fill the matrix.
    SvgGroup rct_gr;
    for( size_t r = 0; r < mat.rows(); ++r ) {
        for( size_t c = 0; c < mat.cols(); ++c ) {

            // Get pixel dimensions.
            double x = c * settings.pixel_width;
            double y = r * settings.pixel_height;
            double w = settings.pixel_width;
            double h = settings.pixel_height;

            // Add overlap where needed (all except last col/row).
            if( c < mat.cols() - 1 ) {
                w += settings.width_overlap;
            }
            if( r < mat.rows() - 1 ) {
                h += settings.height_overlap;
            }

            group << SvgRect(
                x, y, w, h,
                SvgStroke( SvgStroke::Type::kNone ),
                SvgFill( mat( r, c ))
            );
        }
    }
    group << std::move( rct_gr );

    // Fill the row labels, if needed.
    if( ! row_labels.empty() ) {
        SvgGroup row_gr;
        for( size_t i = 0; i < row_labels.size(); ++i ) {
            auto label = settings.label_template;
            label.text = row_labels[i];
            label.anchor = SvgText::Anchor::kEnd;
            label.alignment_baseline = SvgText::AlignmentBaseline::kMiddle;

            double const x = - settings.pixel_width / 2.0;
            double const y = ( i * settings.pixel_height ) + ( settings.pixel_height / 2.0 );
            label.transform.append( SvgTransform::Translate( x, y ));
            row_gr << std::move( label );
        }
        group << std::move( row_gr );
    }

    // Fill the col labels, if needed.
    if( ! col_labels.empty() ) {
        SvgGroup col_gr;
        for( size_t i = 0; i < col_labels.size(); ++i ) {
            auto label = settings.label_template;
            label.text = col_labels[i];
            label.anchor = SvgText::Anchor::kStart;
            label.alignment_baseline = SvgText::AlignmentBaseline::kMiddle;

            double const x = ( i * settings.pixel_width ) + ( settings.pixel_width / 2.0 );
            double const y = - settings.pixel_height / 2.0;
            label.transform.append( SvgTransform::Translate( x, y ));
            label.transform.append( SvgTransform::Rotate( settings.column_label_rotation ));
            col_gr << std::move( label );
        }
        group << std::move( col_gr );
    }

    return group;
}

SvgGroup make_svg_matrix(
    Matrix<Color> const& mat,
    SvgMatrixSettings settings,
    std::vector<std::string> const& row_labels
) {
    return make_svg_matrix(
        mat, settings, row_labels, std::vector<std::string>()
    );
}

SvgGroup make_svg_matrix(
    Matrix<Color> const& mat,
    SvgMatrixSettings settings
) {
    return make_svg_matrix(
        mat, settings, std::vector<std::string>(), std::vector<std::string>()
    );
}

} // namespace utils
} // namespace genesis
