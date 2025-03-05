#ifndef GENESIS_UTILS_TEXT_CONVERT_H_
#define GENESIS_UTILS_TEXT_CONVERT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/text/string.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    #include <charconv>
    #include <string_view>
    #include <system_error>

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

namespace genesis {
namespace utils {

// =================================================================================================
//     Generic Conversion
// =================================================================================================

/**
 * @brief Generic conversion from string to any data type that is supported by `std::stringsteam operator >>`.
 *
 * This function is useful for general conversion. It throw when the string cannot be fully
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
        ss.peek();
        good = ss.eof();
    } catch(...) {
        good = false;
    }

    // If we are here, either the string stream conversion itself failed (that's what the catch
    // is for), or there was more data in the stream that we could convert. Either way, throw.
    if( !good ) {
        throw std::invalid_argument(
            "Cannot convert string \"" + str + "\" to type " + std::string( typeid(T).name() )
        );
    }
    return value;
}

/**
 * @brief Specialization of the generic conversion function for `std::string`.
 */
template <>
inline std::string convert_from_string<std::string>( std::string const& str, bool trim )
{
    // We need special treatment of strings here, as the stringstream would only give
    // us the first word of the input otherwise.
    return (trim ? utils::trim(str) : str);
}

/**
 * @brief Specialization of the generic conversion function for `double`, which also takes
 * `nan` and `inf` into account.
 */
template <>
inline double convert_from_string<double>( std::string const& str, bool trim )
{
    // For double, we use a more fitting function that also parses inf and nan.
    auto const input = (trim ? utils::trim(str) : str);
    char* end = nullptr;
    auto const value = std::strtod( input.c_str(), &end );
    if( end == nullptr || *end != '\0' || end - input.c_str() != static_cast<long>( input.size() )) {
        throw std::runtime_error(
            "Cannot convert string \"" + input + "\" to type double"
        );
    }
    return value;
}

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

/**
 * @brief Converts a string-like object to a numeric value using std::from_chars from C++17.
 *
 * This function accepts any string-like type convertible to std::string_view (e.g., std::string,
 * std::string_view, or even C-style strings) and behaves similarly to the standard conversion
 * functions (like std::stoul) by throwing exceptions on errors.
 *
 * @tparam T The numeric type to convert to.
 * @tparam S The string-like type (e.g., std::string, std::string_view).
 * @param str The input string-like object.
 * @param base Numeric base for the conversion (default is 10).
 * @return The converted numeric value of type T.
 * @throws std::invalid_argument If no conversion could be performed or if there are extra characters.
 * @throws std::out_of_range If the converted value is out of range.
 */
template <typename T, typename S>
T convert_from_chars( S const& str, int base = 10 )
{
    // Convert the input to a string_view, and convert to the output.
    std::string_view sv{str};
    T value{};
    auto [ptr, ec] = std::from_chars( sv.data(), sv.data() + sv.size(), value, base );

    // Check for any conversion errors
    if( ec == std::errc::invalid_argument ) {
        throw std::invalid_argument(
            "Invalid argument in std::from_char: no conversion could be performed, input \"" +
            std::string( sv ) + "\""
        );
    }
    if( ec == std::errc::result_out_of_range ) {
        throw std::out_of_range(
            "Out of range in std::from_char: the value is too large or too small, input \"" +
            std::string( sv ) + "\""
        );
    }

    // Check if there are any trailing characters that weren't processed
    if( ptr != sv.data() + sv.size() ) {
        throw std::invalid_argument(
            "Invalid argument in std::from_char: extra characters found after number, input \"" +
            std::string( sv ) + "\""
        );
    }

    return value;
}

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

// =================================================================================================
//     Bool Text Conversion
// =================================================================================================

/**
 * @brief Convert a string to bool, store the result in @p result,
 * and return whether the conversion as successful.
 *
 * @copydetails is_convertible_to_bool( std::string const& )
 */
bool convert_to_bool( std::string const& str, bool& result );

/**
 * @brief Convert a string to bool, return the result, and throw an exception when the
 * conversion was not successful.
 *
 * @copydetails is_convertible_to_bool( std::string const& )
 */
bool convert_to_bool( std::string const& str );

/**
 * @brief Return whether a string can be converted to bool.
 *
 * Strings that can be converted to bool with this function are `true`, `false`, `yes`, `no`, `on`,
 * `off`, `1`, `0`, and the empty string (evaluated as `false`), with all text case insensitive.
 */
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

/**
 * @brief Convert a string to bool, but store the result as a double in @p result,
 * and return whether the conversion as successful.
 *
 * @copydetails is_convertible_to_bool_double( std::string const& )
 */
bool convert_to_bool_double( std::string const& str, double& result );

/**
 * @brief Convert a string to bool, but return it as a double,
 * and throw an exception when the conversion was not successful.
 *
 * @copydetails is_convertible_to_bool_double( std::string const& )
 */
double convert_to_bool_double( std::string const& str );

/**
 * @brief Return whether a string can be converted to bool (and stored as a double).
 *
 * This conversion is useful to work with dataframes that are all double,
 * for example to work with our Generalized Linear Model implementation, see glm_fit().
 *
 * Strings that can be converted to bool with this function are `true`, `false`, `yes`, `no`, `on`,
 * `off`, `1`, `0`, and the empty string (evaluated as `quiet_NaN`), with all text case insensitive.
 */
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

/**
 * @brief Convert a string to double, store the result in @p result,
 * and return whether the conversion as successful.
 */
bool convert_to_double( std::string const& str, double& result );

/**
 * @brief Convert a string to double, return the result, and throw an exception when the
 * conversion was not successful.
 */
double convert_to_double( std::string const& str );

/**
 * @brief Return whether a string can be converted to double.
 */
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

// =================================================================================================
//     Signed Integer Text Conversion
// =================================================================================================

/**
 * @brief Convert a string to signed integer, store the result in @p result,
 * and return whether the conversion as successful.
 */
bool convert_to_signed_integer( std::string const& str, long long& result );

/**
 * @brief Convert a string to signed integer, return the result, and throw an exception when the
 * conversion was not successful.
 */
long long convert_to_signed_integer( std::string const& str );

/**
 * @brief Return whether a string can be converted to signed integer.
 */
bool is_convertible_to_signed_integer( std::string const& str );

template<typename ForwardIterator>
std::vector<long long> convert_to_signed_integer(
    ForwardIterator first,
    ForwardIterator last,
    size_t size = 0
) {
    // Prep.
    std::vector<long long> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_signed_integer( *first ));
        ++first;
    }
    return ret;
}

template<typename ForwardIterator>
bool is_convertible_to_signed_integer(
    ForwardIterator first,
    ForwardIterator last
) {
    while( first != last ) {
        if( !is_convertible_to_signed_integer( *first )) {
            return false;
        }
        ++first;
    }
    return true;
}

// =================================================================================================
//     Unsigned Integer Text Conversion
// =================================================================================================

/**
 * @brief Convert a string to unsigned integer, store the result in @p result,
 * and return whether the conversion as successful.
 */
bool convert_to_unsigned_integer( std::string const& str, unsigned long long& result );

/**
 * @brief Convert a string to unsigned integer, return the result, and throw an exception when the
 * conversion was not successful.
 */
unsigned long long convert_to_unsigned_integer( std::string const& str );

/**
 * @brief Return whether a string can be converted to unsigned integer.
 */
bool is_convertible_to_unsigned_integer( std::string const& str );

template<typename ForwardIterator>
std::vector<unsigned long long> convert_to_unsigned_integer(
    ForwardIterator first,
    ForwardIterator last,
    size_t size = 0
) {
    // Prep.
    std::vector<unsigned long long> ret;
    ret.reserve( size );

    // Add all values. Throws on error.
    while( first != last ) {
        ret.push_back( convert_to_unsigned_integer( *first ));
        ++first;
    }
    return ret;
}

template<typename ForwardIterator>
bool is_convertible_to_unsigned_integer(
    ForwardIterator first,
    ForwardIterator last
) {
    while( first != last ) {
        if( !is_convertible_to_unsigned_integer( *first )) {
            return false;
        }
        ++first;
    }
    return true;
}

} // namespace utils
} // namespace genesis

#endif // include guard
