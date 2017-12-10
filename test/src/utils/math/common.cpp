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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"

#include<limits>

using namespace genesis::utils;

TEST(Std, RoundTo)
{
    EXPECT_FLOAT_EQ( 3.0     , round_to( 3.1415926535, 0 ) );
    EXPECT_FLOAT_EQ( 3.1     , round_to( 3.1415926535, 1 ) );
    EXPECT_FLOAT_EQ( 3.14    , round_to( 3.1415926535, 2 ) );
    EXPECT_FLOAT_EQ( 3.142   , round_to( 3.1415926535, 3 ) );
    EXPECT_FLOAT_EQ( 3.1416  , round_to( 3.1415926535, 4 ) );
    EXPECT_FLOAT_EQ( 3.14159 , round_to( 3.1415926535, 5 ) );
}

TEST( Math, IntPow )
{
    EXPECT_EQ( std::pow( 0, 0 ), int_pow( 0, 0 ) );
    EXPECT_EQ( std::pow( 2, 5 ), int_pow( 2, 5 ) );

    // Different results!
    // LOG_DBG << int_pow( 3, 40 );
    // LOG_DBG << static_cast<size_t>( std::pow( 3, 40 ));
}

TEST( Math, Median )
{
    // Empty.
    EXPECT_EQ( 0.0,  median({ }) );

    // One value.
    EXPECT_EQ(  0.0,  median({ 0.0 }) );
    EXPECT_EQ(  1.0,  median({ 1.0 }) );
    EXPECT_EQ( -1.0,  median({ -1.0 }) );

    // Two values.
    EXPECT_EQ( 0.0,  median({ 0.0, 0.0 }) );
    EXPECT_EQ( 0.5,  median({ 0.0, 1.0 }) );
    EXPECT_EQ( 2.0,  median({ 1.0, 3.0 }) );
    EXPECT_EQ( 2.5,  median({ 1.0, 4.0 }) );

    EXPECT_EQ( -0.5,  median({ -1.0, 0.0 }) );
    EXPECT_EQ(  0.0,  median({ -1.0, 1.0 }) );

    // Three values.
    EXPECT_EQ( 1.0,  median({ 0.0, 1.0, 2.0 }) );

    // Four values.
    EXPECT_EQ( 1.5,  median({ 0.0, 1.0, 2.0, 3.0 }) );
}

TEST( Math, Quartiles )
{
    // Odd number of elements.
    std::vector<double> v_odd = { 0, 3, 4, 7, 9, 12, 15 };
    auto q_odd = quartiles( v_odd );

    EXPECT_EQ(  0.0, q_odd.q0 );
    EXPECT_EQ(  3.0, q_odd.q1 );
    EXPECT_EQ(  7.0, q_odd.q2 );
    EXPECT_EQ( 12.0, q_odd.q3 );
    EXPECT_EQ( 15.0, q_odd.q4 );

    // Even number of elements.
    std::vector<double> v_even = { 1, 3, 5, 6, 12, 19, 23, 28 };
    auto q_even = quartiles( v_even );

    EXPECT_EQ(  1.0, q_even.q0 );
    EXPECT_EQ(  4.0, q_even.q1 );
    EXPECT_EQ(  9.0, q_even.q2 );
    EXPECT_EQ( 21.0, q_even.q3 );
    EXPECT_EQ( 28.0, q_even.q4 );
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

    EXPECT_FLOAT_EQ( -0.175757575, spearmans_rank_correlation_coefficient( iq, tv ) );
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
