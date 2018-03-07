/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/formats/svg/color_bar.hpp"

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/formats/svg/text.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/color/helpers.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/color/norm_boundary.hpp"
#include "genesis/utils/tools/color/norm_diverging.hpp"
#include "genesis/utils/tools/color/norm_linear.hpp"
#include "genesis/utils/tools/color/norm_logarithmic.hpp"
#include "genesis/utils/tools/color/normalization.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

std::pair<SvgGradientLinear, SvgGroup> make_svg_color_bar_gradient(
    SvgColorBarSettings const& settings,
    ColorMap const& map,
    ColorNormalization const& norm,
    std::string const& id
) {
    // Use a gradient ID with randomness so that we get a different one for each palette.
    std::string const gradient_id = ( id.empty()
        ? "PaletteGradient_" + std::to_string( std::rand() )
        : id
    );

    // Depending on the orientation, set gradient points.
    SvgPoint point_1;
    SvgPoint point_2;
    switch( settings.direction ) {
        case SvgColorBarSettings::Direction::kBottomToTop: {
            point_1 = SvgPoint( 0.0, 1.0 );
            point_2 = SvgPoint( 0.0, 0.0 );
            break;
        }
        case SvgColorBarSettings::Direction::kTopToBottom: {
            point_1 = SvgPoint( 0.0, 0.0 );
            point_2 = SvgPoint( 0.0, 1.0 );
            break;
        }
        case SvgColorBarSettings::Direction::kLeftToRight: {
            point_1 = SvgPoint( 0.0, 0.0 );
            point_2 = SvgPoint( 1.0, 0.0 );
            break;
        }
        case SvgColorBarSettings::Direction::kRightToLeft: {
            point_1 = SvgPoint( 1.0, 0.0 );
            point_2 = SvgPoint( 0.0, 0.0 );
            break;
        }
        default: {
            throw std::runtime_error( "Invalid SvgPalette direction." );
        }
    }

    // Get the gradient list depending on the norm type.
    auto const norm_gradient = color_stops( map, norm );

    // Fill gradient with the colors, add it to a group as a colored rect.
    auto grad = SvgGradientLinear( gradient_id, point_1, point_2 );
    for( auto const& g : norm_gradient ) {
        if( g.first < 0.0 || g.first > 1.0 ) {
            throw std::runtime_error( "Color Normalization gradient out of [ 0.0, 1.0 ]" );
        }
        grad.add_stop({ g.first, g.second });
    }

    // Make group
    SvgGroup group;
    group << SvgRect(
        0.0, 0.0, settings.width, settings.height,
        SvgStroke(),
        // SvgStroke( SvgStroke::Type::kNone ),
        SvgFill( gradient_id )
    );

    return { grad, group };
}

std::pair<SvgGradientLinear, SvgGroup> make_svg_color_bar_discrete(
    SvgColorBarSettings const& settings,
    std::map<double, Color> const& stops
) {
    // Fill a group with colors for the stops.
    SvgGroup group;
    for( auto it = stops.begin(); it != stops.end(); ++it ) {

        // Get and check iterator values.
        auto const& pos = it->first;
        auto const& color = it->second;
        if( pos < 0.0 || pos > 1.0 ) {
            throw std::runtime_error( "Color Normalization stops out of [ 0.0, 1.0 ]" );
        }

        // If we are at the upper bound, we do not need to draw another box.
        if( pos == 1.0 ) {
            continue;
        }

        // We need the next one for calculating the end of the box.
        // We init next pos to 1, which is used if there is no 1.0 stop in the stops list,
        // meaning that the `continue` statement above is never used.
        // In the other case (there is a stop 1.0), next will always point to a valid element,
        // so that next pos is set to it.
        auto next = it;
        ++next;
        double next_pos = 1.0;
        if( next != stops.end() ) {
            next_pos = next->first;
        }
        assert( next_pos > pos );

        // Calculate the box.
        double x = 0.0;
        double y = 0.0;
        double w = settings.width;
        double h = settings.height;
        switch( settings.direction ) {
            case SvgColorBarSettings::Direction::kBottomToTop: {
                y = settings.height - next_pos * settings.height;
                h = ( next_pos - pos ) * settings.height;
                break;
            }
            case SvgColorBarSettings::Direction::kTopToBottom: {
                y = pos * settings.height;
                h = ( next_pos - pos ) * settings.height;
                break;
            }
            case SvgColorBarSettings::Direction::kLeftToRight: {
                x = pos * settings.width;
                w = ( next_pos - pos ) * settings.width;
                break;
            }
            case SvgColorBarSettings::Direction::kRightToLeft: {
                x = settings.width - next_pos * settings.width;
                w = ( next_pos - pos ) * settings.width;
                break;
            }
            default: {
                throw std::runtime_error( "Invalid SvgPalette direction." );
            }
        }

        // Make the box.
        group << SvgRect(
            x, y, w, h,
            SvgStroke( SvgStroke::Type::kNone ),
            SvgFill( color )
        );
    }

    // Add a black line around the bar
    group << SvgRect(
        0.0, 0.0, settings.width, settings.height,
        SvgStroke( Color( 0.0, 0.0, 0.0 )),
        SvgFill( SvgFill::Type::kNone )
    );

    return { SvgGradientLinear(), group };
}

void make_svg_color_bar_tickmarks(
    SvgColorBarSettings const& settings,
    ColorMap const& map,
    ColorNormalization const& norm,
    SvgGroup& group
) {
    // Helper function to make a tick mark with line and text
    // at a relative position [ 0.0, 1.0 ] along the rect.
    auto make_tick = [&]( double rel_pos, std::string label ){
        assert( 0.0 <= rel_pos && rel_pos <= 1.0 );

        // Get positions for needed elements.
        double v = -1.0;
        double h = -1.0;
        switch( settings.direction ) {
            case SvgColorBarSettings::Direction::kBottomToTop: {
                v = settings.height - ( rel_pos * settings.height );
                break;
            }
            case SvgColorBarSettings::Direction::kTopToBottom: {
                v = rel_pos * settings.height;
                break;
            }
            case SvgColorBarSettings::Direction::kLeftToRight: {
                h = rel_pos * settings.width;
                break;
            }
            case SvgColorBarSettings::Direction::kRightToLeft: {
                h = settings.width - ( rel_pos * settings.width );
                break;
            }
            default: {
                throw std::runtime_error( "Invalid SvgPalette direction." );
            }
        }

        // Set elements.
        SvgPoint line1_p1;
        SvgPoint line1_p2;
        SvgPoint line2_p1;
        SvgPoint line2_p2;
        SvgPoint text_p;
        switch( settings.direction ) {
            case SvgColorBarSettings::Direction::kTopToBottom:
            case SvgColorBarSettings::Direction::kBottomToTop:
            {
                assert( v > -1.0 );
                line1_p1 = SvgPoint( 0.0, v );
                line1_p2 = SvgPoint( settings.width * 0.15, v );
                line2_p1 = SvgPoint( settings.width * 0.85, v );
                line2_p2 = SvgPoint( settings.width, v );
                text_p  = SvgPoint( settings.width * 1.05, v );
                break;
            }
            case SvgColorBarSettings::Direction::kLeftToRight:
            case SvgColorBarSettings::Direction::kRightToLeft:
            {
                assert( h > -1.0 );
                line1_p1 = SvgPoint( h, 0.0 );
                line1_p2 = SvgPoint( h, settings.height * 0.15 );
                line2_p1 = SvgPoint( h, settings.height * 0.85 );
                line2_p2 = SvgPoint( h, settings.height );
                text_p  = SvgPoint( h, settings.height * 1.05 );
                break;
            }
            default: {
                throw std::runtime_error( "Invalid SvgPalette direction." );
            }
        }

        // Draw lines and text. Lines only for inners, as we already have a box around the scale.
        if( rel_pos != 0.0 && rel_pos != 1.0 ) {
            group << SvgLine( line1_p1, line1_p2 );
            group << SvgLine( line2_p1, line2_p2 );
        }
        if( settings.with_labels ) {
            if( rel_pos == 1.0 && map.clip_over() ) {
                label = "≥ " + label;
            }
            if( rel_pos == 0.0 && map.clip_under() ) {
                label = "≤ " + label;
            }
            auto text_s = SvgText( label, text_p );
            // text_s.dominant_baseline = SvgText::DominantBaseline::kMiddle;
            // text_s.alignment_baseline = SvgText::AlignmentBaseline::kMiddle;
            // text_s.dy = "0.33em";
            group << text_s;
        }
    };

    // Make tickmarks and labels.
    if( settings.with_tickmarks ) {
        auto const tickmarks = color_tickmarks( norm, settings.num_ticks );
        for( auto const& tick : tickmarks ) {
            if( tick.first < 0.0 || tick.first > 1.0 ) {
                throw std::runtime_error( "Color Normalization tickmark out of [ 0.0, 1.0 ]" );
            }
            make_tick( tick.first, tick.second );
        }
    }
}

// =================================================================================================
//     Svg Color Bar
// =================================================================================================

std::pair<SvgGradientLinear, SvgGroup> make_svg_color_bar(
    SvgColorBarSettings const& settings,
    ColorMap const& map,
    ColorNormalization const& norm,
    std::string const& id
) {

    if( map.palette().size() < 2 ) {
        throw std::runtime_error(
            "Cannot make SvgPalette with a ColorMap of less than two colors."
        );
    }
    if( ! norm.is_valid() ) {
        throw std::runtime_error(
            "Invaid ColorNormalization settings."
        );
    }

    // Prepare result.
    auto result = std::pair<SvgGradientLinear, SvgGroup>();

    // We have an ugly special case for boundary norms, where we do not want to display
    // a gradient, but discrete color bars instead...
    auto norm_boundary = dynamic_cast<ColorNormalizationBoundary const*>( &norm );
    if( norm_boundary ) {
        auto const norm_gradient = color_stops( map, norm );
        result = make_svg_color_bar_discrete( settings, norm_gradient );
    } else {
        result = make_svg_color_bar_gradient( settings, map, norm, id );
    }

    // Add the tickmarks to the bar/
    make_svg_color_bar_tickmarks( settings, map, norm, result.second );

    return result;
}

SvgGroup make_svg_color_list(
    ColorMap const& map,
    std::vector<std::string> const& labels
) {
    SvgGroup group;

    for( size_t i = 0; i < labels.size(); ++i ) {
        group << SvgRect(
            0.0, i * 15.0, 10.0, 10.0,
            SvgStroke( SvgStroke::Type::kNone ),
            SvgFill( map.color(i) )
        );
        group << SvgText( labels[i], SvgPoint( 20.0, i * 15.0 + 10.0 ));
    }

    return group;
}

} // namespace utils
} // namespace genesis
