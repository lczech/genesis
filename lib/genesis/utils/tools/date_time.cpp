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

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Thread Safety
// =================================================================================================

/**
 * @brief The std::localtime and std::gmtime functions are not thread safe, due to their shared
 * internal state. Make sure that we can use them from multiple threads.
 *
 * Caveat: Currently, this file is the only place where we use std::localtime and std::gmtime.
 * If this changes later, we might want to move this mutex to be available for other usages as well.
 */
static std::mutex tm_mutex_;

// =================================================================================================
//     Convenience Functions
// =================================================================================================

std::string current_date()
{
    // std::localtime is not threadsafe.
    std::lock_guard<std::mutex> const tm_lock( tm_mutex_ );

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
    // std::localtime is not threadsafe.
    std::lock_guard<std::mutex> const tm_lock( tm_mutex_ );

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

std::time_t tm_to_time( std::tm time, bool use_local_time )
{
    // We take the @p time object by value, as we need a non-const copy anyway to call the function:
    // std::mktime changes and fixes values in the time object.

    std::time_t ret;
    if( use_local_time ) {
        ret = std::mktime( &time );
    } else {

        // Set the time zone to UTC if needed, and store the previous value.
        // Unfortunately, some of the functions are not in the std namespace... Ugly C++ standard!
        char* tz;
        tz = ::getenv("TZ");
        ::setenv("TZ", "", 1);
        ::tzset();

        // Convert.
        ret = std::mktime( &time );

        // Reverse the time zone if needed.
        if( tz ) {
            ::setenv("TZ", tz, 1);
        } else {
            ::unsetenv("TZ");
        }
        ::tzset();
    }
    if( ret == -1 ) {
        throw std::invalid_argument( "Cannot convert std::tm object to std::time." );
    }
    return ret;
}

std::vector<std::time_t> tm_to_time( std::vector<std::tm> const& times, bool use_local_time )
{
    return tm_to_time( times.begin(), times.end(), use_local_time, times.size() );
}

std::tm time_to_tm( std::time_t const& time, bool use_local_time )
{
    // std::localtime and std::gmtime are not threadsafe.
    std::lock_guard<std::mutex> const tm_lock( tm_mutex_ );

    std::tm* ret;
    if( use_local_time ) {
        ret = std::localtime( &time );
    } else {
        ret = std::gmtime( &time );
    }
    if( ret == nullptr ) {
        if( errno == EOVERFLOW ) {
            throw std::invalid_argument(
                "Cannot convert std::time object to std::tm, because the argument is too large."
            );
        } else {
            throw std::invalid_argument( "Cannot convert std::time object to std::tm." );
        }
    }
    return *ret;
}

std::vector<std::tm> time_to_tm( std::vector<std::time_t> const& times, bool use_local_time )
{
    return time_to_tm( times.begin(), times.end(), use_local_time, times.size() );
}

// =================================================================================================
//     Date/Time Conversion from std::tm
// =================================================================================================

std::string tm_to_string( std::tm const& time, std::string const& format, std::string const& locale )
{
    std::ostringstream ss{};
    ss.imbue(std::locale( locale ));
    ss << std::put_time( &time, format.c_str() );
    return ss.str();
}

std::string tm_to_string( std::tm const& time, std::string const& format )
{
    return tm_to_string( time, format, "C" );
}

std::string tm_to_string( std::tm const& time )
{
    return tm_to_string( time, "%Y-%m-%dT%H:%M:%S", "C" );
}

std::string tm_date_to_string( std::tm const& time )
{
    return tm_to_string( time, "%Y-%m-%d", "C" );
}

std::string tm_time_to_string( std::tm const& time )
{
    return tm_to_string( time, "%H:%M:%S", "C" );
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
    // We are re-using the object when called from looping functions, so this is necessary.
    t.tm_sec   = 0;
    t.tm_min   = 0;
    t.tm_hour  = 0;
    t.tm_mday  = 0;
    t.tm_mon   = 0;
    t.tm_year  = 0;
    t.tm_wday  = 0;
    t.tm_yday  = 0;
    t.tm_isdst = 0;

    // Run the conversion, using all provided information.
    std::istringstream ss( trim( str ));
    ss.imbue( std::locale( locale ));
    ss >> std::get_time( &t, format.c_str() );

    // Return whether that worked or failed, and whether we also reached the end of the stream.
    // If we did not reach EOF, there is more data in the stream, which means, we only partially
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
    // If one of the locales works, the string is convertible.
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
    // If one of the formats works, the string is convertible.
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
