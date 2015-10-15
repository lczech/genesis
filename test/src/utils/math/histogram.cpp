/**
 * @brief Testing Histogram class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/math/histogram.hpp"

using namespace genesis;

TEST(Histogram, UniformRange)
{
    // Using https://www.youtube.com/watch?v=iRiFtrYTH_E
	auto h = Histogram(9, -0.5, 8.5);

    h.accumulate(0, 8.0);
    h.accumulate(1, 11.0);
    h.accumulate(2, 4.0);
    h.accumulate(3, 2.0);
    h.increment(5);
    h.increment(8);

    EXPECT_DOUBLE_EQ (0.0,  h.min_value());
    EXPECT_DOUBLE_EQ (11.0, h.max_value());

    EXPECT_EQ (4, h.min_bin());
    EXPECT_EQ (1, h.max_bin());

    EXPECT_DOUBLE_EQ (1.0,  h.median());
    EXPECT_DOUBLE_EQ (1.4074074074074074, h.mean());
    EXPECT_DOUBLE_EQ (1.7268952928703358, h.sigma());
    EXPECT_DOUBLE_EQ (27.0, h.sum());
}

TEST(Histogram, VariantRanges)
{
    // Using https://www.youtube.com/watch?v=vMrc6dP8pCo
}
