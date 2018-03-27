#ifndef GENESIS_UTILS_TEXT_CHAR_H_
#define GENESIS_UTILS_TEXT_CHAR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
#include <iostream>
#include <string>
#include <sstream>

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
 * @brief Return whether a char is valid in a number, i.e., a digit, a dot, plus, minus, or 'e'/'E'.
 */
inline bool char_is_number_part( char const c )
{
    return char_is_digit( c ) || char_is_sign( c ) || c == '.' || c == 'e' || c == 'E';
}

/**
 * @brief Return whether two chars are the same, case insensitive.
 */
inline bool char_match_ci(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
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
 * @brief Return the hex representation of a char.
 *
 * By default, just the two-byte hex presentation is returned (e.g., `4e` for `N`).
 * If @p full is `true`, a text-representation is returned instead, using the form
 *
 *     'N' (0x4e)
 *
 * instead.
 */
inline std::string char_to_hex( char c, bool full = false )
{
    std::stringstream ss;
    if( full ) {
        ss << "'" << std::string( 1, c ) << "' (0x" << std::hex << static_cast<int>( c ) << ")";
    } else {
        ss << std::hex << static_cast<int>( c );
    }
    return ss.str();
}

} // namespace utils
} // namespace genesis

#endif // include guard
