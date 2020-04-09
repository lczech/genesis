#ifndef GENESIS_UTILS_TOOLS_DATE_TIME_H_
#define GENESIS_UTILS_TOOLS_DATE_TIME_H_

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
 * @brief Provides functions for date and time access.
 *
 * @file
 * @ingroup utils
 */

#include <ctime>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Convenience Functions
// =================================================================================================

/**
 * @brief Returns the current date as a string in the format "2014-12-31".
 */
std::string current_date();

/**
 * @brief Returns the current time as a string in the format "13:37:42".
 */
std::string current_time();

/**
 * @brief Convert `std::tm` object to a `std::time_t` object.
 *
 * By default, we use Coordinated Universal Time (UTC), so that results are reproducible.
 * If local time is needed instead, set @p use_local_time to `true`.
 */
std::time_t tm_to_time( std::tm time, bool use_local_time = false );

/**
 * @brief Convert a `std::vector` of `std::tm` objects to a `std::vector` of `std::time_t` objects.
 *
 * By default, we use Coordinated Universal Time (UTC), so that results are reproducible.
 * If local time is needed instead, set @p use_local_time to `true`.
 */
std::vector<std::time_t> tm_to_time( std::vector<std::tm> const& times, bool use_local_time = false );

/**
 * @brief Convert a list of `std::tm` objects to a `std::vector` of `std::time_t` objects.
 *
 * By default, we use Coordinated Universal Time (UTC), so that results are reproducible.
 * If local time is needed instead, set @p use_local_time to `true`.
 *
 * If the @p size of the list is known, this can be given to reserve that much memory first.
 */
template<typename ForwardIterator>
std::vector<std::time_t> tm_to_time(
    ForwardIterator first,
    ForwardIterator last,
    bool use_local_time = false,
    size_t size = 0
) {
    // Prep.
    std::vector<std::time_t> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( tm_to_time( *first, use_local_time ));
        ++first;
    }
    return ret;
}

/**
 * @brief Convert `std::time_t` object to a `std::tm` object.
 *
 * By default, we use Coordinated Universal Time (UTC), so that results are reproducible.
 * If local time is needed instead, set @p use_local_time to `true`.
 */
std::tm time_to_tm( std::time_t const& time, bool use_local_time = false );

/**
 * @brief Convert a `std::vector` of `std::time_t` objects to a `std::vector` of `std::tm` objects.
 *
 * By default, we use Coordinated Universal Time (UTC), so that results are reproducible.
 * If local time is needed instead, set @p use_local_time to `true`.
 */
std::vector<std::tm> time_to_tm( std::vector<std::time_t> const& times, bool use_local_time = false );

/**
 * @brief Convert a list of `std::time_t` objects to a `std::vector` of `std::tm` objects.
 *
 * By default, we use Coordinated Universal Time (UTC), so that results are reproducible.
 * If local time is needed instead, set @p use_local_time to `true`.
 *
 * If the @p size of the list is known, this can be given to reserve that much memory first.
 */
template<typename ForwardIterator>
std::vector<std::tm> time_to_tm(
    ForwardIterator first,
    ForwardIterator last,
    bool use_local_time = false,
    size_t size = 0
) {
    // Prep.
    std::vector<std::tm> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( time_to_tm( *first, use_local_time ));
        ++first;
    }
    return ret;
}

// =================================================================================================
//     Date/Time Conversion from std::tm
// =================================================================================================

/**
 * @brief Print the given `std::tm` object as a `std::string`, using the @p format and @p locale.
 *
 * For a list of the available @p format parameters, see https://en.cppreference.com/w/cpp/io/manip/get_time
 * or http://www.cplusplus.com/reference/ctime/strftime/
 *
 * The available @p locale strings depend on the operating system and user settings. Typically,
 * on Unix/Linux/POSIX/MacOS machines, the command `locale -a` should give a valid list.
 */
std::string tm_to_string( std::tm const& time, std::string const& format, std::string const& locale );

/**
 * @brief Print the given `std::tm` object as a `std::string`, using the @p format.
 *
 * If the @p format uses locale-depended qualifiers, the "C" locale is used.
 */
std::string tm_to_string( std::tm const& time, std::string const& format );

/**
 * @brief Print the given `std::tm` object as a `std::string`, using the ISO 8601 extended format.
 *
 * <!-- need to escape the % sign here for doxygen... -->
 * The format is `%%Y-%%m-%%dT%%H:%%M:%%S`, for example 2020-04-17T00:27:58.
 */
std::string tm_to_string( std::tm const& time );

/**
 * @brief Print the given `std::tm` object as a `std::string` containing only the date,
 * using the ISO 8601 extended format.
 *
 * <!-- need to escape the % sign here for doxygen... -->
 * The format is `%%Y-%%m-%%d`, for example 2020-04-17.
 */
std::string tm_date_to_string( std::tm const& time );

/**
 * @brief Print the given `std::tm` object as a `std::string` containing only the time,
 * using the ISO 8601 extended format.
 *
 * <!-- need to escape the % sign here for doxygen... -->
 * The format is `%%H:%%M:%%S`, for example 00:27:58.
 */
std::string tm_time_to_string( std::tm const& time );

// =================================================================================================
//     Date/Time Conversion to std::tm
// =================================================================================================

/**
 * @brief Convert a `std::string` to a `std::tm` date/time object, if possible. Throw otherwise.
 *
 * For a list of the available @p format parameters, see https://en.cppreference.com/w/cpp/io/manip/get_time
 * or http://www.cplusplus.com/reference/ctime/strftime/
 *
 * The available @p locale strings depend on the operating system and user settings. Typically,
 * on Unix/Linux/POSIX/MacOS machines, the command `locale -a` should give a valid list.
 */
std::tm convert_to_tm( std::string const& str, std::string const& format, std::string const& locale );

/**
 * @brief Convert a `std::string` to a `std::tm` date/time object, if possible. Throw otherwise.
 *
 * For a list of the available @p format parameters, see https://en.cppreference.com/w/cpp/io/manip/get_time
 * or http://www.cplusplus.com/reference/ctime/strftime/
 *
 * The function tries different std::locale settings for convenience,
 * in the order `"C"` (POSIX), `"en_US.UTF-8"`, and `""` (user-preferred). If a @p format is used
 * that does not depend on the locale, this does not induce a performance penalty. In order to also
 * specify the locale, use convert_to_tm( std::string const&, std::string const&, std::string const& ) instead.
 */
std::tm convert_to_tm( std::string const& str, std::string const& format );

/**
 * @brief Convert a `std::string` to a `std::tm` date/time object, if possible. Throw otherwise.
 *
 * The function tries different common formats for convenience, in the following order:
 * First only dates, then dates/times combined, last only times. The reasoning for that order is
 * that in most typical scientific datasets, the day is more relevant than the exact time during
 * the day, so they mostly likely contain dates only.
 *
 * The exact formats that are tried are as follows:
 *
 * <!-- need to escape the % sign here for doxygen... -->
 * # | Format | Type | Explanation | Example
 * --|--------|------|-------------|---------
 * 1 | `%%Y-%%m-%%d` | Date | ISO 8601 extended format | 2020-04-17
 * 2 | `%%Y%%m%%d` | Date | ISO 8601 basic format | 20200417
 * 3 | `%%Y-%%m-%%dT%%H:%%M:%%S` | Date/Time | ISO 8601 extended format | 2020-04-17T00:27:58
 * 4 | `%%Y-%%m-%%d %%H:%%M:%%S` | Date/Time | ISO 8601 extended format, but separated by a whitespace, as allowed in RFC 3339 | 2020-04-17 00:27:58
 * 5 | `%%Y%%m%%dT%%H%%M%%S` | Date/Time | ISO 8601 basic format | 20200417T002758
 * 6 | `%%Y%%m%%d %%H%%M%%S` | Date/Time | ISO 8601 basic format, but separated by a whitespace | 20200417 002758
 * 7 | `%%Y%%m%%d%%H%%M%%S` | Date/Time | ISO 8601 basic format, with no separator | 20200417002758
 * 8 | `%%H:%%M:%%S` | Time | ISO 8601 extended format | 00:27:58
 * 9 | `%%H%%M%%S` | Time | ISO 8601 basic format | 002758
 *
 * While this will most likely be able to parse any given date/time string that is used in scientific
 * data, it is almost certainly slower than setting the format manually, if known, using the
 * function convert_to_tm( std::string const&, std::string const& )
 *
 * More exotic formats, such as the stupid US format 04/17/2020, or the slighly more reasonable
 * UK format 17/04/2020, are not tried, as they might be mistaken for one another. Furthermore,
 * time zone information is not used and leads to an exception being thrown
 * (e.g., "2020-04-06T00:27:58+00:00" or "20200406T002758Z"). If those are needed, they have to be
 * specified, again using the function convert_to_tm( std::string const&, std::string const& )
 */
std::tm convert_to_tm( std::string const& str );

/**
 * @brief Convert a list of `std::string` to `std::tm` date/time objects, if possible. Throw otherwise.
 *
 * The function uses the given @p format and @p locale. See convert_to_tm( std::string const&, std::string const&, std::string const& ) for details.
 *
 * If the @p size of the list is known, this can be given to reserve that much memory first.
 */
template<typename ForwardIterator>
std::vector<std::tm> convert_to_tm(
    ForwardIterator first,
    ForwardIterator last,
    std::string const& format,
    std::string const& locale,
    size_t size = 0
) {
    // Prep.
    std::vector<std::tm> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_tm( *first, format, locale ));
        ++first;
    }
    return ret;
}

/**
 * @brief Convert a list of `std::string` to `std::tm` date/time objects, if possible. Throw otherwise.
 *
 * The function uses the given @p format. See convert_to_tm( std::string const&, std::string const& ) for details.
 *
 * If the @p size of the list is known, this can be given to reserve that much memory first.
 */
template<typename ForwardIterator>
std::vector<std::tm> convert_to_tm(
    ForwardIterator first,
    ForwardIterator last,
    std::string const& format,
    size_t size = 0
) {
    // Prep.
    std::vector<std::tm> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_tm( *first, format ));
        ++first;
    }
    return ret;
}

/**
 * @brief Convert a list of `std::string` to `std::tm` date/time objects, if possible. Throw otherwise.
 *
 * The function tries to guess the format. See convert_to_tm( std::string const& ) for details.
 * For long lists, this can be expensive, as the formats are tried again for each entry separately.
 *
 * If the @p size of the list is known, this can be given to reserve that much memory first.
 */
template<typename ForwardIterator>
std::vector<std::tm> convert_to_tm(
    ForwardIterator first,
    ForwardIterator last,
    size_t size = 0
) {
    // Prep.
    std::vector<std::tm> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_tm( *first ));
        ++first;
    }
    return ret;
}

/**
 * @brief Return whether a `std::string` is convertible to a `std::tm` date/time object,
 * that is, whether it contains a date/time in a recognizable format.
 *
 * The function uses the given @p format and @p locale. See convert_to_tm( std::string const&, std::string const&, std::string const& ) for details.
 */
bool is_convertible_to_tm( std::string const& str, std::string const& format, std::string const& locale );

/**
 * @brief Return whether a `std::string` is convertible to a `std::tm` date/time object,
 * that is, whether it contains a date/time in a recognizable format.
 *
 * The function uses the given @p format. See convert_to_tm( std::string const&, std::string const& ) for details.
 */
bool is_convertible_to_tm( std::string const& str, std::string const& format );

/**
 * @brief Return whether a `std::string` is convertible to a `std::tm` date/time object,
 * that is, whether it contains a date/time in a recognizable format.
 *
 * The function tries to guess the format. See convert_to_tm( std::string const& ) for details.
 */
bool is_convertible_to_tm( std::string const& str );

/**
 * @brief Return whether a list of `std::string` is convertible to `std::tm` date/time objects.
 *
 * The function uses the given @p format and @p locale. See convert_to_tm( std::string const&, std::string const&, std::string const& ) for details.
 */
template<typename ForwardIterator>
bool is_convertible_to_tm(
    ForwardIterator first,
    ForwardIterator last,
    std::string const& format,
    std::string const& locale
) {
    // If one of the conversions fails, the list is not convertible.
    while( first != last ) {
        if( ! is_convertible_to_tm( *first, format, locale )) {
            return false;
        }
        ++first;
    }
    return true;
}

/**
 * @brief Return whether a list of `std::string` is convertible to `std::tm` date/time objects.
 *
 * The function uses the given @p format. See convert_to_tm( std::string const&, td::string const& ) for details.
 */
template<typename ForwardIterator>
bool is_convertible_to_tm(
    ForwardIterator first,
    ForwardIterator last,
    std::string const& format
) {
    // If one of the conversions fails, the list is not convertible.
    while( first != last ) {
        if( ! is_convertible_to_tm( *first, format )) {
            return false;
        }
        ++first;
    }
    return true;
}

/**
 * @brief Return whether a list of `std::string` is convertible to `std::tm` date/time objects.
 *
 * The function tries to guess the format. See convert_to_tm( std::string const& ) for details.
 * For long lists, this can be expensive, as the formats are tried again for each entry separately.
 */
template<typename ForwardIterator>
bool is_convertible_to_tm(
    ForwardIterator first,
    ForwardIterator last
) {
    // If one of the conversions fails, the list is not convertible.
    while( first != last ) {
        if( ! is_convertible_to_tm( *first )) {
            return false;
        }
        ++first;
    }
    return true;
}

} // namespace utils
} // namespace genesis

#endif // include guard
