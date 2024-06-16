#ifndef GENESIS_UTILS_IO_PARSER_H_
#define GENESIS_UTILS_IO_PARSER_H_

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

#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cmath>
#include <cctype>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Integer
// =================================================================================================

/**
 * @brief Parse the input @p source as an unsigned int into a size_t.
 *
 * This is the basic parsing function that we use internally for the function templates
 * parse_unsigned_integer() and parse_signed_integer(). That means, our internal int size
 * cannot exceed 64 bits. That should be okay.
 */
size_t parse_unsigned_integer_size_t( utils::InputStream& source );

/**
 * @brief Read an unsigned integer from a stream and return it.
 *
 * The function expects a sequence of digits. The current char in the stream has to be a digit,
 * otherwise the function throws `std::runtime_error`. It stops reading at the first non-digit.
 * In case the value range is too small, the function throws `std::overflow_error`.
 */
template<class T>
T parse_unsigned_integer( utils::InputStream& source )
{
    // No need to assert unsignedness here. We will later check that casting to the desired
    // type worked, and we test for the correct sign there as well, so that it workes for
    // signed types.
    // static_assert(
    //     std::is_unsigned<T>::value,
    //     "Need unsigned type for parse_unsigned_integer()"
    // );

    auto const x = parse_unsigned_integer_size_t( source );

    // We parsed as largest, and now try to cast to desired type,
    // testing that back-conversion gives the same value and correct sign.
    auto const r = static_cast<T>(x);
    if( static_cast<size_t>(r) != x || r < 0 ) {
        throw std::overflow_error(
            "Numerical overflow in " + source.source_name() + " at " + source.at() + "."
        );
    }
    return r;
}

/**
 * @brief Read a signed integer from a stream and return it.
 *
 * The function expects a sequence of digits, possibly with a leading `+` or `-`.
 * The first char after that has to be a digit, otherwise the function throws `std::runtime_error`.
 * It stops reading at the first non-digit.
 * In case the value range is too small, the function throws `std::overflow_error`,
 * or `underflow_error`, respectively.
 */
template<class T>
T parse_signed_integer( utils::InputStream& source )
{
    static_assert(
        std::is_signed<T>::value,
        "Need signed type for parse_signed_integer()"
    );

    if( !source ) {
        throw std::runtime_error(
            "Expecting number in " + source.source_name() + " at " + source.at() + "."
        );
    }

    int sign = 1;
    if( *source == '-' || *source == '+' ) {
        if( *source == '-' ) {
            sign = -1;
        }
        ++source;
    }

    // Parse as largest, but report overflow as underflow if negative.
    size_t x;
    try {
        x = parse_unsigned_integer_size_t( source );
    } catch( std::overflow_error const& ) {
        if( sign == -1 ) {
            throw std::underflow_error(
                "Numerical overflow in " + source.source_name() + " at " + source.at() + "."
            );
        } else {
            throw;
        }
    }

    // Lastly, try to cast to desired type, testing that back-conversion
    // gives the same value and sign.
    // The back-cast of `sign * r` is always valid, as the negative range of signed ints
    // is smaller than the positive, so if it's negative, multiplying by -1 will always result
    // in a valid value.
    auto const r = static_cast<T>( sign * static_cast<T>(x) );
    if( static_cast<size_t>( sign * r ) != x || !( r == 0 || (sign < 0) == (r < 0) )) {
        throw std::underflow_error(
            "Numerical overflow in " + source.source_name() + " at " + source.at() + "."
        );
    }
    return r;
}

/**
 * @brief Alias for parse_signed_integer().
 */
template<class T>
T parse_integer( utils::InputStream& source )
{
    return parse_signed_integer<T>(source);
}

// =================================================================================================
//     Float
// =================================================================================================

/**
 * @brief Read a floating point number from a stream and return it.
 *
 * The number is expected to be in the following format:
 *
 *     [+-][123][.456][eE[+-]789]
 *
 * It furthermore supports signed `nan`, `inf`, and `infinity` as special strings, case insensitive.
 * The function stops reading at the first non-fitting character.
 * It throws an `std::overflow_error` or `underflow_error` in case that the exponent (the part
 * after the 'E') does not fit into integer value range.
 */
template<class T>
T parse_float( utils::InputStream& source )
{
    T x = 0.0;

    if( !source ) {
        throw std::runtime_error(
            "Expecting float number in " + source.source_name() + " at " + source.at() + "."
        );
    }

    // Sign
    bool is_neg = false;
    if( *source == '-' ){
        is_neg = true;
        ++source;
    } else if( *source == '+' ) {
        ++source;
    }

    // Integer Part
    bool found_mantisse = false;
    while( source && utils::is_digit( *source )) {
        int y = *source - '0';
        x *= 10;
        x += y;
        ++source;
        found_mantisse = true;
    }

    // Decimal part
    if( source && *source == '.' ) {
        ++source;

        if( ! source || ! utils::is_digit( *source ) ) {
            throw std::runtime_error(
                "Invalid number in " + source.source_name() + " at " + source.at() + "."
            );
        }

        T pos = 1.0;
        while( source && utils::is_digit( *source )) {
            pos /= 10.0;
            int y = *source - '0';
            x += y * pos;
            ++source;
            found_mantisse = true;
        }
    }

    // Special cases.
    if( ! found_mantisse && source ) {
        static_assert(
            std::numeric_limits<double>::is_iec559,
            "Compiler does not use ISO IEC559 / IEEE754 standard."
        );

        // Check against usual special cases. We use value == 0.0 to indicate the default case
        // (no match), and any other value will be what we found then.
        double const sign = is_neg ? -1.0 : 1.0;
        double value = 0.0;
        auto const buffer = source.buffer();
        if( buffer.second >= 8 && strncasecmp( buffer.first, "infinity", 8 ) == 0 ) {
            source.jump_unchecked( 8 );
            value = sign * std::numeric_limits<double>::infinity();
        } else if( buffer.second >= 3 && strncasecmp( buffer.first, "inf", 3 ) == 0 ) {
            source.jump_unchecked( 3 );
            value = sign * std::numeric_limits<double>::infinity();
        }
        if( buffer.second >= 3 && strncasecmp( buffer.first, "nan", 3 ) == 0 ) {
            source.jump_unchecked( 3 );
            value = sign * std::numeric_limits<double>::quiet_NaN();
        }

        // If we found something, we need to check that this is not part of some other
        // random longer string. The next char needs to be something non-alnum.
        // Bit stricter than usual double parsing, but should cover all cases except for
        // the `nanCHAR` notation. If this check fails, we instead continue,
        // which will throw in the next check below.
        if( value != 0.0 && ( !source || ! is_alnum( *source ))) {
            return value;
        }
    }

    // We need to have some digits before the exponential part.
    if( ! found_mantisse ) {
        throw std::runtime_error(
            "Invalid number in " + source.source_name() + " at " + source.at() + "."
        );
    }

    // Exponential part
    if( source && utils::to_lower(*source) == 'e' ) {
        ++source;

        // Read the exp. If there are no digits, this throws.
        int e = parse_signed_integer<int>( source );

        if( e != 0 ) {
            T base;
            if( e < 0 ) {
                base = 0.1;
                e = -e;
            } else {
                base = 10;
            }

            while( e != 1 ) {
                if( ( e & 1 ) == 0 ) {
                    base = base * base;
                    e >>= 1;
                } else {
                    x *= base;
                    --e;
                }
            }
            x *= base;
        }
    }

    // Sign
    if (is_neg) {
        x = -x;
    }

    return x;
}

// =================================================================================================
//     General Number String
// =================================================================================================

/**
 * @brief Read a general number string from an input stream.
 *
 * The function reads from the input as long as it is a valid number of the format
 *
 *     [+-][123][.456][eE[+-]789]
 *
 * and returns this string. This is useful for intermediately parsing a number and then checking
 * whether it is valid. Use parse_float() to turn such a number string into an actual float/double.
 */
std::string parse_number_string(
    utils::InputStream& source
);

// =================================================================================================
//     String
// =================================================================================================

/**
 * @brief Read a string in quotation marks from a stream and return it.
 *
 * The first char of the stream is considered to be the opening quotation mark. Everything up until
 * the closing quotation mark (the same char again, whatever it is) is then read. The stream is
 * then pointing to the char right after the closing quotation mark.
 *
 * If the string ends prematurely, i.e., without the closing quotation mark, or right after a
 * backslash if `use_escapes` is used, the function throws an `std::runtime_error`.
 *
 * @param source Stream to read from.
 *
 * @param use_escapes If this optional parameter is set to `true`, chars preceeded by
 *        a backslash `\` are considered to be "escaped". If the char following the backslash is any
 *        of `r`, `n` or `t`, it is turned into its respective white space equivalent, while all
 *        other chars are copied verbatim to the return string. Thus, by escaping it, a the string
 *        can also include the quotation mark itself. Default of this parameter is `true`.
 *
 * @param use_twin_quotes If this optional parameter is set to `true`, the quotation mark itself
 *        can be escaped using two consecutive quotation marks. This works in addition to escaping
 *        it with a backslash (when `use_escapes` is used).
 *
 * @param include_qmarks The optional parameter `include_qmarks` demtermines whether the quotation
 *        marks are included in the output or not. Default is `false`.
 */
std::string parse_quoted_string(
    utils::InputStream& source,
    bool use_escapes     = true,
    bool use_twin_quotes = false,
    bool include_qmarks  = false
);

} // namespace utils
} // namespace genesis

#endif // include guard
