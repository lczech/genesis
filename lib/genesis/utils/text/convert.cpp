/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

/**
 * @brief Local helper function. Converts a string to bool, storing the result in @p result,
 * and returning whether the conversionw as successfull.
 */
bool convert_to_bool_( std::string const& str, bool& result )
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
    if( !convert_to_bool_( str, result )) {
        throw std::runtime_error( "String is not convertible to bool." );
    }
    return result;
}

bool is_convertible_to_bool( std::string const& str )
{
    bool result;
    return convert_to_bool_( str, result );
}

// =================================================================================================
//     Bool Double Text Conversion
// =================================================================================================

/**
 * @brief Local helper function. Converts a string to bool, but stores it as a double,
 * storing the result in @p result, and returning whether the conversionw as successfull.
 */
bool convert_to_bool_double_( std::string const& str, double& result )
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

    // For old compilers.
    return false;
}

double convert_to_bool_double( std::string const& str )
{
    double result;
    if( !convert_to_bool_double_( str, result )) {
        throw std::runtime_error( "String is not convertible to bool." );
    }
    return result;
}

bool is_convertible_to_bool_double( std::string const& str )
{
    double result;
    return convert_to_bool_double_( str, result );
}

// =================================================================================================
//     Double Text Conversion
// =================================================================================================

/**
 * @brief Local helper function. Converts a string to double, storing the result in @p result,
 * and returning whether the conversionw as successfull.
 */
bool convert_to_double_( std::string const& str, double& result )
{
    bool err = false;
    result = std::numeric_limits<double>::quiet_NaN();
    if( str.empty() ) {
        return true;
    }

    try{
        // Try conversion. Throws on failure.
        auto const val = trim( str );
        std::string::size_type sz;
        result = std::stod( val, &sz );
        if( sz != val.size() ) {
            err = true;
        }
    } catch(...) {
        err = true;
    }
    return !err;
}

double convert_to_double( std::string const& str )
{
    double result;
    if( !convert_to_double_( str, result )) {
        throw std::runtime_error( "String is not convertible to double." );
    }
    return result;
}

bool is_convertible_to_double( std::string const& str )
{
    double result;
    return convert_to_double_( str, result );
}

} // namespace utils
} // namespace genesis
