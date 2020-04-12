#ifndef GENESIS_UTILS_TEXT_CONVERT_H_
#define GENESIS_UTILS_TEXT_CONVERT_H_

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

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bool Text Conversion
// =================================================================================================

bool convert_to_bool( std::string const& str );
bool is_convertible_to_bool( std::string const& str );

template<typename ForwardIterator>
std::vector<bool> convert_to_bool(
    ForwardIterator first,
    ForwardIterator last,
    size_t size = 0
) {
    // Prep.
    std::vector<bool> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_bool( *first ));
        ++first;
    }
    return ret;
}

template<typename ForwardIterator>
bool is_convertible_to_bool(
    ForwardIterator first,
    ForwardIterator last
) {
    while( first != last ) {
        if( !is_convertible_to_bool( *first )) {
            return false;
        }
        ++first;
    }
    return true;
}

// =================================================================================================
//     Bool Double Text Conversion
// =================================================================================================

double convert_to_bool_double( std::string const& str );
bool is_convertible_to_bool_double( std::string const& str );

template<typename ForwardIterator>
std::vector<double> convert_to_bool_double(
    ForwardIterator first,
    ForwardIterator last,
    size_t size = 0
) {
    // Prep.
    std::vector<double> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_bool_double( *first ));
        ++first;
    }
    return ret;
}

template<typename ForwardIterator>
bool is_convertible_to_bool_double(
    ForwardIterator first,
    ForwardIterator last
) {
    while( first != last ) {
        if( !is_convertible_to_bool_double( *first )) {
            return false;
        }
        ++first;
    }
    return true;
}

// =================================================================================================
//     Double Text Conversion
// =================================================================================================

double convert_to_double( std::string const& str );
bool is_convertible_to_double( std::string const& str );

template<typename ForwardIterator>
std::vector<double> convert_to_double(
    ForwardIterator first,
    ForwardIterator last,
    size_t size = 0
) {
    // Prep.
    std::vector<double> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_double( *first ));
        ++first;
    }
    return ret;
}

template<typename ForwardIterator>
bool is_convertible_to_double(
    ForwardIterator first,
    ForwardIterator last
) {
    while( first != last ) {
        if( !is_convertible_to_double( *first )) {
            return false;
        }
        ++first;
    }
    return true;
}

} // namespace utils
} // namespace genesis

#endif // include guard
