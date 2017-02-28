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
 * @brief Testing Color class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/gradient.hpp"
#include "genesis/utils/tools/color/names.hpp"
#include "genesis/utils/tools/color/operators.hpp"

#include <stdexcept>

using namespace genesis;
using namespace utils;

void compare_color(const Color& expected, const Color& actual)
{
    EXPECT_EQ (expected.r(), actual.r());
    EXPECT_EQ (expected.g(), actual.g());
    EXPECT_EQ (expected.b(), actual.b());
}

TEST(Color, Gradient)
{
    // Set up colors for standard heat gradient.
    auto ranges_heat = std::map<double, Color>();
    ranges_heat[ 0.0 ] = Color(   0, 255, 0 );
    ranges_heat[ 0.5 ] = Color( 255, 255, 0 );
    ranges_heat[ 1.0 ] = Color( 255,   0, 0 );

    // Proper value tests.
    compare_color(Color(   0, 255, 0 ), gradient( ranges_heat, 0.0  ));
    compare_color(Color( 128, 255, 0 ), gradient( ranges_heat, 0.25 ));
    compare_color(Color( 255, 255, 0 ), gradient( ranges_heat, 0.5  ));
    compare_color(Color( 255, 128, 0 ), gradient( ranges_heat, 0.75 ));
    compare_color(Color( 255,   0, 0 ), gradient( ranges_heat, 1.0  ));

    // Off-range values.
    compare_color(Color(   0, 255, 0 ), gradient( ranges_heat, -1.0 ));
    compare_color(Color( 255,   0, 0 ), gradient( ranges_heat,  2.0 ));


    // Set up colors for some more complex gradient.
    auto ranges_long = std::map<double, Color>();
    ranges_long[ 0.0  ] = Color(   0,   0,   0 );
    ranges_long[ 0.25 ] = Color(   0, 255,   0 );
    ranges_long[ 0.5  ] = Color( 255, 255,   0 );
    ranges_long[ 1.0  ] = Color( 255,   0, 255 );

    // Test the complex gradient.
    compare_color(Color(   0,   0,   0 ), gradient( ranges_long, 0.0  ));
    compare_color(Color(   0, 102,   0 ), gradient( ranges_long, 0.1  ));
    compare_color(Color(   0, 255,   0 ), gradient( ranges_long, 0.25 ));
    compare_color(Color(  51, 255,   0 ), gradient( ranges_long, 0.3  ));
    compare_color(Color( 255, 255,   0 ), gradient( ranges_long, 0.5  ));
    compare_color(Color( 255, 204,  51 ), gradient( ranges_long, 0.6  ));
    compare_color(Color( 255, 128, 128 ), gradient( ranges_long, 0.75 ));
    compare_color(Color( 255,  51, 204 ), gradient( ranges_long, 0.9  ));
    compare_color(Color( 255,   0, 255 ), gradient( ranges_long, 1.0  ));


    // Invalid ranges.
    auto ranges_inv1 = std::map<double, Color>();
    auto ranges_inv2 = std::map<double, Color>();
    auto ranges_inv3 = std::map<double, Color>();
    auto ranges_inv4 = std::map<double, Color>();

    ranges_inv2[0.0] = Color(   0, 255, 0 );
    ranges_inv2[0.5] = Color( 255, 255, 0 );
    ranges_inv3[0.5] = Color( 255, 255, 0 );
    ranges_inv3[1.0] = Color( 255,   0, 0 );
    ranges_inv4[0.5] = Color( 255, 255, 0 );

    EXPECT_THROW( gradient(ranges_inv1, 0.0), std::invalid_argument );
    EXPECT_THROW( gradient(ranges_inv2, 0.0), std::invalid_argument );
    EXPECT_THROW( gradient(ranges_inv3, 0.0), std::invalid_argument );
    EXPECT_THROW( gradient(ranges_inv4, 0.0), std::invalid_argument );
}

TEST(Color, HeatGradient)
{
    // Proper value tests.
    compare_color(Color(   0, 255, 0 ), heat_gradient( 0.0  ));
    compare_color(Color( 128, 255, 0 ), heat_gradient( 0.25 ));
    compare_color(Color( 255, 255, 0 ), heat_gradient( 0.5  ));
    compare_color(Color( 255, 128, 0 ), heat_gradient( 0.75 ));
    compare_color(Color( 255,   0, 0 ), heat_gradient( 1.0  ));

    // Off-range values.
    compare_color(Color(   0, 255, 0 ), heat_gradient( -1.0 ));
    compare_color(Color( 255,   0, 0 ), heat_gradient(  2.0 ));
}

TEST(Color, FromDoubles)
{
    // Proper value tests.
    compare_color(Color( 255,   0,   0 ), color_from_doubles( 1.0, 0.0, 0.0 ));
    compare_color(Color(   0, 255,   0 ), color_from_doubles( 0.0, 1.0, 0.0 ));
    compare_color(Color(   0,   0, 255 ), color_from_doubles( 0.0, 0.0, 1.0 ));
    compare_color(Color( 128, 128, 128 ), color_from_doubles( 0.5, 0.5, 0.5 ));

    // Off-range values.
    compare_color(Color(   0,   0, 255 ), color_from_doubles( -1.0, 0.0, 10.0 ));
}

TEST(Color, ToAndFromHex)
{
    // Produce hex color strings.
    EXPECT_EQ(  "#0033ff", color_to_hex(Color(   0,  51, 255)) );
    EXPECT_EQ(  "#4201fe", color_to_hex(Color(  66,   1, 254)) );
    EXPECT_EQ(   "000000", color_to_hex(Color(   0,   0,   0), "") );
    EXPECT_EQ( "0XC0FFEE", color_to_hex(Color( 192, 255, 238), "0X", true) );

    // Parse some valid hex color strings.
    compare_color( Color(   0,   0,   0 ), color_from_hex("#000000") );
    compare_color( Color( 171, 205, 239 ), color_from_hex("#abcdef") );
    compare_color( Color( 255, 255, 255 ), color_from_hex("#fFFFff") );
    compare_color( Color( 192, 255, 238 ), color_from_hex("c0ffee", "") );

    // Try to parse some malformed hex color strings.
    EXPECT_THROW(color_from_hex(""),         std::invalid_argument);
    EXPECT_THROW(color_from_hex("abc0123"),  std::invalid_argument);
    EXPECT_THROW(color_from_hex("#0000001"), std::invalid_argument);
    EXPECT_THROW(color_from_hex("#abcdez"),  std::invalid_argument);
}

TEST( Color, Names )
{
    // Find some valid color names.
    EXPECT_TRUE( is_named_color("Dark Sea Green") );
    EXPECT_TRUE( is_named_color("ForestGreen") );
    EXPECT_TRUE( is_named_color("lawn_green") );
    EXPECT_TRUE( is_named_color("lime green") );

    // And invalid.
    EXPECT_FALSE( is_named_color("heavy metal") );
    EXPECT_FALSE( is_named_color("applepie") );

    // Compare some color values.
    compare_color( Color(199, 21, 133), get_named_color("MediumVioletRed") );
    compare_color( Color(205, 92, 92), get_named_color("indian red") );

    // Get invalid color.
    EXPECT_THROW( get_named_color(""), std::invalid_argument );
    EXPECT_THROW( get_named_color("boot polish"), std::invalid_argument );
}
