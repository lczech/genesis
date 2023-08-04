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
#include <deque>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Interpolation algorithm for window sizes across the rows of a cathedral plot.
 *
 * See cathedral_window_width() for details. We highly recommend to use `kExponential`, as this
 * offers the best visualization results where the window widths across rows interpolate
 * exponentially between whole genome and individual pixel of the plot, and hence best show the
 * effects of different orders of magnitude of windows on the computed statistic.
 *
 * In contrast, `kGeometric` decays too fast, where most of the plot has very small window sizes,
 * while `kLinear` does the opposite, and simply shows triangles of large window sizes, omitting
 * most of the fine structure of the statistics for small windows.
 */
enum class CathedralWindowWidthMethod
{
    kExponential,
    kGeometric,
    kLinear
};

/**
 * @brief Compute the window width for a row in a cathedral plot.
 *
 * This uses the chromosome length and the intended plot dimensions to compute window widths
 * where the first row of the image has a width corresponding to the whole image width,
 * the last row has a window width corresponding to a single pixel, and the rows in between
 * are interpolated using one of the CathedralWindowWidthMethod methods, with exponential being
 * the default.
 *
 * @see compute_cathedral_matrix()
 */
double cathedral_window_width(
    size_t chromosome_length, size_t width, size_t height, size_t row,
    CathedralWindowWidthMethod window_width_method = CathedralWindowWidthMethod::kExponential
);

// =================================================================================================
//     Compute Matrix Functions
// =================================================================================================

/**
 * @brief Template function to compute the value matrix for a cathedral plot, given per-position
 * data.
 *
 * The function computes the values for each pixel in a cathedral plot, which can then be visualized
 * as a heat map.
 *
 * It expects @p data to be a type that has a member `size_t chromosome_length`, as well as a member
 * `std::vector<Entry> entries` of some type that itself contains a member `position`.
 * See FstCathedralData for an example.
 *
 * Furthermore, it expects @p accu to be an "accumulator" for data of the `Entry` type above, which
 * needs to have functions `accumulate()` and `dissipate()` that each take an `Entry`. These
 * functions are meant to accumulate values, and then un-do this again, which is what we use to
 * speed up the computation here. Lastly, the @p accu needs to have a `get_result()` function
 * that uses the currently accumulated data to compute the value for a given window.
 * See FstCathedralAccumulator for an example.
 *
 * Lastly, the function expects the indended image dimensions @p width and @p height, as well
 * as the @p window_width_method to use for the cathedral plot.
 *
 * @see See compute_fst_cathedral_matrix() for an applied version of this function,
 * and see compute_fst_cathedral_data() for a function to compute @p data for that case.
 */
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

    // Compute each cell of the result. We experimented with parallelizing this loop across threads,
    // but the computation seems to be memory bound, and even when trying to avoid false sharing
    // (of writing to individual cells of the matrix in each iteration), the result was never faster
    // (and often way slower) than the single threaded code here. So let's keep it simple.
    for( size_t row = 0; row < height; ++row ) {

        // How wide (in genome coordinates) is each window in the current row?
        auto const window_width = cathedral_window_width(
            data.chromosome_length, width, height, row, window_width_method
        );
        assert( std::isfinite( window_width ) && window_width > 0.0 );

        // Per row, we have a lot of overlap between the windows, up until the very last few
        // rows where windows tend to overlap less. Using this gives massive speedup,
        // as we only need to add entries once, and then remove them again once,
        // instead of computing their accumulated sums over and over again.
        // We use a deque for the entries that are in the current window,
        // and keep track of the next index in the entry vector that needs to be enqueued.
        std::deque<typename CathedralData::Entry> queue;
        size_t entry_idx = 0;

        // Start a new accumulation of values for the row.
        accu.reset();

        // Assert that for one row, we accumulate and dissipate each value exactly once.
        size_t accu_cnt = 0;
        size_t diss_cnt = 0;
        (void) accu_cnt;
        (void) diss_cnt;

        // We move along the windows using the cur_gen_pos (in genome coordinates) to indicate
        // where in the data positions we are. As we center the windows around their pixel positions,
        // we start at half the width.
        double cur_gen_pos = - window_width / 2.0;

        // Iterate the pixels of the columns, computing a window for each of them,
        for( size_t col = 0; col < width; ++col ) {
            assert( cur_gen_pos + window_width >= 0.0 );
            assert( cur_gen_pos <= static_cast<double>( data.chromosome_length ));

            // Find the genome positions that correspond to the boundaries of the current window,
            // limited to their possible ranges, and making sure to include the last one
            // (can be a bit off due to rounding?! might need to check).
            auto l_gen_pos = static_cast<size_t>( std::max( cur_gen_pos, 0.0 ));
            auto r_gen_pos = static_cast<size_t>( std::min( cur_gen_pos + window_width, chr_len ));
            if( col == width - 1 ) {
                r_gen_pos = data.chromosome_length;
            }
            cur_gen_pos += advance;

            // Some checks that hold true if this function is called with correct data.
            assert( l_gen_pos <= r_gen_pos );
            assert( r_gen_pos <= data.chromosome_length );

            // Remove entries in the beginning of the queue that are not part of the window any more.
            while( ! queue.empty() && queue.front().position < l_gen_pos ) {
                ++diss_cnt;
                accu.dissipate( queue.front() );
                queue.pop_front();
            }

            // Now accumulate entries that need to be added for the current window.
            // In cases where due to rounding the windows do not overlap and leave entries
            // between their boundaries, this here also incluse those entries in the later window.
            // This is good, as that way, every entry is used at least once.
            while( entry_idx < data.entries.size() && data.entries[entry_idx].position <= r_gen_pos ) {

                // Assert that the entries are in order.
                assert( queue.empty() || queue.back().position < data.entries[entry_idx].position );

                // Accumulate the values and add the entry to the queue.
                ++accu_cnt;
                accu.accumulate( data.entries[entry_idx] );
                queue.push_back( data.entries[entry_idx] );

                // Move to the next entry to be enqueued.
                ++entry_idx;
            }
            assert( entry_idx == data.entries.size() || data.entries[entry_idx].position > r_gen_pos );

            // The queue contains entries that are exactly within the window.
            assert( queue.empty() || queue.front().position >= l_gen_pos );
            assert( queue.empty() || queue.back().position  <= r_gen_pos );

            // Now we have processed everything for this pixel, and can store the result.
            result( row, col ) = accu.get_result();
        }

        // Assert that for one row, we accumulate and dissipate each value exactly once.
        // The dissipated ones are not including the remainder of the queue in the last window,
        // so we need to account for those here.
        assert( data.entries.size() == accu_cnt );
        assert( data.entries.size() == diss_cnt + queue.size() );
    }
    return result;
}

} // namespace population
} // namespace genesis

#endif // include guard
