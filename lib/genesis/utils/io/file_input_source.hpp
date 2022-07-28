#ifndef GENESIS_UTILS_IO_FILE_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_FILE_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/base_input_source.hpp"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <istream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

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
class FileInputSource final : public BaseInputSource
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
        std::string err_str;
        if( ! file_is_readable( file_name )) {
            throw std::runtime_error(
                "Cannot open input file '" + file_name + "': " + err_str
            );
        }
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
        std::setvbuf( file_, nullptr, _IONBF, 0 );
    }

    FileInputSource( FileInputSource const& ) = default;
    FileInputSource( FileInputSource&& )      = default;

    FileInputSource& operator= ( FileInputSource const& ) = default;
    FileInputSource& operator= ( FileInputSource&& )      = default;

    ~FileInputSource() override
    {
        // It might already be closed,
        // but in case of aborting before the end of the file, it might not.
        if( file_ ) {
            std::fclose( file_ );
        }
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
        // Special case: we already finished reading the file.
        // We need this check as otherwise the file would be reopened again and
        // read again from the start.
        if( finished_ ) {
            return 0;
        }

        // We need to do lazy loading, otherwise, we might end up having too many open files
        // if we are using the from_files() function, which opens all files at once...
        if( file_ == nullptr ) {
            errno = 0;
            file_ = std::fopen( file_name_.c_str(), "rb" );

            if( file_ == nullptr ) {
                throw std::runtime_error(
                    "Cannot open input file '" + file_name_ + "': " +
                    std::string( strerror( errno ))
                );
            }

            // We do our own buffering.
            std::setvbuf( file_, nullptr, _IONBF, 0 );
        }

        // Do the reading.
        errno = 0;
        size_t const ret = std::fread( buffer, 1, size, file_ );
        if( std::ferror( file_ ) ) {
            assert( ! std::feof( file_ ));
            throw std::runtime_error(
                "Cannot read from input file '" + file_name_  + "': " +
                std::string( strerror( errno ))
            );
        }

        // Similarly, we need to close again once we are done, so that the file is not kept
        // open unnecessarily long after reading.
        if( ret < size ) {
            assert(  std::feof( file_ ));
            assert( !std::ferror( file_ ));
            std::fclose( file_ );
            file_ = nullptr;
            finished_ = true;
        }

        return ret;
    }

    /**
     * @brief Override of the source name funtion. Returns "input file (<file_name>)".
     */
    std::string source_name_() const override
    {
        return "input file (" + file_name_ + ")";
    }

    /**
     * @brief Override of the source string funtion. Returns the file name.
     */
    std::string source_string_() const override
    {
        return file_name_;
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    FILE*       file_ = nullptr;
    std::string file_name_;
    bool        finished_ = false;
};

} // namespace utils
} // namespace genesis

#endif // include guard
