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

#include "genesis/utils/formats/bmp/writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/tools/color.hpp"

#include <cassert>
#include <fstream>
#include <ostream>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Writing Matrix of Color
// =================================================================================================

void BmpWriter::to_stream( Matrix<Color> const& image, std::ostream& outstream ) const
{
    // Use some nicer names.
    auto const width  = image.cols();
    auto const height = image.rows();

    // Padding needed to fill rows to a multiple of 4 bytes.
    size_t const row_pad = (4 - (( width * 3 ) % 4 )) % 4;

    // Length of one line of the image, in bytes.
    size_t const line_len = width * 3 + row_pad;
    assert(( row_pad < 4 ) && ( line_len % 4 == 0 ));

    // Total resulting data size in bytes
    size_t const data_size = line_len * height;

    // Size checks! Bitmaps can't be larger than ~4GB
    if(
        height > static_cast<size_t>( std::numeric_limits<uint32_t>::max() ) ||
        width  > static_cast<size_t>( std::numeric_limits<uint32_t>::max() ) ||
        14 + 40 + data_size > static_cast<size_t>( std::numeric_limits<uint32_t>::max() )
    ) {
        throw std::runtime_error(
            "Cannot save Bitmap larger than " +
            std::to_string( std::numeric_limits<uint32_t>::max() ) +
            " bytes."
        );
    }

    // File header.
    BitmapFileheader file_header;
    file_header.bfSize     = 14 + 40 + data_size;
    file_header.bfOffBits  = 14 + 40;

    // Info header.
    BitmapInfoheader info_header;
    info_header.biWidth    = width;
    info_header.biHeight   = height;
    info_header.biBitCount = 24;

    // Write headers.
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
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( image, ofs );
}

// =================================================================================================
//     Writing Matrix of unsigned char
// =================================================================================================

void BmpWriter::to_stream( Matrix<unsigned char> const& image, std::ostream& outstream ) const
{
    // Build a simple grayscale palette.
    auto palette = std::vector<Color>( 256 );
    for( size_t i = 0; i < 256; ++i ) {
        palette[i].r(i);
        palette[i].g(i);
        palette[i].b(i);
    }
    to_stream( image, palette, outstream );
}

void BmpWriter::to_file( Matrix<unsigned char> const& image, std::string const& filename ) const
{
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( image, ofs );
}

// =================================================================================================
//     Writing Matrix of unsigned char with Color palette
// =================================================================================================

void BmpWriter::to_stream(
    Matrix<unsigned char> const& image, std::vector<Color> palette, std::ostream& outstream
) const {
    // Use some nicer names.
    auto const width  = image.cols();
    auto const height = image.rows();

    // Padding needed to fill rows to a multiple of 4 bytes.
    size_t const row_pad = (4 - ( width % 4 )) % 4;

    // Length of one line of the image, in bytes.
    size_t const line_len = width + row_pad;
    assert(( row_pad < 4 ) && ( line_len % 4 == 0 ));

    // Total resulting data size in bytes
    size_t const data_size = line_len * height;

    // Size checks! Bitmaps can't be larger than ~4GB
    if(
        height > static_cast<size_t>( std::numeric_limits<uint32_t>::max() ) ||
        width  > static_cast<size_t>( std::numeric_limits<uint32_t>::max() ) ||
        14 + 40 + 256 * 4 + data_size > static_cast<size_t>( std::numeric_limits<uint32_t>::max() )
    ) {
        throw std::runtime_error(
            "Cannot save Bitmap larger than " +
            std::to_string( std::numeric_limits<uint32_t>::max() ) +
            " bytes."
        );
    }

    // File header.
    BitmapFileheader file_header;
    file_header.bfSize        = 14 + 40 + 256 * 4 + data_size;
    file_header.bfOffBits     = 14 + 40 + 256 * 4;

    // Info and info header.
    BitmapInfo info;
    info.bmiHeader.biWidth    = width;
    info.bmiHeader.biHeight   = height;
    info.bmiHeader.biBitCount = 8;

    // Palette size check.
    if( palette.size() != 256 ) {
        throw std::invalid_argument( "Bitmap color palette does not have 256 entries." );
    }

    // Color palette.
    info.bmiColors = std::vector<RgbQuad>( 256 );
    for( size_t i = 0; i < 256; ++i ) {
        info.bmiColors[i].rgbBlue  = palette[i].b();
        info.bmiColors[i].rgbGreen = palette[i].g();
        info.bmiColors[i].rgbRed   = palette[i].r();
    }

    // Write headers.
    write_file_header_( file_header, outstream );
    write_info_( info, outstream );

    // Write data.
    for( size_t yr = 0; yr < height; ++yr ) {
        // Run row-wise backwards (demanded by bmp standard).
        auto y = height - 1 - yr;

        for( size_t x = 0; x < width; ++x ) {
            outstream.put( image( y, x ));
        }

        // Fill row to multiple of 4 bytes.
        for( size_t x = 0; x < row_pad; ++x ) {
            outstream.put( 0 );
        }
    }
}

void BmpWriter::to_file(
    Matrix<unsigned char> const& image, std::vector<Color> palette, std::string const& filename
) const {
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( image, palette, ofs );
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

void BmpWriter::write_info_( BitmapInfo const& info, std::ostream& target ) const
{
    write_info_header_( info.bmiHeader, target );
    for( auto const& c : info.bmiColors ) {
        target.put( c.rgbBlue );
        target.put( c.rgbGreen );
        target.put( c.rgbRed );
        target.put( c.rgbReserved );
    }
}

} // namespace utils
} // namespace genesis
