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

#include "genesis/utils/formats/svg/palette.hpp"

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/formats/svg/text.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Color Palette
// =================================================================================================

std::pair<SvgGradientLinear, SvgGroup> SvgPalette::make() const
{
    if( palette.palette().size() < 2 ) {
        throw std::runtime_error(
            "Cannot make SvgPalette with a ColorPalette of less than two colors."
        );
    }
    if( ! palette.range_check() ) {
        throw std::runtime_error(
            "Invaid ColorPalette min/mid/max settings."
        );
    }

    // Use a gradient ID with randomness so that we get a different one for each palette.
    std::string const gradient_id = "PaletteGradient_" + std::to_string( std::rand() );

    // Depending on the orientation, set gradient points.
    SvgPoint point_1;
    SvgPoint point_2;
    switch( direction ) {
        case Direction::kBottomToTop: {
            point_1 = SvgPoint( 0.0, 1.0 );
            point_2 = SvgPoint( 0.0, 0.0 );
            break;
        }
        case Direction::kTopToBottom: {
            point_1 = SvgPoint( 0.0, 0.0 );
            point_2 = SvgPoint( 0.0, 1.0 );
            break;
        }
        case Direction::kLeftToRight: {
            point_1 = SvgPoint( 0.0, 0.0 );
            point_2 = SvgPoint( 1.0, 0.0 );
            break;
        }
        case Direction::kRightToLeft: {
            point_1 = SvgPoint( 1.0, 0.0 );
            point_2 = SvgPoint( 0.0, 0.0 );
            break;
        }
        default: {
            throw std::runtime_error( "Invalid SvgPalette direction." );
        }
    }

    // Get color palette.
    auto colors = palette.palette();
    if( palette.reverse() ) {
        std::reverse( colors.begin(), colors.end() );
    }

    // Get the fractions of the lower and upper half,
    // which are needed to scale the colors in a diverging palette correctly.
    // For example, a palette with 5, 15, 20 for min, mid and max gets
    // fractions 2/3 and 1/3 here.
    auto const frac_lower = ( palette.mid() - palette.min()) / ( palette.max() - palette.min());
    auto const frac_upper = ( palette.max() - palette.mid()) / ( palette.max() - palette.min());

    // Fill gradient with the colors, add it to a group as a colored rect.
    // Diverging palette scales the lower half up to the mid color
    // differently thatn the upper half up the max.
    auto grad = SvgGradientLinear( gradient_id, point_1, point_2 );
    if( diverging_palette ) {

        // Divide the palette in two, so that the mixed mid color counts as half a step
        // in palettes with even number of colors.
        auto const scale = 2.0 / static_cast<double>( colors.size() - 1 );

        // Lower half.
        for( size_t i = 0; i < colors.size() / 2; ++i ) {
            auto const offset = scale * frac_lower * static_cast<double>( i );
            grad.add_stop({ offset, colors[i] });
        }

        // For an even number of colors, we need to add a mixed middle color.
        if( colors.size() % 2 == 0 ) {
            auto const mid_idx = colors.size() / 2;
            auto const mid_color = interpolate( colors[mid_idx - 1], colors[mid_idx], 0.5 );
            grad.add_stop({ frac_lower, mid_color });
        }

        // Upper half, including mid if uneven number of colors.
        for( size_t i = colors.size() / 2; i < colors.size(); ++i ) {

            // Step away from end: We go backwards.
            auto const step = static_cast<double>( colors.size() - i - 1 );

            // Offset, as before, just going backwards again, so that we end up in right order.
            auto const offset = 1.0 - ( scale * frac_upper * step );
            grad.add_stop({ offset, colors[i] });
        }

    } else {
        for( size_t i = 0; i < colors.size(); ++i ) {
            auto const offset = static_cast<double>( i ) / static_cast<double>( colors.size() - 1 );
            grad.add_stop({ offset, colors[i] });
        }
    }

    // Make group
    SvgGroup group;
    group << SvgRect(
        0.0, 0.0, width, height,
        SvgStroke(),
        // SvgStroke( SvgStroke::Type::kNone ),
        SvgFill( gradient_id )
    );

    // Helpfer function to make a tick mark with line and text
    // at a relative position [ 0.0 - 1.0 ] along the rect.
    auto make_tick = [&]( double rel_pos, std::string const& label ){
        assert( 0.0 <= rel_pos && rel_pos <= 1.0 );

        // Get positions for needed elements.
        double v = -1.0;
        double h = -1.0;
        switch( direction ) {
            case Direction::kBottomToTop: {
                v = height - ( rel_pos * height );
                break;
            }
            case Direction::kTopToBottom: {
                v = rel_pos * height;
                break;
            }
            case Direction::kLeftToRight: {
                h = rel_pos * width;
                break;
            }
            case Direction::kRightToLeft: {
                h = width - ( rel_pos * width );
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
        switch( direction ) {
            case Direction::kTopToBottom:
            case Direction::kBottomToTop:
            {
                assert( v > -1.0 );
                line1_p1 = SvgPoint( 0.0, v );
                line1_p2 = SvgPoint( width * 0.15, v );
                line2_p1 = SvgPoint( width * 0.85, v );
                line2_p2 = SvgPoint( width, v );
                text_p  = SvgPoint( width * 1.05, v );
                break;
            }
            case Direction::kLeftToRight:
            case Direction::kRightToLeft:
            {
                assert( h > -1.0 );
                line1_p1 = SvgPoint( h, 0.0 );
                line1_p2 = SvgPoint( h, height * 0.15 );
                line2_p1 = SvgPoint( h, height * 0.85 );
                line2_p2 = SvgPoint( h, height );
                text_p  = SvgPoint( h, height * 1.05 );
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
        if( with_labels ) {
            auto text_s = SvgText( label, text_p );
            // text_s.dominant_baseline = SvgText::DominantBaseline::kMiddle;
            // text_s.alignment_baseline = SvgText::AlignmentBaseline::kMiddle;
            text_s.dy = "0.33em";
            group << text_s;
        }
    };

    // Make tickmarks and labels.
    if( with_tickmarks ) {
        // make_tick( 0.0, utils::to_string( palette.min() ));
        // make_tick( 1.0, utils::to_string( palette.max() ));

        auto tm = Tickmarks();

        // If we want to make labels for a diverging palette, i.e., with the middle color,
        // we need to take care of correact scaling.
        // Otherwise, just make some labels along the scale.
        if( diverging_palette ) {

            // make_tick( 0.5, utils::to_string( palette.mid() ));

            // Lower half.
            tm.include_max = false;
            auto const tm_labels_l = tm.linear_labels(
                palette.min(), palette.mid(), frac_lower * num_ticks
            );
            for( auto const& tm_label : tm_labels_l ) {
                make_tick(
                    frac_lower * tm_label.relative_position,
                    utils::to_string( tm_label.label )
                );
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
                palette.mid(), palette.max(), frac_upper * num_ticks
            );
            for( auto const& tm_label : tm_labels_u ) {
                make_tick(
                    frac_lower + frac_upper * tm_label.relative_position,
                    utils::to_string( tm_label.label )
                );
            }

        } else {

            auto const tm_labels = tm.linear_labels( palette.min(), palette.max(), num_ticks );
            for( auto const& tm_label : tm_labels ) {
                make_tick( tm_label.relative_position, utils::to_string( tm_label.label ));
            }
        }
    }

    return { grad, group };
}

} // namespace utils
} // namespace genesis
