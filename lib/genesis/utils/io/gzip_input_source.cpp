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
    // Zlib object. This can be destroyed and created multiple times, if the input data
    // consists of multiple concatenated gzip streams. Hence, we need a flag to note
    // whether the zlib stream is initialized or not.
    // NB: `z_stream` is the name of the struct, as provided by zlib.h, while `zstream` is our
    // variable name for the instance of that struct.
    z_stream zstream;
    bool initialized = false;

    // Input buffer, our current position in the buffer, and the past-the-end position
    // (can be shorter than the buffer length, if there is not enough input).
    // These are persistent, even if we destroyed the zstream above due to having reached
    // the end of an input gzip part (for multiple concatenated gzip streams).
    char     in_buf[ BlockLength ];
    size_t   in_pos = 0;
    size_t   in_end = 0;
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
    , format_( format )
    , format_name_( translate_format_( format ))
    , zlib_data_(
        // Create a new instance, and impute a destructor as well.
        new ZlibData(),
        []( ZlibData *impl ) { delete impl; }
    )
{
    create_zstream_();
}

GzipInputSource::~GzipInputSource()
{
    destroy_zstream_();
}

void GzipInputSource::create_zstream_()
{
    assert( zlib_data_ );
    assert( zlib_data_->initialized == false );
    auto& zstream = zlib_data_->zstream;

    // Init zlib inflate state
    zstream = z_stream();
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.avail_in = 0;
    zstream.next_in = Z_NULL;

    // Init zlib
    auto ret = inflateInit2( &zstream, get_format_( format_ ));
    if( ret != Z_OK ) {
        throw except::GzipError( zstream.msg, ret );
    }

    zlib_data_->initialized = true;
}

void GzipInputSource::destroy_zstream_()
{
    // Call the zlib end function. In case this destroy function is called from the destructor,
    // this is called before the inner zlib_data_ class is destroyed, so this is in correct order.
    // Usually, we however already reached the end of the input stream, and hence already
    // have ended the zlib object before. In that case, nothing to do here.

    assert( zlib_data_ );
    if( zlib_data_->initialized ) {

        inflateEnd( &zlib_data_->zstream );
        zlib_data_->initialized = false;
    }
}

size_t GzipInputSource::read_( char* buffer, size_t size )
{
    // Shorthands to the data members.
    assert( zlib_data_ );
    auto& zstream = zlib_data_->zstream;
    auto& in_buf  = zlib_data_->in_buf;
    auto& in_pos  = zlib_data_->in_pos;
    auto& in_end  = zlib_data_->in_end;

    // How much have we already done, how much do we need to do, and where to put it.
    // (The latter two are aliases for consistency of notation...)
    size_t       out_pos = 0;
    size_t const out_end = size;
    char*        out_buf = buffer;

    // Inflate data until the output buffer is full with the desired number of bytes.
    while( out_pos < out_end ) {

        // If the input buffer is already used up (or not yet read, in the beginning),
        // read from the source.
        if( in_pos >= in_end ) {
            in_pos = 0;
            in_end = input_source_->read( in_buf, BlockLength );
        }
        assert( in_end >= in_pos );
        assert( out_end >= out_pos );

        // Reached end of input
        if( in_pos == in_end ) {
            break;
        }

        // If we reached the end of a (partial) gzip stream in input that consists of concatenated
        // gzip streams, we need to create a zlib instance again.
        assert( zlib_data_ );
        if( ! zlib_data_->initialized ) {
            create_zstream_();
        }

        // Read starting from the current input position, as much as there still is data.
        // We use char data, but zlib expects unsigned char. So here, we cast in one direction,
        // and in the output buffer, we again cast back. This doesn't change the byte content,
        // so this is okay.
        zstream.avail_in = static_cast<unsigned int>( in_end - in_pos );
        zstream.next_in = reinterpret_cast<Bytef*>( in_buf ) + in_pos;

        // Write to the current output position, as much as there still is space.
        zstream.avail_out = static_cast<unsigned int>( out_end - out_pos );
        zstream.next_out = reinterpret_cast<Bytef*>( out_buf ) + out_pos;

        // Run.
        auto ret = inflate( &zstream, Z_NO_FLUSH );

        // Error checks.
        assert( ret != Z_STREAM_ERROR );
        if( ret != Z_OK && ret != Z_STREAM_END ) {
            if( ret == Z_NEED_DICT ) {
                ret = Z_DATA_ERROR;
            }
            throw except::GzipError( zstream.msg, ret );
        }

        // Update current positions.
        in_pos = in_end - zstream.avail_in;
        out_pos = out_end - zstream.avail_out;
        assert( reinterpret_cast<char*>( zstream.next_in )  == in_buf + in_pos );
        assert( reinterpret_cast<char*>( zstream.next_out ) == out_buf + out_pos );

        // Check if we reached the end of the input deflated stream. If so, this either means
        // we have reached the valid end of the input data, or the input consists of multiple
        // concatenated gzip streams. In the first case, we start one mor iteration of the loop,
        // but will find the input empty and trigger the break condition there. In the latter case,
        // destroying the zstream means that we are going to instanciate a new one in the next loop.
        if( ret == Z_STREAM_END ) {
            destroy_zstream_();
        }
    }

    // Either we filled up the whole buffer (and read size many bytes),
    // or we reached the end of the input.
    assert( out_pos == out_end || in_pos == in_end );

    // Return how many bytes we have put into the output buffer.
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

void GzipInputSource::create_zstream_()
{
    // Empty on purpose.
}

void GzipInputSource::destroy_zstream_()
{
    // Empty on purpose.
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
    // Avoid compiler warnings: have to use format_ somewhere!
    (void) format_;
    return 0;
}

std::string GzipInputSource::translate_format_( GzipInputSource::Format ) const
{
    return "";
}

#endif // GENESIS_ZLIB

} // namespace utils
} // namespace genesis
