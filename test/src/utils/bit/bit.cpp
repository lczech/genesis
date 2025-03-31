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

#include "genesis/utils/bit/bit.hpp"

using namespace genesis::utils;

TEST( Bit, PopCount )
{
    EXPECT_EQ( 0, pop_count( static_cast<size_t>( 0 )));
    EXPECT_EQ( 1, pop_count( static_cast<size_t>( 1 )));
    EXPECT_EQ( 1, pop_count( static_cast<size_t>( 2 )));
    EXPECT_EQ( 2, pop_count( static_cast<size_t>( 3 )));
    EXPECT_EQ( 3, pop_count( static_cast<size_t>( 7 )));
    EXPECT_EQ( 1, pop_count( static_cast<size_t>( 8 )));
}

TEST( Bit, CountBits )
{
    // UnsignedIntTrailingZeros
    {
        using T = unsigned int;
        EXPECT_EQ(count_trailing_zeros(T(0)), sizeof(T) * 8);
        EXPECT_EQ(count_trailing_zeros(T(1)), 0u);
        EXPECT_EQ(count_trailing_zeros(T(2)), 1u);
        EXPECT_EQ(count_trailing_zeros(T(8)), 3u);
    }

    // UnsignedIntLeadingZeros
    {
        using T = unsigned int;
        EXPECT_EQ(count_leading_zeros(T(0)), sizeof(T) * 8);
        EXPECT_EQ(count_leading_zeros(T(1)), (sizeof(T) * 8) - 1u);
        // For a value with the MSB set (e.g. 0x80000000 for 32-bit).
        EXPECT_EQ(count_leading_zeros(T(0x80000000)), 0u);
        // For a value with the second highest bit set (e.g. 0x40000000).
        EXPECT_EQ(count_leading_zeros(T(0x40000000)), 1u);
    }

    // UnsignedIntTrailingOnes
    {
        using T = unsigned int;
        EXPECT_EQ(count_trailing_ones(T(0)), 0u);
        EXPECT_EQ(count_trailing_ones(T(1)), 1u);
        EXPECT_EQ(count_trailing_ones(T(3)), 2u);       // Binary: 11
        EXPECT_EQ(count_trailing_ones(T(0xB)), 2u);    // Trailing "11", 0b1011
        EXPECT_EQ(count_trailing_ones(T(~0u)), sizeof(T) * 8);
    }

    // UnsignedIntLeadingOnes
    {
        using T = unsigned int;
        EXPECT_EQ(count_leading_ones(T(0)), 0u);
        EXPECT_EQ(count_leading_ones(T(~0u)), sizeof(T) * 8);
        // For a value with the top 4 bits as ones (e.g. 0xF0000000).
        EXPECT_EQ(count_leading_ones(T(0xF0000000)), 4u);
        // For a value with the MSB cleared (e.g. 0x7FFFFFFF).
        EXPECT_EQ(count_leading_ones(T(0x7FFFFFFF)), 0u);
    }

    // --- Tests for unsigned long long (typically 64-bit) ---
    // UnsignedLongLongTrailingZeros
    {
        using T = unsigned long long;
        EXPECT_EQ(count_trailing_zeros(T(0)), sizeof(T) * 8);
        EXPECT_EQ(count_trailing_zeros(T(1)), 0u);
        EXPECT_EQ(count_trailing_zeros(T(2)), 1u);
        EXPECT_EQ(count_trailing_zeros(T(8)), 3u);
    }

    // UnsignedLongLongLeadingZeros
    {
        using T = unsigned long long;
        EXPECT_EQ(count_leading_zeros(T(0)), sizeof(T) * 8);
        EXPECT_EQ(count_leading_zeros(T(1)), (sizeof(T) * 8) - 1u);
        EXPECT_EQ(count_leading_zeros(T(0x8000000000000000ULL)), 0u);
        EXPECT_EQ(count_leading_zeros(T(0x4000000000000000ULL)), 1u);
    }

    // UnsignedLongLongTrailingOnes
    {
        using T = unsigned long long;
        EXPECT_EQ(count_trailing_ones(T(0)), 0u);
        EXPECT_EQ(count_trailing_ones(T(1)), 1u);
        EXPECT_EQ(count_trailing_ones(T(3)), 2u);
        EXPECT_EQ(count_trailing_ones(T(0xBULL)), 2u); // 0b1011
        EXPECT_EQ(count_trailing_ones(T(~0ULL)), sizeof(T) * 8);
    }

    // UnsignedLongLongLeadingOnes
    {
        using T = unsigned long long;
        EXPECT_EQ(count_leading_ones(T(0)), 0u);
        EXPECT_EQ(count_leading_ones(T(~0ULL)), sizeof(T) * 8);
        EXPECT_EQ(count_leading_ones(T(0xF000000000000000ULL)), 4u);
        EXPECT_EQ(count_leading_ones(T(0x7FFFFFFFFFFFFFFFULL)), 0u);
        EXPECT_EQ(count_leading_ones(T(0xC000000000000000ULL)), 2u);
    }
}
