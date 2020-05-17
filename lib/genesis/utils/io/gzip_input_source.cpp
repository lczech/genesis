/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/gzip.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>
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

// =================================================================================================
//     Zlib Data
// =================================================================================================

#ifdef GENESIS_ZLIB

/**
 * @brief Version of the Zlib Data struct if zlib is used.
 */
struct GzipInputSource::ZlibData
{
    // Zlib object
    z_stream z_stream_;

    // Input buffer, our current position in the buffer, and the past-the-end position
    // (can be shorter than the buffer length, if there is not enough input).
    char     in_buf_[ BlockLength ];
    size_t   in_pos_ = 0;
    size_t   in_end_ = 0;
};

#else // GENESIS_ZLIB

/**
 * @brief Version of the Zlib Data struct if zlib is not available.
 */
struct GzipInputSource::ZlibData
{
    // Empty on purpose.
};

#endif // GENESIS_ZLIB

// =================================================================================================
//     Gzip Input Source: Functions with zlib
// =================================================================================================

#ifdef GENESIS_ZLIB

GzipInputSource::GzipInputSource(
    std::shared_ptr<BaseInputSource> input_source,
    GzipInputSource::Format format
)
    : input_source_( input_source )
    , format_name_( translate_format_( format ))
    , zlib_data_(
        new ZlibData(),
        []( ZlibData *impl ) { delete impl; }
    )
{
    // Allocate zlib inflate state
    auto& z_stream_ = zlib_data_->z_stream_;
    z_stream_.zalloc = Z_NULL;
    z_stream_.zfree = Z_NULL;
    z_stream_.opaque = Z_NULL;
    z_stream_.avail_in = 0;
    z_stream_.next_in = Z_NULL;

    // Init zlib
    auto ret = inflateInit2( &z_stream_, get_format_( format ));
    if( ret != Z_OK ) {
        throw except::GzipError( z_stream_.msg, ret );
    }
}

GzipInputSource::~GzipInputSource()
{
    // Call the zlib destructor. This is called before the inner class is destroyed,
    // so this is in correct order.
    inflateEnd( &zlib_data_->z_stream_ );
}

size_t GzipInputSource::read_( char* buffer, size_t size )
{
    // Shorthands to the data members.
    auto& z_stream_ = zlib_data_->z_stream_;
    auto& in_buf_   = zlib_data_->in_buf_;
    auto& in_pos_   = zlib_data_->in_pos_;
    auto& in_end_   = zlib_data_->in_end_;

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
        assert( in_end_ >= in_pos_ );
        assert( out_end >= out_pos );

        // Read starting from the current input position, as much as there still is data.
        // We use char data, but zlib expects unsigned char. So here, we cast in one direction,
        // and in the output buffer, we again cast back. This doesn't change the byte content,
        // so this is okay.
        z_stream_.avail_in = static_cast<unsigned int>( in_end_ - in_pos_ );
        z_stream_.next_in = reinterpret_cast<Bytef*>( in_buf_ ) + in_pos_;

        // Write to the current output position, as much as there still is space.
        z_stream_.avail_out = static_cast<unsigned int>( out_end - out_pos );
        z_stream_.next_out = reinterpret_cast<Bytef*>( out_buf ) + out_pos;

        // Run.
        auto ret = inflate( &z_stream_, Z_NO_FLUSH );

        // Error checks.
        assert( ret != Z_STREAM_ERROR );
        if( ret == Z_NEED_DICT ) {
            ret = Z_DATA_ERROR;
        }
        switch( ret ) {
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                throw except::GzipError( z_stream_.msg, ret );
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

std::string GzipInputSource::source_string_() const
{
    // Check if the extension is one that we want to remove.
    auto const bn = file_basename( input_source_->source_string() );
    auto const ex = file_extension( bn );

    // If so, use the full name again to get the complete path, but remove the extension.
    if( ex == "gz" || ex == "gzip" || ex == "zlib" ) {
        return file_filename( input_source_->source_string() );
    }
    return input_source_->source_string();
}

int GzipInputSource::get_format_( GzipInputSource::Format format ) const
{
    // Get the correct format int from the enum.
    // We could use a typed enum, and directly use the enum to store the values,
    // but this would require to include the zlib header in the header of this class,
    // which we want to avoid.
    switch( format ) {
        case Format::kAutomatic:
            return MAX_WBITS | 32;
        case Format::kGzip:
            return MAX_WBITS | 16;
        case Format::kZlib:
            return MAX_WBITS;
        case Format::kDeflate:
            return -MAX_WBITS;
        default:
            assert( false );
            return 0;
    }
}

std::string GzipInputSource::translate_format_( GzipInputSource::Format format ) const
{
    switch( format ) {
        case Format::kAutomatic:
            return "gzip/zlib";
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

// =================================================================================================
//     Gzip Input Source: Functions without zlib
// =================================================================================================

#else // GENESIS_ZLIB

// Here, we define the class members as empty functions, throwing in the constructor.
// This is offered to be able to write code that mentions the class, without having to have zlib.

GzipInputSource::~GzipInputSource()
{
    // Empty on purpose.

    // For some weird reason, Doxygen messes up the documentation if this function comes after the
    // contructor definition below. Probably due to Doxygen being unable to correctly parse that
    // lambda in the initilizer. Anyway, by putting the destructor definition here, the Doxygen
    // problem is solved.
}

GzipInputSource::GzipInputSource(
    std::shared_ptr<BaseInputSource>,
    GzipInputSource::Format
)
    : input_source_()
    , format_name_()
    , zlib_data_( nullptr, []( ZlibData* ){} )
{
    // Just avoid doing anything really.
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

size_t GzipInputSource::read_( char*, size_t )
{
    return 0;
}

std::string GzipInputSource::source_string_() const
{
    return "";
}

int GzipInputSource::get_format_( GzipInputSource::Format ) const
{
    return 0;
}

std::string GzipInputSource::translate_format_( GzipInputSource::Format ) const
{
    return "";
}

#endif // GENESIS_ZLIB

} // namespace utils
} // namespace genesis
