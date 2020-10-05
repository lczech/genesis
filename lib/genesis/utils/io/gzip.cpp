/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/gzip.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"

#include <cassert>
#include <fstream>
#include <stdexcept>

#ifdef GENESIS_ZLIB

#    include "zlib.h"

#    if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#       include <fcntl.h>
#       include <io.h>
#   endif

#endif // GENESIS_ZLIB

namespace genesis {
namespace utils {

// ================================================================================================
//     General gzip/zlib Functions
// ================================================================================================

bool is_gzip_compressed_file( std::string const& file_name )
{
    // Check if the file extension hints at gzip.
    auto const ext = file_extension( file_name );
    bool const ext_gz = ( ext == "gz" || ext == "gzip" );

    // Open the file in binary mode.
    std::ifstream infile;
    infile.open( file_name, std::ifstream::in | std::ifstream::binary );
    if( !infile.good() ) {
        return false;
        // throw std::runtime_error( "Cannot read from file '" + file_name + "'." );
    }

    // Get the first two characters. If this fails, the file is too short, so it is not a gzip file.
    unsigned char buffer[2];
    infile.read( reinterpret_cast<char*>( &buffer ), 2 );
    if( !infile.good() ) {
        return false;
    }
    infile.close();

    // Check if the file starts with the magic number of gz files.
    bool const magic = ( buffer[0] == 0x1f ) && ( buffer[1] == 0x8b );

    // If extension and magic number agree, we have a clear result.
    // Otherwise, issue a warning, and return the magic bit, because this is what we trust more.
    if( ext_gz && magic ) {
        return true;
    } else if( ! ext_gz && ! magic ) {
        return false;
    } else if( ext_gz && ! magic ) {
        LOG_WARN << "File name '" << file_name << "' ends in '.gz', but the file does not seem "
                 << "to contain gzip content.";
    } else if( ! ext_gz && magic ) {
        LOG_WARN << "File name '" << file_name << "' does not end in '.gz', but the file seems "
                 << "to contain gzip content.";
    } else {
        // We have checked every compbination already.
        assert( false );
    }
    return magic;
}

// ================================================================================================
//     Gzip Exception Class
// ================================================================================================

} // namespace utils
namespace except {

#ifdef GENESIS_ZLIB

GzipError::GzipError( std::string const& z_stream_message, int error_code )
    : except::IOError("")
{
    // Need to have this method in the cpp file, so that we do not expose the zlib header
    // to the header file, which would include all its symbols to whichever class uses our headers...

    message_ = "zlib: ";
    switch( error_code )
    {
    case Z_STREAM_ERROR:
        message_ += "Invalid compression level. [Z_STREAM_ERROR: ";
        break;
    case Z_DATA_ERROR:
        message_ += "Invalid or incomplete deflate data. [Z_DATA_ERROR: ";
        break;
    case Z_MEM_ERROR:
        message_ += "Out of memory. [Z_MEM_ERROR: ";
        break;
    case Z_VERSION_ERROR:
        message_ += "Version mismatch! [Z_VERSION_ERROR: ";
        break;
    case Z_BUF_ERROR:
        message_ += "Buffer error. [Z_BUF_ERROR: ";
        break;
    case Z_ERRNO:
        message_ += "Error while reading zlib/gzip input. [Z_ERRNO: ";
        break;
    default:
        message_ += "Unknown error. [" + std::to_string(error_code) + ": ";
        break;
    }
    message_ += z_stream_message + "]";
}

#else // GENESIS_ZLIB

GzipError::GzipError( std::string const&, int )
    : except::IOError("")
{
    message_ = "zlib: Genesis was not compiled with zlib support.";
}

#endif // GENESIS_ZLIB

} // namespace except
} // namespace genesis
