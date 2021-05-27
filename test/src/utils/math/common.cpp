/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
#include "genesis/utils/math/common.hpp"

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
    EXPECT_ANY_THROW( binomial_coefficient( 0, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient( 0, 5 ));
    EXPECT_ANY_THROW( binomial_coefficient( 5, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient( 5, 7 ));

    // Overflow
    EXPECT_ANY_THROW( binomial_coefficient( 1024, 512 ));

    // Good cases
    EXPECT_EQ( 1, binomial_coefficient( 1, 1 ));
    EXPECT_EQ( 200, binomial_coefficient( 200, 1 ));
    EXPECT_EQ( 200, binomial_coefficient( 200, 199 ));
    EXPECT_EQ( 1, binomial_coefficient( 200, 200 ));
    EXPECT_EQ( 3276, binomial_coefficient( 28, 3 ));
    EXPECT_EQ( 3276, binomial_coefficient( 28, 25 ));

    static_assert( sizeof(size_t) == 8, "Expecting 64bit words." );

    // First case that overflows
    EXPECT_NO_THROW( binomial_coefficient( 63, 28 ));
    EXPECT_ANY_THROW( binomial_coefficient( 63, 29 ));

    // Overflow test, and test that the approximation works as well.
    for( size_t n = 1; n < 63; ++n ) {
        for( size_t k = 1; k < n; ++k ) {
            EXPECT_NO_THROW( binomial_coefficient(n,k) );
            EXPECT_FLOAT_EQ( binomial_coefficient(n,k), binomial_coefficient_approx(n,k) );
        }
    }

    // Also test that all values that can be computed precisely also give the same results.
    for( size_t n = 0; n < 1024; ++n ) {
        for( size_t k = 1; k < n; ++k ) {
            size_t b = 0;
            try {
                b = binomial_coefficient(n,k);
            } catch(...) {
                // From here on, we reached the point where precise does not work any more for
                // this value of n. Skip the whole rest.
                break;
            }
            // LOG_DBG << n << " " << k;
            EXPECT_FLOAT_EQ( b, binomial_coefficient_approx(n,k) );
        }
    }

    // Some explicit test cases for large numbers.
    EXPECT_FLOAT_EQ( 6.3850511926305e139, binomial_coefficient_approx( 1000, 100 ));
    EXPECT_FLOAT_EQ( 2.7028824094544e299, binomial_coefficient_approx( 1000, 500 ));

    // Error cases
    EXPECT_ANY_THROW( binomial_coefficient_approx( 0, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient_approx( 0, 5 ));
    EXPECT_ANY_THROW( binomial_coefficient_approx( 5, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient_approx( 5, 7 ));

    // Overflow or not
    EXPECT_ANY_THROW( binomial_coefficient_approx( 1024, 512 ));
    EXPECT_NO_THROW( binomial_coefficient_approx( 1024, 512, true ));
    EXPECT_FLOAT_EQ(
        std::numeric_limits<double>::infinity(),
        binomial_coefficient_approx( 1024, 512, true )
    );
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
