#ifndef GENESIS_UTILS_STRING_STRING_H_
#define GENESIS_UTILS_STRING_STRING_H_

/**
 * @brief Provides some commonly used string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     First: function declaractions.
// =================================================================================================

std::vector<std::string> string_split (
    const std::string& str,
    const std::string& delimiters = " ",
    const bool trim_empty = true
);

std::string string_replace_all (
    const std::string& text,
    const std::string& search,
    const std::string& replace
);

std::string string_escape   (const std::string& text);

std::string string_deescape (const std::string& text);

std::string string_unify_newlines (const std::string& s);

// =================================================================================================
//     From here on: only inline functions.
// =================================================================================================

// -----------------------------------------------------------------------------
//     Chars
// -----------------------------------------------------------------------------

/** @brief Returns whether a char is a digit (0-9). */
inline bool char_is_digit (const char c)
{
    return ('0' <= c) && (c <= '9');
}

/** @brief Returns whether a char is a sign (+-). */
inline bool char_is_sign (const char c)
{
    return ('+' == c) || ('-' == c);
}

/**
 * @brief Returns whether two chars are the same, case insensitive.
 */
inline bool char_match(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

// -----------------------------------------------------------------------------
//     String Output
// -----------------------------------------------------------------------------

/**
 * @brief Returns a precise(er than to_string) string representation of the input value.
 */
template <typename T>
inline std::string to_string_precise (const T value, const int precision = 6)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

// -----------------------------------------------------------------------------
//     Trimming
// -----------------------------------------------------------------------------

/**
 * @brief Returns a copy of the input string, with left trimmed white spaces.
 */
inline std::string string_trim_right (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(0, s.find_last_not_of(delimiters) + 1);
}

/**
 * @brief Returns a copy of the input string, with right trimmed white spaces.
 */
inline std::string string_trim_left (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return s.substr(s.find_first_not_of(delimiters));
}

/**
 * @brief Returns a copy of the input string, with trimmed white spaces.
 */
inline std::string string_trim (
    const std::string& s,
    const std::string& delimiters = " \f\n\r\t\v"
) {
    return string_trim_left(string_trim_right(s, delimiters), delimiters);
}

} // namespace genesis

#endif // include guard
