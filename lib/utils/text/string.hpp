#ifndef GENESIS_UTILS_TEXT_STRING_H_
#define GENESIS_UTILS_TEXT_STRING_H_

/**
 * @brief Provides some commonly used string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace text {

// =================================================================================================
//     Compare
// =================================================================================================

bool equals_ci( std::string const& lhs, std::string const& rhs );

// =================================================================================================
//     Find and Count
// =================================================================================================

size_t count_substring_occurrences( std::string const& str, std::string const& sub );

std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters = " ",
    const bool trim_empty = true
);

// =================================================================================================
//     Manipulate
// =================================================================================================

std::string replace_all (
    std::string const& text,
    std::string const& search,
    std::string const& replace
);

std::string trim_right (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

std::string trim_left (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

std::string trim (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

// =================================================================================================
//     Normalize
// =================================================================================================

std::string to_lower( std::string const& str );
std::string to_upper( std::string const& str );

std::string escape  ( std::string const& text );
std::string deescape( std::string const& text );

std::string unify_newlines( std::string const& s );

// =================================================================================================
//     Output
// =================================================================================================

std::string to_string_precise( double value, const int precision = 6 );

template <typename T>
std::string join( T const& v, std::string const& delimiter ) {
    std::ostringstream s;
    for( auto const& i : v ) {
        if( &i != &v[0] ) {
            s << delimiter;
        }
        s << i;
    }
    return s.str();
}

} // namespace text
} // namespace genesis

#endif // include guard
