#ifndef GENESIS_UTILS_TEXT_CONVERT_H_
#define GENESIS_UTILS_TEXT_CONVERT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include <algorithm>
#include <cctype>
#include <limits>
#include <string>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bool Text Conversion
// =================================================================================================

inline bool convert_to_bool( std::string const& str )
{
    // Prep.
    auto const cont = to_lower_ascii( trim( str ));

    // Value check.
    if( cont == "true" || cont == "yes" || cont == "on" || cont == "1" ) {
        return true;
    } else if( cont.empty() || cont == "false" || cont == "no" || cont == "off" || cont == "0" ) {
        return false;
    } else {
        throw std::runtime_error( "String is not convertible to bool." );
    }

    // For old compilers.
    return false;
}

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

inline bool is_convertible_to_bool( std::string const& str )
{
    try{
        convert_to_bool( str );
        return true;
    } catch(...) {
        return false;
    }
}

template<typename ForwardIterator>
bool is_convertible_to_bool(
    ForwardIterator first,
    ForwardIterator last
) {
    try{
        while( first != last ) {
            convert_to_bool( *first );
            ++first;
        }
        return true;
    } catch(...) {
        return false;
    }
}

// =================================================================================================
//     Bool Double Text Conversion
// =================================================================================================

inline double convert_to_bool_double( std::string const& str )
{
    // Prep.
    auto const cont = to_lower_ascii( trim( str ));

    // Value check.
    if( cont == "true" || cont == "yes" || cont == "on" || cont == "1" ) {
        return 1.0;
    } else if( cont == "false" || cont == "no" || cont == "off" || cont == "0" ) {
        return 0.0;
    } else if( cont.empty() ) {
        return std::numeric_limits<double>::quiet_NaN();
    } else {
        throw std::runtime_error( "String is not convertible to bool." );
    }

    // For old compilers.
    return false;
}

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

inline bool is_convertible_to_bool_double( std::string const& str )
{
    try{
        convert_to_bool_double( str );
        return true;
    } catch(...) {
        return false;
    }
}

template<typename ForwardIterator>
bool is_convertible_to_bool_double(
    ForwardIterator first,
    ForwardIterator last
) {
    try{
        while( first != last ) {
            convert_to_bool_double( *first );
            ++first;
        }
        return true;
    } catch(...) {
        return false;
    }
}

// =================================================================================================
//     Double Text Conversion
// =================================================================================================

inline double convert_to_double( std::string const& str )
{
    bool err = false;
    double ret = std::numeric_limits<double>::quiet_NaN();
    if( str.empty() ) {
        return ret;
    }

    try{
        // Try conversion. Throws on failure.
        auto const val = trim( str );
        std::string::size_type sz;
        ret = std::stod( val, &sz );
        if( sz != val.size() ) {
            err = true;
        }
    } catch(...) {
        err = true;
    }
    if( err ) {
        throw std::runtime_error( "String is not convertible to double." );
    }
    return ret;
}

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

inline bool is_convertible_to_double( std::string const& str )
{
    try{
        convert_to_double( str );
        return true;
    } catch(...) {
        return false;
    }

    // For old compilers.
    return false;
}

template<typename ForwardIterator>
bool is_convertible_to_double(
    ForwardIterator first,
    ForwardIterator last
) {
    try{
        while( first != last ) {
            convert_to_double( *first );
            ++first;
        }
        return true;
    } catch(...) {
        return false;
    }
}

} // namespace utils
} // namespace genesis

#endif // include guard
