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
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Deserializer
// =================================================================================================

/**
 * @brief
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
    //     Deserialization Raw
    // -------------------------------------------------------------------------

    /**
     * @brief Reads `n` bytes from the stream and returns whether all of them are `\0` bytes.
     */
    inline bool get_null( size_t n )
    {
        char* buffer = new char[n];
        get_raw( buffer, n );

        bool ret = true;
        for( size_t i = 0; i < n; ++i ) {
            ret &= (buffer[i] == '\0');
        }

        delete[] buffer;
        return ret;
    }

    /**
    * @brief Read `n` bytes from the stream and store them in the buffer.
    *
    * The buffer needs to be big enough to hold `n` bytes.
    */
    inline void get_raw( char* buffer, size_t n )
    {
        size_t const got = buffer_.read( buffer, n );
        if( got != n ) {
            throw std::runtime_error(
                "Could only read " + std::to_string(got)  + " bytes instead of n=" +
                std::to_string( n ) + " bytes from Deserializer input."
            );
        }
    }

    /**
     * @brief Read as many bytes from the stream as the type T holds, and return them in form of
     * a value of type T.
     */
    template<typename T>
    inline T get_plain()
    {
        T res;
        get_raw( reinterpret_cast<char*>( &res ), sizeof(T) );
        return res;
    }

    /**
     * @brief Read as many bytes from the stream as the type T holds, and put them in the result
     * value of type T.
     */
    template<typename T>
    inline void get_plain( T& res )
    {
        get_raw( reinterpret_cast<char*>( &res ), sizeof(T) );
    }

    // -------------------------------------------------------------------------
    //     Deserialization Types
    // -------------------------------------------------------------------------

    /**
     * @brief Read `n` bytes from the stream and return them as a string.
     */
    inline std::string get_raw_string( size_t n )
    {
        char* buffer = new char[n];
        get_raw( buffer, n );

        std::string str( buffer, n );
        delete[] buffer;
        return str;
    }

    /**
     * @brief Read a string from the stream, provided that its length it written preceding it, as done
     * by put_string().
     */
    inline std::string get_string()
    {
        size_t len = get_int<size_t>();
        return get_raw_string(len);
    }

    /**
     * @brief Read an integer number from the stream and return it.
     */
    template<typename T>
    inline T get_int()
    {
        return get_plain<T>();
    }

    /**
     * @brief Read an integer number from the stream and store it in the result.
     */
    template<typename T>
    inline void get_int( T& res )
    {
        res = get_plain<T>();
    }

    /**
     * @brief Read a floating point number from the stream and return it.
     */
    template<typename T>
    inline T get_float()
    {
        return get_plain<T>();
    }

    /**
     * @brief Read an floating point number from the stream and store it in the result.
     */
    template<typename T>
    inline void get_float( T& res )
    {
        res = get_plain<T>();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    InputBuffer buffer_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
