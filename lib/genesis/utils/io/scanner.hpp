#ifndef GENESIS_UTILS_IO_SCANNER_H_
#define GENESIS_UTILS_IO_SCANNER_H_

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

#include <assert.h>
#include <cctype>
#include <functional>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Helper Classes and Enums
// =================================================================================================

/**
 * @brief Option to determine how to treat surrounding whitespace when scanning an input stream.
 *
 * This helper enum is used in the input stream scanner functions read_char_or_throw() and
 * affirm_char_or_throw() in order to indicate how they treat whitespace while looking for chars.
 *
 * For checking whether a partcular option is set, it is possible to use the binary and operator `&`:
 *
 *     if( option & SkipWhitespace::kTrailing ) {
 *         // ...
 *     }
 *
 * Of course, simply comparing the values also works.
 */
enum class SkipWhitespace : unsigned char
{
    /**
     * @brief Skip no whitespace. Thus, immediately treat the current input char.
     */
    kNone        = 0,

    /**
     * @brief Skip all whitespace in the input stream, then treat the next non-white char.
     */
    kLeading     = 1,

    /**
     * @brief Treat the current char in the input stream, then skip the following whitespace.
     */
    kTrailing    = 2,

    /**
     * @brief Skip whitespace, treat the first non-white char, then skip all following whitespace.
     */
    kSurrounding = 3
};

/**
 * @brief And-operator to check whether a SkipWhitespace is set.
 *
 * Typical usage:
 *
 *     SkipWhitespace option;
 *     if( option & SkipWhitespace::kTrailing ) {
 *         // Do stuff...
 *     }
 *
 * See ::SkipWhitespace for more information.
 */
inline bool operator & ( SkipWhitespace lhs, SkipWhitespace rhs )
{
    using T = std::underlying_type< SkipWhitespace >::type;
    return static_cast< T >( lhs ) & static_cast< T >( rhs );
}

// =================================================================================================
//     Scanners
// =================================================================================================

// -------------------------------------------------------------------------
//     end of line
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that advances the stream to the end of the line, i.e., to the new line
 * char.
 *
 * If the stream is already at its last line, this function moves to the end of the stream.
 */
template< typename InputStream >
inline void skip_to_end_of_line(
    InputStream& source
) {
    while( source && *source != '\n' ) {
        ++source;
    }
}

/**
 * @brief Lexing function that reads until the end of the line (i.e., to the new line char),
 * and returns the read chars (excluding the new line char).
 *
 * The stream is left at the new line char.
 * If the stream is already at its last line, this function reads to the end of the stream.
 */
template< typename InputStream >
inline std::string read_to_end_of_line(
    InputStream&            source
) {
    std::string target;
    while( source && *source != '\n' ) {
        target += *source;
        ++source;
    }
    return target;
}

// -------------------------------------------------------------------------
//     skip while
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that advances the stream while its current char equals the provided one.
 */
template< typename InputStream >
inline void skip_while(
    InputStream&            source,
    char                    criterion
) {
    while( source && *source == criterion ) {
        ++source;
    }
}

/**
 * @brief Lexing function that advances the stream while its current char fulfills the provided
 * criterion.
 */
template< typename InputStream >
inline void skip_while(
    InputStream&               source,
    std::function<bool (char)> criterion
) {
    while( source && criterion( *source )) {
        ++source;
    }
}

// -------------------------------------------------------------------------
//     skip until
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that advances the stream until its current char equals the provided one.
 */
template< typename InputStream >
inline void skip_until(
    InputStream&            source,
    char                    criterion
) {
    while( source && *source != criterion ) {
        ++source;
    }
}

/**
 * @brief Lexing function that advances the stream until its current char fulfills the provided
 * criterion.
 */
template< typename InputStream >
inline void skip_until(
    InputStream&               source,
    std::function<bool (char)> criterion
) {
    while( source && ! criterion( *source )) {
        ++source;
    }
}

// -------------------------------------------------------------------------
//     read while
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that reads from the stream while its current char equals the provided one.
 * The read chars are returned.
 */
template< typename InputStream >
inline std::string read_while(
    InputStream&            source,
    char                    criterion
) {
    std::string target;
    while( source && *source == criterion ) {
        target += *source;
        ++source;
    }
    return target;
}

/**
 * @brief Lexing function that reads from the stream while its current char fulfills the provided
 * criterion. The read chars are returned.
 */
template< typename InputStream >
inline std::string read_while(
    InputStream&               source,
    std::function<bool (char)> criterion
) {
    std::string target;
    while( source && criterion( *source )) {
        target += *source;
        ++source;
    }
    return target;
}

// -------------------------------------------------------------------------
//     read until
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that reads from the stream until its current char equals the provided one.
 * The read chars are returned.
 */
template< typename InputStream >
inline std::string read_until(
    InputStream&            source,
    char                    criterion
) {
    std::string target;
    while( source && *source != criterion ) {
        target += *source;
        ++source;
    }
    return target;
}

/**
 * @brief Lexing function that reads from the stream until its current char fulfills the provided
 * criterion. The read chars are returned.
 */
template< typename InputStream >
inline std::string read_until(
    InputStream&               source,
    std::function<bool (char)> criterion
) {
    std::string target;
    while( source && ! criterion( *source )) {
        target += *source;
        ++source;
    }
    return target;
}

// -------------------------------------------------------------------------
//     read char
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that reads a single char from the stream and checks whether it equals the
 * provided one.
 *
 * If not, the function throws `std::runtime_error`. The stream is advanced by one position and the
 * char is returned. For a similar function that checks the value of the current char but does
 * not advance in the stream, see affirm_char_or_throw().
 *
 * Using the parameter `skip_ws`, it is possible to skip leading and/or trailing whitespaces
 * before/after treating the `criterion` char. See ::SkipWhitespace for more information.
 */
template< typename InputStream >
inline char read_char_or_throw(
    InputStream&               source,
    char                       criterion,
    SkipWhitespace skip_ws = SkipWhitespace::kNone
) {
    // Skip leading whitespace
    if( skip_ws & SkipWhitespace::kLeading ) {
        skip_while( source, isspace );
    }

    // Check char and move to next.
    if( !source || *source != criterion ) {
        throw std::runtime_error(
            std::string("Expecting '") + criterion + "' at " + source.at() + "."
        );
    }
    assert( source && *source == criterion );
    ++source;

    // Skip trailing whitespace
    if( skip_ws & SkipWhitespace::kTrailing ) {
        skip_while( source, isspace );
    }

    return criterion;
}

/**
 * @brief Lexing function that reads a single char from the stream and checks whether it fulfills
 * the provided criterion.
 *
 * If not, the function throws `std::runtime_error`. The stream is advanced by one position and the
 * char is returned. For a similar function that checks the value of the current char but does
 * not advance in the stream, see affirm_char_or_throw().
 *
 * Using the parameter `skip_ws`, it is possible to skip leading and/or trailing whitespaces
 * before/after treating the `criterion` char. See ::SkipWhitespace for more information.
 */
template< typename InputStream >
inline char read_char_or_throw(
    InputStream&               source,
    std::function<bool (char)> criterion,
    SkipWhitespace skip_ws = SkipWhitespace::kNone
) {
    // Skip leading whitespace
    if( skip_ws & SkipWhitespace::kLeading ) {
        skip_while( source, isspace );
    }

    // Check char and move to next.
    if( !source || ! criterion( *source )) {
        throw std::runtime_error(
            "Unexpected char at " + source.at() + "."
        );
    }
    assert( source );
    auto chr = *source;
    ++source;

    // Skip trailing whitespace
    if( skip_ws & SkipWhitespace::kTrailing ) {
        skip_while( source, isspace );
    }

    return chr;
}

// -------------------------------------------------------------------------
//     expect char
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that checks whether the current char from the stream equals the
 * provided one.
 *
 * If not, the function throws `std::runtime_error`. The stream is not advanced (i.e., it stays at
 * its current position). For a similar function that reads (i.e., also advances) the current char
 * from the stream, see read_char_or_throw().
 *
 * Using the parameter `skip_ws`, it is possible to skip leading and/or trailing whitespaces
 * before/after treating the `criterion` char. See ::SkipWhitespace for more information.
 */
template< typename InputStream >
inline void affirm_char_or_throw(
    InputStream&            source,
    char                    criterion,
    SkipWhitespace skip_ws = SkipWhitespace::kNone
) {
    // Skip leading whitespace
    if( skip_ws & SkipWhitespace::kLeading ) {
        skip_while( source, isspace );
    }

    // Check char.
    if( !source || *source != criterion ) {
        throw std::runtime_error(
            std::string("Expecting '") + criterion + "' at " + source.at() + "."
        );
    }

    // Skip trailing whitespace
    if( skip_ws & SkipWhitespace::kTrailing ) {
        skip_while( source, isspace );
    }
}

/**
 * @brief Lexing function that checks whether the current char from the stream fulfills
 * the provided criterion.
 *
 * If not, the function throws `std::runtime_error`. The stream is not advanced (i.e., it stays at
 * its current position). For a similar function that reads (i.e., also advances) the current char
 * from the stream, see read_char_or_throw().
 *
 * Using the parameter `skip_ws`, it is possible to skip leading and/or trailing whitespaces
 * before/after treating the `criterion` char. See ::SkipWhitespace for more information.
 */
template< typename InputStream >
inline void affirm_char_or_throw(
    InputStream&               source,
    std::function<bool (char)> criterion,
    SkipWhitespace skip_ws = SkipWhitespace::kNone
) {
    // Skip leading whitespace
    if( skip_ws & SkipWhitespace::kLeading ) {
        skip_while( source, isspace );
    }

    // Check char.
    if( !source || ! criterion( *source )) {
        throw std::runtime_error(
            "Unexpected char at " + source.at() + "."
        );
    }

    // Skip trailing whitespace
    if( skip_ws & SkipWhitespace::kTrailing ) {
        skip_while( source, isspace );
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
