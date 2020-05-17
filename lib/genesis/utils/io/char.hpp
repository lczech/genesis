#ifndef GENESIS_UTILS_IO_CHAR_H_
#define GENESIS_UTILS_IO_CHAR_H_

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
#include <climits>
#include <string>
#include <type_traits>

namespace genesis {
namespace utils {

// =================================================================================================
//     ASCII Char Functions
// =================================================================================================

/**
 * @brief Implementation detail for is_ascii(char).
 */
template <typename T>
constexpr bool is_ascii(std::true_type /* signed */, T c) noexcept
{
    // All positive values are valid ASCII codes
    return c >= 0;
}

/**
 * @brief Implementation detail for is_ascii(char).
 */
template <typename T>
constexpr bool is_ascii(std::false_type /* unsigned */, T c) noexcept
{
    // All values less than or equal to 0x7F (DEL) are valid ASCII codes
    return c <= 0x7F;
}

/**
 * @brief Return whether a char is pure ASCII, that is, in the range [0, 127].
 */
constexpr bool is_ascii(char c) noexcept
{
    static_assert(0x7F <= CHAR_MAX, "The compiler is not using ASCII. We cannot work like that!");
    return is_ascii(std::is_signed<char>{}, c);
}

/**
 * @brief Return whether a char is a control character, according to `iscntrl` of the cctype> `heade`
 * but ASCII-only.
 */
constexpr bool is_cntrl(char c) noexcept
{
    return c == 0x7F || (c >= 0 && c <= 0x1F);
}

/**
 * @brief Return whether a char is a control character, excluding white spaces, ASCII-only.
 */
constexpr bool is_control(char c) noexcept
{
    return (c >= 0x00 && c <= 0x08) || (c >= 0x0E && c <= 0x1F) || c == 0x7F;
}

/**
 * @brief Return whether a char is a digit (0-9), ASCII-only.
 */
constexpr bool is_digit(char c) noexcept
{
    static_assert('0' + 9 == '9', "The compiler is not using ASCII. We cannot work like that!");
    return c >= '0' && c <= '9';
}

/**
 * @brief Return whether a char is a hexadecimal digit (0-9 or A-F or a-f), ASCII-only.
 */
constexpr bool is_xdigit(char c) noexcept
{
    static_assert('A' + 5 == 'F', "The compiler is not using ASCII. We cannot work like that!");
    static_assert('a' + 5 == 'f', "The compiler is not using ASCII. We cannot work like that!");
    return is_digit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/**
 * @brief Return whether a char is a lower case letter (a-z), ASCII-only.
 */
constexpr bool is_lower(char c) noexcept
{
    static_assert('a' + 25 == 'z', "The compiler is not using ASCII. We cannot work like that!");
    return c >= 'a' && c <= 'z';
}

/**
 * @brief Return whether a char is an upper case letter (A-Z), ASCII-only.
 */
constexpr bool is_upper(char c) noexcept
{
    static_assert('A' + 25 == 'Z', "The compiler is not using ASCII. We cannot work like that!");
    return c >= 'A' && c <= 'Z';
}

/**
 * @brief Return whether a char is a letter (a-z or A-Z), ASCII-only.
 */
constexpr bool is_alpha(char c) noexcept
{
    return is_lower(c) || is_upper(c);
}

/**
 * @brief Return whether a char is a letter (a-z or A-Z) or a digit (0-9), ASCII-only.
 */
constexpr bool is_alnum(char c) noexcept
{
    return is_alpha(c) || is_digit(c);
}

/**
 * @brief Return whether a char is a punctuation mark, according to `ispunct` of the `cctype`
 * header, but ASCII-only.
 */
constexpr bool is_punct(char c) noexcept
{
    static_assert('!' + 1 == '"',  "The compiler is not using ASCII. We cannot work like that!");
    static_assert(':' + 1 == ';',  "The compiler is not using ASCII. We cannot work like that!");
    static_assert('[' + 1 == '\\', "The compiler is not using ASCII. We cannot work like that!");
    static_assert('{' + 1 == '|',  "The compiler is not using ASCII. We cannot work like that!");

    return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

/**
 * @brief Return whether a char is a punctuation mark, according to `isgraph` of the `cctype`
 * header, but ASCII-only.
 */
constexpr bool is_graph(char c) noexcept
{
    return c >= '!' && c <='~';
}

/**
 * @brief Return whether a char is either a space or a tab character.
 */
constexpr bool is_blank(char c) noexcept
{
    return c == '\t' || c == ' ';
}

/**
 * @brief Return whether a char is either a new line or a carriage return character.
 */
constexpr bool is_newline(char c) noexcept
{
    return c == '\n' || c == '\r';
}

/**
 * @brief Return whether a char is some other white space charater that is neither space, tab,
 * new line, or carriage return - that is, whether it is a form feed or a vertical tab.
 */
constexpr bool is_other_space(char c) noexcept
{
    return c == '\f' || c == '\v';
}

/**
 * @brief Return whether a char is some form of white space charater, so either space, tab,
 * new line, carriage return, form feed, or vertical tab.
 */
constexpr bool is_space(char c) noexcept
{
    return is_blank(c) || is_newline(c) || is_other_space(c);
}

/**
 * @brief Return whether a char is a printable character, according to `isprint` of the `cctype`
 * header, but ASCII-only.
 */
constexpr bool is_print(char c) noexcept
{
    return c >= ' ' && c <= '~';
}

// =================================================================================================
//     Conversion Functions
// =================================================================================================

/**
 * @brief Return the lower case version of a letter, ASCII-only.
 */
constexpr char to_lower(char c) noexcept
{
    static_assert('z' - 'Z' == 0x20,  "The compiler is not using ASCII. We cannot work like that!");
    return ( 'A' <= c && c <= 'Z' ) ? c + 0x20 : c;
}

/**
 * @brief Return the upper case version of a letter, ASCII-only.
 */
constexpr char to_upper(char c) noexcept
{
    static_assert('z' - 'Z' == 0x20,  "The compiler is not using ASCII. We cannot work like that!");
    return ( 'a' <= c && c <= 'z' ) ? c - 0x20 : c;
}

// =================================================================================================
//     Chars Helper Functions
// =================================================================================================

/**
 * @brief Return whether two chars are the same, case insensitive, and ASCII-only.
 */
constexpr bool char_match_ci( char c1,  char c2 ) noexcept
{
    return to_lower( c1 ) == to_lower( c2 );
    // Locale aware version, not used any more:
    // return std::tolower( static_cast<unsigned char>( c1 )) == std::tolower( static_cast<unsigned char>( c2 ));
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
