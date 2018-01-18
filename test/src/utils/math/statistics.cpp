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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/statistics.hpp"

#include<limits>

using namespace genesis::utils;

TEST( Math, Median )
{
    // Empty.
    EXPECT_DOUBLE_EQ( 0.0,  median({ }) );

    // One value.
    EXPECT_DOUBLE_EQ(  0.0,  median({ 0.0 }) );
    EXPECT_DOUBLE_EQ(  1.0,  median({ 1.0 }) );
    EXPECT_DOUBLE_EQ( -1.0,  median({ -1.0 }) );

    // Two values.
    EXPECT_DOUBLE_EQ( 0.0,  median({ 0.0, 0.0 }) );
    EXPECT_DOUBLE_EQ( 0.5,  median({ 0.0, 1.0 }) );
    EXPECT_DOUBLE_EQ( 2.0,  median({ 1.0, 3.0 }) );
    EXPECT_DOUBLE_EQ( 2.5,  median({ 1.0, 4.0 }) );

    EXPECT_DOUBLE_EQ( -0.5,  median({ -1.0, 0.0 }) );
    EXPECT_DOUBLE_EQ(  0.0,  median({ -1.0, 1.0 }) );

    // Three values.
    EXPECT_DOUBLE_EQ( 1.0,  median({ 0.0, 1.0, 2.0 }) );

    // Four values.
    EXPECT_DOUBLE_EQ( 1.5,  median({ 0.0, 1.0, 2.0, 3.0 }) );
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

TEST( Math, SpearmansRankCorrelationCoeffcient )
{
    // Example from https://en.wikipedia.org/wiki/Spearman%27s_rank_correlation_coefficient
    auto iq = std::vector<double>({
        106, 86, 100, 101, 99, 103, 97, 113, 112, 110
    });
    auto tv = std::vector<double>({
        7, 0, 27, 50, 28, 29, 20, 12, 6, 17
    });
    EXPECT_DOUBLE_EQ( -0.17575757575757575, spearmans_rank_correlation_coefficient( iq, tv ) );

    // Add some nan values to it. Nothing should change.
    auto iq2 = std::vector<double>({
        106, 86, (1.0/0.0), 100, 101, 99, 103, 97, 113, 112, 110, (1.0/0.0)
    });
    auto tv2 = std::vector<double>({
        7, 0, 0, 27, 50, 28, 29, 20, 12, 6, 17, 100
    });
    EXPECT_DOUBLE_EQ( -0.17575757575757575, spearmans_rank_correlation_coefficient( iq2, tv2 ) );
}

TEST( Math, FisherTransformation )
{
    double const inf = std::numeric_limits<double>::infinity();

    EXPECT_FLOAT_EQ( -inf,      fisher_transformation( -1 ));
    EXPECT_FLOAT_EQ( -1.47222,  fisher_transformation( -0.9 ));
    EXPECT_FLOAT_EQ( -0.549306, fisher_transformation( -0.5 ));
    EXPECT_FLOAT_EQ( 0.0,       fisher_transformation( 0 ));
    EXPECT_FLOAT_EQ( 0.549306,  fisher_transformation( 0.5 ));
    EXPECT_FLOAT_EQ( 1.47222,   fisher_transformation( 0.9 ));
    EXPECT_FLOAT_EQ( inf,       fisher_transformation( 1 ));
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
