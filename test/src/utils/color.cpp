/**
 * @brief Testing Color class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/tools/color.hpp"
#include "lib/utils/tools/color/gradient.hpp"
#include "lib/utils/tools/color/operators.hpp"

#include <stdexcept>

using namespace genesis;

void compare_color(const Color& expected, const Color& actual)
{
    EXPECT_EQ (expected.r(), actual.r());
    EXPECT_EQ (expected.g(), actual.g());
    EXPECT_EQ (expected.b(), actual.b());
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
