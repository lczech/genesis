/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/utils/formats/svg/axis.hpp"

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/formats/svg/text.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Axis
// =================================================================================================

SvgGroup make_svg_axis(
    SvgAxisSettings const& settings,
    std::map<double, std::string> const& labels,
    std::string const& name
) {
    SvgGroup group;

    // Draw the actual axis line
    auto ax_end_x = 0.0;
    auto ax_end_y = 0.0;
    switch( settings.position ) {
        case SvgAxisSettings::Position::kLeft:
        case SvgAxisSettings::Position::kRight:
        {
            ax_end_y = - settings.length;
            break;
        }
        case SvgAxisSettings::Position::kTop:
        case SvgAxisSettings::Position::kBottom:
        {
            ax_end_x = settings.length;
            break;
        }
        default: {
            throw std::runtime_error( "Invalid SvgAxisSettings::Position" );
        }
    }
    auto axis_line = SvgLine( 0.0, 0.0, ax_end_x, ax_end_y );
    axis_line.stroke.line_cap = SvgStroke::LineCap::kSquare;
    group << std::move( axis_line );

    // Draw all axis ticks
    for( auto const& label : labels ) {
        double const eps = 0.0000001;
        if( !std::isfinite(label.first) || label.first < 0.0 - eps || label.first > 1.0 + eps ) {
            throw std::runtime_error( "Svg axis label position out of [ 0.0, 1.0 ]" );
        }

        // Find the direction away from the axis where to put the tick and the label,
        // using svg coordinate directions.
        double direction = 1.0;
        if(
            settings.position == SvgAxisSettings::Position::kLeft ||
            settings.position == SvgAxisSettings::Position::kTop
        ) {
            direction = -1.0;
        }

        // Find the coordinates
        SvgPoint line_p1;
        SvgPoint line_p2;
        SvgPoint text_p;
        switch( settings.position ) {
            case SvgAxisSettings::Position::kLeft:
            case SvgAxisSettings::Position::kRight:
            {
                auto const y_coord = - label.first * settings.length;
                line_p1 = SvgPoint( 0.0, y_coord );
                line_p2 = SvgPoint( 1.0 * direction * settings.tick_size, y_coord );
                text_p  = SvgPoint( 1.5 * direction * settings.tick_size, y_coord );
                break;
            }
            case SvgAxisSettings::Position::kTop:
            case SvgAxisSettings::Position::kBottom:
            {
                auto const x_coord = label.first * settings.length;
                line_p1 = SvgPoint( x_coord, 0.0 );
                line_p2 = SvgPoint( x_coord, 1.0 * direction * settings.tick_size );
                text_p  = SvgPoint( x_coord, 2.5 * direction * settings.tick_size );
                break;
            }
        }

        // Draw the lines and place the text
        auto tick_line = SvgLine( line_p1, line_p2 );
        tick_line.stroke.line_cap = SvgStroke::LineCap::kSquare;
        group << std::move( tick_line );
        auto label_text = SvgText( label.second );
        label_text.font.size = settings.label_text_size;
        label_text.transform.append( SvgTransform::Translate( text_p ));
        if( settings.position == SvgAxisSettings::Position::kLeft ) {
            label_text.anchor = SvgText::Anchor::kEnd;
        }
        if(
            settings.position == SvgAxisSettings::Position::kTop ||
            settings.position == SvgAxisSettings::Position::kBottom
        ) {
            label_text.anchor = SvgText::Anchor::kMiddle;
        }
        // if( settings.position == SvgAxisSettings::Position::kBottom ) {
        //     label_text.transform.append( SvgTransform::Rotate( 90.0 ));
        // }
        group << std::move( label_text );
    }

    // Move the group by the offset, and add the name if given. We always create the name,
    // but only add it if it's non-empty. That's easier than checking for emptyness every time...
    // (we are not expecting to create millions of axes, so that wastefulnes seems acceotable)
    auto name_text = SvgText( name );
    name_text.font.size = settings.axis_text_size;
    name_text.anchor = SvgText::Anchor::kMiddle;
    if( !std::isfinite( settings.offset ) || settings.offset < 0.0 ) {
        throw std::runtime_error( "Invalid axis offset " + std::to_string( settings.offset ));
    }
    switch( settings.position ) {
        case SvgAxisSettings::Position::kLeft:
        {
            name_text.transform.append( SvgTransform::Translate(
                group.bounding_box().top_left.x - 1.5 * settings.tick_size,
                - 0.5 * settings.length
            ));
            name_text.transform.append( SvgTransform::Rotate( -90.0 ));
            group.transform.append( SvgTransform::Translate( -settings.offset, 0.0 ));
            break;
        }
        case SvgAxisSettings::Position::kRight:
        {
            name_text.transform.append( SvgTransform::Translate(
                group.bounding_box().bottom_right.x + 1.5 * settings.tick_size,
                - 0.5 * settings.length
            ));
            name_text.transform.append( SvgTransform::Rotate( -90.0 ));
            group.transform.append( SvgTransform::Translate( settings.offset, 0.0 ));
            break;
        }
        case SvgAxisSettings::Position::kTop:
        {
            name_text.transform.append( SvgTransform::Translate(
                0.5 * settings.length,
                group.bounding_box().top_left.y - 1.5 * settings.tick_size
            ));
            group.transform.append( SvgTransform::Translate( 0.0, -settings.offset ));
            break;
        }
        case SvgAxisSettings::Position::kBottom:
        {
            name_text.transform.append( SvgTransform::Translate(
                0.5 * settings.length,
                group.bounding_box().bottom_right.y + 1.5 * settings.tick_size
            ));
            group.transform.append( SvgTransform::Translate( 0.0, settings.offset ));
            break;
        }
    }
    if( ! name.empty() ) {
        group << std::move( name_text );
    }
    return group;
}

SvgGroup make_svg_axis(
    SvgAxisSettings const& settings,
    std::vector<Tickmarks::LabeledTick> const& labels,
    std::string const& name,
    bool round_labels
) {
    std::map<double, std::string> ticks;
    for( auto const& tick : labels ) {
        ticks[ tick.relative_position ] = to_string_nice(
            round_labels ? std::round( tick.label ) : tick.label
        );
    }
    return make_svg_axis( settings, ticks, name );
}

} // namespace utils
} // namespace genesis
