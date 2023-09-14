#ifndef GENESIS_UTILS_TEXT_STRING_H_
#define GENESIS_UTILS_TEXT_STRING_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief Provides some commonly used string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/text/char.hpp"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Compare and Find
// =================================================================================================

/**
 * @brief Return whether a vector of strings contains a given string, case insensitive.
 */
bool contains_ci( std::vector<std::string> const& haystack, std::string const& needle );

/**
 * @brief Return whether a vector of strings contains a given string, case insensitive,
 * and ignoring all non-alphanumerical characters.
 */
bool contains_ci_alnum( std::vector<std::string> const& haystack, std::string const& needle );

/**
 * @brief Compares two strings, ignoring case differences.
 *
 * Compares the null-terminated strings @p s1 and @p s2, returning -1, 0, or 1
 * if @p s1 is lexicographically less than, equal to, or greater than @p s2 over those characters,
 * respectively.
 *
 * This is equivalent to the POSIX function of the same name, found in `strings.h`,
 * but included here for portability.
 */
int strcasecmp( char const* s1, char const* s2 );

/**
 * @brief Compares up to @p n chars of two strings, ignoring case differences.
 *
 * Compares up to @p n chars of the null-terminated strings @p s1 and @p s2, returning -1, 0, or 1
 * if @p s1 is lexicographically less than, equal to, or greater than @p s2 over those characters,
 * respectively.
 *
 * This is equivalent to the POSIX function of the same name, found in `strings.h`,
 * but included here for portability.
 */
int strncasecmp( char const* s1, char const* s2, size_t n );

/**
 * @brief Compare two strings, case insensitive.
 */
bool equals_ci( std::string const& lhs, std::string const& rhs );

/**
 * @brief Compare two strings, case insensitive, and ignoring all non-alphanumerical characters.
 */
bool equals_ci_alnum( std::string const& lhs, std::string const& rhs );

/**
 * @brief Return whether a string starts with another string, i.e., check for a @p prefix.
 */
bool starts_with( std::string const& text, std::string const& prefix );

/**
 * @brief Return whether a string starts with another string, i.e., check for a @p prefix.
 *
 * This version of the function then stores the @p suffix of the matched @p text,
 * containing all remaining characters that did not match the @p prefix pattern.
 * If there is no match, the @p suffix is left unchanged.
 */
bool starts_with( std::string const& text, std::string const& prefix, std::string& suffix );

/**
 * @brief Return whether a string starts with another string, i.e., check for a @p prefix,
 * case insensitive.
 */
bool starts_with_ci( std::string const& text, std::string const& prefix );

/**
 * @brief Return whether a string starts with another string, i.e., check for a @p prefix,
 * case insensitive.
 *
 * @copydetails starts_with( std::string const&, std::string const&, std::string& )
 */
bool starts_with_ci( std::string const& text, std::string const& prefix, std::string& suffix );

/**
 * @brief Return whether a string starts with another string (prefix), comparing case-independent,
 * and ignoring all non-alphanumerical characters.
 *
 * For example, `"REF_CNT.S2-1.sorted:1"` starts with `"[ref-cnt]"`, according to this function.
 */
bool starts_with_ci_alnum( std::string const& text, std::string const& prefix );

/**
 * @copydoc starts_with_ci_alnum( std::string const& , std::string const& )
 *
 * This version of the function then stores the @p suffix of the matched @p text,
 * containing all remaining characters that did not match the @p prefix pattern.
 * If there is no match, the @p suffix is left unchanged.
 *
 * If additionally @p trim_suffix is set, all leading non-alphanumerical characters of the
 * @p suffix are removed.
 * For example, `"REF_CNT.S2-1.sorted:1"` starts with `"[ref-cnt]"`, and then contains the suffix
 * `".S2-1.sorted:1"` - with @p trim_suffix, this is reduced to `"S2-1.sorted:1"` instead.
 */
bool starts_with_ci_alnum(
    std::string const& text,
    std::string const& prefix,
    std::string& suffix,
    bool trim_suffix = false
);

/**
 * @brief Return whether a string ends with another string, i.e., check for a @p suffix.
 */
bool ends_with( std::string const& text, std::string const& suffix );

/**
 * @brief Return whether a string ends with another string, i.e., check for a @p suffix.
 *
 * This version of the function then stores the @p prefix of the matched @p text,
 * containing all leading characters that did not match the @p suffix pattern.
 * If there is no match, the @p prefix is left unchanged.
 */
bool ends_with( std::string const& text, std::string const& suffix, std::string& prefix );

/**
 * @brief Return whether a string ends with another string, i.e., check for a suffix,
 * case insensitive.
 */
bool ends_with_ci( std::string const& text, std::string const& suffix );

/**
 * @brief Return whether a string ends with another string, i.e., check for a suffix,
 * case insensitive.
 *
 * @copydetails ends_with( std::string const&, std::string const&, std::string& )
 */
bool ends_with_ci( std::string const& text, std::string const& suffix, std::string& prefix );

/**
 * @brief Return whether a string ends with another string (suffix), comparing case-independent,
 * and ignoring all non-alphanumerical characters.
 *
 * For example, `"S2-1.sorted:1.REF_CNT"` ends with `"[ref-cnt]"`, according to this function.
 */
bool ends_with_ci_alnum( std::string const& text, std::string const& suffix );

/**
 * @copydoc ends_with_ci_alnum( std::string const& , std::string const& )
 *
 * This version of the function then stores the @p prefix of the matched @p text,
 * containing all leading characters that did not match the @p suffix pattern.
 * If there is no match, the @p prefix is left unchanged.
 *
 * If additionally @p trim_prefix is set, all trailing non-alphanumerical characters of the
 * @p prefix are removed.
 * For example, `"S2-1.sorted:1.REF_CNT"` ends with `"[ref-cnt]"`, and then contains the prefix
 * `"S2-1.sorted:1."` - with @p trim_prefix, this is reduced to `"S2-1.sorted:1"` instead.
 */
bool ends_with_ci_alnum(
    std::string const& text,
    std::string const& suffix,
    std::string& prefix,
    bool trim_prefix = false
);

/**
 * @brief Return whether a string is matched by a wildcard pattern containing `?` and `*`
 * for single and mutliple (0 or more) wildcard characters, respectively.
 */
bool match_wildcards( std::string const& str, std::string const& pattern );

/**
 * @brief Compare two strings with natural human sorting, that is "A1", "A2", "A100", instead of the
 * standard sort by ASCII value "A1", "A100", "A2".
 *
 * Returns negavie if @p lhs < @p rhs, `0` if they are equal, and positive if @p lhs > @p rhs.
 */
int compare_natural( std::string const& lhs, std::string const& rhs );

/**
 * @brief Functor class to compare to strings with natural "human" sorting, see compare_natural().
 */
template <class T = std::string>
struct NaturalLess {
    bool operator()( T const& lhs, T const& rhs ) const {
        return compare_natural( lhs, rhs ) < 0;
    }
};

/**
 * @brief Functor class to compare to strings with natural "human" sorting, see compare_natural().
 */
template <class T = std::string>
struct NaturalGreater {
    bool operator()( T const& lhs, T const& rhs ) const {
        return compare_natural( lhs, rhs ) > 0;
    }
};

/**
 * @brief Sort a range of std::string (or convertible to std::string) elements, using natural
 * sorting; see compare_natural().
 */
template <typename RandomAccessIterator>
inline void sort_natural(
    RandomAccessIterator first,
    RandomAccessIterator last,
    bool reverse = false
) {
    // The above implementations of NaturalLess and NaturalGreater were using std::binary_function
    // before, which is deprecated. We hence now simply removed those, which seems to work.
    // If that causes trouble with other compilers, see https://stackoverflow.com/a/33115341 for
    // alternative solutions.

    using T = typename RandomAccessIterator::value_type;
    if( reverse ) {
        std::sort( first, last, NaturalGreater<T>() );
    } else {
        std::sort( first, last, NaturalLess<T>() );
    }
}

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
//     Split and Count
// =================================================================================================

/**
 * @brief Return the number of (possibly overlapping) occurrences of a substring in a string.
 */
size_t count_substring_occurrences( std::string const& str, std::string const& sub );

/**
 * @brief Spilt a @p string into parts, given a @p delimiter char.
 *
 * The @p string is split using the @p delimiter char, and returned as a vector
 * of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are
 * excluded from the output.
 */
std::vector<std::string> split (
    std::string const& str,
    char delimiter = '\t',
    const bool trim_empty = true
);

/**
 * @brief Spilt a @p string into parts, given a @p delimiters set of chars.
 *
 * The @p string is split using any of the chars in @p delimiters, and returned as a vector
 * of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are
 * excluded from the output.
 */
std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters,
    const bool trim_empty = true
);

/**
 * @brief Spilt a @p string into parts, given a @p delimiter_predicate
 * that returns `true` for delimiters chars.
 *
 * The @p string is split using any of the chars for which @p delimiter_predicate is `true`,
 * and returned as a vector of strings.  If `trim_empty` is set, empty strings resulting from
 * adjacent delimiter chars are excluded from the output.
 */
std::vector<std::string> split (
    std::string const& str,
    std::function<bool (char)> delimiter_predicate,
    const bool trim_empty = true
);

/**
 * @brief Spilt a @p string into parts, given a @p delimiter string.
 *
 * The @p string is split where the whole @p delimiter string is found, and returned as a vector
 * of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiters are
 * excluded from the output.
 */
std::vector<std::string> split_at (
    std::string const& str,
    std::string const& delimiter,
    const bool trim_empty = true
);

/**
 * @brief Split a string containing positive interger numbers into its parts and resolve ranges.
 *
 * For example, the string `1, 3, 5-7, 10` results in a vector of all listed numbers and the
 * ranges in between, that is `1, 3, 5, 6, 7, 10`. Whitespace is ignored; the range is sorted.
 */
std::vector<size_t> split_range_list( std::string const& str );

// =================================================================================================
//     Manipulate
// =================================================================================================

/**
 * @brief Wrap a @p text at a given @p line_length.
 */
std::string wrap(
    std::string const& text,
    size_t line_length = 80
);

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
std::string replace_all(
    std::string const& text,
    std::string const& search,
    std::string const& replace
);

/**
 * @brief Return a copy of a string, where all occurrences of a search string are removed.
 */
std::string remove_all(
    std::string const& text,
    std::string const& search
);

/**
 * @brief Replace all occurrences of the @p search_chars in @p text by the @p replace char.
 */
std::string replace_all_chars(
    std::string const& text,
    std::string const& search_chars,
    char               replace
);

/**
 * @brief Replace all occurrences of characters for which @p predicate is `true` in @p text
 * by the @p replace char.
 */
template< class UnaryPredicate >
std::string replace_all_chars_pred(
    std::string const& text,
    UnaryPredicate     predicate,
    char               replace
) {
    auto result = text;
    for( auto& c : result ) {
        if( predicate( c ) ) {
            c = replace;
        }
    }
    return result;
}

/**
 * @brief Remove all occurrences of the @p search_chars in @p text.
 */
std::string remove_all_chars(
    std::string const& text,
    std::string const& search_chars
);

/**
 * @brief Remove all occurrences characters for which @p predicate is `true` in @p text.
 */
template< class UnaryPredicate >
std::string remove_all_chars_pred(
    std::string const& text,
    UnaryPredicate     predicate
) {
    auto result = text;
    result.erase( std::remove_if( result.begin(), result.end(), predicate ), result.end() );
    return result;
}

/**
 * @brief Remove all non-alphanumerical characters from a string.
 */
std::string remove_all_non_alnum( std::string const& text );

/**
 * @brief Return a copy of the input string, with left trimmed white spaces
 * (or any other @p delimiters).
 */
std::string trim_right (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

/**
 * @brief Return a copy of the input string, with right trimmed white spaces
 * (or any other @p delimiters).
 */
std::string trim_left (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

/**
 * @brief Return a copy of the input string, with trimmed white spaces
 * (or any other @p delimiters).
 */
std::string trim (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

// =================================================================================================
//     Case Conversion
// =================================================================================================

/**
 * @brief Return an all-lowercase copy of the given string, locale-aware.
 */
inline std::string to_lower( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        // Weird C relicts need weird conversions...
        // See https://en.cppreference.com/w/cpp/string/byte/tolower
        c = static_cast<char>( std::tolower( static_cast<unsigned char>( c )));
    }
    return res;
}

/**
 * @brief Turn the given string to all-lowercase, locale-aware.
 */
inline void to_lower_inplace( std::string& str )
{
    for( auto& c : str ){
        c = static_cast<char>( std::tolower( static_cast<unsigned char>( c )));
    }
}

/**
 * @brief Return an all-uppercase copy of the given string, locale-aware.
 */
inline std::string to_upper( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = static_cast<char>( std::toupper( static_cast<unsigned char>( c )));
    }
    return res;
}

/**
 * @brief Turn the given string to all-uppercase, locale-aware.
 */
inline void to_upper_inplace( std::string& str )
{
    for( auto& c : str ){
        c = static_cast<char>( std::toupper( static_cast<unsigned char>( c )));
    }
}

/**
 * @brief Turn the given string to all-lowercase, ASCII-only.
 */
void to_lower_ascii_inplace( std::string& str );

/**
 * @brief Return an all-lowercase copy of the given string, ASCII-only.
 */
std::string to_lower_ascii( std::string const& str );

/**
 * @brief Turn the given string to all-uppercase, ASCII-only, inline.
 *
 * Uses AVX2 if available.
 */
void to_upper_ascii_inplace( std::string& str );

/**
 * @brief Return an all-uppercase copy of the given string, ASCII-only.
 */
std::string to_upper_ascii( std::string const& str );

// =================================================================================================
//     Normalize
// =================================================================================================

/**
 * @brief Return a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string escape( std::string const& text );

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
char deescape( char c );

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Take a string and repeat it a given number of times.
 */
std::string repeat( std::string const& word, size_t times );

/**
 * @brief Return a string representation of a `size_t` @p value with a fixed length, that is,
 * by adding leading zeros.
 *
 * If @p value is already longer than @p length, the result will also be longer.
 */
std::string to_string_leading_zeros( size_t value, size_t length = 6 );

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
std::string to_string_nice( T const& v )
{
    std::ostringstream s;
    s << v;
    return s.str();
}

/**
 * @brief Print elements of the given @p container to the output @p stream, joining them
 * with the @p delimiter.
 *
 * The @p delimiter is only printed in between elements, but not after the last.
 * The container is iterated via its range based for loop, thus it needs to have begin() and end()
 * functions. For turning the elements of the container into a string, their operator << is used.
 * Thus, this function can used with all types that support this operator.
 */
template <
    typename C,
    typename std::enable_if<
        ! std::is_same<typename C::value_type, unsigned char>::value &&
        ! std::is_same<typename C::value_type,   signed char>::value
    >::type* = nullptr
>
std::ostream& join( std::ostream& stream, C const& container, std::string const& delimiter = ", " )
{
    for( auto const& element : container ) {
        if( &element != &(*container.begin()) ) {
            stream << delimiter;
        }
        stream << element;
    }
    return stream;
}

/**
 * @brief Print elements of the given @p container to the output @p stream, joining them
 * with the @p delimiter, for a container of signed or unsigned char.
 *
 * We need this specialization, as the unsigned chars are otherwise turned into their char (ASCII)
 * equivalent, which we do not want. Instead, we want to output them here as plain numbers.
 */
template <
    typename C,
    typename std::enable_if<
        std::is_same<typename C::value_type, unsigned char>::value ||
        std::is_same<typename C::value_type,   signed char>::value
    >::type* = nullptr
>
std::ostream& join( std::ostream& stream, C const& container, std::string const& delimiter = ", " )
{
    for( auto const& element : container ) {
        if( &element != &(*container.begin()) ) {
            stream << delimiter;
        }
        stream << static_cast<int>( element );
    }
    return stream;
}

/**
 * @brief Return a string where the elements of a container `v` are joined using the string
 * `delimiter` in between them.
 */
template <typename C>
std::string join( C const& container, std::string const& delimiter = ", " )
{
    std::ostringstream s;
    join( s, container, delimiter );
    return s.str();
}

/**
 * @brief Return the bit representation of an unsigned int.
 *
 * For example, to_bit_string(5) yields "0...0101".
 * Optionally, the chars to use for @p zero and @p one can be set, as well as a flag whether
 * to put spaces in between individual bytes of the output.
 */
template<typename T>
inline std::string to_bit_string(
    T const x, char const zero = '0', char const one = '1', bool const byte_space = true
) {
    static_assert(
        std::is_unsigned<T>::value,
        "Can only use to_bit_string() with unsigned types."
    );

    std::string binary = "";
    T mask = 1;
    for( size_t i = 0; i < sizeof(T) * 8; ++i ) {
        if( byte_space && i > 0 && i % 8 == 0 ) {
            binary = ' ' + binary;
        }
        if( mask & x ) {
            binary = one + binary;
        } else {
            binary = zero + binary;
        }
        mask <<= 1;
    }
    return binary;
}

} // namespace utils
} // namespace genesis

#endif // include guard
