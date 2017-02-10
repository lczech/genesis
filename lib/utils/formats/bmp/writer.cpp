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

#include "utils/formats/bmp/writer.hpp"

#include "utils/core/options.hpp"
#include "utils/core/fs.hpp"
#include "utils/math/matrix.hpp"
#include "utils/tools/color.hpp"

#include <cassert>
#include <fstream>
#include <ostream>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Writing
// =================================================================================================

void BmpWriter::to_stream( Matrix<Color> const& image, std::ostream& outstream ) const
{
    // Use some nicer names.
    auto const width  = image.cols();
    auto const height = image.rows();

    // Padding needed to fill rows to a multiple of 4 bytes, and resulting data size in bytes.
    size_t const row_pad   = (4 - (( width * 3 ) % 4 )) % 4;
    size_t const data_size = ( width * 3 + row_pad ) * height;

    // Size checks!
    if(
        height > static_cast<size_t>( std::numeric_limits<uint32_t>::max() ) ||
        width  > static_cast<size_t>( std::numeric_limits<uint32_t>::max() ) ||
        14 + 40 + data_size > static_cast<size_t>( std::numeric_limits<uint32_t>::max() )
    ) {
        throw std::runtime_error(
            "Cannot save Bitmap with more than ~" +
            std::to_string( std::numeric_limits<uint32_t>::max() ) +
            " pixels."
        );
    }

    // Prepare and write bitmap header.
    BitmapFileheader file_header;
    BitmapInfoheader info_header;

    file_header.bfSize     = 14 + 40 + data_size;
    info_header.biWidth    = width;
    info_header.biHeight   = height;
    info_header.biBitCount = 24;

    write_file_header_( file_header, outstream );
    write_info_header_( info_header, outstream );

    // Write data.
    for( size_t yr = 0; yr < height; ++yr ) {
        // Run row-wise backwards (demanded by bmp standard).
        auto y = height - 1 - yr;

        for( size_t x = 0; x < width; ++x ) {

            // Store colour in BGR order (demanded by bmp standard).
            auto const& color = image( y, x );
            outstream.put( color.b() );
            outstream.put( color.g() );
            outstream.put( color.r() );
        }

        // Fill row to multiple of 4 bytes.
        for( size_t x = 0; x < row_pad; ++x ) {
            outstream.put( 0 );
        }
    }
}

void BmpWriter::to_file( Matrix<Color> const& image, std::string const& filename ) const
{
    if( utils::file_exists(filename) ) {
        throw std::runtime_error( "Bmp file '" + filename + "' already exist." );
    }

    std::ofstream fstr( filename, std::ofstream::binary );
    if( !fstr ) {
        throw std::runtime_error( "Cannot write Bmp file '" + filename + "'." );
    }

    to_stream( image, fstr );
}

// =================================================================================================
//     Internal Helpers
// =================================================================================================

void BmpWriter::write_uint16_( uint16_t data, std::ostream& target ) const
{
    auto const bytes = reinterpret_cast< uint8_t const* >( &data );

    if( Options::is_little_endian() ) {
        target.put( bytes[0] );
        target.put( bytes[1] );
    } else {
        target.put( bytes[1] );
        target.put( bytes[0] );
    }
}

void BmpWriter::write_uint32_( uint32_t data, std::ostream& target ) const
{
    auto const bytes = reinterpret_cast< uint8_t const* >( &data );

    if( Options::is_little_endian() ) {
        target.put( bytes[0] );
        target.put( bytes[1] );
        target.put( bytes[2] );
        target.put( bytes[3] );
    } else {
        target.put( bytes[3] );
        target.put( bytes[2] );
        target.put( bytes[1] );
        target.put( bytes[0] );
    }
}

void BmpWriter::write_file_header_( BitmapFileheader const& header, std::ostream& target ) const
{
    write_uint16_( header.bfType,      target );
    write_uint32_( header.bfSize,      target );
    write_uint16_( header.bfReserved1, target );
    write_uint16_( header.bfReserved2, target );
    write_uint32_( header.bfOffBits,   target );
}

void BmpWriter::write_info_header_( BitmapInfoheader const& header, std::ostream& target ) const
{
    write_uint32_( header.biSize,          target );
    write_uint32_( header.biWidth,         target );
    write_uint32_( header.biHeight,        target );
    write_uint16_( header.biPlanes,        target );
    write_uint16_( header.biBitCount,      target );
    write_uint32_( header.biCompression,   target );
    write_uint32_( header.biSizeImage,     target );
    write_uint32_( header.biXPelsPerMeter, target );
    write_uint32_( header.biYPelsPerMeter, target );
    write_uint32_( header.biClrUsed,       target );
    write_uint32_( header.biClrImportant,  target );
}

} // namespace utils
} // namespace genesis
