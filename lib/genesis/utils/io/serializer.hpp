#ifndef GENESIS_UTILS_IO_SERIALIZER_H_
#define GENESIS_UTILS_IO_SERIALIZER_H_

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
#include "genesis/utils/io/output_target.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Serializer
// =================================================================================================

/**
 * @brief Serialize values or containers and write them to a binary output target.
 *
 * The class provides the basic functions to serialize data types into binary streams,
 * for trivially copyable types, std::strings, and containers.
 *
 * The most convenient way to use this is via the streaming `operator << ()` overloads of this class:
 *
 *     Serializer serial( to_file( "my_file.bin" ));
 *     serial << data;
 *
 * The operator can also be overloaded for user-defined types as needed, and will then also be
 * automatically usable for containers of these types.
 *
 * @see Deserializer for the equalivalent class to load those data again.
 */
class Serializer
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

    explicit Serializer( std::shared_ptr<BaseOutputTarget> output_target )
        : target_( output_target )
    {
        if( !output_target ) {
            throw std::runtime_error( "Cannot create Serializer from null output target." );
        }
    }

    // -------------------------------------------------------------------------
    //     Stream Status
    // -------------------------------------------------------------------------

    inline void flush()
    {
        target_->flush();
    }

    // -------------------------------------------------------------------------
    //     Serialization
    // -------------------------------------------------------------------------

    /**
     * @brief Serialize raw data, provided as a char array of length `n`, to the stream.
     */
    inline void put( char const* data, size_t n )
    {
        target_->ostream().write( data, n );
    }

    /**
     * @brief Serialize trivial types to the stream, by casting it to a char array.
     */
    template<typename T>
    inline
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type
    put( T const& value )
    {
        target_->ostream().write( reinterpret_cast<char const*>( &value ), sizeof(T));
    }

    /**
     * @brief Serialize a string, preceeded by its length, to the stream.
     */
    inline void put( std::string const& str )
    {
        put( str.size() );
        target_->ostream().write( str.data(), str.size() );
    }

    /**
     * @brief Serialize the contents of a container (`std::vector`, `std::array` etc)
     * of other serializable types to the stream.
     *
     * This works on any container that exposes the data via `begin()` and `end()` functions,
     * and has a size() function to get the number of elements.
     */
    template <typename T>
    inline
    typename std::enable_if<is_container<T>::value>::type
    put( T const& container )
    {
        put( container.size() );
        for( auto const& element : container ) {
            put( element );
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<BaseOutputTarget> target_;

};

// =================================================================================================
//     Streaming Functions
// =================================================================================================

/**
 * @brief Generic operator<< template for trivial non-container types to stream to a Serializer.
 */
template <typename T>
inline typename std::enable_if<!is_container<T>::value, Serializer&>::type
operator<<( Serializer& serializer, T const& value )
{
    serializer.put( value );
    return serializer;
}

/**
 * @brief Specialization of operator<< template for c-strings to stream to a Serializer.
 */
inline Serializer& operator<<( Serializer& serializer, char const* cstr )
{
    // Convert to std::string to use put for strings
    serializer.put( std::string( cstr ));
    return serializer;
}

/**
 * @brief Specialization of operator<< template for std::string to stream to a Serializer.
 */
inline Serializer& operator<<( Serializer& serializer, std::string const& str )
{
    serializer.put( str );
    return serializer;
}

/**
 * @brief Specialization of operator<< template for container types to stream to a Serializer.
 */
template <typename Container>
inline typename std::enable_if<is_container<Container>::value, Serializer&>::type
operator<<( Serializer& serializer, Container const& container )
{
    // Serialize each element individually using operator<< recursively on each element,
    // so that additional overloads of operator<<() can be defined for user types.
    serializer << container.size();
    for( auto const& element : container ) {
        serializer << element;
    }
    return serializer;
}

} // namespace utils
} // namespace genesis

#endif // include guard
