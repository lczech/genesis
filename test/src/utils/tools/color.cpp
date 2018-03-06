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
 * @brief Testing Color class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/color/diverging_lists.hpp"
#include "genesis/utils/tools/color/qualitative_lists.hpp"
#include "genesis/utils/tools/color/sequential_lists.hpp"
#include "genesis/utils/tools/color/names.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/color/normalization.hpp"
#include "genesis/utils/tools/color/norm_diverging.hpp"
#include "genesis/utils/tools/color/norm_boundary.hpp"

#include <stdexcept>

using namespace genesis;
using namespace utils;

void compare_color(const Color& expected, const Color& actual)
{
    // Doubles are hard to compare. Just use the approximation here.
    EXPECT_EQ (expected.r_byte(), actual.r_byte());
    EXPECT_EQ (expected.g_byte(), actual.g_byte());
    EXPECT_EQ (expected.b_byte(), actual.b_byte());

    // EXPECT_FLOAT_EQ (expected.r(), actual.r());
    // EXPECT_FLOAT_EQ (expected.g(), actual.g());
    // EXPECT_FLOAT_EQ (expected.b(), actual.b());
}

TEST(Color, Gradient)
{
    // Set up colors for standard heat gradient.
    auto ranges_heat = std::map<double, Color>();
    ranges_heat[ 0.0 ] = color_from_bytes(   0, 255, 0 );
    ranges_heat[ 0.5 ] = color_from_bytes( 255, 255, 0 );
    ranges_heat[ 1.0 ] = color_from_bytes( 255,   0, 0 );

    // Proper value tests.
    compare_color(color_from_bytes(   0, 255, 0 ), gradient( ranges_heat, 0.0  ));
    compare_color(color_from_bytes( 128, 255, 0 ), gradient( ranges_heat, 0.25 ));
    compare_color(color_from_bytes( 255, 255, 0 ), gradient( ranges_heat, 0.5  ));
    compare_color(color_from_bytes( 255, 128, 0 ), gradient( ranges_heat, 0.75 ));
    compare_color(color_from_bytes( 255,   0, 0 ), gradient( ranges_heat, 1.0  ));

    // Off-range values.
    compare_color(color_from_bytes(   0, 255, 0 ), gradient( ranges_heat, -1.0 ));
    compare_color(color_from_bytes( 255,   0, 0 ), gradient( ranges_heat,  2.0 ));


    // Set up colors for some more complex gradient.
    auto ranges_long = std::map<double, Color>();
    ranges_long[ 0.0  ] = color_from_bytes(   0,   0,   0 );
    ranges_long[ 0.25 ] = color_from_bytes(   0, 255,   0 );
    ranges_long[ 0.5  ] = color_from_bytes( 255, 255,   0 );
    ranges_long[ 1.0  ] = color_from_bytes( 255,   0, 255 );

    // Test the complex gradient.
    compare_color(color_from_bytes(   0,   0,   0 ), gradient( ranges_long, 0.0  ));
    compare_color(color_from_bytes(   0, 102,   0 ), gradient( ranges_long, 0.1  ));
    compare_color(color_from_bytes(   0, 255,   0 ), gradient( ranges_long, 0.25 ));
    compare_color(color_from_bytes(  51, 255,   0 ), gradient( ranges_long, 0.3  ));
    compare_color(color_from_bytes( 255, 255,   0 ), gradient( ranges_long, 0.5  ));
    compare_color(color_from_bytes( 255, 204,  51 ), gradient( ranges_long, 0.6  ));
    compare_color(color_from_bytes( 255, 128, 128 ), gradient( ranges_long, 0.75 ));
    compare_color(color_from_bytes( 255,  51, 204 ), gradient( ranges_long, 0.9  ));
    compare_color(color_from_bytes( 255,   0, 255 ), gradient( ranges_long, 1.0  ));
}

TEST(Color, HeatGradient)
{
    // Proper value tests.
    compare_color(color_from_bytes(   0, 255, 0 ), heat_gradient( 0.0  ));
    compare_color(color_from_bytes( 128, 255, 0 ), heat_gradient( 0.25 ));
    compare_color(color_from_bytes( 255, 255, 0 ), heat_gradient( 0.5  ));
    compare_color(color_from_bytes( 255, 128, 0 ), heat_gradient( 0.75 ));
    compare_color(color_from_bytes( 255,   0, 0 ), heat_gradient( 1.0  ));

    // Off-range values.
    compare_color(color_from_bytes(   0, 255, 0 ), heat_gradient( -1.0 ));
    compare_color(color_from_bytes( 255,   0, 0 ), heat_gradient(  2.0 ));
}

// TEST(Color, FromDoubles)
// {
//     // Proper value tests.
//     compare_color(Color( 255,   0,   0 ), color_from_doubles( 1.0, 0.0, 0.0 ));
//     compare_color(Color(   0, 255,   0 ), color_from_doubles( 0.0, 1.0, 0.0 ));
//     compare_color(Color(   0,   0, 255 ), color_from_doubles( 0.0, 0.0, 1.0 ));
//     compare_color(Color( 128, 128, 128 ), color_from_doubles( 0.5, 0.5, 0.5 ));
//
//     // Off-range values.
//     compare_color(Color(   0,   0, 255 ), color_from_doubles( -1.0, 0.0, 10.0 ));
// }

TEST(Color, ToAndFromHex)
{
    // Produce hex color strings.
    EXPECT_EQ(  "#0033ff", color_to_hex(color_from_bytes(   0,  51, 255)) );
    EXPECT_EQ(  "#4201fe", color_to_hex(color_from_bytes(  66,   1, 254)) );
    EXPECT_EQ(   "000000", color_to_hex(color_from_bytes(   0,   0,   0), "") );
    EXPECT_EQ( "0XC0FFEE", color_to_hex(color_from_bytes( 192, 255, 238), "0X", true) );

    // Parse some valid hex color strings.
    compare_color( color_from_bytes(   0,   0,   0 ), color_from_hex("#000000") );
    compare_color( color_from_bytes( 171, 205, 239 ), color_from_hex("#abcdef") );
    compare_color( color_from_bytes( 255, 255, 255 ), color_from_hex("#fFFFff") );
    compare_color( color_from_bytes( 192, 255, 238 ), color_from_hex("c0ffee", "") );

    // Try to parse some malformed hex color strings.
    EXPECT_THROW(color_from_hex(""),         std::invalid_argument);
    EXPECT_THROW(color_from_hex("abc0123"),  std::invalid_argument);
    EXPECT_THROW(color_from_hex("#0000001"), std::invalid_argument);
    EXPECT_THROW(color_from_hex("#abcdez"),  std::invalid_argument);
}

TEST( Color, Names )
{
    // Find some valid color names.
    EXPECT_TRUE( is_web_color_name("Dark Sea Green") );
    EXPECT_TRUE( is_web_color_name("ForestGreen") );
    EXPECT_TRUE( is_web_color_name("lawn_green") );
    EXPECT_TRUE( is_web_color_name("lime green") );

    // And invalid.
    EXPECT_FALSE( is_web_color_name("heavy metal") );
    EXPECT_FALSE( is_web_color_name("applepie") );

    // Compare some color values.
    compare_color( color_from_bytes(199, 21, 133), color_from_name_web("MediumVioletRed") );
    compare_color( color_from_bytes(205, 92, 92), color_from_name_web("indian red") );

    // Get invalid color.
    EXPECT_THROW( color_from_name_web(""), std::invalid_argument );
    EXPECT_THROW( color_from_name_web("boot polish"), std::invalid_argument );
}

TEST( Color, PaletteSpectral )
{
    auto map = ColorMap( color_list_spectral() );
    auto norm = ColorNormalizationDiverging( -1.0, 1.0 );

    compare_color( color_from_bytes( 158,   1,  66 ), map( norm, -1.0 ) );
    compare_color( color_from_bytes( 249, 142,  82 ), map( norm, -0.5 ) );
    compare_color( color_from_bytes( 253, 174,  97 ), map( norm, -0.4 ) );
    compare_color( color_from_bytes( 255, 255, 191 ), map( norm,  0.0 ) );
    compare_color( color_from_bytes( 171, 221, 164 ), map( norm,  0.4 ) );
    compare_color( color_from_bytes( 137, 208, 165 ), map( norm,  0.5 ) );
    compare_color( color_from_bytes(  94,  79, 162 ), map( norm,  1.0 ) );

    // EXPECT_THROW( pal( -2.0 ), std::invalid_argument );
    // EXPECT_THROW( pal(  2.0 ), std::invalid_argument );
}

// TEST( Color, Map )
// {
//     auto const c = apply_color_map( color_list_spectral(), 0.5 );
// }

TEST( Color, NormBoundary )
{
    auto norm = ColorNormalizationBoundary( 3.0, 8.0, 5 );
    auto const bvec = std::vector<double>({ 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 });
    EXPECT_EQ( bvec, norm.boundaries() );

    EXPECT_EQ( -1, norm.interval( 0.0 ) );
    EXPECT_EQ( -1, norm.interval( 2.9 ) );
    EXPECT_EQ(  0, norm.interval( 3.0 ) );
    EXPECT_EQ(  0, norm.interval( 3.1 ) );
    EXPECT_EQ(  0, norm.interval( 3.9 ) );
    EXPECT_EQ(  1, norm.interval( 4.0 ) );
    EXPECT_EQ(  1, norm.interval( 4.1 ) );
    EXPECT_EQ(  4, norm.interval( 7.0 ) );
    EXPECT_EQ(  4, norm.interval( 7.1 ) );
    EXPECT_EQ(  4, norm.interval( 7.9 ) );
    EXPECT_EQ(  4, norm.interval( 8.0 ) );
    EXPECT_EQ(  5, norm.interval( 8.5 ) );

    EXPECT_FLOAT_EQ( -1.00, norm( 0.0 ) );
    EXPECT_FLOAT_EQ( -1.00, norm( 2.9 ) );
    EXPECT_FLOAT_EQ(  0.00, norm( 3.0 ) );
    EXPECT_FLOAT_EQ(  0.00, norm( 3.1 ) );
    EXPECT_FLOAT_EQ(  0.00, norm( 3.9 ) );
    EXPECT_FLOAT_EQ(  0.25, norm( 4.0 ) );
    EXPECT_FLOAT_EQ(  0.25, norm( 4.1 ) );
    EXPECT_FLOAT_EQ(  1.00, norm( 7.0 ) );
    EXPECT_FLOAT_EQ(  1.00, norm( 7.1 ) );
    EXPECT_FLOAT_EQ(  1.00, norm( 7.9 ) );
    EXPECT_FLOAT_EQ(  1.00, norm( 8.0 ) );
    EXPECT_FLOAT_EQ(  2.00, norm( 8.5 ) );
}
