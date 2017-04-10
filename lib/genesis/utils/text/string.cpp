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
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/string.hpp"

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

bool starts_with( std::string const & text, std::string const & start )
{
    if (start.size() > text.size()) {
        return false;
    }
    return std::equal( start.begin(), start.end(), text.begin() );
}

bool ends_with( std::string const & text, std::string const & ending )
{
    if (ending.size() > text.size()) {
        return false;
    }
    return std::equal( ending.rbegin(), ending.rend(), text.rbegin() );
}

// =================================================================================================
//     Substrings
// =================================================================================================

std::string head( std::string const& text, size_t lines )
{
    // Not totally efficient, but works for now.
    auto vec = split( text, "\n", false );
    size_t remove = vec.size() > lines ? vec.size() - lines : 0;
    vec.erase( vec.end() - remove, vec.end() );
    return join( vec, "\n" );
}

std::string tail( std::string const& text, size_t lines )
{
    // Not totally efficient, but works for now.
    auto vec = split( text, "\n", false );
    size_t remove = vec.size() > lines ? vec.size() - lines : 0;
    vec.erase( vec.begin(), vec.begin() + remove );
    return join( vec, "\n" );
}

// =================================================================================================
//     Find and Count
// =================================================================================================

size_t count_substring_occurrences( std::string const& str, std::string const& sub )
{
    if (sub.length() == 0) {
        return 0;
    }

    size_t count = 0;
    for(
        size_t offset = str.find(sub);
        offset != std::string::npos;
        offset = str.find( sub, offset + 1 )
    ) {
        ++count;
    }

    return count;
}

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

std::string indent(
    std::string const& text,
    std::string const& indentation
) {
    auto ret = indentation + replace_all( text, "\n", "\n" + indentation );
    return trim_right( ret, indentation );
}

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

std::string replace_all_chars (
    std::string const& text,
    std::string const& search_chars,
    char               replace
) {
    auto result = text;
    for( auto& c : result ) {
        if( search_chars.find( c ) != std::string::npos ) {
            c = replace;
        }
    }
    return result;
}

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

std::string trim (
    std::string const& s,
    std::string const& delimiters
) {
    return trim_left(trim_right(s, delimiters), delimiters);
}

// =================================================================================================
//     Normalize
// =================================================================================================

std::string to_lower( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = tolower(c);
    }
    return res;
}

void to_lower_inplace( std::string& str )
{
    for( auto& c : str ){
        c = tolower(c);
    }
}

std::string to_upper( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = toupper(c);
    }
    return res;
}

void to_upper_inplace( std::string& str )
{
    for( auto& c : str ){
        c = toupper(c);
    }
}

char to_lower_ascii( char c )
{
    return ( 'A' <= c && c <= 'Z' ) ? c + 0x20 : c;
}

char to_upper_ascii( char c )
{
    return ( 'a' <= c && c <= 'z' ) ? c - 0x20 : c;
}

std::string to_lower_ascii( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = to_lower_ascii(c);
    }
    return res;
}

void to_lower_ascii_inplace( std::string& str )
{
    for( auto& c : str ){
        c = to_lower_ascii(c);
    }
}

std::string to_upper_ascii( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = to_upper_ascii(c);
    }
    return res;
}

void to_upper_ascii_inplace( std::string& str )
{
    for( auto& c : str ){
        c = to_upper_ascii(c);
    }
}

std::string escape( std::string const& text )
{
    // This is slow-ish, because the string is iterated multiple times. Could be done faster.
    std::string tmp;
    tmp = replace_all( text, "\r", "\\r"  );
    tmp = replace_all( tmp,  "\n", "\\n"  );
    tmp = replace_all( tmp,  "\t", "\\t"  );
    tmp = replace_all( tmp,  "\"", "\\\"" );
    tmp = replace_all( tmp,  "\\", "\\\\" );
    return tmp;
}

std::string deescape( std::string const& text )
{
    // Prepare a string that might be a bit too big, but saves reallocation.
    std::string tmp;
    tmp.reserve( text.size() );

    // Copy from text to tmp string, while deescaping.
    for( size_t i = 0; i < text.size(); ++i ) {
        if( text[ i ] == '\\' ) {
            if( i + 1 >= text.size() ){
                break;
            }

            tmp += deescape( text[ i + 1 ] );
            ++i;
        } else {
            tmp += text[ i ];
        }
    }
    return tmp;
}

char deescape( char c )
{
    switch( c ) {
        case 'r' :
            return '\r';

        case 'n' :
            return '\n';

        case 't' :
            return '\t';

        default :
            return c;
    }
}

// =================================================================================================
//     Output
// =================================================================================================

std::string repeat( std::string const& word, size_t times )
{
    // Init and avoid repeated reallocation.
    std::string result;
    result.reserve( times * word.length() );

    // Concat repeats.
    for( size_t i = 0; i < times; ++i ) {
        result += word ;
    }
    return result;
}

std::string to_string_precise( double const value, int const precision )
{
    // Simple and straight forward.
    std::ostringstream s;
    s << std::fixed << std::setprecision( precision ) << value;
    return s.str();
}

std::string to_string_rounded( double const value, int const precision )
{
    // Get fixed precision string.
    std::ostringstream s;
    s << std::fixed << std::setprecision( precision ) << value;
    auto str = s.str();

    // Truncate trailing zeros, unless there are only zeros after the decimal point. Then, also
    // delete the decimal point.
    size_t offset = 1;
    size_t const last_nonzero = str.find_last_not_of('0');
    if( str[ last_nonzero ] == '.' ) {
        offset = 0;
    }
    str.erase( last_nonzero + offset, std::string::npos );
    return str;
}

} // namespace utils
} // namespace genesis
