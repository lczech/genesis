/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/text/convert.hpp"

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

bool convert_to_bool( std::string const& str, bool& result )
{
    // Prep.
    auto const cont = to_lower_ascii( trim( str ));

    // Value check.
    if( cont == "true" || cont == "yes" || cont == "on" || cont == "1" ) {
        result = true;
        return true;
    } else if( cont.empty() || cont == "false" || cont == "no" || cont == "off" || cont == "0" ) {
        result = false;
        return true;
    }
    return false;
}

bool convert_to_bool( std::string const& str )
{
    bool result;
    if( !convert_to_bool( str, result )) {
        throw std::runtime_error( "String is not convertible to bool." );
    }
    return result;
}

bool is_convertible_to_bool( std::string const& str )
{
    bool result;
    return convert_to_bool( str, result );
}

// =================================================================================================
//     Bool Double Text Conversion
// =================================================================================================

bool convert_to_bool_double( std::string const& str, double& result )
{
    // Prep.
    auto const cont = to_lower_ascii( trim( str ));

    // Value check. Need a bit of code duplciation here in order to catch the empty case.
    if( cont == "true" || cont == "yes" || cont == "on" || cont == "1" ) {
        result = 1.0;
        return true;
    } else if( cont == "false" || cont == "no" || cont == "off" || cont == "0" ) {
        result = 0.0;
        return true;
    } else if( cont.empty() ) {
        result = std::numeric_limits<double>::quiet_NaN();
        return true;
    }
    return false;
}

double convert_to_bool_double( std::string const& str )
{
    double result;
    if( !convert_to_bool_double( str, result )) {
        throw std::runtime_error( "String is not convertible to bool." );
    }
    return result;
}

bool is_convertible_to_bool_double( std::string const& str )
{
    double result;
    return convert_to_bool_double( str, result );
}

// =================================================================================================
//     Conversion Helper
// =================================================================================================

template<typename T, class F>
bool convert_to_numeric_( std::string const& str, T& result, T initial, F conversion )
{
    // Some default cases. Ignore whitespace. Accepty emptry string.
    bool err = false;
    result = initial;
    auto const lower = trim( str );
    if( lower.empty() ) {
        return true;
    }

    try{
        // Try conversion. Throws on failure, which we catch.
        std::size_t pos;
        result = conversion( lower, &pos );
        if( pos != lower.size() ) {
            err = true;
        }
    } catch(...) {
        err = true;
    }
    return !err;
}

// =================================================================================================
//     Double Text Conversion
// =================================================================================================

bool convert_to_double( std::string const& str, double& result )
{
    // Need to use a lambda here, see https://stackoverflow.com/a/71310216/4184258
    return convert_to_numeric_(
        str, result, std::numeric_limits<double>::quiet_NaN(),
        []( std::string const& str, std::size_t* pos ){
            return std::stod( str, pos );
        }
    );
}

double convert_to_double( std::string const& str )
{
    double result;
    if( !convert_to_double( str, result )) {
        throw std::runtime_error(
            "String is not convertible to double."
        );
    }
    return result;
}

bool is_convertible_to_double( std::string const& str )
{
    double result;
    return convert_to_double( str, result );
}

// =================================================================================================
//     Signed Integer Text Conversion
// =================================================================================================

bool convert_to_signed_integer( std::string const& str, long long& result )
{
    // Need to use a lambda here, see https://stackoverflow.com/a/71310216/4184258
    return convert_to_numeric_(
        str, result, static_cast<long long>(0),
        []( std::string const& str, std::size_t* pos ){
            return std::stoll( str, pos );
        }
    );
}

long long convert_to_signed_integer( std::string const& str )
{
    long long result;
    if( !convert_to_signed_integer( str, result )) {
        throw std::runtime_error(
            "String is not convertible to signed integer (long long)."
        );
    }
    return result;
}

bool is_convertible_to_signed_integer( std::string const& str )
{
    long long result;
    return convert_to_signed_integer( str, result );
}

// =================================================================================================
//     Unsigned Integer Text Conversion
// =================================================================================================

bool convert_to_unsigned_integer( std::string const& str, unsigned long long& result )
{
    // Need to use a lambda here, see https://stackoverflow.com/a/71310216/4184258
    return convert_to_numeric_(
        str, result, static_cast<unsigned long long>(0),
        []( std::string const& str, std::size_t* pos ){
            return std::stoull( str, pos );
        }
    );
}

unsigned long long convert_to_unsigned_integer( std::string const& str )
{
    unsigned long long result;
    if( !convert_to_unsigned_integer( str, result )) {
        throw std::runtime_error(
            "String is not convertible to unsigned integer (unsigned long long)."
        );
    }
    return result;
}

bool is_convertible_to_unsigned_integer( std::string const& str )
{
    unsigned long long result;
    return convert_to_unsigned_integer( str, result );
}

} // namespace utils
} // namespace genesis
