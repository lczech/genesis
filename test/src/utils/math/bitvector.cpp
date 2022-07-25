/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/helper.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace genesis::utils;

TEST( Bitvector, Arithmetics )
{
    Bitvector const bv0( "010101010101" );
    Bitvector const bv1( "111000111000" );
    Bitvector const bv2( "111100001111" );

    Bitvector const bvz( 12, false );
    Bitvector const bvo( 12, true );

    // Self-and is a fixpoint
    EXPECT_EQ( bv0, bv0 & bv0 );
    EXPECT_EQ( bv1, bv1 & bv1 );
    EXPECT_EQ( bv2, bv2 & bv2 );

    // Self-or is a fixpoint
    EXPECT_EQ( bv0, bv0 | bv0 );
    EXPECT_EQ( bv1, bv1 | bv1 );
    EXPECT_EQ( bv2, bv2 | bv2 );

    // Self-xor gives 0s
    EXPECT_EQ( bvz, bv0 ^ bv0 );
    EXPECT_EQ( bvz, bv1 ^ bv1 );
    EXPECT_EQ( bvz, bv2 ^ bv2 );

    // Test inverse
    EXPECT_EQ( Bitvector( "101010101010" ), ~bv0 );
    EXPECT_EQ( Bitvector( "000111000111" ), ~bv1 );
    EXPECT_EQ( Bitvector( "000011110000" ), ~bv2 );

    // Test pop counting
    EXPECT_EQ( 6, bv0.count() );
    EXPECT_EQ( 6, bv1.count() );
    EXPECT_EQ( 8, bv2.count() );

    // Test inverse as well, which also tests that the padding is 0
    EXPECT_EQ( 6, (~bv0).count() );
    EXPECT_EQ( 6, (~bv1).count() );
    EXPECT_EQ( 4, (~bv2).count() );

    // Test some inequality as well
    EXPECT_FALSE( bv0 == bv1 );
    EXPECT_FALSE( bv0 == bv2 );
    EXPECT_FALSE( bv1 == bv2 );
    EXPECT_TRUE(  bv0 != bv1 );
    EXPECT_TRUE(  bv0 != bv2 );
    EXPECT_TRUE(  bv1 != bv2 );
}

// TEST( Bitvector, LexCompare )
// {
//     Bitvector const bv0( "000000000001" );
//     Bitvector const bv1( "100000000000" );
//     // Bitvector const bv0( "010101010101" );
//     // Bitvector const bv1( "111000111000" );
//
//     LOG_DBG << bv0 << ": " << bv0.data()[0];
//     LOG_DBG << bv1 << ": " << bv1.data()[0];
//
//     EXPECT_FALSE( is_lexicographically_less( bv0, bv0 ));
//     EXPECT_FALSE( is_lexicographically_greater( bv0, bv1 ));
//     EXPECT_FALSE( is_lexicographically_greater( bv1, bv1 ));
//
//     EXPECT_TRUE( is_lexicographically_less( bv0, bv1 ));
//     EXPECT_TRUE( is_lexicographically_less_or_equal( bv0, bv1 ));
//     EXPECT_TRUE( is_lexicographically_less_or_equal( bv0, bv0 ));
// }

TEST( Bitvector, CopyRange )
{
    auto const bv_20 = Bitvector( 20 );
    auto const bv_10 = Bitvector( 10, bv_20 );
    EXPECT_EQ( 10, bv_10.size() );

    auto const bv_cp = Bitvector( 20, bv_20 );
    EXPECT_EQ( 20, bv_cp.size() );
}

TEST( Bitvector, Streams )
{
    std::srand(std::time(nullptr));

    // Create a random bitvector.
    size_t const size = 50;
    auto bv = Bitvector( size );
    for( size_t i = 0; i < size; ++i ) {
        auto const p = ( std::rand() % size );
        bv.flip( p );
    }

    // Stream it back and forth
    std::ostringstream ostr;
    ostr << bv;
    std::istringstream istr( ostr.str() );
    Bitvector cp;
    istr >> cp;
    EXPECT_EQ( bv, cp );

    // Add more stuff to ostr. Shoudl still work.
    ostr << "hello world";
    istr.str( ostr.str() );
    istr.clear();
    istr >> cp;
    EXPECT_EQ( bv, cp );
}

TEST( Bitvector, BoolVec )
{
    {
        auto const v = make_bool_vector_from_indices( { 1, 3, 5 } );
        auto const e = std::vector<bool>{ false, true, false, true, false, true };
        EXPECT_EQ( e, v );
    }
    {
        auto const v = make_bool_vector_from_indices( { 1, 3, 5 }, 8 );
        auto const e = std::vector<bool>{ false, true, false, true, false, true, false, false };
        EXPECT_EQ( e, v );
    }
    {
        // Size smaller than largest index.
        // Here, the size is 5 elements, 0-4, but the largest index is 5.
        EXPECT_ANY_THROW( make_bool_vector_from_indices( { 1, 3, 5 }, 5 ));
    }
}

TEST( Bitvector, Operators )
{
    // Minimal set of vectors to test all combinations of bits.
    auto const bv_s = Bitvector( "0011" );
    auto const bv_l = Bitvector( "010101" );

    // and, using smaller
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, false ));
    EXPECT_EQ( Bitvector("0001"),   bitwise_and( bv_l, bv_s, false ));
    EXPECT_EQ( Bitvector("0001"),   bitwise_and( bv_s, bv_l, false ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, false ));

    // and, using larger
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, true ));
    EXPECT_EQ( Bitvector("000100"), bitwise_and( bv_l, bv_s, true ));
    EXPECT_EQ( Bitvector("000100"), bitwise_and( bv_s, bv_l, true ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, true ));

    // or, using smaller
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, false ));
    EXPECT_EQ( Bitvector("0111"),   bitwise_or( bv_l, bv_s, false ));
    EXPECT_EQ( Bitvector("0111"),   bitwise_or( bv_s, bv_l, false ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, false ));

    // or, using larger
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, true ));
    EXPECT_EQ( Bitvector("011101"), bitwise_or( bv_l, bv_s, true ));
    EXPECT_EQ( Bitvector("011101"), bitwise_or( bv_s, bv_l, true ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, true ));

    // xor, using smaller
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, false ));
    EXPECT_EQ( Bitvector("0110"),   bitwise_xor( bv_l, bv_s, false ));
    EXPECT_EQ( Bitvector("0110"),   bitwise_xor( bv_s, bv_l, false ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, false ));

    // xor, using larger
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, true ));
    EXPECT_EQ( Bitvector("011001"), bitwise_xor( bv_l, bv_s, true ));
    EXPECT_EQ( Bitvector("011001"), bitwise_xor( bv_s, bv_l, true ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, true ));
}
