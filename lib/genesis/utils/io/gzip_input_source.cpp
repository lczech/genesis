/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/io/gzip_input_source.hpp"

// Found zlib
#ifdef GENESIS_ZLIB

#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#    include <fcntl.h>
#    include <io.h>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Gzip Input Source
// =================================================================================================

GzipInputSource::GzipInputSource(
    std::unique_ptr<BaseInputSource> input_source,
    GzipInputSource::Format format
)
    : input_source_( std::move( input_source ))
    , format_name_( translate_format_( format ))
{
    // Allocate inflate state
    z_stream_.zalloc = Z_NULL;
    z_stream_.zfree = Z_NULL;
    z_stream_.opaque = Z_NULL;
    z_stream_.avail_in = 0;
    z_stream_.next_in = Z_NULL;

    // Init zlib
    auto ret = inflateInit2( &z_stream_, static_cast<int>( format ));
    if( ret != Z_OK ) {
        report_zlib_error_( ret );
    }
}

GzipInputSource::~GzipInputSource()
{
    inflateEnd( &z_stream_ );
}

size_t GzipInputSource::read_( char* buffer, size_t size )
{
    // How much have we already done, how much do we need to do, and where to put it.
    // (The latter two are aliases for consistency...)
    size_t       out_pos = 0;
    size_t const out_end = size;
    char*        out_buf = buffer;

    // Inflate data until the output buffer is full with the desired number of bytes.
    while( out_pos < out_end ) {

        // If the input buffer is already used up (or not yet read, in the beginning),
        // read from the source.
        if( in_pos_ >= in_end_ ) {
            in_pos_ = 0;
            in_end_ = input_source_->read( in_buf_, BlockLength );
        }

        // Read starting from the current input position, as much as there still is data.
        z_stream_.avail_in = in_end_ - in_pos_;
        z_stream_.next_in = reinterpret_cast<Bytef*>( in_buf_ ) + in_pos_;

        // Write to the current output position, as much as there still is space.
        z_stream_.avail_out = out_end - out_pos;
        z_stream_.next_out = reinterpret_cast<Bytef*>( out_buf ) + out_pos;

        // Run.
        auto ret = inflate( &z_stream_, Z_NO_FLUSH );

        // Error checks.
        assert( ret != Z_STREAM_ERROR );
        switch( ret ) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                report_zlib_error_( ret );
        }

        // Update current positions.
        in_pos_ = in_end_ - z_stream_.avail_in;
        out_pos = out_end - z_stream_.avail_out;

        // Check if we reached the end of the input deflated stream
        if( ret == Z_STREAM_END ) {

            // We only process whole gz files. So, if we reach the end of the deflate stream,
            // we must also be at the end of the input.
            assert( in_pos_ == in_end_ );

            break;
        }
    }

    // Either we filled up the whole buffer (and read size many bytes),
    // or we reached the end of the input.
    assert( out_pos == out_end || in_pos_ == in_end_ );

    // Return how many bytes we have out into the output buffer.
    return out_pos;
}

void GzipInputSource::report_zlib_error_( int error_code ) const
{
    // Nothing happens if there is no error.
    if( error_code == Z_OK ) {
        return;
    }

    switch( error_code ) {
        case Z_ERRNO:
            throw std::runtime_error( "zlib: Error while reading zlib/gzip input." );
        case Z_STREAM_ERROR:
            throw std::runtime_error( "zlib: Invalid compression level." );
        case Z_DATA_ERROR:
            throw std::runtime_error( "zlib: Invalid or incomplete deflate data." );
        case Z_MEM_ERROR:
            throw std::runtime_error( "zlib: Out of memory." );
        case Z_VERSION_ERROR:
            throw std::runtime_error( "zlib: Version mismatch!" );
        default:
            throw std::runtime_error( "zlib: Unknown error." );
    }
}

std::string GzipInputSource::translate_format_( GzipInputSource::Format format ) const
{
    switch( format ) {
        case Format::kGzip:
            return "gzip";
        case Format::kZlib:
            return "zlib";
        case Format::kDeflate:
            return "deflate";
        default:
            assert( false );
            return "";
    }
}

} // namespace utils
} // namespace genesis

#endif // found zlib
