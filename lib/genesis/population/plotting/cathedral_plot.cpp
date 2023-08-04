/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @ingroup population
 */

#include "genesis/population/plotting/cathedral_plot.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions
// =================================================================================================

double cathedral_window_width(
    size_t chromosome_length, size_t width, size_t height, size_t row,
    CathedralWindowWidthMethod method
) {
    if( width < 2 || height < 2 ) {
        throw std::invalid_argument( "cathedral_window_width invalid: width < 2 || height < 2" );
    }
    if( row >= height ) {
        throw std::invalid_argument( "cathedral_window_width invalid: row >= height" );
    }

    // We need the values as doubles. Do this once here, for conciseness.
    double const chr_len  = static_cast<double>( chromosome_length );
    double const row_d    = static_cast<double>( row );
    double const width_d  = static_cast<double>( width );
    double const height_d = static_cast<double>( height );

    double window_width = 0.0;
    switch( method ) {
        case CathedralWindowWidthMethod::kExponential: {
            auto const decay = - std::log( 1.0 / width_d ) / ( height_d - 1.0 );
            window_width = chr_len * std::exp( -decay * row_d);
            break;
        }
        case CathedralWindowWidthMethod::kGeometric: {
            // We use a scaler based on the image dimensions so that the first window
            // is the whole chromosome, and the last is window width = single pixel.
            auto const scaler = width_d / height_d;
            auto const denom = scaler * row_d + 1.0;
            window_width = chr_len / denom;
            break;
        }
        case CathedralWindowWidthMethod::kLinear: {
            // minimum width of windows, where each window corresponds to one pixel of width,
            // and max width, corresponding to whole genome as one window.
            auto const min_win_width = chr_len / width_d;
            auto const max_win_width = chr_len;

            // How far are we in the rows, as a fraction?
            // Use that to interplate linearly between min and max window width.
            auto const row_frac = row_d / height_d;
            window_width = ( max_win_width - min_win_width ) * ( 1.0 - row_frac ) + min_win_width;
            break;
        }
        default: {
            throw std::invalid_argument( "cathedral_window_width requested with invalid method" );
        }
    }
    return window_width;
}

} // namespace population
} // namespace genesis
