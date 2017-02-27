#ifndef GENESIS_UTILS_IO_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/core/fs.hpp"

#include <cstdio>
#include <cstring>
#include <istream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Base Input Source
// =================================================================================================

/**
 * @brief Abstract base class for reading byte data from input sources.
 *
 * It offers to read() a certain amount of bytes into a char buffer.
 */
class BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    BaseInputSource() = default;

    BaseInputSource( BaseInputSource const& ) = default;
    BaseInputSource( BaseInputSource&& )      = default;

    BaseInputSource& operator= ( BaseInputSource const& ) = default;
    BaseInputSource& operator= ( BaseInputSource&& )      = default;

    virtual ~BaseInputSource()
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

    /**
     * @brief Get a name of the input source. Mainly interesting for user output.
     */
    std::string source_name() const
    {
        // Non-virtual interface.
        return source_name_();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    virtual size_t read_( char* buffer, size_t size ) = 0;

    virtual std::string source_name_() const = 0;

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
 *
 * That implies that the string shall not be modified while this input source is used, thus,
 * only const-members of the string can be called.
 */
class StringInputSource : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source from a char array.
     */
    StringInputSource( char const* str, size_t size )
        : in_str_(    str  )
        , cursor_(    str  )
        , in_size_(   size )
        , rest_size_( size )
    {}

    /**
    * @brief Construct the input source from a `std::string`.
    */
    StringInputSource( std::string const& str )
        : in_str_(    str.c_str() )
        , cursor_(    str.c_str() )
        , in_size_(   str.size()  )
        , rest_size_( str.size()  )
    {}

    StringInputSource( StringInputSource const& ) = default;
    StringInputSource( StringInputSource&& )      = default;

    StringInputSource& operator= ( StringInputSource const& ) = default;
    StringInputSource& operator= ( StringInputSource&& )      = default;

    ~StringInputSource()
    {}

    // -------------------------------------------------------------
    //     Special Members
    // -------------------------------------------------------------

    /**
     * @brief Rewind the source to its start, so that it can be re-read.
     */
    void rewind()
    {
        cursor_    = in_str_;
        rest_size_ = in_size_;
    }

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the read function.
     */
    size_t read_( char* buffer, size_t size ) override
    {
        // Don't overshoot.
        if( size > rest_size_ ) {
            size = rest_size_;
        }

        // Read.
        std::memcpy( buffer, cursor_, size );
        cursor_     += size;
        rest_size_ -= size;
        return size;
    }

    /**
     * @brief Override of the source name funtion. Returns "input string".
     */
    std::string source_name_() const override
    {
        return "input string";
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    // Original and current string position pointer.
    char const* in_str_;
    char const* cursor_;

    // Original and current (remaining) string size.
    size_t      in_size_;
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
class StreamInputSource : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source from an `std::istream`.
     */
    explicit StreamInputSource( std::istream& in )
        : in_( in )
    {}

    StreamInputSource( StreamInputSource const& ) = default;
    StreamInputSource( StreamInputSource&& )      = default;

    StreamInputSource& operator= ( StreamInputSource const& ) = default;
    StreamInputSource& operator= ( StreamInputSource&& )      = default;

    ~StreamInputSource()
    {}

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the read function.
     */
    size_t read_( char* buffer, size_t size ) override
    {
        in_.read( buffer, size );
        return in_.gcount();
    }

    /**
     * @brief Override of the source name funtion. Returns "input stream".
     */
    std::string source_name_() const override
    {
        return "input stream";
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

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
class FileInputSource : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source from a file with the given file name.
     */
    explicit FileInputSource( std::string const& file_name )
        : file_name_( file_name )
    {
        if( ! file_exists( file_name ) ) {
            throw std::runtime_error( "File does not exists: " + file_name );
        }

        file_ = std::fopen( file_name.c_str(), "rb" );

        if( file_ == nullptr ) {
            // TODO use errno here.
            throw std::runtime_error( "Cannot open file: " + file_name );
        }

        // We do our own buffering.
        std::setvbuf( file_, 0, _IONBF, 0 );
    }

    /**
     * @brief Construct the input source from a `FILE` pointer. The `file_name` is used for the
     * source_name() function only.
     */
    explicit FileInputSource( std::string const& file_name, FILE* file )
        : file_( file )
        , file_name_( file_name )
    {
        // We do our own buffering.
        std::setvbuf( file_, 0, _IONBF, 0 );
    }

    FileInputSource( FileInputSource const& ) = default;
    FileInputSource( FileInputSource&& )      = default;

    FileInputSource& operator= ( FileInputSource const& ) = default;
    FileInputSource& operator= ( FileInputSource&& )      = default;

    ~FileInputSource()
    {
        std::fclose( file_ );
    }

    // -------------------------------------------------------------
    //     Special Members
    // -------------------------------------------------------------

    /**
     * @brief Rewind the source to its start, so that it can be re-read.
     */
    void rewind()
    {
        std::rewind( file_ );
    }

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the read function.
     */
    size_t read_( char* buffer, size_t size ) override
    {
        return std::fread( buffer, 1, size, file_ );
    }

    /**
     * @brief Override of the source name funtion. Returns "input file <file_name>".
     */
    std::string source_name_() const override
    {
        return "input file " + file_name_;
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    FILE*       file_;
    std::string file_name_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
