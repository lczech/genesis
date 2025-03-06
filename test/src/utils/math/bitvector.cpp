/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/io/deserializer.hpp"
#include "genesis/utils/io/serializer.hpp"
#include "genesis/utils/math/bit.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/timer.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>

using namespace genesis::utils;

// =================================================================================================
//     I/O Operators
// =================================================================================================

// Make a random bitvector for testing.
// Very slow. We use the built-in function of Bitvector now.
// Bitvector make_random_bitvector_( size_t size )
// {
//     auto bv = Bitvector( size );
//     for( size_t i = 0; i < size; ++i ) {
//         auto const p = ( std::rand() % size );
//         bv.flip( p );
//     }
//     // LOG_DBG << bv;
//     return bv;
// }

TEST( Bits, ToBitString )
{
    EXPECT_EQ(
        "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000",
        to_bit_string<uint64_t>( 0 )
    );
    EXPECT_EQ(
        "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001",
        to_bit_string<uint64_t>( 1 )
    );
    EXPECT_EQ(
        "00000001 10110110 10011011 01001011 10101100 11010000 01011111 00010101",
        to_bit_string<uint64_t>( 123456789123456789ULL )
    );
}

TEST( Bitvector, ToBitString )
{
    EXPECT_EQ(
        "",
        to_bit_string( Bitvector( "" ))
    );
    EXPECT_EQ(
        "01010101 0101",
        to_bit_string( Bitvector( "010101010101" ))
    );
    EXPECT_EQ(
        "01010101 01011010",
        to_bit_string( Bitvector( "0101010101011010" ))
    );
    EXPECT_EQ(
        "01100",
        to_bit_string( Bitvector( 5, { 1, 2 }))
    );
}

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

TEST( Bitvector, Serialization )
{
    std::srand(std::time(nullptr));

    // We test that a container of bitvectors also works, and internally test
    // different sizes that are either exact boundaries, or some arbitrary values.
    std::vector<Bitvector> bvs;
    bvs.push_back( make_random_bitvector( 42 ));
    bvs.push_back( make_random_bitvector( 0 ));
    bvs.push_back( make_random_bitvector( 512 ));
    bvs.push_back( make_random_bitvector( 710 ));

    // Serialize
    std::ostringstream out;
    Serializer serial( to_stream( out ));
    serial << bvs;
    auto const out_str = out.str();

    // Test that the string has the correct size.
    // This is a size_t for the outer std::vector, and then for each internal bitvector,
    // we need its size in bits, its vector size, as well as the data itself.
    size_t total = sizeof( size_t );
    for( auto const& bv : bvs ) {
        total += serialized_bitvector_size( bv );
        // total += 2 * sizeof( size_t );
        // total += Bitvector::get_vector_size( bv.size() ) * sizeof( Bitvector::IntType );
    }
    EXPECT_EQ( out_str.size(), total );

    // Deserialize again
    std::istringstream in( out_str );
    Deserializer deser( from_stream( in ));
    std::vector<Bitvector> bvs_deser;
    deser >> bvs_deser;

    // Finally, compare
    EXPECT_EQ( bvs_deser, bvs );
}

// =================================================================================================
//     Arithmetic and Logic Operators
// =================================================================================================

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
    EXPECT_EQ( 6, pop_count(bv0) );
    EXPECT_EQ( 6, pop_count(bv1) );
    EXPECT_EQ( 8, pop_count(bv2) );

    // Test inverse as well, which also tests that the padding is 0
    EXPECT_EQ( 6, pop_count(~bv0) );
    EXPECT_EQ( 6, pop_count(~bv1) );
    EXPECT_EQ( 4, pop_count(~bv2) );

    // Test some inequality as well
    EXPECT_FALSE( bv0 == bv1 );
    EXPECT_FALSE( bv0 == bv2 );
    EXPECT_FALSE( bv1 == bv2 );
    EXPECT_TRUE(  bv0 != bv1 );
    EXPECT_TRUE(  bv0 != bv2 );
    EXPECT_TRUE(  bv1 != bv2 );
}

TEST( Bitvector, Operators )
{
    // Minimal set of vectors to test all combinations of bits.
    auto const bv_s = Bitvector( "0011" );
    auto const bv_l = Bitvector( "010101" );

    // Shorthands.
    auto const kExpectEqual = BitwiseOperatorLengthPolicy::kExpectEqual;
    auto const kUseShorter  = BitwiseOperatorLengthPolicy::kUseShorter;
    auto const kUseLonger   = BitwiseOperatorLengthPolicy::kUseLonger;
    auto const kUseFirst    = BitwiseOperatorLengthPolicy::kUseFirst;
    auto const kUseSecond   = BitwiseOperatorLengthPolicy::kUseSecond;

    // -----------------------------------
    //     and
    // -----------------------------------

    // and, expect equal length
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, kExpectEqual ));
    EXPECT_ANY_THROW(               bitwise_and( bv_l, bv_s, kExpectEqual ));
    EXPECT_ANY_THROW(               bitwise_and( bv_s, bv_l, kExpectEqual ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, kExpectEqual ));

    // and, using smaller
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, kUseShorter ));
    EXPECT_EQ( Bitvector("0001"),   bitwise_and( bv_l, bv_s, kUseShorter ));
    EXPECT_EQ( Bitvector("0001"),   bitwise_and( bv_s, bv_l, kUseShorter ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, kUseShorter ));

    // and, using larger
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, kUseLonger ));
    EXPECT_EQ( Bitvector("000100"), bitwise_and( bv_l, bv_s, kUseLonger ));
    EXPECT_EQ( Bitvector("000100"), bitwise_and( bv_s, bv_l, kUseLonger ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, kUseLonger ));

    // and, using first
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, kUseFirst ));
    EXPECT_EQ( Bitvector("000100"), bitwise_and( bv_l, bv_s, kUseFirst ));
    EXPECT_EQ( Bitvector("0001"),   bitwise_and( bv_s, bv_l, kUseFirst ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, kUseFirst ));

    // and, using second
    EXPECT_EQ( Bitvector("0011"),   bitwise_and( bv_s, bv_s, kUseSecond ));
    EXPECT_EQ( Bitvector("0001"),   bitwise_and( bv_l, bv_s, kUseSecond ));
    EXPECT_EQ( Bitvector("000100"), bitwise_and( bv_s, bv_l, kUseSecond ));
    EXPECT_EQ( Bitvector("010101"), bitwise_and( bv_l, bv_l, kUseSecond ));

    // -----------------------------------
    //     or
    // -----------------------------------

    // or, expect equal length
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, kExpectEqual ));
    EXPECT_ANY_THROW(               bitwise_or( bv_l, bv_s, kExpectEqual ));
    EXPECT_ANY_THROW(               bitwise_or( bv_s, bv_l, kExpectEqual ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, kExpectEqual ));

    // or, using smaller
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, kUseShorter ));
    EXPECT_EQ( Bitvector("0111"),   bitwise_or( bv_l, bv_s, kUseShorter ));
    EXPECT_EQ( Bitvector("0111"),   bitwise_or( bv_s, bv_l, kUseShorter ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, kUseShorter ));

    // or, using larger
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, kUseLonger ));
    EXPECT_EQ( Bitvector("011101"), bitwise_or( bv_l, bv_s, kUseLonger ));
    EXPECT_EQ( Bitvector("011101"), bitwise_or( bv_s, bv_l, kUseLonger ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, kUseLonger ));

    // or, using first
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, kUseFirst ));
    EXPECT_EQ( Bitvector("011101"), bitwise_or( bv_l, bv_s, kUseFirst ));
    EXPECT_EQ( Bitvector("0111"),   bitwise_or( bv_s, bv_l, kUseFirst ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, kUseFirst ));

    // or, using second
    EXPECT_EQ( Bitvector("0011"),   bitwise_or( bv_s, bv_s, kUseSecond ));
    EXPECT_EQ( Bitvector("0111"),   bitwise_or( bv_l, bv_s, kUseSecond ));
    EXPECT_EQ( Bitvector("011101"), bitwise_or( bv_s, bv_l, kUseSecond ));
    EXPECT_EQ( Bitvector("010101"), bitwise_or( bv_l, bv_l, kUseSecond ));

    // -----------------------------------
    //     xor
    // -----------------------------------

    // xor, expect equal length
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, kExpectEqual ));
    EXPECT_ANY_THROW(               bitwise_xor( bv_l, bv_s, kExpectEqual ));
    EXPECT_ANY_THROW(               bitwise_xor( bv_s, bv_l, kExpectEqual ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, kExpectEqual ));

    // xor, using smaller
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, kUseShorter ));
    EXPECT_EQ( Bitvector("0110"),   bitwise_xor( bv_l, bv_s, kUseShorter ));
    EXPECT_EQ( Bitvector("0110"),   bitwise_xor( bv_s, bv_l, kUseShorter ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, kUseShorter ));

    // xor, using larger
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, kUseLonger ));
    EXPECT_EQ( Bitvector("011001"), bitwise_xor( bv_l, bv_s, kUseLonger ));
    EXPECT_EQ( Bitvector("011001"), bitwise_xor( bv_s, bv_l, kUseLonger ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, kUseLonger ));

    // xor, using first
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, kUseFirst ));
    EXPECT_EQ( Bitvector("011001"), bitwise_xor( bv_l, bv_s, kUseFirst ));
    EXPECT_EQ( Bitvector("0110"),   bitwise_xor( bv_s, bv_l, kUseFirst ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, kUseFirst ));

    // xor, using second
    EXPECT_EQ( Bitvector("0000"),   bitwise_xor( bv_s, bv_s, kUseSecond ));
    EXPECT_EQ( Bitvector("0110"),   bitwise_xor( bv_l, bv_s, kUseSecond ));
    EXPECT_EQ( Bitvector("011001"), bitwise_xor( bv_s, bv_l, kUseSecond ));
    EXPECT_EQ( Bitvector("000000"), bitwise_xor( bv_l, bv_l, kUseSecond ));
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

// =================================================================================================
//     Set Operators
// =================================================================================================

TEST( Bitvector, JaccardIndex )
{
    auto const p1 = Bitvector( "10111" );
    auto const p2 = Bitvector( "10011" );
    EXPECT_EQ(       3.0 / 4.0, jaccard_similarity( p1, p2 ));
    EXPECT_EQ( 1.0 - 3.0 / 4.0, jaccard_distance(   p1, p2 ));
}

TEST( Bitvector, JaccardIndexSpeed )
{
    std::srand(std::time(nullptr));

    // With completely random bitvectors, we expect the Jaccard similarity to be 1/3.
    // Additionally, as we do self-comparisons above, there is always one comparsion with value 1.
    // So, per outer loop iteration, we add 9*1/3 and 1*1 = 4 to the sum, for a total of ~40.
    // size_t const s = 5368709120;
    // size_t const n = 10;

    // Alternative, with some shorter vectors.
    size_t const s = 2000;
    size_t const n = 1000;

    // LOG_TIME << "make bvs";
    auto bvs = std::vector<Bitvector>( n );
    for( size_t i = 0; i < n; ++i ) {
        bvs[i] = make_random_bitvector( s );
    }

    // LOG_TIME << "compute jaccard";
    double sum = 0.0;
    size_t cnt = 0;
    Timer timer;
    timer.start();
    for( size_t i = 0; i < n; ++i ) {
        // LOG_MSG << i;
        for( size_t j = 0; j < n; ++j ) {
            auto const js = jaccard_similarity( bvs[i], bvs[j] );
            EXPECT_LE( js, 1.0 );
            EXPECT_GE( js, 0.0 );
            // LOG_DBG1 << "at " << i << ", " << j << " with " << js;
            sum += js;
            ++cnt;
        }
    }
    timer.stop();

    // LOG_TIME << "done " << cnt << " comps with sum " << sum;
    LOG_MSG << "time:  " << timer.elapsed() << " s, sum = " << sum;
    LOG_MSG << "speed: " << ( static_cast<double>(cnt) / timer.elapsed() ) << " comp/s";
    LOG_MSG << "speed: " << ( static_cast<double>(cnt*s) / timer.elapsed() ) << " bitops/s";
}

TEST( Bitvector, HammingDistance )
{
    auto const p1 = Bitvector( "10110" );
    auto const p2 = Bitvector( "10011" );
    EXPECT_EQ( 2, hamming_distance( p1, p2 ));
}

TEST( Bitvector, SetOperators )
{
    // Set operators
    auto const p1 = Bitvector( "0011" );
    auto const p2 = Bitvector( "0101" );
    EXPECT_EQ( Bitvector( "0010" ), set_minus( p1, p2 ));
    EXPECT_EQ( Bitvector( "0110" ), symmetric_difference( p1, p2 ));
}

TEST( Bitvector, StrictSubset )
{
    // 1. Both vectors empty (identical): not a strict subset.
    EXPECT_FALSE( is_strict_subset( Bitvector( "0000" ), Bitvector( "0000" )));

    // 2. Empty bitvector is a strict subset of a non-empty bitvector.
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0000" ), Bitvector( "1111" )));

    // 3. Single bit cases.
    // Empty set is a strict subset of a one-element set.
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0" ), Bitvector( "1" )));
    // Identical one-element sets are not strict.
    EXPECT_FALSE( is_strict_subset( Bitvector( "1" ), Bitvector( "1" )));

    // 4. Typical cases where one extra bit distinguishes the superset.
    EXPECT_TRUE(  is_strict_subset( Bitvector( "1010" ), Bitvector( "1011" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0010" ), Bitvector( "1010" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "1110" ), Bitvector( "1111" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0100" ), Bitvector( "1100" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0001" ), Bitvector( "1001" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0010" ), Bitvector( "1011" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "0101" ), Bitvector( "0111" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "1010" ), Bitvector( "1110" )));
    EXPECT_TRUE(  is_strict_subset( Bitvector( "1000" ), Bitvector( "1010" )));

    // 5. Cases where the left bitvector has a 1 that the right does not.
    EXPECT_FALSE( is_strict_subset( Bitvector( "1011" ), Bitvector( "1010" )));
    EXPECT_FALSE( is_strict_subset( Bitvector( "1010" ), Bitvector( "0101" )));
    EXPECT_FALSE( is_strict_subset( Bitvector( "1001" ), Bitvector( "1110" )));
    EXPECT_FALSE( is_strict_subset( Bitvector( "0100" ), Bitvector( "0010" )));

    // 6. Equality cases are not considered strict subsets.
    EXPECT_FALSE( is_strict_subset( Bitvector( "1100" ), Bitvector( "1100" )));
}

TEST( Bitvector, Subset )
{
    // 1. Both vectors empty: an empty set is a subset of itself.
    EXPECT_TRUE(  is_subset( Bitvector( "0000" ), Bitvector( "0000" )));

    // 2. Empty bitvector is a subset of any bitvector with at least one bit set.
    EXPECT_TRUE(  is_subset( Bitvector( "0000" ), Bitvector( "1111" )));

    // 3. Single bit cases.
    // Empty set is a subset.
    EXPECT_TRUE(  is_subset( Bitvector( "0" ), Bitvector( "1" )));
    // Equal one-element sets are subsets.
    EXPECT_TRUE(  is_subset( Bitvector( "1" ), Bitvector( "1" )));

    // 4. Typical cases where the left bitvector is a subset of the right.
    EXPECT_TRUE(  is_subset( Bitvector( "1010" ), Bitvector( "1011" )));
    EXPECT_TRUE(  is_subset( Bitvector( "0010" ), Bitvector( "1010" )));
    EXPECT_TRUE(  is_subset( Bitvector( "1110" ), Bitvector( "1111" )));
    EXPECT_TRUE(  is_subset( Bitvector( "0100" ), Bitvector( "1100" )));
    EXPECT_TRUE(  is_subset( Bitvector( "0001" ), Bitvector( "1001" )));
    EXPECT_TRUE(  is_subset( Bitvector( "0010" ), Bitvector( "1011" )));
    EXPECT_TRUE(  is_subset( Bitvector( "0101" ), Bitvector( "0111" )));
    EXPECT_TRUE(  is_subset( Bitvector( "1010" ), Bitvector( "1110" )));
    EXPECT_TRUE(  is_subset( Bitvector( "1000" ), Bitvector( "1010" )));

    // 5. Cases where the left bitvector has a 1 that is not present in the right.
    EXPECT_FALSE( is_subset( Bitvector( "1011" ), Bitvector( "1010" )));
    EXPECT_FALSE( is_subset( Bitvector( "1010" ), Bitvector( "0101" )));
    EXPECT_FALSE( is_subset( Bitvector( "1001" ), Bitvector( "1110" )));
    EXPECT_FALSE( is_subset( Bitvector( "0100" ), Bitvector( "0010" )));

    // 6. Equality cases are valid subsets.
    EXPECT_TRUE(  is_subset( Bitvector( "1100" ), Bitvector( "1100" )));
}

TEST( Bitvector, SubsetSpeed )
{
    std::srand(std::time(nullptr));

    // Large values for a proper test.
    // size_t const s = 5368709120;
    // size_t const n = 10;

    // Alternative, shorter for normal tests.
    size_t const s = 2000;
    size_t const n = 1000;

    Timer timer;
    timer.start();
    size_t cnt = 0;

    // Make some random bitvectors
    LOG_MSG << "make bvs";
    auto bvs = std::vector<Bitvector>( n );
    for( size_t i = 0; i < n; ++i ) {
        bvs[i] = make_random_bitvector( s );
    }
    LOG_MSG << "make bvs:     " << timer.elapsed_and_restart() << " s";

    // Test the subset speed
    LOG_MSG << "comp subset";
    size_t subset_cnt = 0;
    for( size_t i = 0; i < n; ++i ) {
        // LOG_MSG << i;
        for( size_t j = 0; j < n; ++j ) {
            subset_cnt += is_subset( bvs[i], bvs[j] );
            ++cnt;
        }
    }
    LOG_MSG << "comp subset: " << timer.elapsed_and_restart() << " s";

    // Test the strict subset speed
    LOG_MSG << "comp strict";
    size_t strict_cnt = 0;
    for( size_t i = 0; i < n; ++i ) {
        // LOG_MSG << i;
        for( size_t j = 0; j < n; ++j ) {
            strict_cnt += is_strict_subset( bvs[i], bvs[j] );
        }
    }
    LOG_MSG << "comp strict: " << timer.elapsed_and_stop() << " s";
    LOG_MSG << "subset: " << subset_cnt << ", strict: " << strict_cnt;

    // We can basically guarantee that we have only subsets from comparing vectors
    // to themselves, and no strict subsets. The case where there are two exactly
    // identical vectors in our randomly generated ones is negligible.
    EXPECT_EQ( n, subset_cnt );
    EXPECT_EQ( 0, strict_cnt );

    // LOG_MSG << "time:  " << timer.elapsed() << " s";
    // LOG_MSG << "speed: " << ( static_cast<double>(cnt) / timer.elapsed() ) << " comp/s";
    // LOG_MSG << "speed: " << ( static_cast<double>(cnt*s) / timer.elapsed() ) << " bitops/s";
}

// =================================================================================================
//     Count Operators
// =================================================================================================

TEST( Bitvector, CountRange )
{
    // 0 word
    Bitvector const bv_0( 0, true );
    EXPECT_ANY_THROW( pop_count( bv_0,  0,  0 ));
    EXPECT_ANY_THROW( pop_count( bv_0,  0,  1 ));
    EXPECT_ANY_THROW( pop_count( bv_0,  1,  1 ));
    EXPECT_ANY_THROW( pop_count( bv_0,  1,  0 ));

    // 0.5 word
    Bitvector const bv_32( 32, true );
    EXPECT_EQ( 0, pop_count( bv_32, 0, 0 ));
    EXPECT_EQ( 0, pop_count( bv_32, 1, 1 ));
    EXPECT_EQ( 1, pop_count( bv_32, 0, 1 ));
    EXPECT_EQ( 1, pop_count( bv_32, 31, 32 ));
    EXPECT_EQ( 32, pop_count( bv_32, 0, 32 ));

    EXPECT_ANY_THROW( pop_count( bv_32,  1,  0 ));
    EXPECT_ANY_THROW( pop_count( bv_32,  0, 33 ));
    EXPECT_ANY_THROW( pop_count( bv_32, 33, 33 ));

    // 1 word
    Bitvector const bv_64( 64, true );
    EXPECT_EQ( 0, pop_count( bv_64, 0, 0 ));
    EXPECT_EQ( 0, pop_count( bv_64, 1, 1 ));
    EXPECT_EQ( 1, pop_count( bv_64, 0, 1 ));
    EXPECT_EQ( 1, pop_count( bv_64, 63, 64 ));
    EXPECT_EQ( 64, pop_count( bv_64, 0, 64 ));

    EXPECT_ANY_THROW( pop_count( bv_64,  1,  0 ));
    EXPECT_ANY_THROW( pop_count( bv_64,  0, 65 ));
    EXPECT_ANY_THROW( pop_count( bv_64, 65, 64 ));

    // 1.5 word
    Bitvector const bv_96( 96, true );
    EXPECT_EQ( 0, pop_count( bv_96, 0, 0 ));
    EXPECT_EQ( 0, pop_count( bv_96, 1, 1 ));
    EXPECT_EQ( 1, pop_count( bv_96, 0, 1 ));
    EXPECT_EQ( 1, pop_count( bv_96, 95, 96 ));
    EXPECT_EQ( 96, pop_count( bv_96, 0, 96 ));

    EXPECT_ANY_THROW( pop_count( bv_96,  1,  0 ));
    EXPECT_ANY_THROW( pop_count( bv_96,  0, 97 ));
    EXPECT_ANY_THROW( pop_count( bv_96, 97, 97 ));

    // 2.5 word
    Bitvector const bv_160( 160, true );
    EXPECT_EQ( 0, pop_count( bv_160, 0, 0 ));
    EXPECT_EQ( 0, pop_count( bv_160, 1, 1 ));
    EXPECT_EQ( 1, pop_count( bv_160, 0, 1 ));
    EXPECT_EQ( 1, pop_count( bv_160, 159, 160 ));
    EXPECT_EQ( 160, pop_count( bv_160, 0, 160 ));

    EXPECT_ANY_THROW( pop_count( bv_160,   1,  0 ));
    EXPECT_ANY_THROW( pop_count( bv_160,   0, 161 ));
    EXPECT_ANY_THROW( pop_count( bv_160, 161, 161 ));
}

TEST( Bitvector, CountRangeFuzzy )
{
    std::srand(std::time(nullptr));

    size_t const max_size = 1024;
    for( size_t n = 0; n < 50000; ++n ) {

        // Size of the bitvector
        size_t const size = std::rand() % max_size;

        // Edge case. Nothing to test.
        if( size == 0 ) {
            continue;
        }

        // Get some random bits
        auto const bv = make_random_bitvector( size );

        // Get random positions between which to count.
        size_t s = std::rand() % size;
        size_t e = std::rand() % (size + 1);
        if( s > e ) {
            std::swap( s, e );
        }
        ASSERT_LT( s, size );
        ASSERT_LE( e, size );
        ASSERT_LE( s, e );

        // Get the count of bits between the two.
        auto const cnt = pop_count( bv, s, e );

        // Same, but slow, for comparison.
        size_t exp = 0;
        for( size_t i = s; i < e; ++i ) {
            if( bv.get( i )) {
                ++exp;
            }
        }

        EXPECT_EQ( exp, cnt ) << "first: " << s << ", last: " << e << ", bv: " << bv;
        // LOG_DBG << "first: " << s << ", last: " << e << ", count: " << cnt << ", bv: " << bv;
    }
}

// =================================================================================================
//     Find Operators
// =================================================================================================

TEST( Bitvector, AnyAllSetUnset )
{
    // Tests for all_set()
    EXPECT_TRUE( all_set( Bitvector( "1111" )));
    EXPECT_TRUE( all_set( Bitvector( "1" )));
    EXPECT_FALSE( all_set( Bitvector( "1011" )));
    EXPECT_FALSE( all_set( Bitvector( "1101" )));
    EXPECT_FALSE( all_set( Bitvector( "0" )));
    EXPECT_FALSE( all_set( Bitvector( "0111" )));

    // Tests for all_unset()
    EXPECT_TRUE( all_unset( Bitvector( "0000" )));
    EXPECT_TRUE( all_unset( Bitvector( "0" )));
    EXPECT_FALSE( all_unset( Bitvector( "0100" )));
    EXPECT_FALSE( all_unset( Bitvector( "1000" )));
    EXPECT_FALSE( all_unset( Bitvector( "0010" )));
    EXPECT_FALSE( all_unset( Bitvector( "1010" )));

    // Tests for any_set()
    EXPECT_FALSE( any_set( Bitvector( "0000" )));
    EXPECT_FALSE( any_set( Bitvector( "0")));
    EXPECT_TRUE( any_set( Bitvector( "0100" )));
    EXPECT_TRUE( any_set( Bitvector( "1000" )));
    EXPECT_TRUE( any_set( Bitvector( "1010" )));
    EXPECT_TRUE( any_set( Bitvector( "0001" )));

    // Tests for any_unset()
    EXPECT_FALSE( any_unset( Bitvector( "1111" )));
    EXPECT_FALSE( any_unset( Bitvector( "1" )));
    EXPECT_TRUE( any_unset( Bitvector( "1101" )));
    EXPECT_TRUE( any_unset( Bitvector( "1010" )));
    EXPECT_TRUE( any_unset( Bitvector( "0111" )));
    EXPECT_TRUE( any_unset( Bitvector( "1110" )));

    // Tests for a default constructed (size 0) Bitvector.
    // With no bits, "all" predicates are vacuously true.
    EXPECT_TRUE( all_set( Bitvector() ));
    EXPECT_TRUE( all_unset( Bitvector() ));
    // "Any" predicates are false because there are no bits to be set/unset.
    EXPECT_FALSE( any_set( Bitvector() ));
    EXPECT_FALSE( any_unset( Bitvector() ));
}

TEST( Bitvector, AnyAllSetUnsetLargeSizes )
{
    // Tests for Bitvectors of larger sizes, including those with word boundary sizes, as that
    // is where the padding is important. We also want sizes that include initial and middle words.

    for (size_t n = 1; n <= 256; n++) {
        // LOG_MSG << "Size = " << std::to_string(n);

        // 1. All zeros.
        std::string zeros(n, '0');
        Bitvector bvZeros(zeros);
        EXPECT_FALSE(all_set(bvZeros));   // Not all bits are 1.
        EXPECT_TRUE(all_unset(bvZeros));    // All bits are 0.
        EXPECT_FALSE(any_set(bvZeros));     // No bit is 1.
        EXPECT_TRUE(any_unset(bvZeros));    // At least one bit is 0.

        // 2. All ones.
        std::string ones(n, '1');
        Bitvector bvOnes(ones);
        EXPECT_TRUE(all_set(bvOnes));       // All bits are 1.
        EXPECT_FALSE(all_unset(bvOnes));      // Not all bits are 0.
        EXPECT_TRUE(any_set(bvOnes));         // At least one bit is 1.
        EXPECT_FALSE(any_unset(bvOnes));      // No bit is 0.

        // 3. Alternating pattern: "0", "1", "0", "1", ...
        std::string alt;
        alt.reserve(n);
        for (size_t i = 0; i < n; i++) {
            alt.push_back((i % 2 == 0) ? '0' : '1');
        }
        Bitvector bvAlt(alt);
        if (n == 1) {
            // When n == 1, the alternating pattern yields "0"
            EXPECT_FALSE(all_set(bvAlt));
            EXPECT_TRUE(all_unset(bvAlt));
            EXPECT_FALSE(any_set(bvAlt));
            EXPECT_TRUE(any_unset(bvAlt));
        } else {
            // For n >= 2, the pattern contains both 0's and 1's.
            EXPECT_FALSE(all_set(bvAlt));
            EXPECT_FALSE(all_unset(bvAlt));
            EXPECT_TRUE(any_set(bvAlt));
            EXPECT_TRUE(any_unset(bvAlt));
        }

        // 4. Only the first bit set.
        {
            std::string first(n, '0');
            first[0] = '1';
            Bitvector bvFirst(first);
            if (n == 1) {
                // For a single-bit bitvector "1"
                EXPECT_TRUE(all_set(bvFirst));
                EXPECT_FALSE(all_unset(bvFirst));
                EXPECT_TRUE(any_set(bvFirst));
                EXPECT_FALSE(any_unset(bvFirst));
            } else {
                // For n > 1, only the first bit is set.
                EXPECT_FALSE(all_set(bvFirst));
                EXPECT_FALSE(all_unset(bvFirst));
                EXPECT_TRUE(any_set(bvFirst));
                EXPECT_TRUE(any_unset(bvFirst));
            }
        }

        // 5. Only the last bit set.
        {
            std::string last(n, '0');
            last[n - 1] = '1';
            Bitvector bvLast(last);
            if (n == 1) {
                // For a single-bit bitvector "1"
                EXPECT_TRUE(all_set(bvLast));
                EXPECT_FALSE(all_unset(bvLast));
                EXPECT_TRUE(any_set(bvLast));
                EXPECT_FALSE(any_unset(bvLast));
            } else {
                EXPECT_FALSE(all_set(bvLast));
                EXPECT_FALSE(all_unset(bvLast));
                EXPECT_TRUE(any_set(bvLast));
                EXPECT_TRUE(any_unset(bvLast));
            }
        }
    }
}

TEST( Bitvector, FindNextSet )
{
    auto const max = std::numeric_limits<size_t>::max();

    // 0 word
    Bitvector bv_0( 0 );
    EXPECT_EQ( max, find_next_set( bv_0, 0 ));
    EXPECT_EQ( max, find_next_set( bv_0, 1 ));

    // 0.5 word
    Bitvector bv_32( 32 );
    bv_32.set( 0 );
    bv_32.set( 1 );
    bv_32.set( 16 );
    bv_32.set( 31 );
    EXPECT_EQ(   0, find_next_set( bv_32,  0 ));
    EXPECT_EQ(   1, find_next_set( bv_32,  1 ));
    EXPECT_EQ(  16, find_next_set( bv_32,  2 ));
    EXPECT_EQ(  16, find_next_set( bv_32, 15 ));
    EXPECT_EQ(  16, find_next_set( bv_32, 16 ));
    EXPECT_EQ(  31, find_next_set( bv_32, 17 ));
    EXPECT_EQ(  31, find_next_set( bv_32, 30 ));
    EXPECT_EQ(  31, find_next_set( bv_32, 31 ));
    EXPECT_EQ( max, find_next_set( bv_32, 32 ));
    EXPECT_EQ( max, find_next_set( bv_32, 63 ));
    EXPECT_EQ( max, find_next_set( bv_32, 64 ));

    // 1 word
    Bitvector bv_64( 64 );
    bv_64.set( 63 );
    EXPECT_EQ(  63, find_next_set( bv_64, 0 ));
    EXPECT_EQ(  63, find_next_set( bv_64, 62 ));
    EXPECT_EQ(  63, find_next_set( bv_64, 63 ));
    EXPECT_EQ( max, find_next_set( bv_64, 64 ));

    // 1.5 word
    Bitvector bv_96( 96 );
    bv_96.set( 64 );
    bv_96.set( 95 );
    EXPECT_EQ(  64, find_next_set( bv_96, 0 ));
    EXPECT_EQ(  64, find_next_set( bv_96, 63 ));
    EXPECT_EQ(  64, find_next_set( bv_96, 64 ));
    EXPECT_EQ(  95, find_next_set( bv_96, 65 ));
    EXPECT_EQ(  95, find_next_set( bv_96, 95 ));
    EXPECT_EQ( max, find_next_set( bv_96, 96 ));

    // 2.5 word
    Bitvector bv_160( 160 );
    bv_160.set( 63 );
    bv_160.set( 130 );
    EXPECT_EQ(  63, find_next_set( bv_160, 0 ));
    EXPECT_EQ(  63, find_next_set( bv_160, 63 ));
    EXPECT_EQ( 130, find_next_set( bv_160, 64 ));
    EXPECT_EQ( 130, find_next_set( bv_160, 129 ));
    EXPECT_EQ( 130, find_next_set( bv_160, 130 ));
    EXPECT_EQ( max, find_next_set( bv_160, 131 ));
}

TEST( Bitvector, FindNextSetFuzzy )
{
    auto const seed = std::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_DBG << "seed " << seed;

    // Helper to find the smallest number in the vector
    // that is greater than or equal to the given number.
    auto find_next_set_val_ = []( std::vector<size_t> const& vec, size_t val ){
        auto it = std::lower_bound( vec.begin(), vec.end(), val );

        // We always have the max number in the vector, so we always find one.
        EXPECT_TRUE( it != vec.end() );
        return *it;
    };

    // Run a bunch of randomized tests
    size_t const max_size = 1024;
    for( size_t n = 0; n < 5000; ++n ) {

        // Size of the bitvector
        size_t const size = permuted_congruential_generator( 1, max_size );
        auto bv = Bitvector( size );

        // Our vector has up to 1024 bits, and we populate up to 32 bits of them,
        // so that we have test cases where more than one bit is set per word of the bitvector.
        auto const num_bits = permuted_congruential_generator( std::min<size_t>( 32, size ));
        auto selection = select_without_replacement( num_bits, size );
        std::sort( selection.begin(), selection.end() );

        // Now set the bits
        for( auto b : selection ) {
            bv.set(b);
        }

        // LOG_DBG << "list[" << size << "]: " << join(selection);
        // LOG_DBG << "bits: " << bv.dump();

        // We also add the max, so that we always find something in the vector to match.
        selection.push_back( std::numeric_limits<size_t>::max() );

        // Our test now is to go through the whole vector, and test every position.
        // We keep track of the index from our ground truth, so that we do not need to search
        // for it in every iteration.
        size_t next_in_vec = find_next_set_val_( selection, 0 );
        for( size_t i = 0; i < size; ++i ) {
            if( i > next_in_vec ) {
                next_in_vec = find_next_set_val_( selection, i );
            }

            EXPECT_EQ( next_in_vec, find_next_set( bv, i )) << " at " << i;
        }
        EXPECT_EQ( std::numeric_limits<size_t>::max(), find_next_set( bv, size ));
        EXPECT_EQ( std::numeric_limits<size_t>::max(), find_next_set( bv, size + 1 ));
    }
}

TEST( Bitvector, FindFirstLastSet )
{
    // Test different lengths of bitvectors
    for( size_t l = 0; l <= 1024; ++l ) {
        // Test no bit being set
        auto bv = Bitvector( l );
        EXPECT_EQ( Bitvector::npos, find_first_set( bv ));
        EXPECT_EQ( Bitvector::npos, find_last_set( bv ));

        // Test exactly one bit being set, for all bits.
        for( size_t s = 0; s < l; ++s ) {
            // LOG_DBG << "l==" << l << " s==" << s;
            auto bv = Bitvector( l );
            bv.set( s );
            EXPECT_EQ( s, find_first_set( bv ));
            EXPECT_EQ( s, find_last_set( bv ));
        }
    }
}

// =================================================================================================
//     Modifiers
// =================================================================================================

TEST( Bitvector, SetRange )
{
    // We do an exhaustive test, because why not.
    // We tested up to 1024, which takes some minutes,
    // but in the normal case, it should suffice to test fewer,
    // as long as we have cases across word boundaries, with words in the middle, etc
    for( size_t s = 0; s < 256; ++s ) {
        // if( s % 100 == 0 ) {
        //     LOG_DBG << "at " << s;
        // }
        for( size_t f = 0; f < s; ++f ) {
            for( size_t l = f; l <= s; ++l ) {
                // Set true
                {
                    // Use the function to test
                    auto bv = Bitvector( s, false );
                    bv.set_range( f, l, true );

                    // Make expected version using slow setter
                    auto ex = Bitvector( s, false );
                    for( size_t i = f; i < l; ++i ) {
                        ex.set( i, true );
                    }

                    // Now test
                    if( ex != bv ) {
                        LOG_DBG << "s==" << s << " f==" << f << " l==" << l;
                        LOG_DBG << "bv==" << bv;
                        LOG_DBG << "ex==" << ex;
                    }
                    EXPECT_EQ( ex, bv );
                }

                // Set false
                {
                    // Use the function to test
                    auto bv = Bitvector( s, true );
                    bv.set_range( f, l, false );

                    // Make expected version using slow setter
                    auto ex = Bitvector( s, true );
                    for( size_t i = f; i < l; ++i ) {
                        ex.set( i, false );
                    }

                    // Now test
                    if( ex != bv ) {
                        LOG_DBG << "s==" << s << " f==" << f << " l==" << l;
                        LOG_DBG << "bv==" << bv;
                        LOG_DBG << "ex==" << ex;
                    }
                    EXPECT_EQ( ex, bv );
                }
            }
        }
    }
}
