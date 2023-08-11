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

#include "genesis/utils/color/heat_map.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

#include "genesis/utils/color/norm_diverging.hpp"
#include "genesis/utils/color/norm_linear.hpp"
#include "genesis/utils/color/norm_logarithmic.hpp"
#include "genesis/utils/color/normalization.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/statistics.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Helper Functions and Function Templates
// =================================================================================================

MinMaxPair<double> heat_map_matrix_update_min_max_(
    MinMaxPair<double> const& min_max,
    HeatmapParameters const& parameters
) {
    auto result = min_max;
    if( std::isfinite( parameters.min_value )) {
        result.min = parameters.min_value;
    }
    if( std::isfinite( parameters.max_value )) {
        result.max = parameters.max_value;
    }
    return result;
}

template<class ColorNorm>
Matrix<utils::Color> heat_map_matrix_range_all_(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    // Prepare the matrix with the same dimensions as the input.
    auto result = Matrix<utils::Color>( values.rows(), values.cols() );

    // Get the min and max value to use, and make a color normalization.
    auto min_max = finite_minimum_maximum( values.begin(), values.end() );
    min_max = heat_map_matrix_update_min_max_( min_max, parameters );
    auto norm = ColorNorm( min_max.min, min_max.max );

    // Fill the pixels.
    for( size_t row = 0; row < values.rows(); ++row ) {
        for( size_t col = 0; col < values.cols(); ++col ) {
            result( row, col ) = parameters.color_map( norm, values( row, col ));
        }
    }
    return result;
}

template<class ColorNorm>
Matrix<utils::Color> heat_map_matrix_range_row_(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    // Prepare the matrix with the same dimensions as the input.
    auto result = Matrix<utils::Color>( values.rows(), values.cols() );

    // Fill the pixels, normalizing each row.
    for( size_t row = 0; row < values.rows(); ++row ) {
        // Get the min and max value to use, and make a color normalization.
        auto min_max = finite_minimum_maximum( values.row(row).begin(), values.row(row).end() );
        min_max = heat_map_matrix_update_min_max_( min_max, parameters );
        auto norm = ColorNorm( min_max.min, min_max.max );

        for( size_t col = 0; col < values.cols(); ++col ) {
            result( row, col ) = parameters.color_map( norm, values( row, col ));
        }
    }
    return result;
}

template<class ColorNorm>
Matrix<utils::Color> heat_map_matrix_range_col_(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    // Prepare the matrix with the same dimensions as the input.
    auto result = Matrix<utils::Color>( values.rows(), values.cols() );

    // Fill the pixels, normalizing each col.
    // We here traverse the matrix col-first, which is a bit slower, but needed to avoid
    // recomputing the norm for every pixel. Alternatively, we could compute them once in a vector
    // first, but well, this works, too.
    for( size_t col = 0; col < values.cols(); ++col ) {
        // Get the min and max value to use, and make a color normalization.
        auto min_max = finite_minimum_maximum( values.col(col).begin(), values.col(col).end() );
        min_max = heat_map_matrix_update_min_max_( min_max, parameters );
        auto norm = ColorNorm( min_max.min, min_max.max );

        for( size_t row = 0; row < values.rows(); ++row ) {
            result( row, col ) = parameters.color_map( norm, values( row, col ));
        }
    }
    return result;
}

template<class ColorNorm>
Matrix<utils::Color> heat_map_matrix_range_(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    switch( parameters.normalization_range ) {
        case HeatmapParameters::NormalizationRange::kAll: {
            return heat_map_matrix_range_all_<ColorNorm>( values, parameters );
        }
        case HeatmapParameters::NormalizationRange::kRow: {
            return heat_map_matrix_range_row_<ColorNorm>( values, parameters );
        }
        case HeatmapParameters::NormalizationRange::kCol: {
            return heat_map_matrix_range_col_<ColorNorm>( values, parameters );
        }
    }
    throw std::invalid_argument( "heat_map_matrix_range_(): Invalid NormalizationRange" );
}

// =================================================================================================
//     Heat Map Matrix Functions
// =================================================================================================

std::unique_ptr<ColorNormalization> make_heatmap_color_norm(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    // Error checks
    if( parameters.normalization_range != HeatmapParameters::NormalizationRange::kAll ) {
        throw std::invalid_argument(
            "Can only make ColorNormalization for heat map with whole matrix value normalization."
        );
    }

    // Get the min and max value to use. Repetivive call, but well... refactor later...
    auto min_max = finite_minimum_maximum( values.begin(), values.end() );
    min_max = heat_map_matrix_update_min_max_( min_max, parameters );

    // Make a color norm for the given type... this is repetitive code from make_heatmap_matrix(),
    // and we need to refactor that in the future. But works for now.
    switch( parameters.color_norm ) {
        case HeatmapParameters::ColorNorm::kLinear: {
            return utils::make_unique<ColorNormalizationLinear>( min_max.min, min_max.max );
        }
        case HeatmapParameters::ColorNorm::kLogarithmic: {
            return utils::make_unique<ColorNormalizationLogarithmic>( min_max.min, min_max.max );
        }
        case HeatmapParameters::ColorNorm::kDiverging: {
            return utils::make_unique<ColorNormalizationDiverging>( min_max.min, min_max.max );
        }
    }
    throw std::invalid_argument( "make_heatmap_color_norm(): Invalid HeatmapParameters::ColorNorm" );
}

Matrix<Color> make_heatmap_matrix(
    Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    switch( parameters.color_norm ) {
        case HeatmapParameters::ColorNorm::kLinear: {
            return heat_map_matrix_range_<ColorNormalizationLinear>( values, parameters );
        }
        case HeatmapParameters::ColorNorm::kLogarithmic: {
            return heat_map_matrix_range_<ColorNormalizationLogarithmic>( values, parameters );
        }
        case HeatmapParameters::ColorNorm::kDiverging: {
            return heat_map_matrix_range_<ColorNormalizationDiverging>( values, parameters );
        }
    }
    throw std::invalid_argument( "make_heatmap_matrix(): Invalid HeatmapParameters::ColorNorm" );
}

} // namespace utils
} // namespace genesis
