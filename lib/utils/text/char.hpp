#ifndef GENESIS_UTILS_TEXT_CHAR_H_
#define GENESIS_UTILS_TEXT_CHAR_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace text {

// =================================================================================================
//     Chars Functions
// =================================================================================================

/**
 * @brief Returns whether a char is a digit (0-9).
 */
inline bool char_is_digit (const char c)
{
    return ('0' <= c) && (c <= '9');
}

/**
 * @brief Returns whether a char is a sign (+-).
 */
inline bool char_is_sign (const char c)
{
    return ('+' == c) || ('-' == c);
}

/**
 * @brief Returns whether two chars are the same, case insensitive.
 */
inline bool char_match_ci(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

} // namespace text
} // namespace genesis

#endif // include guard
