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
 * @brief Implementation of date and time functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/date_time.hpp"

#include "genesis/utils/text/string.hpp"

#include "genesis/utils/core/logging.hpp"

#include <array>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace genesis {
namespace utils {

// =================================================================================================
//     Convenience Functions
// =================================================================================================

std::string current_date()
{
    std::time_t now = std::time( nullptr );
    std::tm*    ltm = std::localtime( &now );

    if( !ltm ) {
        throw std::runtime_error( "Cannot get current date." );
    }

    char out[12];
    std::sprintf(
        out, "%u-%02u-%02u",
        ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday
    );
    return out;
}

std::string current_time()
{
    std::time_t now = std::time( nullptr );
    std::tm*    ltm = std::localtime( &now );

    if( !ltm ) {
        throw std::runtime_error( "Cannot get current time." );
    }

    char out[10];
    std::sprintf(
        out, "%02u:%02u:%02u",
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec
    );
    return out;
}

// =================================================================================================
//     Date/Time Conversion to std::tm
// =================================================================================================

// Typical locales that we expect to see in scientific data.
static const std::array<std::string, 3> locales_ = { "C", "en_US.UTF-8", "" };

// Typical formats that we expect to see in scientific data.
static const std::array<std::string, 9> formats_ = {
    "%Y-%m-%d", "%Y%m%d", "%Y-%m-%dT%H:%M:%S", "%Y-%m-%d %H:%M:%S", "%Y%m%dT%H%M%S",
    "%Y%m%d %H%M%S", "%Y%m%d%H%M%S", "%H:%M:%S", "%H%M%S"
};

/**
 * @brief Local helper function that does the heavy load of time conversion.
 *
 * We use this variant, which returns its success, to not have to `try catch` in the functions
 * that guess the format.
 */
bool convert_to_tm_( std::string const& str, std::string const& format, std::string const& locale, std::tm& t )
{
    // Init the tm object to all zeros, see https://en.cppreference.com/w/cpp/io/manip/get_time
    t.tm_sec = t.tm_min = t.tm_hour = t.tm_mday = t.tm_mon = t.tm_year = t.tm_wday = t.tm_yday = t.tm_isdst = 0;

    // Run the conversion, using all provided information.
    std::istringstream ss( trim( str ));
    ss.imbue( std::locale( locale ));
    ss >> std::get_time( &t, format.c_str() );

    // Return whether that worked or failed, and whether we also reached the end of the stream.
    // If we did not reach EOF, there is more data in the stream, which means, we only partically
    // matched the string, so that it is not an actual fit.
    return ! ss.fail() && ss.eof();
}

std::tm convert_to_tm( std::string const& str, std::string const& format, std::string const& locale )
{
    std::tm t = {};
    if( !convert_to_tm_( str, format, locale, t )) {
        throw std::invalid_argument(
            "Cannot convert string to tm date/time object."
        );
    }
    return t;
}

std::tm convert_to_tm( std::string const& str, std::string const& format )
{
    std::tm t = {};
    for( auto const& locale : locales_ ) {
        if( convert_to_tm_( str, format, locale, t )) {
            return t;
        }
    }

    throw std::invalid_argument(
        "Cannot convert string to tm date/time object with given format."
    );
}

std::tm convert_to_tm( std::string const& str )
{
    std::tm t = {};

    // The formats that we try here are not dependent on the locale. If we introduce additional
    // formats in the future, it might be necessary to also loop over those, for example the
    // user-preferred locale, to make sure that local date/time formats can be parsed.
    for( auto const& format : formats_ ) {
        if( convert_to_tm_( str, format, "C", t )) {
            return t;
        }
    }

    throw std::invalid_argument(
        "Cannot convert string to tm date/time object with guessed formats."
    );
}

bool is_convertible_to_tm( std::string const& str, std::string const& format, std::string const& locale )
{
    std::tm t = {};
    return convert_to_tm_( str, format, locale, t );
}

bool is_convertible_to_tm( std::string const& str, std::string const& format )
{
    std::tm t = {};
    for( auto const& locale : locales_ ) {
        if( convert_to_tm_( str, format, locale, t )) {
            return true;
        }
    }
    return false;
}

bool is_convertible_to_tm( std::string const& str )
{
    std::tm t = {};
    for( auto const& format : formats_ ) {
        if( convert_to_tm_( str, format, "C", t )) {
            return true;
        }
    }
    return false;
}

} // namespace utils
} // namespace genesis
