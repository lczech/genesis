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

#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/helper.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>

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
                    bv.set( f, l, true );

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
                    bv.set( f, l, false );

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

TEST( Bitvector, CountRange )
{
    // 0 word
    Bitvector const bv_0( 0, true );
    EXPECT_ANY_THROW( bv_0.count(  0,  0 ));
    EXPECT_ANY_THROW( bv_0.count(  0,  1 ));
    EXPECT_ANY_THROW( bv_0.count(  1,  1 ));
    EXPECT_ANY_THROW( bv_0.count(  1,  0 ));

    // 0.5 word
    Bitvector const bv_32( 32, true );
    EXPECT_EQ( 0, bv_32.count( 0, 0 ));
    EXPECT_EQ( 0, bv_32.count( 1, 1 ));
    EXPECT_EQ( 1, bv_32.count( 0, 1 ));
    EXPECT_EQ( 1, bv_32.count( 31, 32 ));
    EXPECT_EQ( 32, bv_32.count( 0, 32 ));

    EXPECT_ANY_THROW( bv_32.count(  1,  0 ));
    EXPECT_ANY_THROW( bv_32.count(  0, 33 ));
    EXPECT_ANY_THROW( bv_32.count( 33, 33 ));

    // 1 word
    Bitvector const bv_64( 64, true );
    EXPECT_EQ( 0, bv_64.count( 0, 0 ));
    EXPECT_EQ( 0, bv_64.count( 1, 1 ));
    EXPECT_EQ( 1, bv_64.count( 0, 1 ));
    EXPECT_EQ( 1, bv_64.count( 63, 64 ));
    EXPECT_EQ( 64, bv_64.count( 0, 64 ));

    EXPECT_ANY_THROW( bv_64.count(  1,  0 ));
    EXPECT_ANY_THROW( bv_64.count(  0, 65 ));
    EXPECT_ANY_THROW( bv_64.count( 65, 64 ));

    // 1.5 word
    Bitvector const bv_96( 96, true );
    EXPECT_EQ( 0, bv_96.count( 0, 0 ));
    EXPECT_EQ( 0, bv_96.count( 1, 1 ));
    EXPECT_EQ( 1, bv_96.count( 0, 1 ));
    EXPECT_EQ( 1, bv_96.count( 95, 96 ));
    EXPECT_EQ( 96, bv_96.count( 0, 96 ));

    EXPECT_ANY_THROW( bv_96.count(  1,  0 ));
    EXPECT_ANY_THROW( bv_96.count(  0, 97 ));
    EXPECT_ANY_THROW( bv_96.count( 97, 97 ));

    // 2.5 word
    Bitvector const bv_160( 160, true );
    EXPECT_EQ( 0, bv_160.count( 0, 0 ));
    EXPECT_EQ( 0, bv_160.count( 1, 1 ));
    EXPECT_EQ( 1, bv_160.count( 0, 1 ));
    EXPECT_EQ( 1, bv_160.count( 159, 160 ));
    EXPECT_EQ( 160, bv_160.count( 0, 160 ));

    EXPECT_ANY_THROW( bv_160.count(   1,  0 ));
    EXPECT_ANY_THROW( bv_160.count(   0, 161 ));
    EXPECT_ANY_THROW( bv_160.count( 161, 161 ));
}

TEST( Bitvector, CountRangeFuzzy )
{
    std::srand(std::time(nullptr));

    size_t const max_size = 1024;
    for( size_t n = 0; n < 50000; ++n ) {

        // Size of the bitvector
        size_t const size = std::rand() % max_size;
        auto bv = Bitvector( size );

        // Edge case. Nothing to test.
        if( size == 0 ) {
            continue;
        }

        // Set some random bits
        for( size_t i = 0; i < size; ++i ) {
            auto const p = ( std::rand() % size );
            bv.flip( p );
        }
        // LOG_DBG << bv;

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
        auto const cnt = bv.count( s, e );

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

TEST( Bitvector, FindNextSet )
{
    auto const max = std::numeric_limits<size_t>::max();

    // 0 word
    Bitvector bv_0( 0 );
    EXPECT_EQ( max, bv_0.find_next_set( 0 ));
    EXPECT_EQ( max, bv_0.find_next_set( 1 ));

    // 0.5 word
    Bitvector bv_32( 32 );
    bv_32.set( 0 );
    bv_32.set( 1 );
    bv_32.set( 16 );
    bv_32.set( 31 );
    EXPECT_EQ(   0, bv_32.find_next_set(  0 ));
    EXPECT_EQ(   1, bv_32.find_next_set(  1 ));
    EXPECT_EQ(  16, bv_32.find_next_set(  2 ));
    EXPECT_EQ(  16, bv_32.find_next_set( 15 ));
    EXPECT_EQ(  16, bv_32.find_next_set( 16 ));
    EXPECT_EQ(  31, bv_32.find_next_set( 17 ));
    EXPECT_EQ(  31, bv_32.find_next_set( 30 ));
    EXPECT_EQ(  31, bv_32.find_next_set( 31 ));
    EXPECT_EQ( max, bv_32.find_next_set( 32 ));
    EXPECT_EQ( max, bv_32.find_next_set( 63 ));
    EXPECT_EQ( max, bv_32.find_next_set( 64 ));

    // 1 word
    Bitvector bv_64( 64 );
    bv_64.set( 63 );
    EXPECT_EQ(  63, bv_64.find_next_set( 0 ));
    EXPECT_EQ(  63, bv_64.find_next_set( 62 ));
    EXPECT_EQ(  63, bv_64.find_next_set( 63 ));
    EXPECT_EQ( max, bv_64.find_next_set( 64 ));

    // 1.5 word
    Bitvector bv_96( 96 );
    bv_96.set( 64 );
    bv_96.set( 95 );
    EXPECT_EQ(  64, bv_96.find_next_set( 0 ));
    EXPECT_EQ(  64, bv_96.find_next_set( 63 ));
    EXPECT_EQ(  64, bv_96.find_next_set( 64 ));
    EXPECT_EQ(  95, bv_96.find_next_set( 65 ));
    EXPECT_EQ(  95, bv_96.find_next_set( 95 ));
    EXPECT_EQ( max, bv_96.find_next_set( 96 ));

    // 2.5 word
    Bitvector bv_160( 160 );
    bv_160.set( 63 );
    bv_160.set( 130 );
    EXPECT_EQ(  63, bv_160.find_next_set( 0 ));
    EXPECT_EQ(  63, bv_160.find_next_set( 63 ));
    EXPECT_EQ( 130, bv_160.find_next_set( 64 ));
    EXPECT_EQ( 130, bv_160.find_next_set( 129 ));
    EXPECT_EQ( 130, bv_160.find_next_set( 130 ));
    EXPECT_EQ( max, bv_160.find_next_set( 131 ));
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

            EXPECT_EQ( next_in_vec, bv.find_next_set(i) ) << " at " << i;
        }
        EXPECT_EQ( std::numeric_limits<size_t>::max(), bv.find_next_set( size ));
        EXPECT_EQ( std::numeric_limits<size_t>::max(), bv.find_next_set( size + 1 ));
    }
}
