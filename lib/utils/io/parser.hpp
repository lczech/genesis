#ifndef GENESIS_UTILS_IO_PARSER_H_
#define GENESIS_UTILS_IO_PARSER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "utils/io/input_stream.hpp"

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
 * The function expects a sequence of digits. It stops reading at the first non-digit.
 * In case the value range is too small, the function throws `std::overflow_error`.
 */
template<class T>
T parse_unsigned_integer( utils::InputStream& source )
{
    T x = 0;
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
 * It stops reading at the first non-digit.
 * In case the value range is too small, the function throws `std::overflow_error`,
 * or `underflow_error`, respectively.
 */
template<class T>
T parse_signed_integer( utils::InputStream& source )
{
    if( !source ) {
        return 0;
    }

    if( *source == '-' ) {
        ++source;

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
 *     [+-]123[.456][eE[+-]789]
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
        return x;
    }

    bool is_neg = false;
    if( *source == '-' ){
        is_neg = true;
        ++source;
    } else if( *source == '+' ) {
        ++source;
    }

    while( source && isdigit( *source )) {
        int y = *source - '0';
        x *= 10;
        x += y;
        ++source;
    }

    if( source && ( *source == '.' || *source == ',' )) {
        ++source;

        T pos = 1.0;
        while( source && isdigit( *source )) {
            pos /= 10.0;
            int y = *source - '0';
            x += y * pos;
            ++source;
        }
    }

    if( source && tolower(*source) == 'e' ) {
        ++source;

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

    if (is_neg) {
        x = -x;
    }

    return x;
}

// =================================================================================================
//     String
// =================================================================================================

std::string parse_quoted_string(
    utils::InputStream& source,
    bool use_escapes     = true,
    bool use_twin_quotes = false,
    bool include_qmarks  = false
);

} // namespace utils
} // namespace genesis

#endif // include guard
