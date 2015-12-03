#ifndef GENESIS_UTILS_TEXT_STRING_H_
#define GENESIS_UTILS_TEXT_STRING_H_

/**
 * @brief Provides some commonly used string utility functions.
 *
 * @file
 * @ingroup utils
 */

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

/**
 * @brief Returns a copy of the input string, with left trimmed white spaces.
 */
inline std::string trim_right (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(0, s.find_last_not_of(delimiters) + 1);
}

/**
 * @brief Returns a copy of the input string, with right trimmed white spaces.
 */
inline std::string trim_left (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(s.find_first_not_of(delimiters));
}

/**
 * @brief Returns a copy of the input string, with trimmed white spaces.
 */
inline std::string trim (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return trim_left(trim_right(s, delimiters), delimiters);
}

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

} // namespace text
} // namespace genesis

#endif // include guard
