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
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/text/string.hpp"

#include "genesis/utils/math/common.hpp"

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

bool contains_ci( std::vector<std::string> const& haystack, std::string const& needle )
{
    auto const l_needle = to_lower( needle );
    for( auto const& val : haystack ) {
        if( to_lower( val ) == l_needle ) {
            return true;
        }
    }
    return false;
}

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

/**
 * @brief Local function that does the work for the split cuntions.
 */
std::vector<std::string> split (
    std::string const& string,
    std::function<size_t ( std::string const&, size_t )> find_pos,
    size_t advance_by,
    const bool trim_empty
) {
    size_t pos;
    size_t last_pos = 0;

    std::vector<std::string> result;

    while( true ) {
        // Find first matching char.
        pos = find_pos( string, last_pos );

        // If not found, push back rest and stop.
        if( pos == std::string::npos ) {
           pos = string.length();

           if( pos != last_pos || !trim_empty ) {
              result.push_back( std::string( string.data() + last_pos, pos - last_pos ));
           }

           break;

        // If found, push back and continue.
        } else {
           if( pos != last_pos || !trim_empty ) {
              result.push_back( std::string( string.data() + last_pos, pos - last_pos ));
           }
        }

        last_pos = pos + advance_by;
    }

    return result;
}

std::vector<std::string> split (
    std::string const& string,
    std::string const& delimiters,
    const bool trim_empty
) {
    return split(
        string,
        [&]( std::string const& str, size_t last_pos ){
            return str.find_first_of( delimiters, last_pos );
        },
        1,
        trim_empty
    );
}

std::vector<std::string> split (
    std::string const& string,
    std::function<bool(char)> delimiter_predicate,
    const bool trim_empty
) {
    return split(
        string,
        [&]( std::string const& str, size_t last_pos ){
            // Find first matching char.
            size_t pos = std::string::npos;
            for( size_t i = last_pos; i < str.size(); ++i ) {
                if( delimiter_predicate( str[i] ) ) {
                    pos = i;
                    break;
                }
            }
            return pos;
        },
        1,
        trim_empty
    );
}

std::vector<std::string> split_at (
    std::string const& string,
    std::string const& delimiter,
    const bool trim_empty
) {
    return split(
        string,
        [&]( std::string const& str, size_t last_pos ){
            return str.find( delimiter, last_pos );
        },
        delimiter.size(),
        trim_empty
    );
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

        if (pos == std::string::npos){
            break;
        }

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

std::string to_string_leading_zeros( size_t value, size_t length )
{
    std::stringstream ss;
    ss << std::setw( length ) << std::setfill( '0' ) << value;
    return ss.str();
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
