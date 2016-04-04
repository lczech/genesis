/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/text/string.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Compare
// =================================================================================================

/**
 * @brief Compare two strings case insensitive.
 */
bool equals_ci( std::string const& lhs, std::string const& rhs)
{
    const size_t sz = lhs.size();
    if( rhs.size() != sz ) {
        return false;
    }
    for( size_t i = 0; i < sz; ++i ) {
        if( tolower( lhs[i] ) != tolower( rhs[i] ) ) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Return whether a string starts with another string.
 */
bool starts_with( std::string const & text, std::string const & start )
{
    if (start.size() > text.size()) {
        return false;
    }
    return std::equal( start.begin(), start.end(), text.begin() );
}

/**
 * @brief Return whether a string ends with another string.
 */
bool ends_with( std::string const & text, std::string const & ending )
{
    if (ending.size() > text.size()) {
        return false;
    }
    return std::equal( ending.rbegin(), ending.rend(), text.rbegin() );
}

// =================================================================================================
//     Find and Count
// =================================================================================================

/**
 * @brief Return the number of non-overlapping occurrences of a substring in a string.
 */
size_t count_substring_occurrences( std::string const& str, std::string const& sub )
{
    if (sub.length() == 0) {
        return 0;
    }

    size_t count = 0;
    for(
        size_t offset = str.find(sub);
        offset != std::string::npos;
        offset = str.find( sub, offset + sub.length() )
    ) {
        ++count;
    }

    return count;
}

/**
 * @brief Spilt a string int parts, given a set of delimiter chars.
 *
 * The string `str` is split using any of the chars in `delimiters` and returned as a vector
 * of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are
 * excluded from the output.
 */
std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters,
    const bool trim_empty
) {
    size_t pos;
    size_t last_pos = 0;

    std::vector<std::string> result;

    while (true) {
        pos = str.find_first_of(delimiters, last_pos);

        if (pos == std::string::npos) {
           pos = str.length();

           if (pos != last_pos || !trim_empty) {
              result.push_back(std::string(str.data() + last_pos, pos - last_pos));
           }

           break;
        } else {
           if(pos != last_pos || !trim_empty) {
              result.push_back(std::string(str.data() + last_pos, pos - last_pos));
           }
        }

        last_pos = pos + 1;
    }

    return result;
}

// =================================================================================================
//     Manipulate
// =================================================================================================

/**
 * @brief Return a copy of a string, where all occurences of a search string
 * are replaced by a replace string.
 */
std::string replace_all (
    std::string const& text, std::string const& search, std::string const& replace
) {
    std::string tmp = text;
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = tmp.find(search, pos);

        if (pos == std::string::npos)
            break;

        tmp.erase(pos, search.length());
        tmp.insert(pos, replace);
    }
    return tmp;
}

// inline version
/*
void replace_all(
    std::string &s, const std::string &search, const std::string &replace
) {
    for (size_t pos = 0; ; pos += replace.length() ) {
        pos = s.find(search, pos);

        if (pos == string::npos)
            break;

        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}
*/

/**
 * @brief Return a copy of the input string, with left trimmed white spaces.
 */
std::string trim_right (
    std::string const& s,
    std::string const& delimiters
) {
    auto const pos = s.find_last_not_of(delimiters);
    if( std::string::npos == pos ) {
        return "";
    } else {
        return s.substr( 0, pos + 1 );
    }
}

/**
 * @brief Return a copy of the input string, with right trimmed white spaces.
 */
std::string trim_left (
    std::string const& s,
    std::string const& delimiters
) {
    auto const pos = s.find_first_not_of(delimiters);
    if( std::string::npos == pos ) {
        return "";
    } else {
        return s.substr(pos);
    }
}

/**
 * @brief Return a copy of the input string, with trimmed white spaces.
 */
std::string trim (
    std::string const& s,
    std::string const& delimiters
) {
    return trim_left(trim_right(s, delimiters), delimiters);
}

// =================================================================================================
//     Normalize
// =================================================================================================

/**
 * @brief Return an all-lowercase copy of the given string.
 */
std::string to_lower( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = tolower(c);
    }
    return res;
}

/**
 * @brief Return an all-uppercase copy of the given string.
 */
std::string to_upper( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = toupper(c);
    }
    return res;
}

/**
 * @brief Return a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string escape( std::string const& text )
{
    std::string tmp;
    tmp = replace_all(text, "\r", "\\r");
    tmp = replace_all(tmp,  "\n", "\\n");
    tmp = replace_all(tmp,  "\t", "\\t");
    tmp = replace_all(tmp,  "\"", "\\\"");
    tmp = replace_all(tmp,  "\\", "\\\\");
    return tmp;
}

/**
 * @brief Return a string where the escaped characters are transformed into
 * their respective string form.
 *
 * For example, the escape sequence \\n (backshlash n) will be translated into
 * a new line. The same applies for tabs and carrier returns. All other
 * escaped sequences will simply translate into the second char, e.g., a double
 * backslash will become one backslash.
 */
std::string deescape( std::string const& text )
{
    std::string tmp = "";
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\\') {
            if (i+1 >= text.size()){
                break;
            }
            switch (text[i+1]) {
                case 'r' : tmp += '\r'; break;
                case 'n' : tmp += '\n'; break;
                case 't' : tmp += '\t'; break;
                default  : tmp += text[i+1];
            }
            ++i;
        } else {
            tmp += text[i];
        }
    }
    return tmp;
}

std::string unify_newlines( std::string const& s )
{
    throw std::domain_error("unify_newlines is not yet implemented.");
    return s;
}

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Return a precise(er than to_string) string representation of the input value, using
 * the provided precision value. This is most suitable for floating point to string conversions.
 */
std::string to_string_precise( double value, const int precision )
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

} // namespace utils
} // namespace genesis
