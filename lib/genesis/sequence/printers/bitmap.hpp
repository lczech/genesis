#ifndef GENESIS_SEQUENCE_PRINTERS_BITMAP_H_
#define GENESIS_SEQUENCE_PRINTERS_BITMAP_H_

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
 * @ingroup sequence
 */

#include "genesis/utils/tools/color.hpp"

#include <iosfwd>
#include <map>
#include <string>

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace genesis {
namespace utils {

    template<typename T>
    class Matrix;

} // namespace utils

namespace sequence {

    class Sequence;

    class SequenceSet;
} // namespace sequence
} // namespace genesis

// =================================================================================================
//     Printer Bitmap
// =================================================================================================

namespace genesis {
namespace sequence {

/**
 * @brief Print the sites of a SequenceSet as pixels in a bitmap.
 *
 * By default, no colors are set, thus, the image will be completely black. You have to set the
 * @link color_map( std::map<char, utils::Color> const& ) color_map()@endlink first, for
 * example using the colors defined in nucleic_acid_colors() or amino_acid_colors().
 */
class PrinterBitmap
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    PrinterBitmap()  = default;
    ~PrinterBitmap() = default;

    PrinterBitmap( PrinterBitmap const& ) = default;
    PrinterBitmap( PrinterBitmap&& )      = default;

    PrinterBitmap& operator= ( PrinterBitmap const& ) = default;
    PrinterBitmap& operator= ( PrinterBitmap&& )      = default;

    // -------------------------------------------------------------------------
    //     Print
    // -------------------------------------------------------------------------

    void to_stream( SequenceSet const& set, std::ostream& outstream ) const;
    void to_file( SequenceSet const& set, std::string const& filename ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Set the list of Color%s to use for the Sequence charaters.
     *
     * The provided map sets the colors for each Sequence character.
     * The presettings `nucleic_acid_colors()` and `amino_acid_colors()` for default sequence
     * types can be used as input for this parameter.
     * If the `colors` map does not contain a key for one of the chars in the sequence, the char is
     * printed in black.
     */
    PrinterBitmap& color_map( std::map<char, utils::Color> const& value );

    /**
     * @brief Get the currently set list of Color%s for each Sequence character.
     *
     * See @link color_map( std::map<char, utils::Color> const& ) the setter@endlink for details.
     */
    std::map<char, utils::Color> const& color_map() const;

    PrinterBitmap& pixel_height_per_char( size_t value );

    size_t pixel_height_per_char() const;

    PrinterBitmap& pixel_width_per_char( size_t value );

    size_t pixel_width_per_char() const;

    // -------------------------------------------------------------------------
    //     Interal Helper Functions
    // -------------------------------------------------------------------------

private:

    utils::Matrix<utils::Color> make_image_( SequenceSet const& set ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::map<char, utils::Color> color_map_;

    size_t pixel_height_ = 1;
    size_t pixel_width_  = 1;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
