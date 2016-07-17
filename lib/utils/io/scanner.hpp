#ifndef GENESIS_UTILS_IO_SCANNER_H_
#define GENESIS_UTILS_IO_SCANNER_H_

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

#include <assert.h>
#include <cctype>
#include <functional>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Scanners
// =================================================================================================

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
 * advance in the stream, see expect_char().
 */
template< typename InputStream >
inline char read_char_if(
    InputStream&            source,
    char                    criterion
) {
    if( !source || *source != criterion ) {
        throw std::runtime_error(
            std::string("Expecting '") + criterion + "' at " + source.at() + "."
        );
    }
    assert( source && *source == criterion );
    ++source;
    return criterion;
}

/**
 * @brief Lexing function that reads a single char from the stream and checks whether it fulfills
 * the provided criterion.
 *
 * If not, the function throws `std::runtime_error`. The stream is advanced by one position and the
 * char is returned. For a similar function that checks the value of the current char but does
 * advance in the stream, see expect_char().
 */
template< typename InputStream >
inline char read_char_if(
    InputStream&               source,
    std::function<bool (char)> criterion
) {
    if( !source || ! criterion( *source )) {
        throw std::runtime_error(
            "Unexpected char at " + source.at() + "."
        );
    }
    assert( source );
    auto chr = *source;
    ++source;
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
 * from the stream, see read_char_if().
 */
template< typename InputStream >
inline void expect_char(
    InputStream&            source,
    char                    criterion
) {
    if( !source || *source != criterion ) {
        throw std::runtime_error(
            std::string("Expecting '") + criterion + "' at " + source.at() + "."
        );
    }
}

/**
 * @brief Lexing function that checks whether the current char from the stream fulfills
 * the provided criterion.
 *
 * If not, the function throws `std::runtime_error`. The stream is not advanced (i.e., it stays at
 * its current position). For a similar function that reads (i.e., also advances) the current char
 * from the stream, see read_char_if().
 */
template< typename InputStream >
inline void expect_char(
    InputStream&               source,
    std::function<bool (char)> criterion
) {
    if( !source || ! criterion( *source )) {
        throw std::runtime_error(
            "Unexpected char at " + source.at() + "."
        );
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
