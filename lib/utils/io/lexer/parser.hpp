#ifndef GENESIS_UTILS_IO_LEXER_PARSER_H_
#define GENESIS_UTILS_IO_LEXER_PARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/counting_istream.hpp"

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
T parse_unsigned_integer( utils::CountingIstream& source )
{
    T x = 0;
    while( source && isdigit( *source )) {
        T y = *source - '0';

        if( x > ( std::numeric_limits<T>::max() - y ) / 10 ) {
            throw std::overflow_error(
                "Numerical overflow at " + source.at() + "."
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
T parse_signed_integer( utils::CountingIstream& source )
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
                    "Numerical underflow at " + source.at() + "."
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
T parse_integer( utils::CountingIstream& source )
{
    return parse_integer<T>(source);
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
T parse_float( utils::CountingIstream& source )
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

/**
 * @brief Read a string in quotation marks from a stream and return it.
 *
 * The first char of the stream is considered to be the opening quotation mark. Everything up until
 * the closing quotation mark (the same char again, whatever it is) is then read.
 *
 * If the optional parameter `use_escapes` is set to `true`, chars preceeded by a backslash are
 * considered to be "escaped". The chars `\r`, `\n` and `\t` are then turned into their
 * respective white space equivalents, while all other chars are copied verbatim to the return
 * string. Thus, by escaping it, a the string can also include the quotation mark itself.
 * Default of this parameter is `true`.
 *
 * The optional parameter `include_qmarks` demtermines whether the quotation marks are included in
 * the output or not. Default is `false`.
 *
 * If the string ends prematurely, i.e., without the closing quotation mark, or right after a
 * backslash if `use_escapes` is used, the function throws an `std::runtime_error`.
 */
std::string parse_quoted_string(
    utils::CountingIstream& source,
    bool use_escapes = true,
    bool include_qmarks = false
) {
    if( !source ) {
        return "";
    }

    // Read the introductory quotation mark. We will read until it appears again.
    char qmark = *source;
    ++source;

    // Prepare the return value.
    std::string value;
    if( include_qmarks ) {
        value += qmark;
    }

    while( source && *source != qmark ) {
        // Treat escape sequences.
        if( *source == '\\' && use_escapes ) {

            ++source;

            // We found an escaping backslash. This cannot be the end of the stream.
            if( !source ) {
                throw std::runtime_error(
                    "Unexpected end of string at " + source.at() + "."
                );
            }

            switch( *source ) {
                case 'r' :
                    value += '\r';
                    break;

                case 'n' :
                    value += '\n';
                    break;

                case 't' :
                    value += '\t';
                    break;

                default :
                    value += *source;
            }
            ++source;

        } else {
            // Treat normal (non-escape) chars.
            value += *source;
            ++source;
        }
    }

    // We need to find the closing qmark, otherwise it's an error.
    if( !source ) {
        throw std::runtime_error(
            "Unexpected end of string at " + source.at() + "."
        );
    }

    assert( source && *source == qmark );
    ++source;

    // Finish the return value.
    if( include_qmarks ) {
        value += qmark;
    }
    return value;
}

} // namespace utils
} // namespace genesis

#endif // include guard
