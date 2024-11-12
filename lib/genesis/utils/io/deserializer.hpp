#ifndef GENESIS_UTILS_IO_DESERIALIZER_H_
#define GENESIS_UTILS_IO_DESERIALIZER_H_

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief Header of Serializer and Deserializer class.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_buffer.hpp"
#include "genesis/utils/io/input_reader.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Deserializer
// =================================================================================================

/**
 * @brief Deserialize values or containers from a binary input stream.
 *
 * The class provides the basic functions to deserialize data types from binary streams,
 * for trivially copyable types, std::strings, and containers.
 *
 * The most convenient way to use this is via the streaming `operator >> ()` overloads of this class:
 *
 *     Deserializer deser( from_file( "my_file.bin" ));
 *     deser >> data;
 *
 * The operator can also be overloaded for user-defined types as needed, and will then also be
 * automatically usable for containers of these types.
 *
 * @see Serializer for the equalivalent class to save those data to a stream.
 */
class Deserializer
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

    explicit Deserializer( std::shared_ptr<BaseInputSource> input_source )
        : buffer_( input_source )
    {
        if( !input_source ) {
            throw std::runtime_error( "Cannot create Deserializer from null input source." );
        }
        if( !buffer_ ) {
            throw std::runtime_error( "Creating Deserializer failed." );
        }
    }

    // -------------------------------------------------------------------------
    //     Stream Status
    // -------------------------------------------------------------------------

    inline operator bool() const
    {
        return buffer_;
    }

    inline bool finished() const
    {
        return !buffer_;
    }

    // -------------------------------------------------------------------------
    //     Deserialization Argument
    // -------------------------------------------------------------------------

    /**
    * @brief Read `n` bytes from the stream and store them in the data.
    *
    * The @p needs to be big enough to hold @p n bytes.
    */
    inline void get( char* data, size_t n )
    {
        size_t const got = buffer_.read( data, n );
        if( got != n ) {
            throw std::runtime_error(
                "Could only read " + std::to_string(got)  + " bytes instead of n=" +
                std::to_string( n ) + " bytes from Deserializer input."
            );
        }
    }

    /**
     * @brief Deserialize trivial types to the stream, by casting it from a char array.
     */
    template <typename T>
    inline
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type
    get( T& value )
    {
        get( reinterpret_cast<char*>( &value ), sizeof(T) );
    }

    /**
     * @brief Deserialize a string, preceeded by its length, from the stream.
     */
    inline void get( std::string& str )
    {
        size_t size = 0;
        get( size );
        str.resize( size );
        get( &str[0], size );
    }

    /**
     * @brief Deserialize the contents of a container (`std::vector`, `std::array` etc)
     * of other serializable types from the stream.
     */
    template <typename T>
    inline
    typename std::enable_if<is_container<T>::value>::type
    get( T& container )
    {
        size_t size = 0;
        get( size );
        container.clear();
        for( size_t i = 0; i < size; ++i ) {
            typename T::value_type element;
            get( element );
            container.insert( container.end(), std::move( element ));
        }
    }

    // -------------------------------------------------------------------------
    //     Deserialization Return
    // -------------------------------------------------------------------------

    /**
     * @brief For trivially copyable types, return the deserialized value directly.
     */
    template <typename T>
    inline
    typename std::enable_if<std::is_trivially_copyable<T>::value, T>::type
    get()
    {
        T data = T{};
        get( data );
        return data;
    }

    /**
     * @brief For strings, return the deserialized string.
     */
    inline std::string get()
    {
        std::string str;
        get( str );
        return str;
    }

    /**
     * @brief For container types, return the deserialized container.
     */
    template <typename T>
    inline
    typename std::enable_if<is_container<T>::value, T>::type
    get()
    {
        T container;
        get( container );
        return container;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    InputBuffer buffer_;

};

// =================================================================================================
//     Streaming Functions
// =================================================================================================

/**
 * @brief Generic operator>> template for trivial non-container types to stream from a Deserializer.
 */
template <typename T>
inline typename std::enable_if<!is_container<T>::value, Deserializer&>::type
operator>>( Deserializer& deserializer, T& value )
{
    deserializer.get( value );
    return deserializer;
}

/**
 * @brief Specialization of operator>> template for std::string to stream from a Deserializer.
 */
inline Deserializer& operator>>( Deserializer& deserializer, std::string& str )
{
    deserializer.get( str );
    return deserializer;
}

/**
 * @brief Specialization of operator>> template for container types to stream from a Deserializer.
 *
 * Overload for container types that have reserve(), using SFINAE
 */
template <typename Container>
inline typename std::enable_if<
    is_container<Container>::value && has_reserve<Container>::value, Deserializer&
>::type
operator>>( Deserializer& deserializer, Container& container )
{
    // First, deserialize the container size
    size_t size = 0;
    deserializer >> size;

    // Clear the container and reserve space
    container.clear();
    container.reserve( size );

    // Deserialize each element individually using operator>> recursively on each element,
    // so that additional overloads of operator>>() can be defined for user types.
    for( size_t i = 0; i < size; ++i ) {
        typename Container::value_type element;
        deserializer >> element;
        container.insert( container.end(), std::move( element ));
    }

    return deserializer;
}

/**
 * @brief Specialization of operator>> template for container types to stream from a Deserializer.
 *
 * Overload for container types that do not have reserve(), using SFINAE
 */
template <typename Container>
inline typename std::enable_if<
    is_container<Container>::value && !has_reserve<Container>::value, Deserializer&
>::type
operator>>( Deserializer& deserializer, Container& container )
{
    // Same as above, but without the reserve() call on the container.
    size_t size = 0;
    deserializer >> size;
    container.clear();
    for( size_t i = 0; i < size; ++i ) {
        typename Container::value_type element;
        deserializer >> element;
        container.insert( container.end(), std::move( element ));
    }
    return deserializer;
}

} // namespace utils
} // namespace genesis

#endif // include guard
