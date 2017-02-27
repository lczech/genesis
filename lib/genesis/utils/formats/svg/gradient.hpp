#ifndef GENESIS_UTILS_FORMATS_SVG_GRADIENT_H_
#define GENESIS_UTILS_FORMATS_SVG_GRADIENT_H_

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

#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/tools/color.hpp"

#include <cassert>
#include <iosfwd>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Gradient Stop
// =================================================================================================

struct SvgGradientStop
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgGradientStop;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    SvgGradientStop(
        double       offset,
        Color const& stop_color = Color(),
        double       stop_opacity = 1.0
    )
        : offset( offset )
        , stop_color( stop_color )
        , stop_opacity( stop_opacity )
    {
        validate();
    }

    ~SvgGradientStop() = default;

    SvgGradientStop( SvgGradientStop const& ) = default;
    SvgGradientStop( SvgGradientStop&& )      = default;

    SvgGradientStop& operator= ( SvgGradientStop const& ) = default;
    SvgGradientStop& operator= ( SvgGradientStop&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void validate() const;
    void write( std::ostream& out ) const;

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    double offset;
    Color  stop_color;
    double stop_opacity;

};

struct SvgGradientLinear
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    using self_type = SvgGradientLinear;

    enum class SpreadMethod
    {
        kPad,
        kRepeat,
        kReflect
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct a gradient, with an ID and a vector determining its direction.
     *
     * The coordinates of the two points need to be in the range [ 0.0, 1.0 ], as they are
     * interpreted as percentages. This is what most SVG tools expect, so we stick to this
     * convention.
     *
     * By default, the points are set to a horizontal gradient spanning 0-100%.
     */
    SvgGradientLinear(
        std::string id,
        SvgPoint point_1 = SvgPoint( 0.0, 0.0 ),
        SvgPoint point_2 = SvgPoint( 1.0, 0.0 )
    )
        : id( id )
        , point_1( point_1 )
        , point_2( point_2 )
    {
        validate();
    }

    ~SvgGradientLinear() = default;

    SvgGradientLinear( SvgGradientLinear const& ) = default;
    SvgGradientLinear( SvgGradientLinear&& )      = default;

    SvgGradientLinear& operator= ( SvgGradientLinear const& ) = default;
    SvgGradientLinear& operator= ( SvgGradientLinear&& )      = default;

    // -------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------

    void validate() const;
    void write( std::ostream& out, size_t indent = 0 ) const;

    SvgGradientLinear& set_stops( std::map<double, Color> const& ranges );

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string id;

    SvgPoint  point_1;
    SvgPoint  point_2;

    SpreadMethod spread_method = SpreadMethod::kPad;

    std::vector< SvgGradientStop > stops;
    SvgTransform                   transform;

};

} // namespace utils
} // namespace genesis

#endif // include guard
