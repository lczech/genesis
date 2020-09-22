/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

TEST( Math, BinomialCoefficient )
{
    // Error cases
    EXPECT_ANY_THROW( binomial_coefficient( 0, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient( 0, 5 ));
    EXPECT_ANY_THROW( binomial_coefficient( 5, 0 ));
    EXPECT_ANY_THROW( binomial_coefficient( 5, 7 ));

    // Overflow
    EXPECT_ANY_THROW( binomial_coefficient( 5000, 50 ));

    // Good cases
    EXPECT_EQ( 1, binomial_coefficient( 1, 1 ));
    EXPECT_EQ( 200, binomial_coefficient( 200, 1 ));
    EXPECT_EQ( 200, binomial_coefficient( 200, 199 ));
    EXPECT_EQ( 1, binomial_coefficient( 200, 200 ));
    EXPECT_EQ( 3276, binomial_coefficient( 28, 3 ));
    EXPECT_EQ( 3276, binomial_coefficient( 28, 25 ));

    static_assert( sizeof(size_t) == 8, "Expecting 64bit words." );

    // Overflow test
    for( size_t n = 1; n < 63; ++n ) {
        for( size_t k = 1; k < n; ++k ) {
            EXPECT_NO_THROW( binomial_coefficient(n,k) );
        }
    }

    // First case that overflows
    EXPECT_NO_THROW( binomial_coefficient( 63, 28 ));
    EXPECT_ANY_THROW( binomial_coefficient( 63, 29 ));
}
