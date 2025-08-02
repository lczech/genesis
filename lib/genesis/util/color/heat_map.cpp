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
 * @ingroup util
 */

#include <genesis/util/color/heat_map.hpp>

#include <cassert>
#include <cmath>
#include <stdexcept>

#include <genesis/util/color/norm_diverging.hpp>
#include <genesis/util/color/norm_linear.hpp>
#include <genesis/util/color/norm_logarithmic.hpp>
#include <genesis/util/color/normalization.hpp>
#include <genesis/util/core/std.hpp>
#include <genesis/util/math/matrix.hpp>
#include <genesis/util/math/statistic.hpp>

namespace genesis {
namespace util {
namespace color {

// =================================================================================================
//     Helper Functions and Function Templates
// =================================================================================================

genesis::util::math::MinMaxPair<double> heat_map_matrix_update_min_max_(
    genesis::util::math::MinMaxPair<double> const& min_max,
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
genesis::util::container::Matrix<genesis::util::color::Color> heat_map_matrix_range_all_(
    genesis::util::container::Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    using namespace genesis::util::color;
    using namespace genesis::util::container;

    // Prepare the matrix with the same dimensions as the input.
    auto result = Matrix<Color>( values.rows(), values.cols() );

    // Get the min and max value to use, and make a color normalization.
    auto min_max = genesis::util::math::finite_minimum_maximum( values.begin(), values.end() );
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
genesis::util::container::Matrix<genesis::util::color::Color> heat_map_matrix_range_row_(
    genesis::util::container::Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    using namespace genesis::util::color;
    using namespace genesis::util::container;

    // Prepare the matrix with the same dimensions as the input.
    auto result = Matrix<Color>( values.rows(), values.cols() );

    // Fill the pixels, normalizing each row.
    for( size_t row = 0; row < values.rows(); ++row ) {
        // Get the min and max value to use, and make a color normalization.
        auto min_max = genesis::util::math::finite_minimum_maximum(
            values.row(row).begin(), values.row(row).end()
        );
        min_max = heat_map_matrix_update_min_max_( min_max, parameters );
        auto norm = ColorNorm( min_max.min, min_max.max );

        for( size_t col = 0; col < values.cols(); ++col ) {
            result( row, col ) = parameters.color_map( norm, values( row, col ));
        }
    }
    return result;
}

template<class ColorNorm>
genesis::util::container::Matrix<genesis::util::color::Color> heat_map_matrix_range_col_(
    genesis::util::container::Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    using namespace genesis::util::color;
    using namespace genesis::util::container;

    // Prepare the matrix with the same dimensions as the input.
    auto result = Matrix<Color>( values.rows(), values.cols() );

    // Fill the pixels, normalizing each col.
    // We here traverse the matrix col-first, which is a bit slower, but needed to avoid
    // recomputing the norm for every pixel. Alternatively, we could compute them once in a vector
    // first, but well, this works, too.
    for( size_t col = 0; col < values.cols(); ++col ) {
        // Get the min and max value to use, and make a color normalization.
        auto min_max = genesis::util::math::finite_minimum_maximum(
            values.col(col).begin(), values.col(col).end()
        );
        min_max = heat_map_matrix_update_min_max_( min_max, parameters );
        auto norm = ColorNorm( min_max.min, min_max.max );

        for( size_t row = 0; row < values.rows(); ++row ) {
            result( row, col ) = parameters.color_map( norm, values( row, col ));
        }
    }
    return result;
}

template<class ColorNorm>
genesis::util::container::Matrix<genesis::util::color::Color> heat_map_matrix_range_(
    genesis::util::container::Matrix<double> const& values,
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
    genesis::util::container::Matrix<double> const& values,
    HeatmapParameters const& parameters
) {
    // Error checks
    if( parameters.normalization_range != HeatmapParameters::NormalizationRange::kAll ) {
        throw std::invalid_argument(
            "Can only make ColorNormalization for heat map with whole matrix value normalization."
        );
    }

    // Get the min and max value to use. Repetivive call, but well... refactor later...
    auto min_max = genesis::util::math::finite_minimum_maximum( values.begin(), values.end() );
    min_max = heat_map_matrix_update_min_max_( min_max, parameters );

    // Make a color norm for the given type... this is repetitive code from make_heatmap_matrix(),
    // and we need to refactor that in the future. But works for now.
    switch( parameters.color_norm ) {
        case HeatmapParameters::ColorNorm::kLinear: {
            return genesis::util::core::make_unique<ColorNormalizationLinear>( min_max.min, min_max.max );
        }
        case HeatmapParameters::ColorNorm::kLogarithmic: {
            return genesis::util::core::make_unique<ColorNormalizationLogarithmic>( min_max.min, min_max.max );
        }
        case HeatmapParameters::ColorNorm::kDiverging: {
            return genesis::util::core::make_unique<ColorNormalizationDiverging>( min_max.min, min_max.max );
        }
    }
    throw std::invalid_argument( "make_heatmap_color_norm(): Invalid HeatmapParameters::ColorNorm" );
}

genesis::util::container::Matrix<Color> make_heatmap_matrix(
    genesis::util::container::Matrix<double> const& values,
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

} // namespace color
} // namespace util
} // namespace genesis
