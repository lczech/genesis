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

#include "genesis/utils/color/helpers.hpp"

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/color/color.hpp"
#include "genesis/utils/color/functions.hpp"
#include "genesis/utils/color/map.hpp"
#include "genesis/utils/color/norm_boundary.hpp"
#include "genesis/utils/color/norm_diverging.hpp"
#include "genesis/utils/color/norm_linear.hpp"
#include "genesis/utils/color/norm_logarithmic.hpp"
#include "genesis/utils/color/normalization.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Gradients
// =================================================================================================

std::map<double, Color> color_stops( ColorMap const& map, ColorNormalization const& norm )
{
    // Super duper ugly code.
    // Need to do linear last, because the other two are derived from it.

    auto norm_log = dynamic_cast<ColorNormalizationLogarithmic const*>( &norm );
    if( norm_log ) {
        return color_stops( map, *norm_log );
    }
    auto norm_div = dynamic_cast<ColorNormalizationDiverging const*>( &norm );
    if( norm_div ) {
        return color_stops( map, *norm_div );
    }
    auto norm_bnd = dynamic_cast<ColorNormalizationBoundary const*>( &norm );
    if( norm_bnd ) {
        return color_stops( map, *norm_bnd );
    }
    auto norm_lin = dynamic_cast<ColorNormalizationLinear const*>( &norm );
    if( norm_lin ) {
        return color_stops( map, *norm_lin );
    }

    return std::map<double, Color>{};
}

std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationLinear const& norm )
{
    (void) norm;
    std::map<double, Color> result;
    for( size_t i = 0; i < map.size(); ++i ) {
        auto const offset = static_cast<double>( i ) / static_cast<double>( map.size() - 1 );
        result[ offset ] = map.color( i );
    }
    return result;
}

std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationLogarithmic const& norm )
{
    return color_stops( map, dynamic_cast<ColorNormalizationLinear const&>( norm ));
}

std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationDiverging const& norm )
{
    std::map<double, Color> result;

    // Get the fractions of the lower and upper half,
    // which are needed to scale the colors in a diverging palette correctly.
    // For example, a palette with 5, 15, 20 for min, mid and max gets
    // fractions 2/3 and 1/3 here.
    auto const frac_lower = ( norm.mid_value() - norm.min_value() ) / ( norm.max_value() - norm.min_value() );
    auto const frac_upper = ( norm.max_value() - norm.mid_value() ) / ( norm.max_value() - norm.min_value() );

    // Divide the palette in two, so that the mixed mid color counts as half a step
    // in palettes with even number of colors.
    auto const scale = 2.0 / static_cast<double>( map.size() - 1 );

    // Lower half.
    for( size_t i = 0; i < map.size() / 2; ++i ) {
        auto const offset = scale * frac_lower * static_cast<double>( i );
        result[ offset ] = map.color( i );
    }

    // For an even number of colors, we need to add a mixed middle color.
    if( map.size() % 2 == 0 ) {
        auto const mid_idx = map.size() / 2;
        auto const mid_color = interpolate(
            map.color( mid_idx - 1 ), map.color( mid_idx ), 0.5
        );
        result[ frac_lower ] = mid_color;
    }

    // Upper half, including mid if uneven number of colors.
    for( size_t i = map.size() / 2; i < map.size(); ++i ) {

        // Step away from end: We go backwards.
        auto const step = static_cast<double>( map.size() - i - 1 );

        // Offset, as before, just going backwards again, so that we end up in right order.
        auto const offset = 1.0 - ( scale * frac_upper * step );
        result[ offset ] = map.color( i );
    }

    return result;
}

std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationBoundary const& norm )
{
    std::map<double, Color> result;

    // Get range.
    auto const min = norm.boundaries().front();
    auto const max = norm.boundaries().back();
    auto const len = max - min;

    for( auto const& bound : norm.boundaries() ) {
        result[ ( bound - min ) / len ] = map( norm, bound );
    }
    return result;
}

// =================================================================================================
//     Tickmarks
// =================================================================================================

std::map<double, std::string> color_tickmarks( ColorNormalization const& norm, size_t num_ticks )
{
    // Super duper ugly code.
    // Need to do linear last, because the other two are derived from it.

    auto norm_log = dynamic_cast<ColorNormalizationLogarithmic const*>( &norm );
    if( norm_log ) {
        return color_tickmarks( *norm_log, num_ticks );
    }
    auto norm_div = dynamic_cast<ColorNormalizationDiverging const*>( &norm );
    if( norm_div ) {
        return color_tickmarks( *norm_div, num_ticks );
    }
    auto norm_bnd = dynamic_cast<ColorNormalizationBoundary const*>( &norm );
    if( norm_bnd ) {
        return color_tickmarks( *norm_bnd, num_ticks );
    }
    auto norm_lin = dynamic_cast<ColorNormalizationLinear const*>( &norm );
    if( norm_lin ) {
        return color_tickmarks( *norm_lin, num_ticks );
    }

    return std::map<double, std::string>{};
}

std::map<double, std::string> color_tickmarks( ColorNormalizationLinear const& norm, size_t num_ticks )
{
    std::map<double, std::string> result;
    auto tm = Tickmarks();
    auto const tm_labels = tm.linear_labels( norm.min_value(), norm.max_value(), num_ticks );
    for( auto const& tm_label : tm_labels ) {
        result[ tm_label.relative_position ] = to_string_nice( tm_label.label );
    }
    return result;
}

std::map<double, std::string> color_tickmarks( ColorNormalizationLogarithmic const& norm, size_t num_ticks )
{
    std::map<double, std::string> result;

    // Don't use them for log scale.
    (void) num_ticks;

    auto tm = Tickmarks();
    auto const tm_labels_u = tm.logarithmic_labels( norm.min_value(), norm.max_value(), norm.base() );
    for( auto const& tm_label : tm_labels_u ) {
        auto label = ( norm.exponential_labels()
            ? to_string_nice( norm.base() ) + "^" + to_string_nice(
                std::log( tm_label.label ) / std::log( norm.base() )
            )
            : to_string_nice( tm_label.label )
        );
        result[ tm_label.relative_position ] = label;
    }

    return result;
}

std::map<double, std::string> color_tickmarks( ColorNormalizationDiverging const& norm, size_t num_ticks )
{
    std::map<double, std::string> result;
    auto tm = Tickmarks();

    // Get the fractions of the lower and upper half,
    // which are needed to scale the colors in a diverging palette correctly.
    // For example, a palette with 5, 15, 20 for min, mid and max gets
    // fractions 2/3 and 1/3 here.
    auto const frac_lower = ( norm.mid_value() - norm.min_value() ) / ( norm.max_value() - norm.min_value() );
    auto const frac_upper = ( norm.max_value() - norm.mid_value() ) / ( norm.max_value() - norm.min_value() );
    if( frac_lower <= 0.0 || frac_upper <= 0.0 ) {
        throw std::runtime_error( "Invalid color normalization for calculating tickmarks." );
    }

    // Lower half.
    tm.include_max = false;
    auto const tm_labels_l = tm.linear_labels(
        norm.min_value(), norm.mid_value(), static_cast<size_t>( frac_lower * num_ticks )
    );
    for( auto const& tm_label : tm_labels_l ) {
        auto const pos =  frac_lower * tm_label.relative_position;
        result[ pos ] = to_string_nice( tm_label.label );
    }

    // In cases where the mid value is a nice tickmark number (0 for example),
    // it will be included in the tickmarks, although it is the upper limit for
    // the lower half (that is, equal to the max for the half).
    // Thus, we already have a tickmark for the mid value, and now do not need it again
    // when making the upper half ticks. So, exclude the min for the upper half in this case.
    if( tm_labels_l.size() > 0 && tm_labels_l.back().relative_position == 1.0 ) {
        tm.include_min =  false;
    }

    // Upper half.
    tm.include_max = true;
    auto const tm_labels_u = tm.linear_labels(
        norm.mid_value(), norm.max_value(), static_cast<size_t>( frac_upper * num_ticks )
    );
    for( auto const& tm_label : tm_labels_u ) {
        auto const pos =  frac_lower + frac_upper * tm_label.relative_position;
        result[ pos ] = to_string_nice( tm_label.label );
    }

    return result;
}

std::map<double, std::string> color_tickmarks( ColorNormalizationBoundary const& norm, size_t num_ticks )
{
    // Ignore. We use the number of boundaries coming from the normalization.
    (void) num_ticks;

    // Get range.
    auto const min = norm.boundaries().front();
    auto const max = norm.boundaries().back();
    auto const len = max - min;

    std::map<double, std::string> result;
    for( auto const& bound : norm.boundaries() ) {
        result[ ( bound - min ) / len ] = to_string_nice( bound );
    }
    return result;
}

} // namespace utils
} // namespace genesis
