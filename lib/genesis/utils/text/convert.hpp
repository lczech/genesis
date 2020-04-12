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

#include "genesis/utils/text/string.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Generic Conversion
// =================================================================================================

/**
 * @brief Generic conversion from string to any data type that is supported by `std::stringsteam operator >>`.
 *
 * This function is useful for general conversion. It throws when the string cannot be fully
 * converted. That is, if there is more content after the converted value. If @p trim is `false`
 * (default), this is also affected by white space. In that case, the value to be parsed has to be
 * the only content of the input string.
 */
template <typename T>
T convert_from_string( std::string const& str, bool trim = false )
{
    T value;

    // Generic parser that just uses a string stream.
    // Convert, and fail if there is more in the string that we expect.
    bool good = true;
    try {
        std::stringstream ss( trim ? utils::trim(str) : str );
        ss >> std::noskipws >> value;
        good = ss.eof();
    } catch(...) {
        good = false;
    }

    // If we are here, either the string stream conversion itself failed (that's what the catch
    // is for), or there was more data in the stream that we could convert. Either way, throw.
    if( !good ) {
        throw std::invalid_argument( "Cannot convert string '" + str + "' to specified type." );
    }
    return value;
}

/**
 * @brief Specialization of the generic conversion function for for `std::string`.
 */
template <>
inline std::string convert_from_string<std::string>( std::string const& str, bool trim )
{
    // We need special treatment of strings here, as the stringstream would only give
    // us the first word of the input otherwise.
    return (trim ? utils::trim(str) : str);
}

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
