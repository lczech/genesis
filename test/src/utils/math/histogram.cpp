/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Testing Histogram class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/math/histogram.hpp"
#include "lib/utils/math/histogram/accumulator.hpp"
#include "lib/utils/math/histogram/distances.hpp"
#include "lib/utils/math/histogram/stats.hpp"

using namespace genesis;
using namespace utils;

// TODO add border cases and exception throwing stuff!

TEST(Histogram, UniformRange)
{
    // Using https://www.youtube.com/watch?v=iRiFtrYTH_E

    // As the example uses integer ranges, we need to set our ranges so that their midpoints align
    // with the integer values.
    auto h = Histogram(9, -0.5, 8.5);

    // Fill in values.
    h.accumulate(0.0, 8.0);
    h.accumulate(1.0, 11.0);
    h.accumulate(2.0, 4.0);
    h.accumulate(3.0, 2.0);
    h.increment(5.0);
    h.increment(8.0);

    // Check highest and lowest bin value.
    EXPECT_DOUBLE_EQ (0.0,  min_value(h));
    EXPECT_DOUBLE_EQ (11.0, max_value(h));

    // Check if highest and lowest values are in the corrent bins.
    EXPECT_EQ (4, min_bin(h));
    EXPECT_EQ (1, max_bin(h));

    // Check stats.
    EXPECT_DOUBLE_EQ (1.0, median(h));
    EXPECT_DOUBLE_EQ (1.4074074074074074, mean(h));
    EXPECT_DOUBLE_EQ (1.7268952928703358, sigma(h));
    EXPECT_DOUBLE_EQ (27.0, sum(h));

    // Check exceptions.
    EXPECT_NO_THROW(h.at(0));
    EXPECT_THROW(h.at(12), std::out_of_range);
}

TEST(Histogram, VariableRanges)
{
    // Using https://www.youtube.com/watch?v=vMrc6dP8pCo

    // Create with non-uniform ranges.
    std::vector<double> ranges = {1.0, 3.0, 7.0, 9.0, 12.0, 14.0};
    auto h = Histogram(ranges);

    // Fill in values.
    h.accumulate(2.0, 4.0);
    h.accumulate(5.0, 8.0);
    h.accumulate(8.0, 5.0);
    h.accumulate(10.0, 12.0);
    h.accumulate(13.0, 6.0);

    // Check highest and lowest bin value.
    EXPECT_DOUBLE_EQ (4.0,  min_value(h));
    EXPECT_DOUBLE_EQ (12.0, max_value(h));

    // Check if highest and lowest values are in the corrent bins.
    EXPECT_EQ (0, min_bin(h));
    EXPECT_EQ (3, max_bin(h));

    // Check stats.
    EXPECT_DOUBLE_EQ (9.125, median(h));
    EXPECT_DOUBLE_EQ (8.3428571428571434, mean(h));
    EXPECT_DOUBLE_EQ (3.5330509449471341, sigma(h));
    EXPECT_DOUBLE_EQ (35.0, sum(h));

    // Check exceptions.
    EXPECT_NO_THROW(h.at(0));
    EXPECT_THROW(h.at(12), std::out_of_range);
}

TEST(Histogram, EarthMoversDistanceSimple)
{
    // So far, EMD supports only Histograms with equal ranges. Build some.
    auto h1 = Histogram(4, 0.0, 4.0);
    auto h2 = Histogram(4, 0.0, 4.0);

    // Fill in values.
    h1.accumulate(0.0, 2.0);
    h1.accumulate(1.0, 4.0);
    h1.accumulate(2.0, 1.0);
    h1.accumulate(3.0, 0.0);

    // Fill in more values.
    h2.accumulate(0.0, 1.0);
    h2.accumulate(1.0, 2.0);
    h2.accumulate(2.0, 3.0);
    h2.accumulate(3.0, 1.0);

    // The first one does not normalize. The second one does. As the sum of weights in both
    // Histograms is 7, the expected distance then is 5/7 = 0.714.
    EXPECT_DOUBLE_EQ(5.0,                 earth_movers_distance(h1, h2, false));
    EXPECT_DOUBLE_EQ(0.71428571428571419, earth_movers_distance(h1, h2, true));

    // The EMD between a Histogram and itself needs to be zero.
    EXPECT_DOUBLE_EQ(0.0, earth_movers_distance(h1, h1, false));
    EXPECT_DOUBLE_EQ(0.0, earth_movers_distance(h1, h1, true));
    EXPECT_DOUBLE_EQ(0.0, earth_movers_distance(h2, h2, false));
    EXPECT_DOUBLE_EQ(0.0, earth_movers_distance(h2, h2, true));
}

TEST(Histogram, EarthMoversDistanceExtended)
{
    // Build two Histograms.
    auto h1 = Histogram(4, 0.0, 4.0);
    auto h2 = Histogram(4, 0.0, 4.0);

    // Fill in values.
    h1.accumulate(0.0, 2.0);
    h1.accumulate(3.0, 1.0);
    h2.accumulate(3.0, 1.0);

    // The first one does not normalize - this gives weird results.
    // The second one normalizes - better results.
    EXPECT_DOUBLE_EQ(6.0, earth_movers_distance(h1, h2, false));
    EXPECT_DOUBLE_EQ(2.0, earth_movers_distance(h1, h2, true));

    // Create a Histogram with same range but different number of bins. Should currenlty throw
    // as there is no bin-mapping approach implemented in EMD. Once there is, this test will reveal
    // this and needs to be modified.
    auto h3 = Histogram(5, 0.0, 4.0);
    EXPECT_THROW(earth_movers_distance(h1, h3), std::range_error);

    // Create a Histogram with different ranges. EMD should throw. If we extend the calculations
    // to also allow for different ranges, this test will reveal this and needs to be modified.
    auto h4 = Histogram(4, 2.0, 6.0);
    EXPECT_THROW(earth_movers_distance(h1, h4), std::range_error);
}

TEST(Histogram, Accumulator)
{
    auto a = HistogramAccumulator({1.1, 1.2, 1.3});
    a.increment(1.9);
    a.accumulate(1.8, 2.0);

    auto h1 = a.build_uniform_ranges_histogram(2);

    EXPECT_DOUBLE_EQ (1.1, h1.range_min());
    EXPECT_DOUBLE_EQ (1.9, h1.range_max());

    EXPECT_DOUBLE_EQ (3.0, h1[0]);
    EXPECT_DOUBLE_EQ (3.0, h1[1]);

    auto h2 = a.build_uniform_ranges_histogram(2, true);

    EXPECT_DOUBLE_EQ (1.0, h2.range_min());
    EXPECT_DOUBLE_EQ (2.0, h2.range_max());

    EXPECT_DOUBLE_EQ (3.0, h2[0]);
    EXPECT_DOUBLE_EQ (3.0, h2[1]);
}
