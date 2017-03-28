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

#include "genesis/sequence/printers/bitmap.hpp"

#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/tools/color.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Print
// =================================================================================================

void PrinterBitmap::to_stream( SequenceSet const& set, std::ostream& outstream ) const
{
    auto image = make_image_( set );
    auto writer = utils::BmpWriter();
    writer.to_stream( image, outstream );
}

void PrinterBitmap::to_file( SequenceSet const& set, std::string const& filename ) const
{
    auto image = make_image_( set );
    auto writer = utils::BmpWriter();
    writer.to_file( image, filename );
}

// =================================================================================================
//     Internal Helper Functions
// =================================================================================================

utils::Matrix<utils::Color> PrinterBitmap::make_image_( SequenceSet const& set ) const
{
    // Create a black image of the correct size.
    size_t const max_line = longest_sequence_length( set );
    auto image = utils::Matrix<utils::Color>(
        set.size() * pixel_height_,
        max_line   * pixel_width_,
        utils::Color( 0, 0 , 0 )
    );

    // Iterate the sequences in the set.
    for( size_t r = 0; r < set.size(); ++r ) {
        auto const& seq = set[r];

        // Iterate the chars of the sequence for this row.
        for( size_t c = 0; c < seq.length(); ++c ) {

            // Find the color for the current char, or use black if no color available.
            auto pixel = utils::Color( 0, 0, 0 );
            auto color_it = color_map_.find( seq[c] );
            if( color_it != color_map_.end() ) {
                pixel = color_it->second;
            }

            // Repeat the line as many times as needed.
            for( size_t ph = 0; ph < pixel_height_; ++ph ) {

                // Set as many pixel in width as needed.
                for( size_t pw = 0; pw < pixel_width_; ++pw ) {
                    image( r * pixel_height_ + ph, c * pixel_width_ + pw ) = pixel;
                }
            }
        }
    }

    return image;
}

// =================================================================================================
//     Properties
// =================================================================================================

PrinterBitmap& PrinterBitmap::color_map( std::map<char, utils::Color> const& value )
{
    color_map_ = value;
    return *this;
}

std::map<char, utils::Color> const& PrinterBitmap::color_map() const
{
    return color_map_;
}

PrinterBitmap& PrinterBitmap::pixel_height_per_char( size_t value )
{
    if( value == 0 ) {
        throw std::invalid_argument( "Pixel height has to be 1 or more." );
    }
    pixel_height_ = value;
    return *this;
}

size_t PrinterBitmap::pixel_height_per_char() const
{
    return pixel_height_;
}

PrinterBitmap& PrinterBitmap::pixel_width_per_char( size_t value )
{
    if( value == 0 ) {
        throw std::invalid_argument( "Pixel width has to be 1 or more." );
    }
    pixel_width_ = value;
    return *this;
}

size_t PrinterBitmap::pixel_width_per_char() const
{
    return pixel_width_;
}

} // namespace sequence
} // namespace genesis
