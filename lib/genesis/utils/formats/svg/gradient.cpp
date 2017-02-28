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

#include "genesis/utils/formats/svg/gradient.hpp"

#include "genesis/utils/formats/svg/document.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color/operators.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Gradient Stop
// =================================================================================================

void SvgGradientStop::validate() const
{
    // Boundary checks.
    if( offset < 0.0 || offset > 1.0 ) {
        throw std::invalid_argument(
            "Invalid Svg Gradient Stop offset. Has to be in range [ 0.0, 1.0 ], but is " +
            std::to_string( offset ) + "."
        );
    }
    if( stop_opacity < 0.0 || stop_opacity > 1.0 ) {
        throw std::invalid_argument(
            "Invalid Svg Gradient Stop stop_opacity. Has to be in range [ 0.0, 1.0 ], but is " +
            std::to_string( stop_opacity ) + "."
        );
    }
}

void SvgGradientStop::write( std::ostream& out ) const
{
    validate();
    out << "<stop";
    out << svg_attribute( "offset",       100 * offset, "%" );
    out << svg_attribute( "stop-color",   color_to_hex( stop_color ) );
    out << svg_attribute( "stop-opacity", stop_opacity );
    out << " />\n";
}

// =================================================================================================
//     Svg Gradient Linear
// =================================================================================================

void SvgGradientLinear::validate() const
{
    if(
        point_1.x < 0.0 || point_1.x > 1.0 || point_1.y < 0.0 || point_1.y > 1.0 ||
        point_2.x < 0.0 || point_2.x > 1.0 || point_2.y < 0.0 || point_2.y > 1.0
    ) {
        throw std::invalid_argument(
            "Invalid Svg Linar Gradient point. All coordinates of the points need to be in "
            "range [ 0.0, 1.0 ]."
        );
    }
}

void SvgGradientLinear::write( std::ostream& out, size_t indent ) const
{
    validate();

    out << repeat( SvgDocument::indentation_string, indent );
    out << "<linearGradient";
    out << svg_attribute( "id", id );

    out << svg_attribute( "x1", 100 * point_1.x, "%" );
    out << svg_attribute( "y1", 100 * point_1.y, "%" );
    out << svg_attribute( "x2", 100 * point_2.x, "%" );
    out << svg_attribute( "y2", 100 * point_2.y, "%" );

    switch( spread_method ) {
        case SpreadMethod::kPad: {
            out << svg_attribute( "spreadMethod", "pad" );
            break;
        }
        case SpreadMethod::kRepeat: {
            out << svg_attribute( "spreadMethod", "repeat" );
            break;
        }
        case SpreadMethod::kReflect: {
            out << svg_attribute( "spreadMethod", "reflect" );
            break;
        }
        default: {
            assert( false );
        }
    }
    transform.write( out );

    out << " >\n";

    for( auto const& stop : stops ) {
        out << repeat( SvgDocument::indentation_string, indent + 1 );
        stop.write( out );
    }

    out << repeat( SvgDocument::indentation_string, indent );
    out << "</linearGradient>\n";
}

SvgGradientLinear& SvgGradientLinear::set_stops( std::map<double, Color> const& ranges )
{
    // Check range sanity.
    if( ranges.size() < 2 ) {
        throw std::invalid_argument(
            "Svg Linar Gradient range needs to contain at least two colors."
        );
    }
    if( ranges.begin()->first != 0.0 ) {
        throw std::invalid_argument(
            "Svg Linar Gradient range needs to start with key value 0.0."
        );
    }
    if( ranges.rbegin()->first != 1.0 ) {
        throw std::invalid_argument(
            "Svg Linar Gradient range needs to end with key value 1.0."
        );
    }

    // Set new stops.
    stops.clear();
    for( auto const& stop : ranges ) {
        stops.emplace_back( stop.first, stop.second );
    }

    return *this;
}

} // namespace utils
} // namespace genesis
