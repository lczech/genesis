#ifndef GENESIS_UTILS_TEXT_STRING_H_
#define GENESIS_UTILS_TEXT_STRING_H_

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

bool equals_ci( std::string const& lhs, std::string const& rhs );

bool starts_with( std::string const & text, std::string const & start );
bool ends_with(   std::string const & text, std::string const & ending );

// =================================================================================================
//     Find and Count
// =================================================================================================

size_t count_substring_occurrences( std::string const& str, std::string const& sub );

std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters = " ",
    const bool trim_empty = true
);

// =================================================================================================
//     Manipulate
// =================================================================================================

std::string replace_all (
    std::string const& text,
    std::string const& search,
    std::string const& replace
);

std::string trim_right (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

std::string trim_left (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

std::string trim (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

// =================================================================================================
//     Normalize
// =================================================================================================

std::string to_lower( std::string const& str );
std::string to_upper( std::string const& str );

std::string escape  ( std::string const& text );
std::string deescape( std::string const& text );

std::string unify_newlines( std::string const& s );

// =================================================================================================
//     Output
// =================================================================================================

std::string to_string_precise( double value, const int precision = 6 );

template <typename T>
std::string join( T const& v, std::string const& delimiter ) {
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
