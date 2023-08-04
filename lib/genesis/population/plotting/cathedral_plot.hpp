#ifndef GENESIS_POPULATION_PLOTTING_CATHEDRAL_PLOT_H_
#define GENESIS_POPULATION_PLOTTING_CATHEDRAL_PLOT_H_

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

#include "genesis/utils/containers/matrix.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions
// =================================================================================================

enum class CathedralWindowWidthMethod
{
    kExponential,
    kGeometric,
    kLinear
};

double cathedral_window_width(
    size_t chromosome_length, size_t width, size_t height, size_t row,
    CathedralWindowWidthMethod window_width_method = CathedralWindowWidthMethod::kExponential
);

// =================================================================================================
//     Compute Matrix Functions
// =================================================================================================

template<class CathedralData, class CathedralAccumulator>
genesis::utils::Matrix<double> compute_cathedral_matrix(
    CathedralData const& data,
    CathedralAccumulator& accu,
    size_t width, size_t height,
    CathedralWindowWidthMethod window_width_method = CathedralWindowWidthMethod::kExponential
) {
    auto result = genesis::utils::Matrix<double>( height, width );

    // How far (in genome coordinates) do we advance between windows?
    double const chr_len  = static_cast<double>( data.chromosome_length );
    auto const advance = chr_len / static_cast<double>( width );

    // Compute each cell of the result.
    for( size_t row = 0; row < height; ++row ) {

        // How wide (in genome coordinates) is each window in the current row?
        auto const window_width = cathedral_window_width(
            data.chromosome_length, width, height, row, window_width_method
        );

        // Compute each pixel in the row, moving along the windows using the cur_gen_pos
        // (in genome coordinates) to indicate where in the data positions we are.
        // As we center the windows around their pixel positions, we start at half the width.
        double cur_gen_pos = - window_width / 2.0;
        for( size_t col = 0; col < width; ++col ) {
            assert( cur_gen_pos + window_width >= 0.0 );

            // Find the genome positions that correspond to the boundaries of the current window,
            // limited to their possible ranges, and making sure to include the last one.
            auto l_gen_pos = static_cast<size_t>( std::max( cur_gen_pos, 0.0 ));
            auto r_gen_pos = static_cast<size_t>( std::min( cur_gen_pos + window_width, chr_len ));
            if( col == width - 1 ) {
                r_gen_pos = data.chromosome_length;
            }
            cur_gen_pos += advance;

            // Find the first entry that is within those genome position boundaries.
            auto const first_entry_it = std::lower_bound(
                data.entries.begin(), data.entries.end(), l_gen_pos,
                []( typename CathedralData::Entry const& entry, size_t position ){
                    return entry.pos < position;
                }
            );
            size_t entry_idx = std::distance( data.entries.begin(), first_entry_it );
            assert( entry_idx <= data.entries.size() );
            assert( entry_idx == data.entries.size() || data.entries[ entry_idx ].pos >= l_gen_pos );
            assert( entry_idx == 0 || data.entries[ entry_idx - 1 ].pos < l_gen_pos );

            // Accumulate the partial values across the positions in our window.
            accu.reset();
            while( entry_idx < data.entries.size() && data.entries[entry_idx].pos <= r_gen_pos ) {
                accu.accumulate( data.entries[entry_idx] );
                ++entry_idx;
            }
            assert( entry_idx == data.entries.size() || data.entries[entry_idx].pos > r_gen_pos );

            // Now we have processed everything for this pixel, and can store the result.
            result( row, col ) = accu.get_result();
        }
    }
    return result;
}

} // namespace population
} // namespace genesis

#endif // include guard
