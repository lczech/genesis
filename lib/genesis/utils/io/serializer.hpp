#ifndef GENESIS_UTILS_IO_SERIALIZER_H_
#define GENESIS_UTILS_IO_SERIALIZER_H_

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

    explicit Serializer( std::string const& filename )
        : outstream (outfile)
    {
        utils::file_output_stream( filename, outfile, std::ofstream::out | std::ofstream::binary );
    }

    explicit Serializer (std::ostream& outstream)
        : outstream (outstream)
    {}

    ~Serializer()
    {
        outfile.close();
    }

    // -------------------------------------------------------------------------
    //     Stream Status
    // -------------------------------------------------------------------------

    inline operator bool() const
    {
        return !outstream.fail();
    }

    inline bool good() const
    {
        return outstream.good();
    }

    inline bool eof() const
    {
        return outstream.eof();
    }

    inline bool fail() const
    {
        return outstream.fail();
    }

    inline bool bad() const
    {
        return outstream.bad();
    }

    // -------------------------------------------------------------------------
    //     File Status
    // -------------------------------------------------------------------------

    inline bool is_open() const
    {
        return outfile.is_open();
    }

    inline void flush()
    {
        outfile.flush();
    }

    inline void close()
    {
        outfile.close();
    }

    // -------------------------------------------------------------------------
    //     Serialization
    // -------------------------------------------------------------------------

    /**
     * @brief Write `n` zero bytes (`\0`) to the stream.
     */
    void put_null (const size_t n)
    {
        char* buffer = new char[n];
        std::fill(buffer, buffer+n, '\0');
        outstream.write (buffer, n);
        delete[] buffer;
    }

    /**
     * @brief Write raw data, provided as a char array of length `n`, to the stream.
     */
    void put_raw( char const* data, size_t n )
    {
        outstream.write (data, n);
    }

    /**
     * @brief Write raw data, provided as a string, to the stream, without writing its length.
     */
    void put_raw_string (const std::string& v)
    {
        outstream.write (v.c_str(), v.length());
    }

    /**
     * @brief Write a string, preceded by its length, to the stream. Use get_string() to read it.
     */
    void put_string (const std::string& v)
    {
        size_t len = v.length();
        put_int(len);
        put_raw_string(v);
    }

    /**
     * @brief Write plain data to the stream, by casting it to a char array.
     */
    template<typename T>
    void put_plain (const T v)
    {
        outstream.write( reinterpret_cast<char const*>( &v ), sizeof(v));
    }

    /**
     * @brief Write an integer number to the stream.
     *
     * Currently, this simply uses put_plain(), but future versions might change this behaviour and
     * use specific conversions (litte/big endianness, signed/unsigned) before writing.
     */
    template<typename T>
    void put_int (const T v)
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
    void put_float (const T v)
    {
        put_plain(v);
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

protected:
    std::ofstream outfile;
    std::ostream& outstream;
};

} // namespace utils
} // namespace genesis

#endif // include guard
