/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/distance.hpp"
#include "genesis/utils/math/ranking.hpp"
#include "genesis/utils/math/statistics.hpp"

#include <limits>

using namespace genesis::utils;

TEST( Math, Closure )
{
    // Get a vector of some elements, and the expected closure.
    auto vals = std::vector<double>{
        1.0, 2.0, 3.0, 4.0, 0.0,
        std::numeric_limits<double>::infinity()
    };
    auto const expd = std::vector<double>{
        1.0 / 10.0, 2.0 / 10.0, 3.0 / 10.0, 4.0 / 10.0, 0.0,
        std::numeric_limits<double>::infinity()
    };

    // Run twice, because a closure should not change.
    closure( vals );
    EXPECT_EQ( expd, vals );
    closure( vals );
    EXPECT_EQ( expd, vals );

    vals.push_back( -1.0 );
    EXPECT_ANY_THROW( closure( vals ));
}

TEST( Math, ArithmeticMean )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  arithmetic_mean({ }) );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  arithmetic_mean({ 1.0 }) );
    EXPECT_DOUBLE_EQ(  5.0,  arithmetic_mean({ 5.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 5.0,  arithmetic_mean({ 2.0, 8.0 }) );
    EXPECT_DOUBLE_EQ( 6.0,  arithmetic_mean({ 6.0, 6.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 7.0,  arithmetic_mean({ 4.0, 6.0, 11.0 }) );
}

TEST( Math, WeightedArithmeticMean )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  weighted_arithmetic_mean({}, {}) );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  weighted_arithmetic_mean({ 1.0 }, { 1.0 }) );
    EXPECT_DOUBLE_EQ(  5.0,  weighted_arithmetic_mean({ 5.0 }, { 3.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 86.0,  weighted_arithmetic_mean({ 80.0, 90.0 }, { 20.0, 30.0 }) );
}

TEST( Math, GeometricMean )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  geometric_mean({ }) );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  geometric_mean({ 1.0 }) );
    EXPECT_DOUBLE_EQ(  5.0,  geometric_mean({ 5.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 4.0,  geometric_mean({ 2.0, 8.0 }) );
    EXPECT_DOUBLE_EQ( 6.0,  geometric_mean({ 6.0, 6.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 0.5,  geometric_mean({ 4.0, 1.0, 1.0/32.0 }) );

    // Edge cases.
    EXPECT_ANY_THROW( geometric_mean({  0.0 }) );
    EXPECT_ANY_THROW( geometric_mean({ -1.0 }) );
}

TEST( Math, WeightedGeometricMean )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  weighted_geometric_mean({}, {}) );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  weighted_geometric_mean({ 1.0 }, { 1.0 }) );
    EXPECT_DOUBLE_EQ(  5.0,  weighted_geometric_mean({ 5.0 }, { 3.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 4.0,  weighted_geometric_mean({ 2.0, 8.0 }, { 1.0, 1.0 }) );
    EXPECT_DOUBLE_EQ( 6.0,  weighted_geometric_mean({ 6.0, 6.0 }, { 3.0, 8.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 0.5,  weighted_geometric_mean({ 4.0, 1.0, 1.0/32.0 }, { 1.0, 1.0, 1.0 }) );

    // Edge cases.
    EXPECT_ANY_THROW( weighted_geometric_mean({  0.0 }, { 1.0 }) );
    EXPECT_ANY_THROW( weighted_geometric_mean({ -1.0 }, { 1.0 }) );
    EXPECT_ANY_THROW( weighted_geometric_mean({ 5.0 }, { 1.0, 2.0 }) );
    EXPECT_ANY_THROW( weighted_geometric_mean({ 5.0, 2.0 }, { 1.0 }) );
}

TEST( Math, HarmonicMean )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  harmonic_mean({ }) );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  harmonic_mean({ 1.0 }) );
    EXPECT_DOUBLE_EQ(  5.0,  harmonic_mean({ 5.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 3.2,  harmonic_mean({ 2.0, 8.0 }) );
    EXPECT_DOUBLE_EQ( 6.0,  harmonic_mean({ 6.0, 6.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 2.0,  harmonic_mean({ 1.0, 4.0, 4.0 }) );

    // Edge cases.
    EXPECT_ANY_THROW( harmonic_mean({  0.0 }) );
    EXPECT_ANY_THROW( harmonic_mean({ -1.0 }) );
    EXPECT_ANY_THROW( harmonic_mean({ 1.0, 0.0 }) );

    // Zero policy: Ignore.
    EXPECT_DOUBLE_EQ( 2.0,  harmonic_mean({ 1.0, 4.0, 4.0 },      HarmonicMeanZeroPolicy::kIgnore) );
    EXPECT_DOUBLE_EQ( 2.0,  harmonic_mean({ 1.0, 4.0, 4.0, 0.0 }, HarmonicMeanZeroPolicy::kIgnore) );

    // Zero policy: Return zero.
    EXPECT_DOUBLE_EQ( 2.0,  harmonic_mean({ 1.0, 4.0, 4.0 },      HarmonicMeanZeroPolicy::kReturnZero) );
    EXPECT_DOUBLE_EQ( 0.0,  harmonic_mean({ 1.0, 4.0, 4.0, 0.0 }, HarmonicMeanZeroPolicy::kReturnZero) );

    // Zero policy: Correction.
    EXPECT_DOUBLE_EQ( 2.0,  harmonic_mean({ 1.0, 4.0, 4.0 },      HarmonicMeanZeroPolicy::kCorrection) );
    EXPECT_DOUBLE_EQ(
        3.5327102803738315,
        harmonic_mean({ 0.0, 0.0, 5.0, 6.0, 7.0 }, HarmonicMeanZeroPolicy::kCorrection)
    );
}

TEST( Math, WeightedHarmonicMean )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  weighted_harmonic_mean({}, {}) );

    // One value.
    EXPECT_DOUBLE_EQ(  1.0,  weighted_harmonic_mean({ 1.0 }, { 1.0 }) );
    EXPECT_DOUBLE_EQ(  5.0,  weighted_harmonic_mean({ 5.0 }, { 3.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 3.2,  weighted_harmonic_mean({ 2.0, 8.0 }, { 1.0, 1.0 }) );
    EXPECT_DOUBLE_EQ( 6.0,  weighted_harmonic_mean({ 6.0, 6.0 }, { 3.0, 8.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 2.0,  weighted_harmonic_mean({ 1.0, 4.0, 4.0 }, { 1.0, 1.0, 1.0 }) );

    // Edge cases.
    EXPECT_ANY_THROW( weighted_harmonic_mean({  0.0 }, { 1.0 }) );
    EXPECT_ANY_THROW( weighted_harmonic_mean({ -1.0 }, { 1.0 }) );
    EXPECT_ANY_THROW( weighted_harmonic_mean({ 5.0 }, { 1.0, 2.0 }) );
    EXPECT_ANY_THROW( weighted_harmonic_mean({ 5.0, 2.0 }, { 1.0 }) );

    // Zero policy: Ignore.
    EXPECT_DOUBLE_EQ( 2.0,  weighted_harmonic_mean(
        { 1.0, 4.0, 4.0 }, { 1.0, 1.0, 1.0 },
        HarmonicMeanZeroPolicy::kIgnore
    ));
    EXPECT_DOUBLE_EQ( 2.0,  weighted_harmonic_mean(
        { 1.0, 4.0, 4.0, 0.0 }, { 1.0, 1.0, 1.0, 1.0 },
        HarmonicMeanZeroPolicy::kIgnore
    ));

    // Zero policy: Return zero.
    EXPECT_DOUBLE_EQ( 2.0,  weighted_harmonic_mean(
        { 1.0, 4.0, 4.0 }, { 1.0, 1.0, 1.0 },
        HarmonicMeanZeroPolicy::kReturnZero
    ));
    EXPECT_DOUBLE_EQ( 0.0,  weighted_harmonic_mean(
        { 1.0, 4.0, 4.0, 0.0 }, { 1.0, 1.0, 1.0, 1.0 },
        HarmonicMeanZeroPolicy::kReturnZero
    ));

    // Zero policy: Correction.
    EXPECT_DOUBLE_EQ( 2.0,  weighted_harmonic_mean(
        { 1.0, 4.0, 4.0 }, { 1.0, 1.0, 1.0 },
        HarmonicMeanZeroPolicy::kCorrection
    ));
    EXPECT_DOUBLE_EQ(
        3.5327102803738315,
        weighted_harmonic_mean(
            { 0.0, 0.0, 5.0, 6.0, 7.0 }, { 1.0, 1.0, 1.0, 1.0, 1.0 },
            HarmonicMeanZeroPolicy::kCorrection
        )
    );
}

TEST( Math, PNorm )
{
    auto const vals = std::vector<double>{ 1.0, 2.0, 3.0 };
    auto const inf  = std::numeric_limits<double>::infinity();

    // Valid cases
    EXPECT_DOUBLE_EQ( 6.0, p_norm( vals, 1.0 ));
    EXPECT_DOUBLE_EQ( 3.7416573867739413, p_norm( vals, 2.0 ));
    EXPECT_DOUBLE_EQ( 3.3019272488946263, p_norm( vals, 3.0 ));
    EXPECT_DOUBLE_EQ( 3.1463462836457885, p_norm( vals, 4.0 ));
    EXPECT_DOUBLE_EQ( 3.0, p_norm( vals, inf ));

    // Invalid cases
    EXPECT_ANY_THROW( p_norm( vals, -inf ));
    EXPECT_ANY_THROW( p_norm( vals, 0.0 ));
    EXPECT_ANY_THROW( p_norm( vals, std::numeric_limits<double>::quiet_NaN() ));

    // Named variants
    EXPECT_DOUBLE_EQ( 6.0,                manhattan_norm( vals ));
    EXPECT_DOUBLE_EQ( 3.7416573867739413, euclidean_norm( vals ));
    EXPECT_DOUBLE_EQ( 3.0,                maximum_norm( vals ));
}

TEST( Math, AitchisonNorm )
{
    auto const vals = std::vector<double>{ 1.0, 2.0, 3.0 };
    EXPECT_DOUBLE_EQ( 0.78566403520078676, aitchison_norm( vals ));
}

TEST( Math, Distances )
{
    EXPECT_DOUBLE_EQ( 5.0, euclidean_distance({ 2.0, -1.0 }, { -2.0, 2.0 }));
}

TEST( Math, Median )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  median(std::vector<double>{ }) );

    // One value.
    EXPECT_DOUBLE_EQ(  0.0,  median(std::vector<double>{ 0.0 }) );
    EXPECT_DOUBLE_EQ(  1.0,  median(std::vector<double>{ 1.0 }) );
    EXPECT_DOUBLE_EQ( -1.0,  median(std::vector<double>{ -1.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 0.0,  median(std::vector<double>{ 0.0, 0.0 }) );
    EXPECT_DOUBLE_EQ( 0.5,  median(std::vector<double>{ 0.0, 1.0 }) );
    EXPECT_DOUBLE_EQ( 2.0,  median(std::vector<double>{ 1.0, 3.0 }) );
    EXPECT_DOUBLE_EQ( 2.5,  median(std::vector<double>{ 1.0, 4.0 }) );

    EXPECT_DOUBLE_EQ( -0.5,  median(std::vector<double>{ -1.0, 0.0 }) );
    EXPECT_DOUBLE_EQ(  0.0,  median(std::vector<double>{ -1.0, 1.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 1.0,  median(std::vector<double>{ 0.0, 1.0, 2.0 }) );

    // Four values.
    EXPECT_DOUBLE_EQ( 1.5,  median(std::vector<double>{ 0.0, 1.0, 2.0, 3.0 }) );
}

TEST( Math, Quartiles )
{
    // Odd number of elements.
    std::vector<double> v_odd = { 0, 3, 4, 7, 9, 12, 15 };
    auto q_odd = quartiles( v_odd );

    EXPECT_DOUBLE_EQ(  0.0, q_odd.q0 );
    EXPECT_DOUBLE_EQ(  3.0, q_odd.q1 );
    EXPECT_DOUBLE_EQ(  7.0, q_odd.q2 );
    EXPECT_DOUBLE_EQ( 12.0, q_odd.q3 );
    EXPECT_DOUBLE_EQ( 15.0, q_odd.q4 );

    // Even number of elements.
    std::vector<double> v_even = { 1, 3, 5, 6, 12, 19, 23, 28 };
    auto q_even = quartiles( v_even );

    EXPECT_DOUBLE_EQ(  1.0, q_even.q0 );
    EXPECT_DOUBLE_EQ(  4.0, q_even.q1 );
    EXPECT_DOUBLE_EQ(  9.0, q_even.q2 );
    EXPECT_DOUBLE_EQ( 21.0, q_even.q3 );
    EXPECT_DOUBLE_EQ( 28.0, q_even.q4 );
}

TEST( Math, RankingSorted )
{
    // Example from https://rosettacode.org/wiki/Ranking_methods
    auto numbers = std::vector<double>({ 39, 41, 41, 41, 42, 42, 44 });
    EXPECT_EQ( std::vector<size_t>({ 1, 2, 2, 2, 5, 5, 7 }), ranking_standard( numbers ));
    EXPECT_EQ( std::vector<size_t>({ 1, 4, 4, 4, 6, 6, 7 }), ranking_modified( numbers ));
    EXPECT_EQ( std::vector<size_t>({ 1, 2, 2, 2, 3, 3, 4 }), ranking_dense( numbers ));
    EXPECT_EQ( std::vector<size_t>({ 1, 2, 3, 4, 5, 6, 7 }), ranking_ordinal( numbers ));
    EXPECT_EQ(
        std::vector<double>({ 1.0, 3.0, 3.0, 3.0, 5.5, 5.5, 7.0 }),
        ranking_fractional( numbers )
    );

    // Example from https://en.wikipedia.org/wiki/Ranking
    EXPECT_EQ(
        std::vector<double>({ 1.5, 1.5, 3.0, 4.5, 4.5, 6.0, 8.0, 8.0, 8.0 }),
        ranking_fractional({ 1.0, 1.0, 2.0, 3.0, 3.0, 4.0, 5.0, 5.0, 5.0 })
    );
}

TEST( Math, RankingUnsorted )
{
    // Example from https://rosettacode.org/wiki/Ranking_methods
    auto numbers = std::vector<double>({ 41, 39, 42, 41, 44, 42, 41 });
    EXPECT_EQ( std::vector<size_t>({ 2, 1, 5, 2, 7, 5, 2 }), ranking_standard( numbers ));
    EXPECT_EQ( std::vector<size_t>({ 4, 1, 6, 4, 7, 6, 4 }), ranking_modified( numbers ));
    EXPECT_EQ( std::vector<size_t>({ 2, 1, 3, 2, 4, 3, 2 }), ranking_dense( numbers ));
    EXPECT_EQ( std::vector<size_t>({ 2, 1, 5, 3, 7, 6, 4 }), ranking_ordinal( numbers ));
    EXPECT_EQ(
        std::vector<double>({ 3.0, 1.0, 5.5, 3.0, 7.0, 5.5, 3.0 }),
        ranking_fractional( numbers )
    );

    // Example from https://en.wikipedia.org/wiki/Ranking
    EXPECT_EQ(
        std::vector<double>({ 4.5, 6.0, 8.0, 1.5, 3.0, 4.5, 8.0, 8.0, 1.5 }),
        ranking_fractional({ 3.0, 4.0, 5.0, 1.0, 2.0, 3.0, 5.0, 5.0, 1.0 })
    );
}

TEST( Math, QuartileCoefficientOfDispersion )
{
    // Examples from https://en.wikipedia.org/wiki/Quartile_coefficient_of_dispersion
    auto a = std::vector<double>({ 2, 4, 6, 8, 10, 12, 14 });
    auto b = std::vector<double>({1.8, 2, 2.1, 2.4, 2.6, 2.9, 3});

    auto const ms_a = mean_stddev( a );
    auto const ms_b = mean_stddev( b );
    auto const q_a = quartiles( a );
    auto const q_b = quartiles( b );

    // n = 7, range = 12, mean = 8, median = 8, Q1 = 4, Q3 = 12, coefficient of dispersion = 0.5
    // n = 7, range = 1.2, mean = 2.4, median = 2.4, Q1 = 2, Q3 = 2.9, coefficient of dispersion = 0.18

    EXPECT_DOUBLE_EQ(  8.0, ms_a.mean );
    EXPECT_DOUBLE_EQ(  2.4, ms_b.mean );
    EXPECT_DOUBLE_EQ(  4.0, q_a.q1 );
    EXPECT_DOUBLE_EQ(  8.0, q_a.q2 );
    EXPECT_DOUBLE_EQ( 12.0, q_a.q3 );
    EXPECT_DOUBLE_EQ(  2.0, q_b.q1 );
    EXPECT_DOUBLE_EQ(  2.4, q_b.q2 );
    EXPECT_DOUBLE_EQ(  2.9, q_b.q3 );

    EXPECT_DOUBLE_EQ( 0.5, quartile_coefficient_of_dispersion( q_a ));
    EXPECT_DOUBLE_EQ( 0.18367346938775508, quartile_coefficient_of_dispersion( q_b ));
}
