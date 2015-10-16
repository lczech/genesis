/**
 * @brief Testing Histogram class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/math/histogram/histogram.hpp"
#include "lib/utils/math/histogram/distances.hpp"
#include "lib/utils/math/histogram/stats.hpp"

using namespace genesis;

// TODO add border cases and exception throwing stuff!

TEST(Histogram, UniformRange)
{
    // Using https://www.youtube.com/watch?v=iRiFtrYTH_E

	auto h = Histogram(9, -0.5, 8.5);

    h.accumulate(0.0, 8.0);
    h.accumulate(1.0, 11.0);
    h.accumulate(2.0, 4.0);
    h.accumulate(3.0, 2.0);
    h.increment(5.0);
    h.increment(8.0);

    EXPECT_DOUBLE_EQ (0.0,  min_value(h));
    EXPECT_DOUBLE_EQ (11.0, max_value(h));

    EXPECT_EQ (4, min_bin(h));
    EXPECT_EQ (1, max_bin(h));

    EXPECT_DOUBLE_EQ (1.0, median(h));
    EXPECT_DOUBLE_EQ (1.4074074074074074, mean(h));
    EXPECT_DOUBLE_EQ (1.7268952928703358, sigma(h));
    EXPECT_DOUBLE_EQ (27.0, sum(h));
}

TEST(Histogram, VariableRanges)
{
    // Using https://www.youtube.com/watch?v=vMrc6dP8pCo

    std::vector<double> ranges = {1.0, 3.0, 7.0, 9.0, 12.0, 14.0};
    auto h = Histogram(ranges);

    h.accumulate(2.0, 4.0);
    h.accumulate(5.0, 8.0);
    h.accumulate(8.0, 5.0);
    h.accumulate(10.0, 12.0);
    h.accumulate(13.0, 6.0);

    EXPECT_DOUBLE_EQ (4.0,  min_value(h));
    EXPECT_DOUBLE_EQ (12.0, max_value(h));

    EXPECT_EQ (0, min_bin(h));
    EXPECT_EQ (3, max_bin(h));

    EXPECT_DOUBLE_EQ (9.125, median(h));
    EXPECT_DOUBLE_EQ (8.3428571428571434, mean(h));
    EXPECT_DOUBLE_EQ (3.5330509449471341, sigma(h));
    EXPECT_DOUBLE_EQ (35.0, sum(h));
}

TEST(Histogram, EarthMoversDistance)
{
	auto h1 = Histogram(4, 0.0, 4.0);
	auto h2 = Histogram(4, 0.0, 4.0);

	h1.accumulate(0.0, 2.0);
	h1.accumulate(1.0, 4.0);
	h1.accumulate(2.0, 1.0);
	h1.accumulate(3.0, 0.0);

	h2.accumulate(0.0, 1.0);
	h2.accumulate(1.0, 2.0);
	h2.accumulate(2.0, 3.0);
	h2.accumulate(3.0, 1.0);

	EXPECT_DOUBLE_EQ(5.0, earth_movers_distance(h1, h2));
}
