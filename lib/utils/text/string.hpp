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

bool equals_ci( std::string const& lhs, std::string const& rhs);

// =================================================================================================
//     Find and Count
// =================================================================================================

size_t count_substring_occurrences( const std::string& str, const std::string& sub );

std::vector<std::string> split (
    const std::string& str,
    const std::string& delimiters = " ",
    const bool trim_empty = true
);

// =================================================================================================
//     Manipulate
// =================================================================================================

std::string replace_all (
    const std::string& text,
    const std::string& search,
    const std::string& replace
);

std::string trim_right (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
);

std::string trim_left (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
);

std::string trim (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
);

// =================================================================================================
//     Normalize
// =================================================================================================

std::string escape   (const std::string& text);

std::string deescape (const std::string& text);

std::string unify_newlines (const std::string& s);

// =================================================================================================
//     Output
// =================================================================================================

std::string to_string_precise (double value, const int precision = 6);

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
