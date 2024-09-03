/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/utils/containers/bitpacked_vector.hpp"

#include <climits>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//     Functionality Tests
// =================================================================================================

TEST( BitpackedVector, Basics )
{
    // Test Direct Access (Same Type and Full Bit Width)
    // DirectAccessUnsigned64Bit
    {
        BitpackedVector<uint64_t, uint64_t> bpvec(10, 64);

        bpvec.set(0, 1234567890123456789ULL);
        bpvec.set(1, 9876543210987654321ULL);

        EXPECT_EQ(bpvec.get(0), 1234567890123456789ULL);
        EXPECT_EQ(bpvec.get(1), 9876543210987654321ULL);
    }

    // DirectAccessUnsigned32Bit
    {
        BitpackedVector<uint32_t, uint32_t> bpvec(10, 32);

        bpvec.set(0, 1234567890U);
        bpvec.set(1, 987654321U);

        EXPECT_EQ(bpvec.get(0), 1234567890U);
        EXPECT_EQ(bpvec.get(1), 987654321U);
    }

    // Test Bit-Packed Access (Smaller Bit Widths)
    // BitPackedUnsigned7Bit
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);

        bpvec.set(0, 15);  // 15 = 0001111 in binary
        bpvec.set(1, 100); // 100 = 1100100 in binary
        bpvec.set(2, 7);   // 7  = 0000111 in binary

        EXPECT_EQ(bpvec.get(0), 15);
        EXPECT_EQ(bpvec.get(1), 100);
        EXPECT_EQ(bpvec.get(2), 7);
    }

    // BitPackedUnsigned16Bit
    {
        BitpackedVector<uint64_t, uint16_t> bpvec(10, 16);

        bpvec.set(0, 65535); // Maximum 16-bit unsigned value
        bpvec.set(1, 32768); // 32768 = 1000000000000000 in binary
        bpvec.set(2, 12345); // Random 16-bit number

        EXPECT_EQ(bpvec.get(0), 65535);
        EXPECT_EQ(bpvec.get(1), 32768);
        EXPECT_EQ(bpvec.get(2), 12345);
    }

    // Test Edge Cases

    // MinimumBitValue
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);
        bpvec.set(0, 0); // Minimum value for 7 bits
        EXPECT_EQ(bpvec.get(0), 0);
    }

    // MaximumBitValue
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);
        bpvec.set(0, 127); // Maximum value for 7 bits (01111111 in binary)
        EXPECT_EQ(bpvec.get(0), 127);
    }

    // OutOfBoundsIndex
    {
        BitpackedVector<uint64_t, uint8_t> bpvec(10, 7);

        // Set and get within bounds
        bpvec.set(0, 10);
        EXPECT_EQ(bpvec.get(0), 10);

        // Attempt to set out of bounds should throw
        EXPECT_THROW(bpvec.set(10, 20), std::out_of_range);
        // Attempt to get out of bounds should throw
        EXPECT_THROW(bpvec.get(10), std::out_of_range);
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
        EXPECT_THROW(bpvec.set(0, 200), std::invalid_argument);
    }

    // ValueOutOfRange
    {
        // Attempt to set a value that is out of the 16-bit range
        // BitpackedVector<uint64_t, uint16_t> bpvec_16(10, 16);
        // EXPECT_THROW(bpvec_16.set(0, 70000), std::invalid_argument);
        // Doesn't compile, as the compiler already catches the overflow on T
    }

    // Test Direct Access With Smaller Type
    // DirectAccessWithSmallerType
    {
        BitpackedVector<uint64_t, uint32_t> bpvec(10, 32);

        bpvec.set(0, 1234567890U);
        bpvec.set(1, 987654321U);

        EXPECT_EQ(bpvec.get(0), 1234567890U);
        EXPECT_EQ(bpvec.get(1), 987654321U);
    }

    // Test Performance with Maximum Size
    // LargeArrayPerformance
    {
        size_t const large_size = 1000000;
        BitpackedVector<uint64_t, uint8_t> bpvec(large_size, 8);

        // Set values in a loop
        for( size_t i = 0; i < large_size; ++i ) {
            bpvec.set( i, static_cast<uint8_t>(i % 256) );
        }

        // Validate values in a loop
        for( size_t i = 0; i < large_size; ++i ) {
            EXPECT_EQ( bpvec.get(i), static_cast<uint8_t>(i % 256) );
        }
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
            bpvec.set( i, static_cast<T>( i % limit_value ));
        }

        // Validate values in a loop
        for( size_t i = 0; i < n; ++i ) {
            EXPECT_EQ( bpvec.get(i), static_cast<T>( i % limit_value ));
        }
    }
}

TEST( BitpackedVector, FullRange )
{
    test_bitpacked_vector_range_<uint8_t, uint8_t>();
    test_bitpacked_vector_range_<uint64_t, uint8_t>();
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
        bpvec.set( i, static_cast<T>( i % limit_value ));
    }
    LOG_TIME << "set";

    // Validate values in a loop
    for( size_t i = 0; i < bpvec.size(); ++i ) {
        EXPECT_EQ( bpvec.get(i), static_cast<T>( i % limit_value ));
    }
    LOG_TIME << "get";
}

TEST( BitpackedVector, PerformancePackedMismatch )
{
    test_bitpacked_vector_performance_<uint64_t, uint8_t>( 7 );
}

TEST( BitpackedVector, PerformancePackedContained )
{
    test_bitpacked_vector_performance_<uint64_t, uint8_t>( 8 );
}

TEST( BitpackedVector, PerformancePackedExact )
{
    test_bitpacked_vector_performance_<uint8_t, uint8_t>( 8 );
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
