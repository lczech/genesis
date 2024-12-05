#ifndef GENESIS_UTILS_CORE_STD_H_
#define GENESIS_UTILS_CORE_STD_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
 * @brief Provides some valuable additions to STD.
 *
 * @file
 * @ingroup utils
 */

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     C++ Standard
// =================================================================================================

// Define a macro constant that we can use for testing the C++ compiler standard.
// This is meant in order to avoid the cumbersome tests usually needed if spelled out each time.
// The values are set to what `__cplusplus` gives with GCC and Clang, for ease of comparison.
//
// We set the following values as our internal points of reference:
//
//     #define GENESIS_CPP_STD_11 201103L
//     #define GENESIS_CPP_STD_14 201402L
//     #define GENESIS_CPP_STD_17 201703L
//     #define GENESIS_CPP_STD_20 202002L
//
// These constants shall be used for all tests of the version of the C++ standard,
// for instance `#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17`, around code that needs C++17.
#ifndef GENESIS_CPP_STD

// Define our internally used values for version checks.
#define GENESIS_CPP_STD_11 201103L
#define GENESIS_CPP_STD_14 201402L
#define GENESIS_CPP_STD_17 201703L
#define GENESIS_CPP_STD_20 202002L

// Detect the C++ standard version based on __cplusplus
#if defined(__cplusplus)
    #define GENESIS_CPP_STD __cplusplus
    #pragma message ( "defined(__cplusplus)" )
#else
    // __cplusplus not defined, use C++11 by default
    #define GENESIS_CPP_STD GENESIS_CPP_STD_11
    #pragma message ( "undefined(__cplusplus)" )
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#pragma message ("__cplusplus is " TOSTRING(__cplusplus))

// For MSVC (Microsoft Visual C++)
// Since MSVC does not provide an accurate __cplusplus before C++20 without /Zc:__cplusplus,
// we approximate the C++ standard version using _MSC_VER.
#if defined(_MSC_VER)
    #if _MSC_VER >= 1920  // MSVC 2019 and later
        #if __cplusplus == GENESIS_CPP_STD_20
            // Already correct for C++20
        #else
            // MSVC 2017 and later typically support C++17
            #undef GENESIS_CPP_STD
            #define GENESIS_CPP_STD GENESIS_CPP_STD_17
        #endif
    #elif _MSC_VER >= 1910  // MSVC 2017
        // Approximate as C++14
        #undef GENESIS_CPP_STD
        #define GENESIS_CPP_STD GENESIS_CPP_STD_14
    #elif _MSC_VER >= 1900  // MSVC 2015
        // Approximate as C++11
        #undef GENESIS_CPP_STD
        #define GENESIS_CPP_STD GENESIS_CPP_STD_11
    #else
        // Older MSVC, approximate as C++98.
        // That is too old for our code base,
        // but let's add it for completeness.
        #undef GENESIS_CPP_STD
        #define GENESIS_CPP_STD 199711L
    #endif
#endif

#endif // GENESIS_CPP_STD

// =================================================================================================
//     Expection Handling
// =================================================================================================

// Try to find a macro that expands to the current function name.
#ifdef __cplusplus
#    define GENESIS_FUNC __PRETTY_FUNCTION__
#else
#    if defined __STDC_VERSION__
#        define GENESIS_FUNC __func__
#    else
#        define GENESIS_FUNC __FUNCTION__
// #        define GENESIS_FUNC ((const char *) 0)
#    endif
#endif

// =================================================================================================
//     Compiler Attributes
// =================================================================================================

// Since C++20, there are attributes to help the compiler with branch prediction.
// We use them when available, and otherwise set them to empty statements.
#if ( GENESIS_CPP_STD >= GENESIS_CPP_STD_20 ) && __has_cpp_attribute( likely ) && __has_cpp_attribute( unlikely )
    #define GENESIS_CPP_LIKELY   [[likely]]
    #define GENESIS_CPP_UNLIKELY [[unlikely]]
#else
    #define GENESIS_CPP_LIKELY
    #define GENESIS_CPP_UNLIKELY
#endif

// =================================================================================================
//     Shortcomings of the C++ 11 STL...
// =================================================================================================

/**
 * @brief Returns a std::unique_ptr for a given type.
 *
 * Since this is not available in C++11, we need our own implementation.
 * It is following http://herbsutter.com/gotw/_102/
 * When using this function, make sure to specify at least the `utils` namespace, even from within
 * that namespace, in order to avoid name overloads if compiled with C++17.
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * @brief Proxy class to hold an element accessible via arrow operator.
 *
 * This is useful for implementing an iterator whose arrow operator would return an r-value.
 * Inspired by https://stackoverflow.com/a/26496041/4184258
 */
template <typename T>
class ArrowOperatorProxy {
public:

    explicit ArrowOperatorProxy( T const& v )
        : t(v)
    {}

    T* operator ->() const {
        return &t;
    }

private:

    T t;
};

// =================================================================================================
//     Helpers for portability between C++ 11 and later
// =================================================================================================

// -------------------------------------------------------------------------
//     invoke_result / result_of
// -------------------------------------------------------------------------

/**
 * @brief Helper to abstract from `std::invoke_result` and `std::result_of`
 * for different C++ standards.
 *
 * This simply switches between the two, depending on the C++ standard being compiled with.
 * This is necessary as in later versions, `std::result_of` is deprecated and removed.
 * Usage similar to `std::invoke_result`, such as
 *
 *     typename genesis_invoke_result<F, Args...>::type
 *
 * to obtain the resulting type of a call of function `F` with arguments `Args`.
 */
template <typename F, typename... Args>
struct genesis_invoke_result
{
    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
        // C++17 or later
        using type = typename std::invoke_result<F, Args...>::type;
    #else
        // C++11 and C++14. Tricky to get right.

        // First attempt, simply forwarding.
        // using type = typename std::result_of<F(Args...)>::type;

        // Using decltype to get the correct return type for callable objects, including lambdas
        // using type = decltype( std::declval<F>()( std::declval<Args>()... ));

        // Use decltype to deduce the result type, ensuring F is callable with Args...
        // using type = typename std::enable_if<
        //     std::is_function<typename std::remove_pointer<F>::type>::value ||
        //     std::is_function<F>::value,
        //     decltype(std::declval<F>()(std::declval<Args>()...))
        // >::type;

        // Refined version with decay for lambdas.
        using type = typename std::result_of<typename std::decay<F>::type(Args...)>::type;
    #endif
};

// -------------------------------------------------------------------------
//     container traits
// -------------------------------------------------------------------------

// Implement void_t for C++11.
// We are not using the C++17 version here, for simplicity.
template <typename...>
using void_t = void;

template <typename T, typename = void>
struct is_container : std::false_type {};

/**
 * @brief Helper trait to detect if T has an iterator, i.e., offers begin() and end() functions,
 * indicating it is a container.
 */
template <typename T>
struct is_container<
    T, void_t<
        typename T::value_type,
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end())
    >
> : std::true_type {};

// // Specialization to explicitly exclude std::string from being a container
// template <>
// struct is_container<std::string> : std::false_type {};

// Trait to detect the presence of a reserve method
template <typename T, typename = void>
struct has_reserve : std::false_type {};

template <typename T>
struct has_reserve<
    T, void_t<decltype(std::declval<T&>().reserve(std::declval<size_t>()))>
> : std::true_type {};

// Convenience variable template for has_reserve trait
// template <typename T>
// constexpr bool has_reserve_v = has_reserve<T>::value;

// Convenience bool for has_reserve trait (C++11 alternative to constexpr variable template)
template <typename T>
struct has_reserve_v {
    static const bool value = has_reserve<T>::value;
};

// =================================================================================================
//     Hash Helpers
// =================================================================================================

inline constexpr std::size_t combine_hashes_32( std::size_t h1, std::size_t h2 )
{
    return h1 ^ ( h2 + 0x9e3779b9 + ( h1 << 6 ) + ( h1 >> 2 ));
}

inline constexpr std::size_t combine_hashes_64( std::size_t h1, std::size_t h2 )
{
    return h1 ^ ( h2 + 0x9e3779b97f4a7c16 + ( h1 << 12 ) + ( h1 >> 4 ));
}

/**
 * @brief Combine two hash values.
 */
inline constexpr std::size_t combine_hashes( std::size_t h1, std::size_t h2 )
{
    // We use a run-time check for the size of size_t,
    // which however most likely is already resolved at compile time.
    // The inner functions use the golden ratio phi = (1 + sqrt(5))/2 as an irrational number
    // with random independent bits, by using its inverse and the max size:
    // 2^64 / phi = 0x9e3779b97f4a7c16 for the 64bit version for example.
    // Furthermore, shifting is added in order to spread bits around for greater diversity.
    // This whole approach follows the Boost hash combine functions.

    static_assert(
        ( sizeof( std::size_t ) == 4 ) || ( sizeof( std::size_t ) == 8 ),
        "Need sizeof( std::size_t ) to be 4 or 8 (32bit or 64bit integer)"
    );
    return ( sizeof( std::size_t ) == 4 )
        ? combine_hashes_32( h1, h2 )
        : combine_hashes_64( h1, h2 )
    ;

    // constexpr in C++11 cannot have multiple return values
    // if( sizeof( std::size_t ) == 4 ) {
    //     return combine_hashes_32( seed, value );
    // } else if( sizeof( std::size_t ) == 8 ) {
    //     return combine_hashes_64( seed, value );
    // } else {
    //     throw std::runtime_error( "Invalid std::size_t size." );
    // }
}

/**
 * @brief Combine a seed value (e.g., another hash) with the hash of a given type.
 */
template <typename T>
inline constexpr std::size_t hash_combine( std::size_t seed, T const& value )
{
    return combine_hashes( seed, std::hash<T>()( value ));
}

} // namespace utils
} // namespace genesis

#endif // include guard
