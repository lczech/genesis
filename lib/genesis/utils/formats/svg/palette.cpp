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
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/formats/svg/object.hpp"

#include <cstdlib>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Color Palette
// =================================================================================================

std::pair<SvgGradientLinear, SvgGroup> SvgPalette::make() const{

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

    // Make color palette.
    auto const& colors = palette.palette();

    // Fill gradient with the colors, add it to a group as a colored rect.
    auto grad = SvgGradientLinear( gradient_id, point_1, point_2 );
    for( size_t i = 0; i < colors.size(); ++i ) {
        auto const offset = static_cast<double>( i ) / static_cast<double>( colors.size() - 1 );
        grad.add_stop({ offset, colors[i] });
    }

    // Make group
    SvgGroup group;
    group << SvgRect(
        0.0, 0.0, width, height,
        SvgStroke( SvgStroke::Type::kNone ),
        SvgFill( gradient_id )
    );

    return { grad, group };
}

} // namespace utils
} // namespace genesis
