#ifndef GENESIS_UTILS_IO_DESERIALIZER_H_
#define GENESIS_UTILS_IO_DESERIALIZER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
#include <cstring>
#include <fstream>
#include <iostream>
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

    explicit Deserializer( std::string const& file_name )
        : buffer_( std::make_shared< FileInputSource >( file_name ) )
    {
        if( ! buffer_ ) {
            throw std::runtime_error("Creating Deserializer from file failed.");
        }
    }

    explicit Deserializer( std::istream& instream )
        : buffer_( std::make_shared< StreamInputSource >( instream ) )
    {
        if( ! buffer_ ) {
            throw std::runtime_error("Creating Deserializer from stream failed.");
        }
    }

    // -------------------------------------------------------------------------
    //     Stream Status
    // -------------------------------------------------------------------------

    inline operator bool() const
    {
        return buffer_;
    }

    // inline bool good() const
    // {
    //     return instream.good();
    // }
    //
    // inline bool eof() const
    // {
    //     return instream.eof();
    // }
    //
    // inline bool fail() const
    // {
    //     return instream.fail();
    // }
    //
    // inline bool bad() const
    // {
    //     return instream.bad();
    // }
    //
    // inline bool succeeded() const
    // {
    //     return !instream.eof() && instream.peek() == EOF;
    // }

    inline bool finished() const
    {
        return ! buffer_;
    }

    // -------------------------------------------------------------------------
    //     File Status
    // -------------------------------------------------------------------------

    // inline bool is_open() const
    // {
    //     return infile.is_open();
    // }
    //
    // inline void close()
    // {
    //     infile.close();
    // }

    // -------------------------------------------------------------------------
    //     Deserialization
    // -------------------------------------------------------------------------

    /**
    * @brief Read `n` bytes from the stream and store them in the buffer.
    *
    * The buffer needs to be big enough to hold `n` bytes.
    */
    void get_raw(char* buffer, size_t n)
    {
        size_t const got = buffer_.read(buffer, n);
        if( got != n ) {
            throw std::runtime_error(
                "Could only read " + std::to_string(got)  + " bytes instead of n=" +
                std::to_string( n ) + " bytes from Deserializer input."
            );
        }
    }

    /**
     * @brief Reads `n` bytes from the stream and returns whether all of them are `\0` bytes.
     */
    bool get_null (size_t n)
    {
        char* buffer = new char[n];
        get_raw( buffer, n );

        bool ret = true;
        for (size_t i = 0; i < n; ++i) {
            ret &= (buffer[i] == '\0');
        }

        delete[] buffer;
        return ret;
    }

    /**
     * @brief Read `n` bytes from the stream and return them as a string.
     */
    std::string get_raw_string(size_t n)
    {
        char* buffer = new char[n];
        get_raw( buffer, n );

        std::string str (buffer, n);
        delete[] buffer;
        return str;
    }

    /**
     * @brief Read a string from the stream, provided that its length it written preceding it, as done
     * by put_string().
     */
    std::string get_string ()
    {
        size_t len = get_int<size_t>();
        return get_raw_string(len);
    }

    // TODO maybe trailing return types is a solution to make this work without having to specify the template parameters? (also for the othter, similar methods in this class)
    /**
     * @brief Read as many bytes from the stream as the type T holds, and return them in form of
     * a value of type T.
     */
    template<typename T>
    T get_plain ()
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
    void get_plain (T& res)
    {
        get_raw( reinterpret_cast<char*>( &res ), sizeof(T) );
    }

    /**
     * @brief Read an integer number from the stream and return it.
     */
    template<typename T>
    T get_int ()
    {
        return get_plain<T>();
    }

    /**
     * @brief Read an integer number from the stream and store it in the result.
     */
    template<typename T>
    void get_int (T& res)
    {
        res = get_plain<T>();
    }

    /**
     * @brief Read a floating point number from the stream and return it.
     */
    template<typename T>
    T get_float ()
    {
        return get_plain<T>();
    }

    /**
     * @brief Read an floating point number from the stream and store it in the result.
     */
    template<typename T>
    void get_float (T& res)
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
