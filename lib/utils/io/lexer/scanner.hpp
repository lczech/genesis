#ifndef GENESIS_UTILS_IO_LEXER_SCANNER_H_
#define GENESIS_UTILS_IO_LEXER_SCANNER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/counting_istream.hpp"

#include <cctype>
#include <functional>

namespace genesis {
namespace lexer {

// =================================================================================================
//     Scanners
// =================================================================================================

// -------------------------------------------------------------------------
//     skip while
// -------------------------------------------------------------------------

inline void skip_while(
    utils::CountingIstream& source,
    char                    criterion
) {
    while( source && *source == criterion ) {
        ++source;
    }
}

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

inline void skip_until(
    utils::CountingIstream& source,
    char                    criterion
) {
    while( source && *source != criterion ) {
        ++source;
    }
}

inline void skip_until(
    utils::CountingIstream&    source,
    std::function<bool (char)> criterion
) {
    while( source && !criterion(*source) ) {
        ++source;
    }
}

// -------------------------------------------------------------------------
//     copy while
// -------------------------------------------------------------------------

inline void copy_while(
    utils::CountingIstream& source,
    std::string&            target,
    char                    criterion
) {
    while( source && *source == criterion ) {
        target += *source;
        ++source;
    }
}

inline void copy_while(
    utils::CountingIstream&    source,
    std::string&               target,
    std::function<bool (char)> criterion
) {
    while( source && criterion(*source) ) {
        target += *source;
        ++source;
    }
}

// -------------------------------------------------------------------------
//     copy until
// -------------------------------------------------------------------------

inline void copy_until(
    utils::CountingIstream& source,
    std::string&            target,
    char                    criterion
) {
    while( source && *source != criterion ) {
        target += *source;
        ++source;
    }
}

inline void copy_until(
    utils::CountingIstream&    source,
    std::string&               target,
    std::function<bool (char)> criterion
) {
    while( source && !criterion(*source) ) {
        target += *source;
        ++source;
    }
}

} // namespace lexer
} // namespace genesis

#endif // include guard
