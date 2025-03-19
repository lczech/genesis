#ifndef GENESIS_UTILS_MATH_BIT_H_
#define GENESIS_UTILS_MATH_BIT_H_

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
 * @ingroup utils
 */


#include "genesis/utils/core/info.hpp"
#include "genesis/utils/core/std.hpp"

#include <array>
#include <climits>
#include <cstdint>
#include <string>
#include <type_traits>

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_20
#    include <bit>
#endif

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
    #define GENESIS_IF_CONSTEXPR constexpr
#else
    #define GENESIS_IF_CONSTEXPR
#endif

// #include <immintrin.h>  // _pext_u64
// #ifdef __BMI2__
// #    define GENSIS_HAS_COMPILETIME_BMI2 1
// #else
// #    define GENSIS_HAS_COMPILETIME_BMI2 0
// #endif

namespace genesis {
namespace utils {

// ================================================================================================
//     Print
// ================================================================================================

// See genesis/utils/text/string.hpp
// /**
//  * @brief Print the bits in an int as a string.
//  */
// template <typename T>
// std::string to_bit_string( T value, bool with_byte_spaces = true )
// {
//     static_assert(
//         std::is_unsigned<T>::value, "Template parameter must be an unsigned integer type."
//     );
//     static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );
//
//     std::string res = "";
//     T const one = 1;
//     for( size_t i = 0; i < sizeof(T) * 8; ++i ) {
//         res += (( value & (one << i)) ? "1" : "0");
//         if( with_byte_spaces && (i+1) % 8 == 0 ) {
//             res += " ";
//         }
//     }
//     return res;
// }

// ================================================================================================
//     Pop Count
// ================================================================================================

// Following the preprocessor documentation for __has_builtin
// https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005fbuiltin.html
// Nope, changed to just check for gcc or cland. Should work.
#ifdef __has_builtin
    #if __has_builtin(__builtin_popcount) && __has_builtin(__builtin_popcountl) && __has_builtin(__builtin_popcountll)
        #define GENESIS_HAS_BUILTIN_POPCOUNT
    #endif
#endif

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
    #define GENESIS_POP_COUNT_CONSTEXPR constexpr
#else
    #define GENESIS_POP_COUNT_CONSTEXPR
#endif

/**
 * @brief Compute the pop count (Hamming weight) of an unsigned int.
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
inline GENESIS_POP_COUNT_CONSTEXPR size_t pop_count( T n )
{
    #if defined(__cpp_lib_bitops)

        // Use this if the bit header with std::popcount is available (C++20),
        // which is included above if we are compiling with C++20 or later.
        // https://en.cppreference.com/w/cpp/numeric/popcount
        return std::popcount(n);

    #elif defined(__GNUC__) || defined(__clang__)
    // #elif defined(GENESIS_HAS_BUILTIN_POPCOUNT)

        // Otherwise, use GCC/Clang intrinsic if available.
        if (sizeof(T) <= sizeof(unsigned int)) {
            return __builtin_popcount( static_cast<unsigned int>( n ));
        } else if (sizeof(T) <= sizeof(unsigned long)) {
            return __builtin_popcountl( static_cast<unsigned long>( n ));
        } else {
            return __builtin_popcountll( static_cast<unsigned long long>( n ));
        }

    #else

        // Lastly, fallback to bit magic,
        // see e.g., https://en.wikipedia.org/wiki/Hamming_weight
        static const std::array<uint64_t, 4> count_mask_ =
        {{
            0x5555555555555555,  //binary: 0101...
            0x3333333333333333,  //binary: 00110011...
            0x0f0f0f0f0f0f0f0f,  //binary: 4 zeros, 4 ones...
            0x0101010101010101   //the sum of 256 to the power of 0,1,2,3...
        }};

        uint64_t x = static_cast<uint64_t>( n );
        // Put count of each 2 bits into those 2 bits
        x -= (x >> 1) & count_mask_[0];
        // Put count of each 4 bits into those 4 bits
        x = (x & count_mask_[1]) + ((x >> 2) & count_mask_[1]);
        // Put count of each 8 bits into those 8 bits
        x = (x + (x >> 4)) & count_mask_[2];
        // Take left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
        return (x * count_mask_[3]) >> 56;

    #endif
}

#undef GENESIS_HAS_BUILTIN_POPCOUNT

// ================================================================================================
//     Count Trailing/Leading Bits
// ================================================================================================

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
    #define GENESIS_COUNT_BITS_CONSTEXPR constexpr
#else
    #define GENESIS_COUNT_BITS_CONSTEXPR
#endif

/**
 * @brief Returns the number of consecutive 0 bits starting from the least significant
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
inline GENESIS_COUNT_BITS_CONSTEXPR size_t count_trailing_zeros(T x)
{
    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_20

        return std::countr_zero(x);

    #elif defined(__GNUC__) || defined(__clang__)

        // __builtin_ctzll is undefined for x==0, so handle that.
        if (x == 0) {
            return sizeof(T) * 8;
        }
        if GENESIS_IF_CONSTEXPR (sizeof(T) <= sizeof(unsigned int)) {
            return __builtin_ctz(static_cast<unsigned int>(x));
        } else if GENESIS_IF_CONSTEXPR (sizeof(T) <= sizeof(unsigned long)) {
            return __builtin_ctzl(static_cast<unsigned long>(x));
        } else {
            return __builtin_ctzll(static_cast<unsigned long long>(x));
        }

    #else

        // Fallback for counting trailing zeros for any unsigned type.
        if( x == 0 ) {
            // All bits are zero.
            return sizeof(T) * 8;
        }
        size_t count = 0;
        while( (x & T(1)) == 0 ) {
            ++count;
            x >>= 1;
        }
        return count;

    #endif
}

/**
 * @brief Returns the number of consecutive 0 bits starting from the most significant bit.
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
inline GENESIS_COUNT_BITS_CONSTEXPR size_t count_leading_zeros(T x)
{
    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_20

        return std::countl_zero(x);

    #elif defined(__GNUC__) || defined(__clang__)

        // __builtin_clzll is undefined for x==0, so handle that.
        if (x == 0) {
            return sizeof(T) * 8;
        }
        if GENESIS_IF_CONSTEXPR (sizeof(T) <= sizeof(unsigned int)) {
            return __builtin_clz(static_cast<unsigned int>(x));
        } else if GENESIS_IF_CONSTEXPR (sizeof(T) <= sizeof(unsigned long)) {
            return __builtin_clzl(static_cast<unsigned long>(x));
        } else {
            return __builtin_clzll(static_cast<unsigned long long>(x));
        }

    #else

        // Fallback for counting leading zeros for any unsigned type.
        size_t total = sizeof(T) * 8;
        size_t count = 0;
        // Check bits from the most significant (bit total-1) down to bit 0.
        for( size_t i = static_cast<int>(total) - 1; i >= 0; --i ) {
            if( x & (T(1) << i) ) {
                break;
            }
            ++count;
        }
        return count;

    #endif
}

/**
 * @briefR eturns the number of consecutive 1 bits starting from the least significant bit.
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
inline GENESIS_COUNT_BITS_CONSTEXPR size_t count_trailing_ones(T x)
{
    // If x is 0, there are no trailing ones.
    if( x == 0 ) {
        return 0;
    }
    // Trailing ones in x equal trailing zeros in the bitwise NOT of x.
    return count_trailing_zeros(~x);
}

/**
 * @brief Returns the number of consecutive 1 bits starting from the most significant bit.
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
inline GENESIS_COUNT_BITS_CONSTEXPR size_t count_leading_ones(T x)
{
    // If x is all ones, then all bits are ones.
    if( x == static_cast<T>(~T(0)) ) {
        return sizeof(T) * 8;
    }
    // Leading ones in x equal leading zeros in the bitwise NOT of x.
    return count_leading_zeros(~x);
}

// ================================================================================================
//     Bit Extract
// ================================================================================================

// /**
//  * @brief
//  *
//  *     word:   0010101011010111
//  *     mask:   0011100100100010
//  *     masked: __101__0__0___1_
//  *     PEXT:   __________101001
//  *
//  * https://www.felixcloutier.com/x86/pext
//  * https://stackoverflow.com/a/73381159/4184258
//  */
// template <typename Integral>
// inline constexpr Integral extract_bits( Integral source, Integral mask )
// {
//     Integral res = 0;
//     Integral bb = 1;
//     do {
//         Integral lsb = mask & -mask;
//         mask &= ~lsb;
//         bool isset = source & lsb;
//         res |= isset ? bb : 0;
//         bb += bb;
//     } while (mask);
//
//     return res;
// }
//
// inline uint64_t bit_extract( uint64_t source, uint64_t mask )
// {
//     return _pext_u64( source, mask );
//
//     // static const bool has_pext_support = info_get_hardware().HW_BMI2;
//     // if( GENSIS_HAS_COMPILETIME_BMI2 && has_pext_support ) {
//     //     return _pext_u64( source, mask );
//     // } else {
//     //
//     // }
// }

} // namespace utils
} // namespace genesis

#undef GENESIS_IF_CONSTEXPR

#endif // include guard
