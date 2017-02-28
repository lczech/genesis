#ifndef GENESIS_UTILS_IO_PARSER_H_
#define GENESIS_UTILS_IO_PARSER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/input_stream.hpp"

#include <assert.h>
#include <cctype>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Integer
// =================================================================================================

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
    T x = 0;

    if( ! source || ! isdigit( *source ) ) {
        throw std::runtime_error(
            "Expecting digit in " + source.source_name() + " at " + source.at() + "."
        );
    }

    while( source && isdigit( *source )) {
        T y = *source - '0';

        if( x > ( std::numeric_limits<T>::max() - y ) / 10 ) {
            throw std::overflow_error(
                "Numerical overflow in " + source.source_name() + " at " + source.at() + "."
            );
        }

        x = 10 * x + y;
        ++source;
    }
    return x;
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
    if( !source ) {
        throw std::runtime_error(
            "Expecting number in " + source.source_name() + " at " + source.at() + "."
        );
    }

    if( *source == '-' ) {
        ++source;

        if( ! source || ! isdigit( *source ) ) {
            throw std::runtime_error(
                "Expecting digit in " + source.source_name() + " at " + source.at() + "."
            );
        }

        T x = 0;
        while( source && isdigit( *source )) {
            T y = *source - '0';

            if( x < ( std::numeric_limits<T>::min() + y ) / 10 ) {
                throw std::underflow_error(
                    "Numerical underflow in " + source.source_name() + " at " + source.at() + "."
                );
            }

            x = 10 * x - y;
            ++source;
        }
        return x;
    }

    if( *source == '+' ) {
        ++source;
    }
    return parse_unsigned_integer<T>(source);
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
 * The function stops reading at the first non-fitting digit.
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
    while( source && isdigit( *source )) {
        int y = *source - '0';
        x *= 10;
        x += y;
        ++source;
        found_mantisse = true;
    }

    // Decimal part
    if( source && *source == '.' ) {
        ++source;

        if( ! source || ! isdigit( *source ) ) {
            throw std::runtime_error(
                "Invalid float number in " + source.source_name() + " at " + source.at() + "."
            );
        }

        T pos = 1.0;
        while( source && isdigit( *source )) {
            pos /= 10.0;
            int y = *source - '0';
            x += y * pos;
            ++source;
            found_mantisse = true;
        }
    }

    // We need to have some digits before the exponential part.
    if( ! found_mantisse ) {
        throw std::runtime_error(
            "Invalid float number in " + source.source_name() + " at " + source.at() + "."
        );
    }

    // Exponential part
    if( source && tolower(*source) == 'e' ) {
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
