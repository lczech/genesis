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
//     Serializer
// =================================================================================================

/**
 * @brief
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
    //     Serialization Raw
    // -------------------------------------------------------------------------

    /**
     * @brief Write `n` zero bytes (`\0`) to the stream.
     */
    inline void put_null( size_t n = 1 )
    {
        for( size_t i = 0; i < n; ++i ) {
            target_->ostream().put( 0 );
        }
    }

    /**
     * @brief Write raw data, provided as a char array of length `n`, to the stream.
     */
    inline void put_raw( char const* data, size_t n )
    {
        target_->ostream().write( data, n );
    }

    /**
     * @brief Write plain data to the stream, by casting it to a char array.
     */
    template<typename T>
    inline void put_plain( T const v )
    {
        target_->ostream().write( reinterpret_cast<char const*>( &v ), sizeof(v));
    }

    // -------------------------------------------------------------------------
    //     Serialization Types
    // -------------------------------------------------------------------------

    /**
     * @brief Write raw data, provided as a string, to the stream, without writing its length.
     */
    inline void put_raw_string( std::string const& v )
    {
        target_->ostream().write( v.c_str(), v.length() );
    }

    /**
     * @brief Write a string, preceded by its length, to the stream. Use get_string() to read it.
     */
    inline void put_string( std::string const& v )
    {
        size_t len = v.length();
        put_int(len);
        put_raw_string(v);
    }

    /**
     * @brief Write an integer number to the stream.
     *
     * Currently, this simply uses put_plain(), but future versions might change this behaviour and
     * use specific conversions (litte/big endianness, signed/unsigned) before writing.
     */
    template<typename T>
    inline void put_int( T const v )
    {
        put_plain(v);
    }

    /**
     * @brief Write a floating point number to the stream.
     *
     * Currently, this simply uses put_plain(), but future versions might change this behaviour and
     * convert it to some machine-independent format.
     */
    template<typename T>
    inline void put_float( T const v )
    {
        put_plain(v);
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<BaseOutputTarget> target_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
