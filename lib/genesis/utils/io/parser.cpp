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

#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <limits>
#include <stdexcept>

// For C++17, we have a little speedup in the integer parsing part.
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    #include <charconv>

#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Integer
// =================================================================================================

// -------------------------------------------------------------------------
//     parse_unsigned_integer_gcc_intrinsic_
// -------------------------------------------------------------------------

// Only use intrinsics version for the compilers that support them!
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

// Forward declaration
size_t parse_unsigned_integer_naive_( utils::InputStream& source );

/**
 * @brief Super fast loop-less parsing of unsigned ints from < 8 chars,
 * using GCC/Clang compiler intrinsic builtins.
 */
size_t parse_unsigned_integer_gcc_intrinsic_( utils::InputStream& source )
{
    // This function only works on little endian systems (I think).
    // We do not check this here, as so far, no one has tried to run our code on any machine
    // that is not little endian. So we are good for now. In case this code needs to be adapted
    // to big endian as well: I think the only change required is the `chunk <<= ...` that needs
    // to turn into a right shift instead. Not entirely sure though.
    // Also, in this function, we make use of the fact that our internal buffer is always way larger
    // than any valid integer input. That is, we may read from after the block end, or even the
    // stream end, but we have enough buffer for this to be okay (after all, we are just reading
    // eight bytes here). We then check for this later.

    // Get the internals of the input stream.
    auto buffer_pair = source.buffer();
    auto buffer = buffer_pair.first;
    size_t data_pos = 0;
    size_t data_end = buffer_pair.second;

    // Copy 8 bytes into a chunk that we process as one unit.
    std::uint64_t chunk = 0;
    std::memcpy( &chunk, &buffer[ data_pos ], sizeof( chunk ));

    // Helper macro functions to check whether a word has bytes that are less than or greater
    // than some specified value, and mark these bytes.
    // http://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
    // http://graphics.stanford.edu/~seander/bithacks.html#HasMoreInWord
    auto const zero = static_cast<uint64_t>(0);
    #define hasless(x,n) (((x)-~zero/255*(n))&~(x)&~zero/255*128)
    #define hasmore(x,n) ((((x)+~zero/255*(127-(n)))|(x))&~zero/255*128)

    // Get all positions that are not digits, by marking a bit in their respective byte.
    auto const l = hasless( chunk, '0' );
    auto const m = hasmore( chunk, '9' );
    auto const p = l | m;

    // Example:
    // String "167\n253\n" turns into chunk c (on little endian systems)
    //         \n        3        5        2       \n        7        6        1
    // c 00001010 00110011 00110101 00110010 00001010 00110111 00110110 00110001
    // l 10000000 00000000 00000000 00000000 10000000 00000000 00000000 00000000
    // m 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    // p 10000000 00000000 00000000 00000000 10000000 00000000 00000000 00000000
    //   ^                                   ^
    // with the two '\n' bytes marked.

    #undef hasless
    #undef hasmore

    // Find the index of the first byte that is not a digit. We first get the bit position
    // using an intrinsic, and then divite by 8 to get the byte. The branching to select the
    // correct intrinsic should be resolved at compile time already.
    // We are using __builtin_ffs and its variants:
    // Returns one plus the index of the least significant 1-bit of x, or if x is zero, returns zero.
    // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#Other-Builtins
    int idx = 0;
    if( sizeof(int) == sizeof(std::uint64_t) ) {
        idx = __builtin_ffs(p) / 8;
    } else if( sizeof(long) == sizeof(std::uint64_t) ) {
        idx = __builtin_ffsl(p) / 8;
    } else if( sizeof(long long) == sizeof(std::uint64_t) ) {
        idx = __builtin_ffsll(p) / 8;
    } else {
        static_assert(
            ( sizeof(int) == sizeof(std::uint64_t) ) ||
            ( sizeof(long) == sizeof(std::uint64_t) ) ||
            ( sizeof(long long) == sizeof(std::uint64_t) ),
            "No compilter intrinsic __builtin_ffs[l][l] for std::uint64_t"
        );
        throw std::runtime_error(
            "No compilter intrinsic __builtin_ffs[l][l] for std::uint64_t"
        );
    }
    assert( 0 <= idx && idx <= 8 );

    // Not needed but kept for reference: Mask out all bits that we do not want.
    // auto const mask = ~(~zero << ((idx-1)*8));
    // chunk &= mask;

    // On little endian systems, we need to move the actual data chars that we want to parse to the
    // left-most position for the following code to work. So, for our example from above, we need
    // to move the "xxxx x761" in the chunk so that we get "7610 0000".
    chunk <<= (8 * ( 8 - idx + 1 ));

    // Now use an O(log(n)) method of computing the result, where we combine adjacent parts into
    // numbers, first 2 bytes, then 4 bytes, then all 8 bytes. Inspired by parse_8_chars() from
    // https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html

    // 1-byte mask trick (works on 4 pairs of single digits)
    std::uint64_t lower_digits = (chunk & 0x0f000f000f000f00) >> 8;
    std::uint64_t upper_digits = (chunk & 0x000f000f000f000f) * 10;
    chunk = lower_digits + upper_digits;

    // 2-byte mask trick (works on 2 pairs of two digits)
    lower_digits = (chunk & 0x00ff000000ff0000) >> 16;
    upper_digits = (chunk & 0x000000ff000000ff) * 100;
    chunk = lower_digits + upper_digits;

    // 4-byte mask trick (works on pair of four digits)
    lower_digits = (chunk & 0x0000ffff00000000) >> 32;
    upper_digits = (chunk & 0x000000000000ffff) * 10000;
    chunk = lower_digits + upper_digits;

    // Edge cases. We treat them at the end, so that in the standard cases, the processor
    // does not come to a grinding halt when trying to figure out if these cases apply;
    // this might be premature optimization, but in our tests, it made the function slightly faster.
    // If the returned index is 0, there was no non-digit byte in the chunk,
    // so we run the naive loop instead. We could also call this function here again recursively,
    // summing up parts of large numbers. But that would mean that we need to do overflow
    // detection and all that, and currently, this does not seem needed. Let's be lazy today.
    // Furthermore, if the 8 bytes that we process here are at the end of the stream, we cannot
    // confidently use them, in cases for example where the stream ends in a number, but does
    // not have a new line char at the end. So in that case, better parse naievely.
    // Lastly, if the index is 1, the first byte is not a digit, which is an error, as this function
    // is only called from parsers that expect a number.
    if( idx == 0 || data_pos + 8 >= data_end ) {
        return parse_unsigned_integer_naive_( source );
    }
    if( idx == 1 ) {
        throw std::runtime_error(
            "Expecting integer in " + source.source_name() + " at " + source.at() + "."
        );
    }

    // Now move as far as needed in the buffer, and return our findings.
    data_pos += idx - 1;
    source.jump_unchecked( data_pos );
    return chunk;
}

#endif // defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

// -------------------------------------------------------------------------
//     parse_unsigned_integer_from_chars_
// -------------------------------------------------------------------------

// Completely deactivated for now, as we are not using it anyway,
// and it's causing trouble with clang 5-7.
// #if ( defined(__GNUC__) || defined(__GNUG__) ) && ( !defined(__clang__) || ( __clang_major__ >= 8 ))

// We used to have to following in our main CMakeList.txt to try to support this,
// but that lead to linker errors downstream, and was hence not worth keeping.
// Keeping it here for future reference, but likeley not needed any more.

// # With clang 5 to 7, we run into a bug (https://stackoverflow.com/a/49795448) of clang,
// # because we are using `__builtin_mul_overflow` in genesis/utils/io/input_stream.cpp
// # This here tries to fix this. If this causes more trouble in the future, we might instead
// # use the native algorithm in that function...
// # Update: Yes, that causes trouble, as we then get linker errors:
// # undefined reference to symbol '_Unwind_Resume@@GCC_3.0'
// # see https://stackoverflow.com/a/22774687 - so instead we deactivate this hack here for now,
// # and use a different implementation in input_stream.cpp instead when using clang 5-7.
// # if(
// #     "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"
// #     AND ( NOT ( ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "5" ))
// #     AND ( ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "8" )
// # )
// #     message(STATUS "Building with Clang 5, 6 or 7. Switching to --rtlib=compiler-rt")
// #
// #     set( CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} ")
// #     set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --rtlib=compiler-rt" )
// #
// #     set( GENESIS_CXX_FLAGS        "${GENESIS_CXX_FLAGS} ")
// #     set( GENESIS_EXE_LINKER_FLAGS "${GENESIS_EXE_LINKER_FLAGS} --rtlib=compiler-rt" )
// # endif()

// This implementation needs __builtin_mul_overflow, which is buggy on Clang 5-7.
// We currently do not use it anyway, so let's deactivate it completely for now.
// #if ( defined(__GNUC__) || defined(__GNUG__) ) && ( !defined(__clang__) || ( __clang_major__ >= 8 ))
#if 0

/**
 * @brief Re-implementation of the C++17 function std::from_chars().
 * Currently not in use and not well tested!
 */
size_t parse_unsigned_integer_from_chars_( utils::InputStream& source )
{
    // Re-implementation of the gcc from_chars() code.
    // https://github.com/gcc-mirror/gcc/blob/12bb62fbb47bd2848746da53c72ed068a4274daf/libstdc++-v3/include/std/charconv
    // Currently not in use and not well tested!

    // Get the internals of the input stream.
    auto buffer_pair = source.buffer();
    auto buffer = buffer_pair.first;
    size_t data_pos = 0;
    size_t data_end = buffer_pair.second;

    // Prepare. We alias T, in case we want to refactor to a template function at some point.
    using T = size_t;
    using namespace utils;
    T x = 0;

    // Hardcoded base 10. See below for other version that allows to select base.
    auto raise_and_add_ = []( T& val, unsigned char c ) {
        return !(
            __builtin_mul_overflow( val, 10, &val ) ||
            __builtin_add_overflow( val, c, &val )
        );
    };
    // int const base = 10;
    // auto raise_and_add_ = [base]( T& val, unsigned char c ) {
    //     return !(
    //         __builtin_mul_overflow( val, base, &val ) ||
    //         __builtin_add_overflow( val, c, &val )
    //     );
    // };

    auto from_chars_digit_ = [&]( char const*& first, char const* last, T& val ) {
        while( first != last ) {
            char const c = *first;
            if( is_digit(c) ) {
                if( !raise_and_add_(val, c - '0') ) {
                    return false;
                }
                first++;
            } else {
                return true;
            }
        }
        return true;
    };

    char const* start = &buffer[ data_pos ];
    char const* end   = &buffer[ data_end ];
    auto const valid = from_chars_digit_( start, end, x );
    auto const dist = start - &buffer[ data_pos ];

    if( dist == 0 ) {
        throw std::runtime_error(
            "Expecting integer in " + source_name() + " at " + at() + "."
        );
    } else if( !valid ) {
        throw std::overflow_error(
            "Numerical overflow in " + source_name() + " at " + at() + "."
        );
    } else if( std::is_signed<T>::value ) {
        assert( false );
        // T tmp;
        // if (__builtin_mul_overflow(x, sign, &tmp)) {
        //     throw std::overflow_error(
        //         "Numerical overflow in " + source_name() + " at " + at() + "."
        //     );
        // }
    }

    // Move to where we the parsing left us.
    data_pos += dist;
    source.jump_unchecked( data_pos );
    return x;
}

#endif // ( defined(__GNUC__) || defined(__GNUG__) ) && ( !defined(__clang__) || ( __clang_major__ >= 8 ))

// -------------------------------------------------------------------------
//     parse_unsigned_integer_std_from_chars_
// -------------------------------------------------------------------------

// Only use C++17 code if we are compiled with that version.
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

/**
 * @brief Another speedup technique using std::from_chars(),
 * which however only works when compiled with C++17 or later.
 */
size_t parse_unsigned_integer_std_from_chars_( utils::InputStream& source )
{
    // Uses the C++17 std::from_chars() function.
    // Currently not in use and not well tested!

    // Get the internals of the input stream.
    auto buffer_pair = source.buffer();
    auto buffer = buffer_pair.first;
    size_t data_pos = 0;
    size_t data_end = buffer_pair.second;

    // Prepare. We alias T, in case we want to refactor to a template function at some point.
    using T = size_t;
    using namespace utils;
    T x = 0;

    // Fastest method accoing to
    // https://www.fluentcpp.com/2018/07/27/how-to-efficiently-convert-a-string-to-an-int-in-c/
    // is from_chars(), so let's us it!

    auto const conv = std::from_chars( &buffer[ data_pos ], &buffer[ data_end ], x );

    // How many chars did we consume?
    auto const dist = conv.ptr - &buffer[ data_pos ];

    // Check that we processed at least one digit, as this function is only called when the
    // input format requires an integer. This is equivalent to the check in the non C++17 version
    // below for data_pos >= data_end || ! is_digit( buffer[ data_pos ] )
    if( dist == 0 ) {
        throw std::runtime_error(
            "Expecting integer in " + source.source_name() + " at " + source.at() + "."
        );
    }

    if( conv.ec != std::errc() ) {
        if( conv.ec == std::errc::result_out_of_range ) {
            throw std::overflow_error(
                "Numerical overflow in " + source.source_name() + " at " + source.at() + "."
            );
        } else if( conv.ec == std::errc::invalid_argument ) {
            // Cannot happen, as we above checked that there is at least one digit.
            assert( false );
        } else {
            // Cannot happen, as we caught every case of `ec`.
            assert( false );
        }

        // In either case, we need to stop here.
        throw std::overflow_error(
            "Integer parsing error in " + source.source_name() + " at " + source.at() + "."
        );
    }

    // Move to where we the parsing left us.
    data_pos += dist;
    source.jump_unchecked( data_pos );
    return x;
}

#endif // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

// -------------------------------------------------------------------------
//     parse_unsigned_integer_naive_
// -------------------------------------------------------------------------

/**
 * @brief Naive parsing that simply loops over chars.
 */
size_t parse_unsigned_integer_naive_( utils::InputStream& source )
{
    // Get the internals of the input stream.
    auto buffer_pair = source.buffer();
    auto buffer = buffer_pair.first;
    size_t data_pos = 0;
    size_t data_end = buffer_pair.second;

    // Prepare. We alias T, in case we want to refactor to a template function at some point.
    using T = size_t;
    using namespace utils;
    T x = 0;

    if( data_pos >= data_end || ! utils::is_digit( buffer[ data_pos ] ) ) {
        throw std::runtime_error(
            "Expecting digit in " + source.source_name() + " at " + source.at() + "."
        );
    }

    while(( data_pos < data_end ) && utils::is_digit( buffer[ data_pos ] )) {
        T y = buffer[ data_pos ] - '0';

        if( x > ( std::numeric_limits<T>::max() - y ) / 10 ) {
            throw std::overflow_error(
                "Numerical overflow in " + source.source_name() + " at " + source.at() + "."
            );
        }

        x = 10 * x + y;

        // In the original function that was not part of this class, we simply called
        // advance() here, to move to the next char. However, here, we already know that
        // we have data_pos < data_end, and that we do not have a new line.
        // Furthermore, we also can ignore the update for block length while in this loop
        // (or maybe even completely), as it does not matter much if we move a bit into the
        // second block before starting the reading thread again. This loop here cannot
        // iterate that many times anyway before we overflow the interger.
        // So let's simply move on to the next char.
        assert( data_pos < data_end );
        assert( buffer[ data_pos ] != '\n' );
        ++data_pos;
    }

    // Move to where we the parsing left us.
    source.jump_unchecked( data_pos );
    return x;
}

// -------------------------------------------------------------------------
//     parse_unsigned_integer_size_t
// -------------------------------------------------------------------------

size_t parse_unsigned_integer_size_t( utils::InputStream& source )
{
    // Select the fastest alternative available for a given compiler and C++ version.
    #if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

        // If we have GCC or Clang, use our own handcrafted fast-as-hell implementation.
        return parse_unsigned_integer_gcc_intrinsic_( source );

    // #elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    //
    //     // Otherwise, if this is C++17, at least use its own fast version,
    //     // that can use some compiler intrinsics itself.
    //     return parse_unsigned_integer_std_from_chars_();

    #else

        // If neither, just use the slow, naive loop.
        return parse_unsigned_integer_naive_( source );

    #endif
}

// =================================================================================================
//     General Number String
// =================================================================================================

std::string parse_number_string(
    utils::InputStream& source
) {
    // Parse the format [+-][123][.456][eE[+-]789]
    std::string result;

    // Need to keep track whether we found a number.
    bool found_digits = false;

    // Sign.
    if( source && ( *source == '+' || *source == '-' )) {
        result += *source;
        ++source;
    }

    // Integer part. Read while char is digit.
    while( source && utils::is_digit( *source )) {
        result += *source;
        ++source;
        found_digits = true;
    }

    // Decimal dot?
    if( source && *source == '.' ) {
        result += '.';
        ++source;
    }

    // Decimal part. Read while char is digit.
    while( source && utils::is_digit( *source )) {
        result += *source;
        ++source;
        found_digits = true;
    }

    // If there was no match so far, stop here.
    // Otherwise, a string starting with "E" will be read as a number...
    if( ! found_digits ) {
        return result;
    }

    // Is there an exponent? If not, we are done.
    if( source && char_match_ci( *source, 'e' ) ) {
        result += *source;
        ++source;
    } else {
        return result;
    }

    // Sign.
    if( source && ( *source == '+' || *source == '-' )) {
        result += *source;
        ++source;
    }

    // Exponent. Read while char is digit.
    while( source && utils::is_digit( *source )) {
        result += *source;
        ++source;
    }

    return result;
}

// =================================================================================================
//     String
// =================================================================================================

std::string parse_quoted_string(
    utils::InputStream& source,
    bool use_escapes,
    bool use_twin_quotes,
    bool include_qmarks
) {
    // Prepare the return value.
    std::string value = "";

    // Nothing to do.
    if( !source ) {
        return value;
    }

    // Read the introductory quotation mark. We will read until it appears again.
    char qmark = *source;
    ++source;

    // Include the quotation mark if needed.
    if( include_qmarks ) {
        value += qmark;
    }

    bool found_closing_qmark = false;
    while( source ) {

        // Treat quotation marks.
        if( *source == qmark ) {
            ++source;

            // This is the end if we are not looking for double qmarks.
            if( ! use_twin_quotes ) {
                found_closing_qmark = true;
                break;
            }

            // If we are here, this is potentially a double qmark.
            // If so, it belongs to the result string. If not, this is the end.
            if( source && *source == qmark ) {
                value += qmark;
            } else {
                found_closing_qmark = true;
                break;
            }

        // Treat escape sequences.
        } else if( *source == '\\' && use_escapes ) {

            // Skip the backslash.
            ++source;

            // We found an escaping backslash. This cannot be the end of the stream.
            if( !source ) {
                throw std::runtime_error(
                    "Unexpected end of " + source.source_name() + " at " + source.at()
                    + ". Expecting escape sequence."
                );
            }

            // Turn the char after the backslash into its correct de-escaped char.
            value += deescape( *source );

        // Treat normal (non-escape) chars.
        } else {
            value += *source;
        }

        // Next char.
        ++source;
    }

    // We need to find the closing qmark, otherwise it's an error.
    // This case only occurs if the stream ends before the qmark is found, so assert this.
    // (This is not true the other way round: the stream can have reached its end right after
    // the closing qmark!)
    if( ! found_closing_qmark ) {
        assert( ! source );
        throw std::runtime_error(
            "Unexpected end of " + source.source_name() + " at " + source.at()
            + ". Expected closing quotation mark."
        );
    }

    // Finish the return value.
    if( include_qmarks ) {
        value += qmark;
    }
    return value;
}

} // namespace utils
} // namespace genesis
