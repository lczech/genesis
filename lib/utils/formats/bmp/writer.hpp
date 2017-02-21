#ifndef GENESIS_UTILS_FORMATS_BMP_WRITER_H_
#define GENESIS_UTILS_FORMATS_BMP_WRITER_H_

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

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Color;

template<typename T>
class Matrix;

// =================================================================================================
//     Bmp Writer
// =================================================================================================

/**
 * @brief Write Bitmap image files.
 */
class BmpWriter
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    /**
     * @brief Bitmap File Header.
     *
     * This POD structure holds information about a Bitmap file.
     * Usually, it is followed by a BitmapInfoheader.
     *
     * The data is stored in the following order:
     *
     * start | size | name        | stdvalue | purpose
     * -----:|-----:|:------------|---------:|:-----------------------------------------------------
     *     0 |    2 | bfType      |    19778 | Must be 'BM' to declare that this is a .bmp-file.
     *     2 |    4 | bfSize      |       ?? | Size of the file in bytes, including headers, pixel data and padding.
     *     6 |    2 | bfReserved1 |        0 | Must be set to zero.
     *     8 |    2 | bfReserved2 |        0 | Must be set to zero.
     *    10 |    4 | bfOffBits   |       54 | Offset from the start of the file to the pixel data.
     *
     * @see BmpWriter
     * @see BitmapInfoheader
     */
    struct BitmapFileheader
    {
        uint16_t bfType      = 0x4D42; // B=42 M=4D
        uint32_t bfSize;
        uint16_t bfReserved1 = 0;
        uint16_t bfReserved2 = 0;
        uint32_t bfOffBits   = 14 + 40;
    };

    /**
     * @brief Bitmap Info Header.
     *
     * This POD structure holds information about a Bitmap image. In the Bitmap file format, it
     * immediately follows the BitmapFileheader, thus the start of the data is at byte 14.
     *
     * Furthermore, for Bitmaps that do not use full 24 (or more) bit colors, a color palette is
     * necessary. The BitmapInfo structure is able to hold this additional information, see there
     * for details.
     *
     * The data in this struct is stored in the following order:
     *
     * start | size | name            | stdvalue | purpose
     * -----:|-----:|:----------------|---------:|:--------------------------------------------------
     *    14 |    4 | biSize          |       40 | Size of the BitmapInfoheader structure, in bytes.
     *    18 |    4 | biWidth         |       ?? | Width of the image, in pixels.
     *    22 |    4 | biHeight        |       ?? | Height of the image, in pixels.
     *    26 |    2 | biPlanes        |        1 | Number of planes of the target device.
     *    28 |    2 | biBitCount      |        8 | Number of bits per pixel.
     *    30 |    4 | biCompression   |        0 | Compression type, usually zero (no compression).
     *    34 |    4 | biSizeImage     |        0 | Size of the image data, in bytes. Without compression, set this member to zero.
     *    38 |    4 | biXPelsPerMeter |        0 | Horizontal pixels per meter on the designated target device, usually set to zero.
     *    42 |    4 | biYPelsPerMeter |        0 | Vertical pixels per meter on the designated targer device, usually set to zero.
     *    46 |    4 | biClrUsed       |        0 | Number of colors used in the bitmap, if zero the number is calculated using biBitCount.
     *    50 |    4 | biClrImportant  |        0 | Number of color that are 'important'. If zero, all colors are important.
     *
     * @see BmpWriter
     * @see BitmapFileheader
     * @see BitmapInfo
     */
    struct BitmapInfoheader
    {
        uint32_t biSize          = 40;
        uint32_t biWidth;
        uint32_t biHeight;
        uint16_t biPlanes        = 1;
        uint16_t biBitCount      = 8;
        uint32_t biCompression   = 0;
        uint32_t biSizeImage     = 0;
        uint32_t biXPelsPerMeter = 0;
        uint32_t biYPelsPerMeter = 0;
        uint32_t biClrUsed       = 0;
        uint32_t biClrImportant  = 0;
    };

    /**
     * @brief Palette entry that describes an RGB Color.
     *
     * This POD struct stores an RGB Color plus padding in the byte order needed for Bitmap files.
     *
     * @see BmpWriter
     * @see BitmapInfo
     */
    struct RgbQuad
    {
        unsigned char rgbBlue;
        unsigned char rgbGreen;
        unsigned char rgbRed;
        unsigned char rgbReserved = 0;
    };

    /**
     * @brief Bitmap info that describes dimensions and color information.
     *
     * This struct is an extension of the BitmapInfoheader struct. It is used to additionally store
     * a palette of colors in form of a `vector` of RgbQuad. This is needed if the Bitmap uses
     * fewer than 24 bits per pixel, that is, if the Bitmap is not a full RGB Color image.
     *
     * @see BmpWriter
     * @see BitmapInfoheader
     * @see RgbQuad
     */
    struct BitmapInfo {
        BitmapInfoheader     bmiHeader;
        std::vector<RgbQuad> bmiColors;
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    /// @brief Default constructor.
    BmpWriter()  = default;

    /// @brief Default destructor.
    ~BmpWriter() = default;

    /// @brief Default copy constructor.
    BmpWriter( BmpWriter const& ) = default;

    /// @brief Default move constructor.
    BmpWriter( BmpWriter&& )      = default;

    /// @brief Default copy assignemnt.
    BmpWriter& operator= ( BmpWriter const& ) = default;

    /// @brief Default move assignemnt.
    BmpWriter& operator= ( BmpWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    /**
     * @brief Write a full 24bit RGB Color image to a stream.
     */
    void to_stream( Matrix<Color> const& image, std::ostream& outstream ) const;

    /**
     * @brief Write a full 24bit RGB Color image to a file.
     */
    void to_file( Matrix<Color> const& image, std::string const& filename ) const;

    /**
     * @brief Write an 8 bit grayscale image to a stream.
     *
     * The entries of the given Matrix are simply translated into a grayscale image, where
     * `0` gives black and `255` gives white pixels.
     */
    void to_stream( Matrix<unsigned char> const& image, std::ostream& outstream ) const;

    /**
     * @brief Write an 8 bit grayscale image to a file.
     *
     * The entries of the given Matrix are simply translated into a grayscale image, where
     * `0` gives black and `255` gives white pixels.
     */
    void to_file( Matrix<unsigned char> const& image, std::string const& filename ) const;

    /**
     * @brief Write an 8 bit image with a Color palette to a stream.
     *
     * The given `palette` needs to contain exaclty 256 Color entries. The values of the given
     * `image` Matrix are then mapped to the palette entries, e.g., the value at `image( 2, 3 ) == 5`
     * maps to the Color `palette[ 5 ]`.
     */
    void to_stream(
        Matrix<unsigned char> const& image, std::vector<Color> palette, std::ostream& outstream
    ) const;

    /**
     * @brief Write an 8 bit image with a Color palette to a file.
     *
     * The given `palette` needs to contain exaclty 256 Color entries. The values of the given
     * `image` Matrix are then mapped to the palette entries, e.g., the value at `image( 2, 3 ) == 5`
     * maps to the Color `palette[ 5 ]`.
     */
    void to_file(
        Matrix<unsigned char> const& image, std::vector<Color> palette, std::string const& filename
    ) const;

    // ---------------------------------------------------------------------
    //     Internal Helpers
    // ---------------------------------------------------------------------

private:

    void write_uint16_( uint16_t data, std::ostream& target ) const;
    void write_uint32_( uint32_t data, std::ostream& target ) const;

    void write_file_header_( BitmapFileheader const& header, std::ostream& target ) const;
    void write_info_header_( BitmapInfoheader const& header, std::ostream& target ) const;

    void write_info_( BitmapInfo const& info, std::ostream& target ) const;

};

} // namespace utils
} // namespace genesis

#endif // include guard
