#ifndef GENESIS_UTILS_TEXT_CHAR_H_
#define GENESIS_UTILS_TEXT_CHAR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include <cctype>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Chars Functions
// =================================================================================================

/**
 * @brief Return whether a char is a digit (0-9).
 */
inline bool char_is_digit (const char c)
{
    return ('0' <= c) && (c <= '9');
}

/**
 * @brief Return whether a char is a sign (+-).
 */
inline bool char_is_sign (const char c)
{
    return ('+' == c) || ('-' == c);
}

/**
 * @brief Return whether two chars are the same, case insensitive.
 */
inline bool char_match_ci(const char c1, const char c2)
{
    return std::tolower( static_cast<unsigned char>( c1 )) == std::tolower( static_cast<unsigned char>( c2 ));
}

/**
 * @brief Return the lower case of a given char, ascii-only.
 */
inline char to_lower_ascii( char c )
{
    return ( 'A' <= c && c <= 'Z' ) ? c + 0x20 : c;
}

/**
 * @brief Return the upper case of a given char, ascii-only.
 */
inline char to_upper_ascii( char c )
{
    return ( 'a' <= c && c <= 'z' ) ? c - 0x20 : c;
}

/**
 * @brief Return the name and hex representation of a `char`.
 *
 * This is meant for user output, for example to warn about invalid input characters.
 * Hence, by default, a text-representation is returned, using the form
 *
 *     LF (0x0A)
 *     'N' (0x4E)
 *
 * where non-printable characters are abbreviated by their symbol, and printable
 * characters are included in single quotation marks.
 *
 * Non-ASCII characters (those outside of the 0-127 interval) are written in the form
 *
 *     non-ASCII char (0xF7)
 *
 * If @p full is set to `false`, just the two-byte hex presentation is returned (e.g., `4E` for `N`).
 */
std::string char_to_hex( char c, bool full = true );

/**
 * @brief Return the name and hex representation of a `char`, given as an `unsigned char`.
 *
 * This overload is meant for situations were a text input is read from file as `unsigned char`.
 * The rest of the function is identical to char_to_hex( char, bool ), see there for details.
 */
std::string char_to_hex( unsigned char c, bool full = true );

} // namespace utils
} // namespace genesis

#endif // include guard
