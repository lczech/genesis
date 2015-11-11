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

using namespace genesis;

void compare_color(const Color& c1, const Color& c2)
{
    EXPECT_EQ (c1.r(), c2.r());
    EXPECT_EQ (c1.g(), c2.g());
    EXPECT_EQ (c1.b(), c2.b());
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
