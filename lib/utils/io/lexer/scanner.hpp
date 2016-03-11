#ifndef GENESIS_UTILS_IO_LEXER_SCANNER_H_
#define GENESIS_UTILS_IO_LEXER_SCANNER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/counting_istream.hpp"

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
inline void skip_while(
    utils::CountingIstream& source,
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
inline void skip_while(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    while( source && criterion(*source) ) {
        ++source;
    }
}

// -------------------------------------------------------------------------
//     skip until
// -------------------------------------------------------------------------

/**
 * @brief Lexing function that advances the stream unts its current char equals the provided one.
 */
inline void skip_until(
    utils::CountingIstream& source,
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
inline void skip_until(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    while( source && !criterion(*source) ) {
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
inline std::string read_while(
    utils::CountingIstream& source,
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
inline std::string read_while(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    std::string target;
    while( source && criterion(*source) ) {
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
inline std::string read_until(
    utils::CountingIstream& source,
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
inline std::string read_until(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    std::string target;
    while( source && !criterion(*source) ) {
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
 * provided one. If not, the function throws `std::runtime_error`. The stream is advanced by one
 * position and the char is returned.
 */
inline char read_char(
    utils::CountingIstream& source,
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
 * the provided criterion. If not, the function throws `std::runtime_error`. The stream is advanced
 * by one position and the char is returned.
 */
inline char read_char(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    if( !source || !criterion(*source) ) {
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
 * provided one. If not, the function throws `std::runtime_error`.
 */
inline void check_char(
    utils::CountingIstream& source,
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
 * the provided criterion. If not, the function throws `std::runtime_error`.
 */
inline void check_char(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    if( !source || !criterion(*source) ) {
        throw std::runtime_error(
            "Unexpected char at " + source.at() + "."
        );
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
