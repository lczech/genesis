/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
#include "genesis/utils/math/binomial.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/compensated_sum.hpp"

#include <limits>

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

TEST( Math, BinomialCoefficient )
{
    // Error cases
    EXPECT_ANY_THROW( binomial_coefficient_int( 0, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient_int( 0, 5 ));
    EXPECT_ANY_THROW( binomial_coefficient_int( 5, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient_int( 5, 7 ));

    // Overflow
    EXPECT_ANY_THROW( binomial_coefficient_int( 1024, 512 ));

    // Good cases
    EXPECT_EQ( 1, binomial_coefficient_int( 1, 1 ));
    EXPECT_EQ( 200, binomial_coefficient_int( 200, 1 ));
    EXPECT_EQ( 200, binomial_coefficient_int( 200, 199 ));
    EXPECT_EQ( 1, binomial_coefficient_int( 200, 200 ));
    EXPECT_EQ( 3276, binomial_coefficient_int( 28, 3 ));
    EXPECT_EQ( 3276, binomial_coefficient_int( 28, 25 ));

    static_assert( sizeof(size_t) == 8, "Expecting 64bit words." );

    // First case that overflows
    EXPECT_NO_THROW( binomial_coefficient_int( 63, 28 ));
    EXPECT_ANY_THROW( binomial_coefficient_int( 63, 29 ));

    // Overflow test, and test that the approximation works as well.
    for( size_t n = 1; n < 63; ++n ) {
        for( size_t k = 1; k < n; ++k ) {
            EXPECT_NO_THROW( binomial_coefficient_int(n,k) );
            EXPECT_FLOAT_EQ( binomial_coefficient_int(n,k), binomial_coefficient(n,k) );
        }
    }

    // Also test that all values that can be computed precisely also give the same results.
    for( size_t n = 0; n < 1024; ++n ) {
        for( size_t k = 1; k < n; ++k ) {
            size_t b = 0;
            try {
                b = binomial_coefficient_int(n,k);
            } catch(...) {
                // From here on, we reached the point where precise does not work any more for
                // this value of n. Skip the whole rest.
                break;
            }
            // LOG_DBG << n << " " << k;
            EXPECT_FLOAT_EQ( b, binomial_coefficient(n,k) );
        }
    }

    // Some explicit test cases for large numbers.
    // We are not quite hitting the right numbers here, due to lack of precision...
    // EXPECT_DOUBLE_EQ( 6.3850511926305e139, binomial_coefficient( 1000, 100 ));
    // EXPECT_DOUBLE_EQ( 2.7028824094544e299, binomial_coefficient( 1000, 500 ));
    EXPECT_DOUBLE_EQ( 6.3850511926560918e139, binomial_coefficient( 1000, 100 ));
    EXPECT_DOUBLE_EQ( 2.7028824094562908e299, binomial_coefficient( 1000, 500 ));

    // Error cases
    EXPECT_ANY_THROW( binomial_coefficient( 0, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient( 0, 5 ));
    EXPECT_ANY_THROW( binomial_coefficient( 5, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient( 5, 7 ));

    // Overflow or not. 1029 is the last n for which all k yield finite results.
    for( size_t k = 1; k < 1029; ++k ) {
        EXPECT_TRUE( std::isfinite( binomial_coefficient( 1029, k )));
    }

    // At n=1030, we encounter the first overflow.
    EXPECT_TRUE(  std::isfinite( binomial_coefficient( 1030, 499 )));
    EXPECT_FALSE( std::isfinite( binomial_coefficient( 1030, 500 )));
    EXPECT_TRUE(  std::isinf(    binomial_coefficient( 1030, 500 )));
    // for( size_t k = 1; k < 1030; ++k ) {
    //     EXPECT_TRUE( std::isfinite( binomial_coefficient( 1030, k ))) << k;
    // }
}

TEST( Math, LogFactorial )
{
    // Test all values. The first ~1000 are the same as the lookup table, and after that,
    // we test the Stirling approximation.
    double prev = 0.0;
    for( size_t i = 1; i < 1000000; ++i) {
        double cur = prev + std::log(i);
        EXPECT_FLOAT_EQ( cur, log_factorial( i ));
        prev = cur;
    }
}

TEST( Math, AlmostEqualRelative )
{
    EXPECT_TRUE( almost_equal_relative( 1.0, 1.0 ));
    EXPECT_TRUE( almost_equal_relative( 1.0, 1.0, 1.0 ));

    EXPECT_FALSE( almost_equal_relative( 1.0, 1.1, 0.05 ));
    EXPECT_FALSE( almost_equal_relative( 1.0, 1.1, 0.09 ));
    EXPECT_TRUE(  almost_equal_relative( 1.0, 1.1, 0.10 ));
    EXPECT_TRUE(  almost_equal_relative( 1.0, 1.1, 0.11 ));

    EXPECT_FALSE( almost_equal_relative( 1.0, 2.0 ));
    EXPECT_FALSE( almost_equal_relative( 1.0, 2.0, 0.49 ));
    EXPECT_TRUE(  almost_equal_relative( 1.0, 2.0, 0.50 ));
    EXPECT_TRUE(  almost_equal_relative( 1.0, 2.0, 0.51 ));
    EXPECT_TRUE(  almost_equal_relative( 1.0, 2.0, 1.00 ));
}

TEST( Math, KahanSum )
{
    size_t const k = 1000000;

    // Simple, for comparison.
    // double x = static_cast<double>(k) / 10.0;
    // for( size_t i = 0; i < k; ++i ) {
    //     x -= 0.1;
    // }
    // LOG_DBG << x;
    // k = 1e6 -->  -1.33288e-06

    // With Kahan.
    KahanSum s{ static_cast<double>(k) / 10.0 };
    for( size_t i = 0; i < k; ++i ) {
        s += -0.1;
    }
    // LOG_DBG << s.get();
    // k = 1e6 -->  -5.55112e-12

    // It's still not perfect. But double the digits of precision of summing without Kahan.
    // So let's check that we got better than that - say, 1e-11. Kinda arbitrary though.
    // If this test fails at some point, it's likely due to a compiler optimizing Kahan out again.
    // We have activated the volatile implementation that is already in the class, but still...
    EXPECT_TRUE( s.get() < 1e-11 );
}

TEST( Math, NeumaierSum )
{
    size_t const k = 1000000;
    NeumaierSum s{ static_cast<double>(k) / 10.0 };
    for( size_t i = 0; i < k; ++i ) {
        s += -0.1;
    }
    // LOG_DBG << s.get();
    EXPECT_TRUE( s.get() < 1e-11 );
}

TEST( Math, KleinSum )
{
    size_t const k = 1000000;
    KleinSum s{ static_cast<double>(k) / 10.0 };
    for( size_t i = 0; i < k; ++i ) {
        s += -0.1;
    }
    // LOG_DBG << s.get();
    EXPECT_TRUE( s.get() < 1e-11 );
}

TEST( Math, KahanSumFail )
{
    // Example from https://en.wikipedia.org/wiki/Kahan_summation_algorithm
    // Supposed to yield 2.0, but fails with standard Kahan sum, so we use the other two algorithms
    // to test whether they do better. Also, this tests the tag dispatch of the Kahan Sum class.

    {
        KahanSum s;
        s += 1.0;
        s += 1.0e100;
        s += 1.0;
        s -= 1.0e100;
        EXPECT_DOUBLE_EQ( 0.0, s );
    }
    {
        NeumaierSum s;
        s += 1.0;
        s += 1.0e100;
        s += 1.0;
        s -= 1.0e100;
        EXPECT_DOUBLE_EQ( 2.0, s );
    }
    {
        KleinSum s;
        s += 1.0;
        s += 1.0e100;
        s += 1.0;
        s -= 1.0e100;
        EXPECT_DOUBLE_EQ( 2.0, s );
    }
}
