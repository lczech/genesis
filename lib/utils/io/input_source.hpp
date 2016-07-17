#ifndef GENESIS_UTILS_IO_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/core/fs.hpp"

#include <cstdio>
#include <cstring>
#include <istream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Source Interface
// =================================================================================================

/**
 * @brief Abstract base class for reading byte data from input sources.
 *
 * It offers to read() a certain amount of bytes into a char buffer.
 */
class InputSourceInterface
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    InputSourceInterface() = default;

    InputSourceInterface( InputSourceInterface const& ) = default;
    InputSourceInterface( InputSourceInterface&& )      = default;

    InputSourceInterface& operator= ( InputSourceInterface const& ) = default;
    InputSourceInterface& operator= ( InputSourceInterface&& )      = default;

    virtual ~InputSourceInterface()
    {}

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Read `size` many bytes into the char `buffer`.
     */
    size_t read( char* buffer, size_t size )
    {
        // Non-virtual interface.
        return read_( buffer, size );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    virtual size_t read_( char* buffer, size_t size ) = 0;

};

// =================================================================================================
//     String Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from a string.
 *
 * The input string is provided via the constructor. It is not owned by this class, thus
 * the owner must keep it alive as long as reading from it is required, and is responsbile for
 * destroying it. This class merely keeps a pointer to it.
 */
class StringInputSource : public InputSourceInterface
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    StringInputSource( char const* str, size_t size )
        : instr_( str )
        , rest_size_( size )
    {}

    StringInputSource( std::string const& str )
        : instr_( str.c_str() )
        , rest_size_( str.size() )
    {}

    ~StringInputSource()
    {}

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    size_t read_( char* buffer, size_t size ) override
    {
        // Don't overshoot.
        if( size > rest_size_ ) {
            size = rest_size_;
        }

        // Read.
        std::memcpy( buffer, instr_, size );
        instr_     += size;
        rest_size_ -= size;
        return size;
    }


    char const* instr_;
    size_t      rest_size_;

};

// =================================================================================================
//     Stream Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from an istream.
 *
 * The input stream is provided via the constructor. It is not owned by this class, thus
 * the owner must keep it alive as long as reading from it is required, and is responsbile for
 * destroying it. This class merely keeps a reference to it.
 */
class StreamInputSource : public InputSourceInterface
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit StreamInputSource( std::istream& in )
        : in_( in )
    {}

    ~StreamInputSource()
    {}

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    size_t read_( char* buffer, size_t size ) override
    {
        in_.read( buffer, size );
        return in_.gcount();
    }

    std::istream& in_;
};

// =================================================================================================
//     File Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from a file.
 *
 * The input file name is provided via the constructor.
 *
 * It is also possible to provide a `FILE` pointer directly. In this case, the ownership of the file
 * pointer is taken by this class. Thus, closing the file is done when destructing this class.
 */
class FileInputSource : public InputSourceInterface
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit FileInputSource( std::string const& filename )
    {
        if( ! file_exists( filename ) ) {
            throw std::runtime_error( "File does not exists: " + filename );
        }

        file_ = std::fopen( filename.c_str(), "rb" );

        if( file_ == nullptr ) {
            // TODO use errno here.
            throw std::runtime_error( "Cannot open file: " + filename );
        }

        // We do our own buffering.
        std::setvbuf( file_, 0, _IONBF, 0 );
    }

    explicit FileInputSource( FILE* file )
        : file_( file )
    {
        // We do our own buffering.
        std::setvbuf( file_, 0, _IONBF, 0 );
    }

    ~FileInputSource(){
        std::fclose( file_ );
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    size_t read_( char* buffer, size_t size ) override
    {
        return std::fread( buffer, 1, size, file_ );
    }

    FILE* file_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
