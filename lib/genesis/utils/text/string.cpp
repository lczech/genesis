/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/text/string.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

#if defined(GENESIS_AVX) || defined(GENESIS_AVX2) || defined(GENESIS_AVX512)

    #include <immintrin.h>

#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Compare and Find
// =================================================================================================

bool contains_ci( std::vector<std::string> const& haystack, std::string const& needle )
{
    // Lazy. Could be more optimized for sure, if this ever becomes a bottleneck.
    auto const l_needle = to_lower( needle );
    for( auto const& val : haystack ) {
        if( to_lower( val ) == l_needle ) {
            return true;
        }
    }
    return false;
}

bool contains_ci_alnum( std::vector<std::string> const& haystack, std::string const& needle )
{
    // Lazy. Could be more optimized for sure, if this ever becomes a bottleneck.
    auto const lower_alnum_needle = to_lower( remove_all_non_alnum( needle ));
    for( auto const& val : haystack ) {
        auto alnum_val = remove_all_non_alnum( val );
        if( equals_ci( alnum_val, lower_alnum_needle )) {
            return true;
        }
    }
    return false;
}

int strcasecmp( char const* s1, char const* s2 )
{
    // Avoid code duplication at minimal runtime cost.
    return strncasecmp( s1, s2, std::numeric_limits<size_t>::max() );
}

int strncasecmp( char const* s1, char const* s2, size_t n )
{
    // Fast edge case
    if( s1 == s2 ) {
        return 0;
    }

    // Need to convert to unsigned, https://stackoverflow.com/a/51992138
    auto ucs1 = reinterpret_cast<unsigned char const*>( s1 );
    auto ucs2 = reinterpret_cast<unsigned char const*>( s2 );

    // Run the comparison
    int d = 0;
    for( ; n != 0; n-- ) {
        int const c1 = tolower( *ucs1++ );
        int const c2 = tolower( *ucs2++ );
        d = c1 - c2;
        if(( d != 0 ) || ( c1 == '\0' )) {
            break;
        }
    }
    return d;
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

bool equals_ci_alnum( std::string const& lhs, std::string const& rhs )
{
    // Lazy. Could be more optimized for sure, if this ever becomes a bottleneck.
    auto const alnum_lhs = remove_all_non_alnum( lhs );
    auto const alnum_rhs = remove_all_non_alnum( rhs );
    return equals_ci( alnum_lhs, alnum_rhs );
}

bool starts_with( std::string const& text, std::string const& prefix )
{
    if( prefix.size() > text.size() ) {
        return false;
    }
    return std::equal( prefix.begin(), prefix.end(), text.begin() );
}

bool starts_with( std::string const& text, std::string const& prefix, std::string& suffix )
{
    auto const res = starts_with( text, prefix );
    if( res ) {
        assert( prefix.size() <= text.size() );
        suffix = text.substr( prefix.size() );
    }
    return res;
}

bool starts_with_ci( std::string const& text, std::string const& prefix )
{
    // Lazy. Could be more optimized for sure, if this ever becomes a bottleneck.
    return starts_with( to_lower( text ), to_lower( prefix ));
}

bool starts_with_ci( std::string const& text, std::string const& prefix, std::string& suffix )
{
    auto const res = starts_with_ci( text, prefix );
    if( res ) {
        assert( prefix.size() <= text.size() );
        suffix = text.substr( prefix.size() );
    }
    return res;
}

bool starts_with_ci_alnum( std::string const& text, std::string const& prefix )
{
    // Ignore the suffix
    std::string suffix;
    return starts_with_ci_alnum( text, prefix, suffix );
}

bool starts_with_ci_alnum(
    std::string const& text, std::string const& prefix, std::string& suffix, bool trim_suffix
) {
    // Pattern matching of trying to find the prefix in the text,
    // ignorning all non-alnum characters, and case-insensitive.
    // For example, we want prefix == "refcnt" to match text == "REF_CNT.S2-1.sorted:1",
    // and then return the remainder "S2-1.sorted:1" as our suffix.
    size_t p = 0;
    size_t t = 0;
    while( p < prefix.size() && t < text.size() ) {
        // assert( is_graph(prefix[p]) );
        // assert( is_graph(text[t]) );

        if( ! is_alnum(prefix[p]) ) {
            ++p;
            continue;
        }
        if( ! is_alnum(text[t]) ) {
            ++t;
            continue;
        }
        if( char_match_ci( prefix[p], text[t] )) {
            ++p;
            ++t;
        } else {
            return false;
        }
    }
    assert( p <= prefix.size() );
    assert( t <= text.size() );

    // If our pattern matching reached the end of the text,
    // there is nothing left in the text that we could use as a suffix.
    if( t == text.size() ) {
        suffix = "";
        return true;
    }

    // However, if we are not yet at the end of the text, we must have reached
    // the end of the search pattern (prefix), as otherwise the loop would not have termined.
    assert( t < text.size() );
    assert( p == prefix.size() );

    // Now we skip all non-alnum chars in the text from where we are, if requested.
    while( trim_suffix && t < text.size() && ! is_alnum(text[t]) ) {
        ++t;
    }

    // If there is stuff left, that is the suffix that we use.
    suffix = text.substr(t);
    return true;
}

bool ends_with( std::string const& text, std::string const& suffix )
{
    if( suffix.size() > text.size() ) {
        return false;
    }
    return std::equal( suffix.rbegin(), suffix.rend(), text.rbegin() );
}

bool ends_with( std::string const& text, std::string const& suffix, std::string& prefix )
{
    auto const res = ends_with( text, suffix );
    if( res ) {
        assert( suffix.size() <= text.size() );
        prefix = text.substr( 0, text.size() - suffix.size() );
    }
    return res;
}

bool ends_with_ci( std::string const& text, std::string const& suffix )
{
    // Lazy. Could be more optimized for sure, if this ever becomes a bottleneck.
    return ends_with( to_lower( text ), to_lower( suffix ));
}

bool ends_with_ci( std::string const& text, std::string const& suffix, std::string& prefix )
{
    auto const res = ends_with_ci( text, suffix );
    if( res ) {
        assert( suffix.size() <= text.size() );
        prefix = text.substr( 0, text.size() - suffix.size() );
    }
    return res;
}

bool ends_with_ci_alnum( std::string const& text, std::string const& suffix )
{
    // Ignore the prefix
    std::string prefix;
    return ends_with_ci_alnum( text, suffix, prefix );
}

bool ends_with_ci_alnum(
    std::string const& text, std::string const& suffix, std::string& prefix, bool trim_prefix
) {
    // Lazy. Could be more optimized for sure, if this ever becomes a bottleneck.
    // Find a prefix, by reversing all strings.
    auto const text_r   = std::string( text.rbegin(),   text.rend() );
    auto const suffix_r = std::string( suffix.rbegin(), suffix.rend() );
    if( starts_with_ci_alnum( text_r, suffix_r, prefix, trim_prefix )) {
        // If this succeeded, reverse the result again, to make it correct.
        prefix = std::string( prefix.rbegin(), prefix.rend() );
        return true;
    }
    return false;
}

bool match_wildcards( std::string const& str, std::string const& pattern )
{
    // Code adapted from https://www.geeksforgeeks.org/wildcard-pattern-matching/

    // The empty pattern can only match with the empty string
    if( pattern.empty() ) {
        return str.empty();
    }

    // Lookup table for dynamic programming approach of subproblem solutions, and init to zero.
    // We use a vec of bool, and a lambda for access as if it was a matrix.
    auto lookup_ = std::vector<bool>(( str.size() + 1 ) * ( pattern.size() + 1 ), false);
    auto lookup = [&]( size_t i, size_t j ) -> std::vector<bool>::reference {
        return lookup_[ i * ( pattern.size() + 1 ) + j ];
    };

    // The empty pattern can match with empty string
    lookup( 0, 0 ) = true;

    // Only '*' can match with empty string
    for( size_t j = 1; j <= pattern.size(); j++ ) {
        if( pattern[j - 1] == '*' ) {
            lookup( 0, j ) = lookup( 0, j - 1 );
        }
    }

    // Fill the table in bottom-up fashion
    for( size_t i = 1; i <= str.size(); i++ ) {
        for( size_t j = 1; j <= pattern.size(); j++ ) {
            if( pattern[j - 1] == '*' ) {

                // Two cases if we see a '*':
                // a) We ignore ‘*’ character and move to next  character in the pattern,
                //    i.e., ‘*’ indicates an empty sequence.
                // b) '*' character matches with ith character in input
                lookup( i, j ) = lookup( i, j - 1 ) || lookup( i - 1, j );

            } else if( pattern[j - 1] == '?' || str[i - 1] == pattern[j - 1] )

                // Current characters are considered as matching in two cases:
                // (a) current character of pattern is '?'
                // (b) characters actually match
                lookup( i, j ) = lookup( i - 1, j - 1 );

            else {

                // If characters don't match
                lookup( i, j ) = false;
            }
        }
    }

    return lookup( str.size(), pattern.size() );
}

int compare_natural( std::string const& lhs, std::string const& rhs )
{
    // Implementation inspired by http://www.davekoelle.com/files/alphanum.hpp
    // Released under the MIT License - https://opensource.org/licenses/MIT
    // We however heavily modified it, in particular to work with arbitrary runs of digits.

    // Edge cases of empty strings.
    if( lhs.empty() || rhs.empty() ) {
        // Smart! Let's avoid to do all three cases, and instead convert to int (0 or 1):
        //  * lhs empty, but rhs not:  0 - 1 = -1
        //  * rhs empty, but lhs not:  1 - 0 = +1
        //  * both empty:              1 - 1 =  0
        return static_cast<int>( rhs.empty() ) - static_cast<int>( lhs.empty() );
    }

    // We need to switch between modes. Clear semantics instead of just a bool.
    enum class ParseMode
    {
        kString,
        kNumber
    };
    auto mode = ParseMode::kString;

    // Helper function to parse a string into an unsigned number, quickly.
    // Advances the given pos while parsing, until either the end of the string or no more digits.
    // --> not used, as it can only handle numbers up to the size of unsigned long...
    // auto parse_unsigned_number_ = []( std::string const& str, size_t& pos ){
    //     unsigned long num = 0;
    //     while( pos < str.size() && is_digit( str[pos] )) {
    //         auto dig = str[pos] - '0';
    //         if( num > ( std::numeric_limits<T>::max() - dig ) / 10 ) {
    //             // This is ugly, and a proper solution would be to take string lengths into
    //             // account, but that would probably require to fully load them, and then compare...
    //             throw std::overflow_error( "Numerical overflow in compare_natural()" );
    //         }
    //         num = 10 * num + dig;
    //         ++pos;
    //     }
    //     return num;
    // };

    // Iterate positions in the strings.
    size_t l = 0;
    size_t r = 0;
    while( l < lhs.size() && r < rhs.size() ) {
        if( mode == ParseMode::kString ) {

            // Iterate as long as there are strings/chars in both.
            while( l < lhs.size() && r < rhs.size() ) {

                // Check if these are digits.
                bool const l_digit = is_digit( lhs[l] );
                bool const r_digit = is_digit( rhs[r] );

                // If both are digits, we continue in number mode.
                if( l_digit && r_digit ) {
                    mode = ParseMode::kNumber;
                    break;
                }

                // If only one of them is a digit, we have a result.
                if( l_digit ) {
                    return -1;
                }
                if( r_digit ) {
                    return +1;
                }

                // Neither is a digit, so compare as ASCII chars; if they differ, we have a result.
                assert( ! l_digit && ! r_digit );
                int const diff = static_cast<int>( lhs[l] ) - static_cast<int>( rhs[r] );
                if( diff != 0 ) {
                    return diff;
                }

                // Otherwise, process the next character.
                ++l;
                ++r;
            }

        } else {
            assert( mode == ParseMode::kNumber );

            // Here, a first idea was to parse both strings as numbers for as long as they contain
            // digits, and then compare the resulting numbers. However, this overflows for larger
            // numbers, and we can easily avoid that by an equally simple solution. We might need
            // to iterate the digits twice, but save the effort of actually building the numbers!
            // (see above parse_unsigned_number_() for the parsing function that we first had)

            // Parse the strings as long as they contain digits, advancing helper indices here.
            size_t ld = l;
            size_t rd = r;
            while( ld < lhs.size() && is_digit( lhs[ld] )) {
                ++ld;
            }
            while( rd < rhs.size() && is_digit( rhs[rd] )) {
                ++rd;
            }

            // If the lengths of digit runs differ, one of them is a larger number than the other.
            // In that case, we have a result.
            if( ld != rd ) {
                return static_cast<int>( ld ) - static_cast<int>( rd );
            }

            // If those numbers are the same length, we need to iterate again,
            // and check digit by digit. Iterate as long as there are digits in both.
            while( l < lhs.size() && r < rhs.size() ) {

                // Check if these are digits.
                bool const l_digit = is_digit( lhs[l] );
                bool const r_digit = is_digit( rhs[r] );

                // If there are no more digits, we continue in string mode.
                if( ! l_digit || ! r_digit ) {
                    // In that case, both have to be not digits, as we just checked same length
                    // of the digit run, and both have to be the same as our previous iteration.
                    assert( ! l_digit && ! r_digit );
                    assert( ld == rd && l == ld && r == rd );
                    mode = ParseMode::kString;
                    break;
                }

                // Compare the digits as ASCII chars; if they differ, we have a result.
                assert( l_digit && r_digit );
                int const diff = static_cast<int>( lhs[l] ) - static_cast<int>( rhs[r] );
                if( diff != 0 ) {
                    return diff;
                }

                // Otherwise, process the next character.
                ++l;
                ++r;
            }
        }
    }

    // Lastly, if we are here, both strings are identical up to the point to which the were compared.
    // So now, remaining lenghts checks. Only if everything is identical, return 0.
    if( l < lhs.size() ) {
        assert( r == rhs.size() );
        return +1;
    }
    if( r < rhs.size() ) {
        assert( l == lhs.size() );
        return -1;
    }
    assert( l == lhs.size() && r == rhs.size() );
    return 0;
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
//     Split and Count
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
static std::vector<std::string> split_ (
    std::string const& string,
    std::function<size_t ( std::string const&, size_t )> find_pos,
    size_t advance_by,
    const bool trim_empty
) {
    std::vector<std::string> result;

    size_t last_pos = 0;
    while( true ) {
        // Find first matching char.
        size_t pos = find_pos( string, last_pos );

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
    std::string const& str,
    char delimiter,
    const bool trim_empty
) {
    return split( str, std::string( 1, delimiter ), trim_empty );
}

std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters,
    const bool trim_empty
) {
    return split_(
        str,
        [&]( std::string const& str, size_t last_pos ){
            return str.find_first_of( delimiters, last_pos );
        },
        1,
        trim_empty
    );
}

std::vector<std::string> split (
    std::string const& str,
    std::function<bool(char)> delimiter_predicate,
    const bool trim_empty
) {
    return split_(
        str,
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
    std::string const& str,
    std::string const& delimiter,
    const bool trim_empty
) {
    return split_(
        str,
        [&]( std::string const& str, size_t last_pos ){
            return str.find( delimiter, last_pos );
        },
        delimiter.size(),
        trim_empty
    );
}

std::vector<size_t> split_range_list( std::string const& str )
{
    std::vector<size_t> result;

    auto is_digits = []( std::string const& s ){
        return trim( s ).find_first_not_of( "0123456789" ) == std::string::npos;
    };

    auto get_number = []( std::string const& s ){
        size_t n;
        sscanf( trim( s ).c_str(), "%zu", &n );
        return n;
    };

    if( trim( str ).empty() ) {
        return result;
    }

    auto const lst = split( str, "," );
    for( auto const& le : lst ) {
        // if just digits, done. if not, split -, repeat.
        if( is_digits( le ) ) {
            result.push_back( get_number( le ));
        } else {
            auto const rng = split( le, "-" );
            if( rng.size() != 2 || ! is_digits( rng[0] ) || ! is_digits( rng[1] ) ) {
                throw std::runtime_error( "Invalid range list string." );
            }
            auto const b = get_number( rng[0] );
            auto const e = get_number( rng[1] );
            for( size_t i = b; i <= e; ++i ) {
                result.push_back( i );
            }
        }
    }

    std::sort( result.begin(), result.end() );
    return result;
}

// =================================================================================================
//     Manipulate
// =================================================================================================

std::string wrap(
    std::string const& text,
    size_t line_length
) {
    /*
        Code is adapted from https://www.rosettacode.org/wiki/Word_wrap#C.2B.2B,
        which is published under the GNU Free Documentation License 1.2,
        see https://www.gnu.org/licenses/old-licenses/fdl-1.2.html
        We fixed the handling of overly long words,
        and added correct handling of new lines in the text.
        It is totally inefficient, but we only need this function for small texts anyway,
        so for now, this is good enough.
     */

    std::ostringstream output;
    auto const lines = split( text, "\n", false );
    for( auto const& line : lines ) {
        std::istringstream text_stream( line );
        std::string word;

        if( text_stream >> word ) {
            output << word;
            long space_left = static_cast<long>( line_length ) - static_cast<long>( word.length() );
            while( text_stream >> word ) {
                if( space_left < static_cast<long>( word.length() + 1 )) {
                    output << "\n" << word;
                    space_left = line_length - word.length();
                } else {
                    output << " " << word;
                    space_left -= word.length() + 1;
                }
            }
        }
        output << "\n";
    }

    return output.str();
}

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

std::string remove_all(
    std::string const& text,
    std::string const& search
) {
    return replace_all( text, search, "" );
}

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

std::string remove_all_chars(
    std::string const& text,
    std::string const& search_chars
) {
    // We do multiple rounds... bit easier for now, and not performance critical.
    auto result = text;
    for( size_t i = 0; i < search_chars.length(); ++i ) {
        result.erase( std::remove( result.begin(), result.end(), search_chars[i] ), result.end() );
    }
    return result;
}

std::string remove_all_non_alnum( std::string const& text )
{
    return remove_all_chars_pred(
        text, []( char c ){
            return ! is_alnum(c);
        }
    );
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
//     Case Conversion
// =================================================================================================

#ifdef GENESIS_AVX2

inline void toggle_case_ascii_inplace_avx_( std::string& str, char char_a, char char_z )
{
    // We use AVX2 here, which uses 256bit = 32byte. Hence, we move through the string in strides
    // of 32. Concidentally, the ASCII marker for "upper/lower case" also has the value 32 (0x20),
    // which might lead to confusion when reading the following code. Be warned.

    // Fill val_32 with 32x 0x20=32
    auto static const val_32 = _mm256_set1_epi8( 0x20 );

    // Fill mask_a with 32x 'a/A', mask_z with 32x 'z/Z'
    auto const mask_a = _mm256_set1_epi8( char_a );
    auto const mask_z = _mm256_set1_epi8( char_z );

    // Loop in increments of 32, which is the AVX vector size in bytes.
    for( size_t i = 0; i < str.size() / 32 * 32; i += 32 ) {
        auto reg = _mm256_loadu_si256( reinterpret_cast<__m256i*>( &str[i] ) );

        // mask_az contains 0x00 where the character is between 'a/A' and 'z/Z', 0xff otherwise.
        auto mask_az = _mm256_or_si256(
            _mm256_cmpgt_epi8( mask_a, reg ), _mm256_cmpgt_epi8( reg, mask_z )
        );

        // Toggle the upper/lower char bit (0x20), 1 means lower case, 0 means upper case.
        reg = _mm256_xor_si256( _mm256_andnot_si256( mask_az, val_32 ), reg );

        _mm256_storeu_si256( reinterpret_cast<__m256i*>( &str[i] ), reg );
    }

    // Convert the rest that remains by toggling the upper/lower case bit.
    for( size_t i = str.size() / 32 * 32; i < str.size(); ++i ) {
        if( char_a <= str[i] && str[i] <= char_z ){
            str[i] ^= 0x20;
        }
    }
}

#endif // GENESIS_AVX2

void to_lower_ascii_inplace( std::string& str )
{
    #ifdef GENESIS_AVX2

    // Toggle the ascii case bit for all values between the two mask boundaries.
    toggle_case_ascii_inplace_avx_( str, 'A', 'Z' );

    #else // GENESIS_AVX2

    // Naive implementation that might use compiler-generated vector intrinsics.
    for( auto& c : str ){
        c = to_lower(c);
    }

    #endif // GENESIS_AVX2
}

std::string to_lower_ascii( std::string const& str )
{
    auto res = str;
    to_lower_ascii_inplace( res );
    return res;
}

void to_upper_ascii_inplace( std::string& str )
{
    #ifdef GENESIS_AVX2

    // Toggle the ascii case bit for all values between the two mask boundaries.
    toggle_case_ascii_inplace_avx_( str, 'a', 'z' );

    #else // GENESIS_AVX2

    // Naive implementation that might use compiler-generated vector intrinsics.
    for( auto& c : str ){
        c = to_upper(c);
    }

    #endif // GENESIS_AVX2
}

std::string to_upper_ascii( std::string const& str )
{
    auto res = str;
    to_upper_ascii_inplace( res );
    return res;
}

// =================================================================================================
//     Normalize
// =================================================================================================

std::string escape( std::string const& text )
{
    static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );
    std::string tmp;
    for( auto c : text ) {
        if( ' ' <= c && c <= '~' && c != '\\' && c != '"') {
            tmp += c;
        } else {
            tmp += '\\';
            switch( c ) {
                case '"':  tmp += '"';  break;
                case '\\': tmp += '\\'; break;
                case '\t': tmp += 't';  break;
                case '\r': tmp += 'r';  break;
                case '\n': tmp += 'n';  break;
                default: {
                    // Without any encoding, we simply opt for a hex representation of the value.
                    char const* const hexdig = "0123456789ABCDEF";
                    tmp += 'x';
                    tmp += hexdig[c >> 4];
                    tmp += hexdig[c & 0xF];
                }
            }
        }
    }
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
        result += word;
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

std::string to_string_byte_format( size_t value )
{
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
    size_t magnitude = 0;
    double size = static_cast<double>(value);

    // Loop until we find the fitting order of magnitude.
    while( size >= 1024 && magnitude < (sizeof(suffixes) / sizeof(suffixes[0])) - 1 ) {
        size /= 1024;
        ++magnitude;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << suffixes[magnitude];
    return oss.str();
}

} // namespace utils
} // namespace genesis
