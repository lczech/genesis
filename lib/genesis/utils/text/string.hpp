#ifndef GENESIS_UTILS_TEXT_STRING_H_
#define GENESIS_UTILS_TEXT_STRING_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
namespace utils {

// =================================================================================================
//     Compare
// =================================================================================================

/**
 * @brief Compare two strings case insensitive.
 */
bool equals_ci( std::string const& lhs, std::string const& rhs );

/**
 * @brief Return whether a string starts with another string.
 */
bool starts_with( std::string const & text, std::string const & start );

/**
 * @brief Return whether a string ends with another string.
 */
bool ends_with(   std::string const & text, std::string const & ending );

// =================================================================================================
//     Substrings
// =================================================================================================

/**
 * @brief Return the first lines of the text.
 */
std::string head( std::string const& text, size_t lines = 10 );

/**
 * @brief Return the last lines of the text.
 */
std::string tail( std::string const& text, size_t lines = 10 );

// =================================================================================================
//     Find and Count
// =================================================================================================

/**
 * @brief Return the number of (possibly overlapping) occurrences of a substring in a string.
 */
size_t count_substring_occurrences( std::string const& str, std::string const& sub );

/**
 * @brief Spilt a string into parts, given a set of delimiter chars.
 *
 * The string `str` is split using any of the chars in `delimiters` and returned as a vector
 * of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are
 * excluded from the output.
 */
std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters = " ",
    const bool trim_empty = true
);

// =================================================================================================
//     Manipulate
// =================================================================================================

/**
 * @brief Indent each line of `text` with `indentation` and return the result.
 *
 * By default, four spaces are used to indent. Whether the text ends with a new line or not is not
 * changed. Any trailing indentation chars are trimmed, in order to not have trailing whitespaces
 * in the result (except for the new line, if the text ends in one).
 */
std::string indent(
    std::string const& text,
    std::string const& indentation = "    "
);

/**
 * @brief Return a copy of a string, where all occurrences of a search string
 * are replaced by a replace string.
 */
std::string replace_all (
    std::string const& text,
    std::string const& search,
    std::string const& replace
);

/**
 * @brief Replace all occurrences of the `search_chars` in `text` by the `replace` char.
 */
std::string replace_all_chars (
    std::string const& text,
    std::string const& search_chars,
    char               replace
);

/**
 * @brief Return a copy of the input string, with left trimmed white spaces.
 */
std::string trim_right (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

/**
 * @brief Return a copy of the input string, with right trimmed white spaces.
 */
std::string trim_left (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

/**
 * @brief Return a copy of the input string, with trimmed white spaces.
 */
std::string trim (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

// =================================================================================================
//     Normalize
// =================================================================================================

/**
 * @brief Return an all-lowercase copy of the given string, locale-aware.
 */
std::string to_lower(         std::string const& str );

/**
 * @brief Turn the given string to all-lowercase, locale-aware.
 */
void        to_lower_inplace( std::string&       str );

/**
 * @brief Return an all-uppercase copy of the given string, locale-aware.
 */
std::string to_upper(         std::string const& str );

/**
 * @brief Turn the given string to all-uppercase, locale-aware.
 */
void        to_upper_inplace( std::string&       str );

/**
 * @brief Return the lower case of a given char, ascii-only.
 */
char        to_lower_ascii( char c );

/**
 * @brief Return the upper case of a given char, ascii-only.
 */
char        to_upper_ascii( char c );

/**
 * @brief Return an all-lowercase copy of the given string, ascii-only.
 */
std::string to_lower_ascii(         std::string const& str );

/**
 * @brief Turn the given string to all-lowercase, ascii-only.
 */
void        to_lower_ascii_inplace( std::string&       str );

/**
 * @brief Return an all-uppercase copy of the given string, ascii-only.
 */
std::string to_upper_ascii(         std::string const& str );

/**
 * @brief Turn the given string to all-uppercase, ascii-only.
 */
void        to_upper_ascii_inplace( std::string&       str );

/**
 * @brief Return a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string escape  ( std::string const& text );

/**
 * @brief Return a string where backslash-escaped characters are transformed into
 * their respective string form.
 *
 * All occurrences of `backslash + char` in the string are de-escaped. That is, all `\n`, `\t` and
 * `\r` are turned into their respective control sequences, while all other chars folloing a
 * backslash are translated into the char itself (so that e.g., quotation marks or backslashes
 * themself can be escaped).
 *
 * Also see deescape( char c ).
 */
std::string deescape( std::string const& text );

/**
 * @brief Return the de-escaped char for a backslash-escaped char.
 *
 * The function takes the char that follows a backslash in an escaped string and returns its
 * de-escaped char. That is, `n` is turned into a new line (`\n`), `t` is turned into a tab (`\t`)
 * and `r` is turned into a carrier return (`\r`). All other chars (e.g., quotation marks or
 * the backslash itself) are simply returned as-is.
 */
char        deescape( char c );

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Take a string and repeat it a given number of times.
 */
std::string repeat( std::string const& word, size_t times );

/**
 * @brief Return a precise string representation of the input value, using the provided precision
 * value (determining its decimal places).
 *
 * This function rounds the value to the given precision, and then returns its string representation
 * with possible trailing zeros. Thus, it uses fixed precision. This is useful for e.g., output
 * in a table format.
 *
 * For a version of this function that truncates trailing zeros, see to_string_rounded().
 * Also, see to_string() if you simply want to output a string representation of a double.
 */
std::string to_string_precise( double value, int precision = 6 );

/**
 * @brief Return a string representation of the input value, using the provided precision value
 * (determining its decimal places) to round, and truncate trailing zeros.
 *
 * This function rounds the value to the given precision, and then returns its string representation
 * without trailing zeros. This is useful for output that keeps a certain amount of significant
 * decimal digits, while making the output as short as possible.
 *
 * If you want to round, but also keep trailing zeros, see to_string_precise().
 * Also, see to_string() if you simply want to output a string representation of a double.
 */
std::string to_string_rounded( double value, int precision = 6 );

/**
 * @brief Return a string representation of a given value.
 *
 * This function template is a drop-in replacement for std::to_string, with the difference that it
 * treats floating point numbers more nicely: Instead of printing a fixed amount of digits, it
 * only prints digits without trailing zeros.
 *
 * If you also want to round the value, or need more precision, see to_string_precise() and
 * to_string_rounded().
 *
 * As it uses operator << on the given value, it is suitable for any class or value for which this
 * stream operator is available. Thus, this function can also be used for conveniently returning a
 * string where otherwise some stream operations would have been necessary.
 */
template <typename T>
std::string to_string( T const& v )
{
    std::ostringstream s;
    s << v;
    return s.str();
}

/**
 * @brief Return a string where the elements of a container `v` are joined using the string
 * `delimiter` in between them.
 *
 * The container is iterated via its range based for loop, thus it needs to have begin() and end()
 * functions.
 *
 * For turning the elements of the container into a string, their operator << is used.
 * Thus, this function can used with all types that support this operator.
 */
template <typename T>
std::string join( T const& v, std::string const& delimiter )
{
    std::ostringstream s;
    for( auto const& i : v ) {
        if( &i != &v[0] ) {
            s << delimiter;
        }
        s << i;
    }
    return s.str();
}

} // namespace utils
} // namespace genesis

#endif // include guard
