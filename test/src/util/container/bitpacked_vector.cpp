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

#include "genesis/util/container/bitpacked_vector.hpp"

#include <climits>
#include <vector>
#include <limits>
#include <utility>

using namespace genesis::util;
using namespace genesis::util::core;
using namespace genesis::util::container;

// =================================================================================================
//     Functionality Tests
// =================================================================================================

TEST( BitpackedVector, Basics )
{
    // DefaultConstructed
    {
        BitpackedVector<uint64_t, uint8_t> bpvec;
        EXPECT_EQ( bpvec.size(), 0 );
        EXPECT_EQ( bpvec.bit_width(), 0 );
        EXPECT_TRUE( bpvec.data().empty() );
    }

    // Test Direct Access (Same Type and Full Bit Width)
    // DirectAccessUnsigned64Bit
    {
        BitpackedVector<uint64_t, uint64_t> bpvec(10, 64);

        bpvec.set_at(0, 1234567890123456789ULL);
        bpvec.set_at(1, 9876543210987654321ULL);

        EXPECT_EQ(bpvec.at(0), 1234567890123456789ULL);
        EXPECT_EQ(bpvec.at(1), 9876543210987654321ULL);
    }

    // DirectAccessUnsigned32Bit
    {
        BitpackedVector<uint32_t, uint32_t> bpvec(10, 32);

        bpvec.set_at(0, 1234567890U);
        bpvec.set_at(1, 987654321U);

        EXPECT_EQ(bpvec.at(0), 1234567890U);
        EXPECT_EQ(bpvec.at(1), 987654321U);
    }

    // Test Bit-Packed Access (Smaller Bit Widths)
    // BitPackedUnsigned7Bit
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);

        bpvec.set_at(0, 15);  // 15 = 0001111 in binary
        bpvec.set_at(1, 100); // 100 = 1100100 in binary
        bpvec.set_at(2, 7);   // 7  = 0000111 in binary

        EXPECT_EQ(bpvec.at(0), 15);
        EXPECT_EQ(bpvec.at(1), 100);
        EXPECT_EQ(bpvec.at(2), 7);
    }

    // BitPackedUnsigned16Bit
    {
        BitpackedVector<uint64_t, uint16_t> bpvec(10, 16);

        bpvec.set_at(0, 65535); // Maximum 16-bit unsigned value
        bpvec.set_at(1, 32768); // 32768 = 1000000000000000 in binary
        bpvec.set_at(2, 12345); // Random 16-bit number

        EXPECT_EQ(bpvec.at(0), 65535);
        EXPECT_EQ(bpvec.at(1), 32768);
        EXPECT_EQ(bpvec.at(2), 12345);
    }

    // Test Edge Cases

    // MinimumBitValue
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);
        bpvec.set_at(0, 0); // Minimum value for 7 bits
        EXPECT_EQ(bpvec.at(0), 0);
    }

    // MaximumBitValue
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);
        bpvec.set_at(0, 127); // Maximum value for 7 bits (01111111 in binary)
        EXPECT_EQ(bpvec.at(0), 127);
    }

    // OutOfBoundsIndex
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);

        // Set and get within bounds
        bpvec.set_at(0, 10);
        EXPECT_EQ(bpvec.at(0), 10);

        // Attempt to set out of bounds should throw
        EXPECT_THROW(bpvec.set_at(10, 20), std::out_of_range);
        // Attempt to get out of bounds should throw
        EXPECT_THROW((void) bpvec.at(10), std::out_of_range);
    }

    // Contructor out of range
    {
        // Attempt to create a vector with 0 bit width, and higher bit width than the storage.
        // We need a lambda, as the test macro does not allow a constructor inplace.
        auto create_bpv_ = []( size_t bit_width ){
            auto bpvec = BitpackedVector<uint8_t, uint8_t>(10, bit_width);
            return bpvec;
        };

        EXPECT_THROW( create_bpv_(0), std::invalid_argument );
        EXPECT_THROW( create_bpv_(9), std::invalid_argument );
    }

    // ValueOutOfRange
    {
        // Attempt to set a value that is out of the 7-bit range
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);
        EXPECT_THROW(bpvec.set_at(0, 200), std::invalid_argument);
    }

    // ValueOutOfRange
    {
        // Attempt to set a value that is out of the 16-bit range
        // BitpackedVector<uint64_t, uint16_t> bpvec_16(10, 16);
        // EXPECT_THROW(bpvec_16.set_at(0, 70000), std::invalid_argument);
        // Doesn't compile, as the compiler already catches the overflow on T
    }

    // Test Direct Access With Smaller Type
    // DirectAccessWithSmallerType
    {
        BitpackedVector<uint64_t, uint32_t> bpvec(10, 32);

        bpvec.set_at(0, 1234567890U);
        bpvec.set_at(1, 987654321U);

        EXPECT_EQ(bpvec.at(0), 1234567890U);
        EXPECT_EQ(bpvec.at(1), 987654321U);
    }

    // Test Performance with Maximum Size
    // LargeArrayPerformance
    {
        size_t const large_size = 1000000;
        BitpackedVector<uint64_t, uint8_t> bpvec(large_size, 8);

        // Set values in a loop
        for( size_t i = 0; i < large_size; ++i ) {
            bpvec.set_at( i, static_cast<uint8_t>(i % 256) );
        }

        // Validate values in a loop
        for( size_t i = 0; i < large_size; ++i ) {
            EXPECT_EQ( bpvec.at(i), static_cast<uint8_t>(i % 256) );
        }
    }

    // Constructor Rejects Width Larger Than Value Type
    {
        EXPECT_THROW(( BitpackedVector<uint64_t, uint8_t>( 10, 9 )),  std::invalid_argument );
        EXPECT_THROW(( BitpackedVector<uint64_t, uint16_t>( 10, 17 )), std::invalid_argument );
        EXPECT_THROW(( BitpackedVector<uint32_t, uint8_t>( 10, 9 )),  std::invalid_argument );
    }
}

namespace
{

template<typename U>
U bit_mask_( size_t bits )
{
    constexpr size_t U_BITS = std::numeric_limits<U>::digits;
    EXPECT_GT( bits, 0 );
    EXPECT_LE( bits, U_BITS );
    return bits == U_BITS ? ~U{0} : (( U{1} << bits ) - 1 );
}

template<typename U, typename T>
void test_bitpacked_vector_all_widths_()
{
    size_t const n = 100000;
    constexpr size_t T_BITS = std::numeric_limits<T>::digits;

    for( size_t bit_width = 1; bit_width <= T_BITS; ++bit_width ) {
        BitpackedVector<U, T> bpvec( n, bit_width );
        U const mask = bit_mask_<U>( bit_width );

        for( size_t i = 0; i < n; ++i ) {
            U value = static_cast<U>(( i * 1315423911u ) + ( i >> 3 ));
            value &= mask;
            bpvec.set_unchecked( i, static_cast<T>( value ));
        }

        for( size_t i = 0; i < n; ++i ) {
            U expected = static_cast<U>(( i * 1315423911u ) + ( i >> 3 ));
            expected &= mask;

            EXPECT_EQ( bpvec.at( i ), static_cast<T>( expected ));
            EXPECT_EQ( bpvec[i],    static_cast<T>( expected ));
        }
    }
}

template<typename U, typename T>
void test_bitpacked_vector_copy_move_()
{
    BitpackedVector<U, T> original( 1024, std::numeric_limits<T>::digits );
    for( size_t i = 0; i < original.size(); ++i ) {
        original.set_unchecked( i, static_cast<T>( i ));
    }

    auto copy = original;
    for( size_t i = 0; i < original.size(); ++i ) {
        EXPECT_EQ( copy.at( i ), original.at( i ));
    }

    auto moved = std::move( copy );
    for( size_t i = 0; i < original.size(); ++i ) {
        EXPECT_EQ( moved.at( i ), original.at( i ));
    }
}

template<typename U, typename T>
void test_bitpacked_vector_range_()
{
    size_t const n = 1000000;

    // Test all bit widths that fit in a byte. This covers all cases with uneven widths that do not
    // fit in the word evenly, so that we test the across-word-boundary part as well.
    for( size_t bit_width = 1; bit_width <= 8; ++bit_width ) {
        BitpackedVector<U, T> bpvec( n, bit_width );

        // Compute the max value that we can store in the given bit width
        size_t const limit_value = 1ULL << bit_width;

        // Set values in a loop
        for( size_t i = 0; i < n; ++i) {
            bpvec.set_at( i, static_cast<T>( i % limit_value ));
        }

        // Validate values in a loop
        for( size_t i = 0; i < n; ++i ) {
            EXPECT_EQ( bpvec.at(i), static_cast<T>( i % limit_value ));
        }
    }
}

} // anonymous namespace

TEST( BitpackedVector, FullRange )
{
    {
        test_bitpacked_vector_range_<uint8_t, uint8_t>();
        test_bitpacked_vector_range_<uint64_t, uint8_t>();
    }

    // FullRangeAllWidths
    {
        test_bitpacked_vector_all_widths_<uint8_t,  uint8_t>();
        test_bitpacked_vector_all_widths_<uint64_t, uint8_t>();
        test_bitpacked_vector_all_widths_<uint64_t, uint16_t>();
        test_bitpacked_vector_all_widths_<uint64_t, uint32_t>();
    }
}

TEST( BitpackedVector, Boundaries )
{
    // OverwriteAcrossWordBoundaryKeepsNeighborsIntact
    {
        BitpackedVector<uint64_t, uint8_t> bpvec( 32, 7 );

        // index 9 starts at bit 63 and therefore spans two uint64_t words.
        bpvec.set_at( 8, 11 );
        bpvec.set_at( 9, 22 );
        bpvec.set_at( 10, 33 );

        EXPECT_EQ( bpvec.at( 8 ), 11 );
        EXPECT_EQ( bpvec.at( 9 ), 22 );
        EXPECT_EQ( bpvec.at( 10 ), 33 );

        bpvec.set_at( 9, 99 );
        EXPECT_EQ( bpvec.at( 8 ), 11 );
        EXPECT_EQ( bpvec.at( 9 ), 99 );
        EXPECT_EQ( bpvec.at( 10 ), 33 );

        bpvec.set_at( 9, 1 );
        EXPECT_EQ( bpvec.at( 8 ), 11 );
        EXPECT_EQ( bpvec.at( 9 ), 1 );
        EXPECT_EQ( bpvec.at( 10 ), 33 );
    }

    // CheckedAndUncheckedAccessAgree
    {
        BitpackedVector<uint64_t, uint16_t> bpvec( 4096, 13 );

        for( size_t i = 0; i < bpvec.size(); ++i ) {
            bpvec.set_unchecked( i, static_cast<uint16_t>(( i * 17u ) & 0x1FFFu ));
        }

        for( size_t i = 0; i < bpvec.size(); ++i ) {
            EXPECT_EQ( bpvec.at( i ), bpvec[i] );
        }
    }

    // Copy and Move
    {
        test_bitpacked_vector_copy_move_<uint8_t,  uint8_t>();
        test_bitpacked_vector_copy_move_<uint64_t, uint16_t>();
        test_bitpacked_vector_copy_move_<uint64_t, uint32_t>();
    }
}

// =================================================================================================
//     Performance Tests
// =================================================================================================

// size_t const bitpacked_vector_performance_n_ = 10000000000; // 60s per test
// size_t const bitpacked_vector_performance_n_ = 1000000000; // 6s per test
size_t const bitpacked_vector_performance_n_ = 10000000; // quick

template<typename T, typename U>
void test_bitpacked_vector_performance_( size_t bit_width )
{
    BitpackedVector<T, U> bpvec( bitpacked_vector_performance_n_, bit_width );
    LOG_DBG << "data size: " << bpvec.data().size();

    // Compute the max value that we can store in the given bit width
    size_t const limit_value = 1ULL << bit_width;

    // Set values in a loop
    LOG_TIME << "start";
    for( size_t i = 0; i < bpvec.size(); ++i) {
        bpvec.set_at( i, static_cast<T>( i % limit_value ));
    }
    LOG_TIME << "set";

    // Validate values in a loop
    for( size_t i = 0; i < bpvec.size(); ++i ) {
        EXPECT_EQ( bpvec.at(i), static_cast<T>( i % limit_value ));
    }
    LOG_TIME << "get";
}

template<typename U, typename T>
void test_bitpacked_vector_performance_unchecked_( size_t bit_width )
{
    BitpackedVector<U, T> bpvec( bitpacked_vector_performance_n_, bit_width );
    LOG_DBG << "data size: " << bpvec.data().size();

    U const mask = bit_mask_<U>( bit_width );

    LOG_TIME << "start";
    for( size_t i = 0; i < bpvec.size(); ++i ) {
        U value = static_cast<U>(( i * 1315423911u ) + ( i >> 3 ));
        value &= mask;
        bpvec.set_unchecked( i, static_cast<T>( value ));
    }
    LOG_TIME << "set_unchecked";

    for( size_t i = 0; i < bpvec.size(); ++i ) {
        U expected = static_cast<U>(( i * 1315423911u ) + ( i >> 3 ));
        expected &= mask;
        EXPECT_EQ( bpvec[i], static_cast<T>( expected ));
    }
    LOG_TIME << "get_unchecked";
}

TEST( BitpackedVector, PerformancePackedMismatch )
{
    test_bitpacked_vector_performance_<uint64_t, uint8_t>( 7 );
    test_bitpacked_vector_performance_unchecked_<uint64_t, uint8_t>( 7 );
}

TEST( BitpackedVector, PerformancePackedContained )
{
    test_bitpacked_vector_performance_<uint64_t, uint8_t>( 8 );
    test_bitpacked_vector_performance_unchecked_<uint64_t, uint8_t>( 8 );
}

TEST( BitpackedVector, PerformancePackedExact )
{
    test_bitpacked_vector_performance_<uint8_t, uint8_t>( 8 );
    test_bitpacked_vector_performance_unchecked_<uint8_t, uint8_t>( 8 );
}

TEST( BitpackedVector, PerformanceStandardOperator )
{
    std::vector<uint8_t> vec( bitpacked_vector_performance_n_ );
    LOG_DBG << "data size: " << vec.size();

    // Compute the max value that we can store in the given bit width
    size_t const limit_value = 1ULL << 8;

    // Set values in a loop
    LOG_TIME << "start";
    for( size_t i = 0; i < vec.size(); ++i) {
        vec[i] = static_cast<uint8_t>( i % limit_value );
    }
    LOG_TIME << "set";

    // Validate values in a loop
    for( size_t i = 0; i < vec.size(); ++i ) {
        EXPECT_EQ( vec[i], static_cast<uint8_t>( i % limit_value ));
    }
    LOG_TIME << "get";
}

TEST( BitpackedVector, PerformanceStandardAt )
{
    std::vector<uint8_t> vec( bitpacked_vector_performance_n_ );
    LOG_DBG << "data size: " << vec.size();

    // Compute the max value that we can store in the given bit width
    size_t const limit_value = 1ULL << 8;

    // Set values in a loop
    LOG_TIME << "start";
    for( size_t i = 0; i < vec.size(); ++i) {
        vec.at(i) = static_cast<uint8_t>( i % limit_value );
    }
    LOG_TIME << "set";

    // Validate values in a loop
    for( size_t i = 0; i < vec.size(); ++i ) {
        EXPECT_EQ( vec.at(i), static_cast<uint8_t>( i % limit_value ));
    }
    LOG_TIME << "get";
}
