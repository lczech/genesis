/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2026 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied wbpvecanty of
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

#include "genesis/util/container/bitpacked_pair_vector.hpp"

#include <climits>
#include <vector>
#include <limits>
#include <utility>
#include <vector>

using namespace genesis::util;
using namespace genesis::util::core;
using namespace genesis::util::container;

// =================================================================================================
//     Helpers
// =================================================================================================

namespace
{

// size_t const bitpacked_pair_vector_performance_n_ = 10000000000; // 60s per test
// size_t const bitpacked_pair_vector_performance_n_ = 1000000000; // 6s per test
size_t const bitpacked_pair_vector_performance_n_ = 10000000; // quick

template<typename U>
U pair_mask_( size_t bits )
{
    constexpr size_t U_BITS = std::numeric_limits<U>::digits;
    EXPECT_GT( bits, 0 );
    EXPECT_LE( bits, U_BITS );
    return bits == U_BITS ? ~U{0} : (( U{1} << bits ) - 1 );
}

template<typename U, typename A, typename B>
void test_bitpacked_pair_vector_range_( size_t width_a, size_t width_b )
{
    size_t const n = 100000;

    BitpackedPairVector<U, A, B> bpvec( n, width_a, width_b );

    U const mask_a = pair_mask_<U>( width_a );
    U const mask_b = pair_mask_<U>( width_b );

    for( size_t i = 0; i < n; ++i ) {
        A a = static_cast<A>(( static_cast<U>( i * 17u ) + 3u ) & mask_a );
        B b = static_cast<B>(( static_cast<U>( i * 29u ) + 5u ) & mask_b );
        bpvec.set_unchecked( i, a, b );
    }

    for( size_t i = 0; i < n; ++i ) {
        A expected_a = static_cast<A>(( static_cast<U>( i * 17u ) + 3u ) & mask_a );
        B expected_b = static_cast<B>(( static_cast<U>( i * 29u ) + 5u ) & mask_b );

        auto value = bpvec.at( i );
        EXPECT_EQ( value.first, expected_a );
        EXPECT_EQ( value.second, expected_b );

        auto unchecked = bpvec[i];
        EXPECT_EQ( unchecked.first, expected_a );
        EXPECT_EQ( unchecked.second, expected_b );

        EXPECT_EQ( bpvec.first_at( i ), expected_a );
        EXPECT_EQ( bpvec.second_at( i ), expected_b );
        EXPECT_EQ( bpvec.first_unchecked( i ), expected_a );
        EXPECT_EQ( bpvec.second_unchecked( i ), expected_b );
    }
}

template<typename U, typename A, typename B>
void test_bitpacked_pair_vector_performance_( size_t width_a, size_t width_b )
{
    BitpackedPairVector<U, A, B> bpvec( bitpacked_pair_vector_performance_n_, width_a, width_b );
    LOG_DBG << "packed data size: " << bpvec.storage().data().size();

    U const mask_a = pair_mask_<U>( width_a );
    U const mask_b = pair_mask_<U>( width_b );

    LOG_TIME << "start";
    for( size_t i = 0; i < bpvec.size(); ++i ) {
        A a = static_cast<A>(( static_cast<U>( i * 17u ) + 3u ) & mask_a );
        B b = static_cast<B>(( static_cast<U>( i * 29u ) + 5u ) & mask_b );
        bpvec.set_at( i, a, b );
    }
    LOG_TIME << "set";

    for( size_t i = 0; i < bpvec.size(); ++i ) {
        A expected_a = static_cast<A>(( static_cast<U>( i * 17u ) + 3u ) & mask_a );
        B expected_b = static_cast<B>(( static_cast<U>( i * 29u ) + 5u ) & mask_b );
        auto value = bpvec.at( i );
        EXPECT_EQ( value.first, expected_a );
        EXPECT_EQ( value.second, expected_b );
    }
    LOG_TIME << "get";
}

template<typename U, typename A, typename B>
void test_bitpacked_pair_vector_performance_unchecked_( size_t width_a, size_t width_b )
{
    BitpackedPairVector<U, A, B> bpvec( bitpacked_pair_vector_performance_n_, width_a, width_b );
    LOG_DBG << "packed data size: " << bpvec.storage().data().size();

    U const mask_a = pair_mask_<U>( width_a );
    U const mask_b = pair_mask_<U>( width_b );

    LOG_TIME << "start";
    for( size_t i = 0; i < bpvec.size(); ++i ) {
        A a = static_cast<A>(( static_cast<U>( i * 17u ) + 3u ) & mask_a );
        B b = static_cast<B>(( static_cast<U>( i * 29u ) + 5u ) & mask_b );
        bpvec.set_unchecked( i, a, b );
    }
    LOG_TIME << "set_unchecked";

    for( size_t i = 0; i < bpvec.size(); ++i ) {
        A expected_a = static_cast<A>(( static_cast<U>( i * 17u ) + 3u ) & mask_a );
        B expected_b = static_cast<B>(( static_cast<U>( i * 29u ) + 5u ) & mask_b );
        auto value = bpvec[i];
        EXPECT_EQ( value.first, expected_a );
        EXPECT_EQ( value.second, expected_b );
    }
    LOG_TIME << "get_unchecked";
}

} // anonymous namespace

// =================================================================================================
//     Functionality Tests
// =================================================================================================

TEST( BitpackedPairVector, Basics )
{
    {
        BitpackedPairVector<uint64_t, uint32_t, uint32_t> bpvec( 10, 32, 32 );

        bpvec.set_at( 0, 1234567890U, 4000000000U );
        bpvec.set_at( 1, 42U,         17U );

        auto v0 = bpvec.at( 0 );
        auto v1 = bpvec.at( 1 );

        EXPECT_EQ( v0.first,  1234567890U );
        EXPECT_EQ( v0.second, 4000000000U );
        EXPECT_EQ( v1.first,  42U );
        EXPECT_EQ( v1.second, 17U );
    }

    {
        BitpackedPairVector<uint64_t, uint8_t, uint16_t> bpvec( 10, 7, 13 );

        bpvec.set_at( 0, 15,   1000 );
        bpvec.set_at( 1, 127,  8191 );
        bpvec.set_at( 2, 0,    0 );

        auto v0 = bpvec.at( 0 );
        auto v1 = bpvec.at( 1 );
        auto v2 = bpvec.at( 2 );

        EXPECT_EQ( v0.first, 15 );
        EXPECT_EQ( v0.second, 1000 );
        EXPECT_EQ( v1.first, 127 );
        EXPECT_EQ( v1.second, 8191 );
        EXPECT_EQ( v2.first, 0 );
        EXPECT_EQ( v2.second, 0 );
    }
}

TEST( BitpackedPairVector, DefaultConstructed )
{
    BitpackedPairVector<uint64_t, uint8_t, uint16_t> bpvec;
    EXPECT_EQ( bpvec.size(), 0 );
    EXPECT_EQ( bpvec.first_bit_width(), 0 );
    EXPECT_EQ( bpvec.second_bit_width(), 0 );
    EXPECT_EQ( bpvec.total_bit_width(), 0 );
    EXPECT_TRUE( bpvec.storage().data().empty() );
}

TEST( BitpackedPairVector, InvalidConstructorArguments )
{
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint8_t,  uint16_t>( 10, 0,  1 )), std::invalid_argument );
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint8_t,  uint16_t>( 10, 1,  0 )), std::invalid_argument );
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint8_t,  uint16_t>( 10, 9,  1 )), std::invalid_argument );
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint8_t,  uint16_t>( 10, 1, 17 )), std::invalid_argument );
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint32_t, uint32_t>( 10, 40, 32 )), std::invalid_argument );
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint32_t, uint32_t>( 10, 32, 40 )), std::invalid_argument );
    EXPECT_THROW(( BitpackedPairVector<uint64_t, uint32_t, uint32_t>( 10, 33, 32 )), std::invalid_argument );
}

TEST( BitpackedPairVector, ValueOutOfRange )
{
    BitpackedPairVector<uint64_t, uint8_t, uint16_t> bpvec( 10, 7, 13 );

    EXPECT_THROW( bpvec.set_at( 0, 128, 0 ),    std::invalid_argument );
    EXPECT_THROW( bpvec.set_at( 0, 0,   8192 ), std::invalid_argument );
    EXPECT_THROW((void) bpvec.at( 10 ),         std::out_of_range );
    EXPECT_THROW( bpvec.set_at( 10, 1, 1 ),     std::out_of_range );
}

TEST( BitpackedPairVector, Overwrite )
{
    // Test that overwrite across word boundary keeps neighbors intact

    BitpackedPairVector<uint64_t, uint8_t, uint16_t> bpvec( 16, 7, 10 );

    // total width = 17, so element 3 starts at bit 51 and spans two uint64_t words.
    bpvec.set_at( 2, 11, 111 );
    bpvec.set_at( 3, 22, 222 );
    bpvec.set_at( 4, 33, 333 );

    EXPECT_EQ( bpvec.at( 2 ).first, 11 );
    EXPECT_EQ( bpvec.at( 2 ).second, 111 );
    EXPECT_EQ( bpvec.at( 3 ).first, 22 );
    EXPECT_EQ( bpvec.at( 3 ).second, 222 );
    EXPECT_EQ( bpvec.at( 4 ).first, 33 );
    EXPECT_EQ( bpvec.at( 4 ).second, 333 );

    bpvec.set_at( 3, 99, 999 );

    EXPECT_EQ( bpvec.at( 2 ).first, 11 );
    EXPECT_EQ( bpvec.at( 2 ).second, 111 );
    EXPECT_EQ( bpvec.at( 3 ).first, 99 );
    EXPECT_EQ( bpvec.at( 3 ).second, 999 );
    EXPECT_EQ( bpvec.at( 4 ).first, 33 );
    EXPECT_EQ( bpvec.at( 4 ).second, 333 );
}

TEST( BitpackedPairVector, PackedAccessorsAgree )
{
    BitpackedPairVector<uint64_t, uint8_t, uint16_t> bpvec( 1024, 7, 13 );

    for( size_t i = 0; i < bpvec.size(); ++i ) {
        bpvec.set_unchecked(
            i,
            static_cast<uint8_t>(( i * 3u ) & 0x7Fu ),
            static_cast<uint16_t>(( i * 5u ) & 0x1FFFu )
        );
    }

    for( size_t i = 0; i < bpvec.size(); ++i ) {
        auto value = bpvec.at( i );
        auto packed = bpvec.packed_at( i );

        EXPECT_EQ( static_cast<uint8_t>( packed & 0x7Fu ), value.first );
        EXPECT_EQ( static_cast<uint16_t>(( packed >> 7 ) & 0x1FFFu ), value.second );
        EXPECT_EQ( bpvec.first_at( i ), value.first );
        EXPECT_EQ( bpvec.second_at( i ), value.second );
        EXPECT_EQ( bpvec.first_unchecked( i ), value.first );
        EXPECT_EQ( bpvec.second_unchecked( i ), value.second );
    }
}

TEST( BitpackedPairVector, FullRangeSmallWidths )
{
    for( size_t width_a = 1; width_a <= 4; ++width_a ) {
        for( size_t width_b = 1; width_b <= 4; ++width_b ) {
            test_bitpacked_pair_vector_range_<uint64_t, uint8_t, uint8_t>( width_a, width_b );
        }
    }
}

TEST( BitpackedPairVector, FullRangeLargerWidths )
{
    test_bitpacked_pair_vector_range_<uint64_t, uint8_t,  uint16_t>( 7, 13 );
    test_bitpacked_pair_vector_range_<uint64_t, uint16_t, uint16_t>( 12, 12 );
    test_bitpacked_pair_vector_range_<uint64_t, uint32_t, uint32_t>( 21, 17 );
}

TEST( BitpackedPairVector, PerformanceChecked )
{
    test_bitpacked_pair_vector_performance_<uint64_t, uint8_t, uint16_t>( 7, 13 );
}

TEST( BitpackedPairVector, PerformanceUnchecked )
{
    test_bitpacked_pair_vector_performance_unchecked_<uint64_t, uint8_t, uint16_t>( 7, 13 );
}

TEST( BitpackedPairVector, PerformanceStdPairBaseline )
{
    std::vector<std::pair<uint8_t, uint16_t>> vec( bitpacked_pair_vector_performance_n_ );
    LOG_DBG << "data size: " << vec.size();

    LOG_TIME << "start";
    for( size_t i = 0; i < vec.size(); ++i ) {
        vec[i].first  = static_cast<uint8_t>(( i * 17u + 3u ) & 0x7Fu );
        vec[i].second = static_cast<uint16_t>(( i * 29u + 5u ) & 0x1FFFu );
    }
    LOG_TIME << "set";

    for( size_t i = 0; i < vec.size(); ++i ) {
        EXPECT_EQ( vec[i].first,  static_cast<uint8_t>(( i * 17u + 3u ) & 0x7Fu ) );
        EXPECT_EQ( vec[i].second, static_cast<uint16_t>(( i * 29u + 5u ) & 0x1FFFu ) );
    }
    LOG_TIME << "get";
}
