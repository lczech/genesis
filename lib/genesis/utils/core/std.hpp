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

namespace genesis {
namespace utils {

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

// We define them here as specified by later standards.
// Using them this way allows us to easily switch them off (by turning them into empty statements).
// #define GENESIS_LIKELY   [[likely]]
// #define GENESIS_UNLIKELY [[unlikely]]
#define GENESIS_LIKELY
#define GENESIS_UNLIKELY

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
