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
#include <string>
#include <utility>
#include <vector>

// =================================================================================================
//     Forward declacations
// =================================================================================================

namespace genesis {
namespace utils {

    class JsonDocument;

} // namespace utils
} // namespace genesis

namespace genesis {
namespace population {

// =================================================================================================
//     Cathedral Plot Parameters
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
 * @brief Plot parameters to make a cathedral plot.
 *
 * Meant for the user-provided parameters for making a cathedral plot, such as the image dimensions.
 *
 * @see compute_cathedral_matrix()
 */
struct CathedralPlotParameters
{
    // Plot parameters
    size_t width = 0;
    size_t height = 0;
    CathedralWindowWidthMethod window_width_method = CathedralWindowWidthMethod::kExponential;
};

/**
 * @brief Collection of the data used for making for a cathedral plot.
 *
 * Base class that contains the data-derived parameters, such as chromsome name.
 * In each of the steps during the creating of a cathedral plot, it receives more data,
 * resulting from what the step did.
 *
 * Meant to be derived from to add more parameters for specific types of cathedral plots,
 * as well as the data needed by compute_cathedral_matrix() and its accumulator to compute the
 * data matrix. In particular, the derived class shall add a `std::vector<Entry> entries` that
 * contains the per-position entries that are the data to compute the per-window (per-pixel) values.
 *
 * @see compute_cathedral_matrix()
 */
struct CathedralPlotRecord
{
    // Virtual destructor, just in case. We are problably not going to delete the derived class
    // through a pointer to base in our use case, but one never knows...
    virtual ~CathedralPlotRecord() = default;

    // Data-derived parameters from the initial input.
    std::string title;
    std::string chromosome_name;
    size_t chromosome_length = 0;

    // User-provided parameters, added here to keep track of the record.
    CathedralPlotParameters parameters;

    // Data matrix containing per-pixel values.
    genesis::utils::Matrix<double> value_matrix;
    std::vector<double> window_widths;
};

/**
 * @brief Compute the window width for a row in a cathedral plot.
 *
 * This uses the chromosome length and the intended plot dimensions to compute window widths
 * where the first row of the image has a width corresponding to the whole image width,
 * the last row has a window width corresponding to a single pixel, and the rows in between
 * are interpolated using one of the CathedralWindowWidthMethod methods.
 *
 * @see compute_cathedral_matrix()
 */
double cathedral_window_width(
    CathedralPlotRecord const& record, size_t row
);

/**
 * @brief Helper function to return a textual representation of the @p method
 */
std::string cathedral_window_width_method_to_string( CathedralWindowWidthMethod method );

// =================================================================================================
//     Compute Matrix Functions
// =================================================================================================

/**
 * @brief Template function to compute the value matrix for a cathedral plot, given a recored
 * with plot parameters and per-position data to accumulate per window.
 *
 * The function computes the accumulated values across windows for each pixel in a cathedral plot,
 * which can then be visualized as a heat map.
 *
 * The function expects a cathedral plot @p record, containing data needed to compute the values
 * per pixel. It expects @p record to contain an iterable container `std::vector<Entry> entries`
 * whose contained elements have a member `position`, and also contain the data that is needed
 * by the @p accumulator. See FstCathedralPlotRecord for an example.
 *
 * The @p accumulator needs to have functions `accumulate()` and `dissipate()` that each take an
 * element of the @p record entries.
 * These functions are meant to accumulate values, and then un-do this again, which is what we use
 * to speed up the computation here. Also, the @p accumulator needs to have a `aggregate()`
 * function that uses the currently accumulated data to compute the value for a given window.
 * See FstCathedralAccumulator for an example. We take this as an (optional) argument, so
 * that it can be set up with other parameters as needed.
 *
 * @see See compute_fst_cathedral_matrix() for an applied version of this function,
 * and see compute_fst_cathedral_records() for a function to compute @p record for that case.
 */
template<class Record, class Accumulator>
void compute_cathedral_matrix(
    CathedralPlotParameters const& parameters,
    Record& record,
    Accumulator accumulator = Accumulator{}
) {
    // Prepare a result matrix for the values, of the desired dimensions.
    record.value_matrix = genesis::utils::Matrix<double>( parameters.height, parameters.width );
    record.window_widths = std::vector<double>( parameters.height );

    // Also store the parameters in the record, for later reference to have them in one place.
    record.parameters = parameters;

    // How far (in genome coordinates) do we advance between windows?
    double const chr_len  = static_cast<double>( record.chromosome_length );
    auto const advance = chr_len / static_cast<double>( parameters.width );

    // Compute each cell of the result. We experimented with parallelizing this loop across threads,
    // but the computation seems to be memory bound, and even when trying to avoid false sharing
    // (of writing to individual cells of the matrix in each iteration), the result was never faster
    // (and often way slower) than the single threaded code here. So let's keep it simple.
    for( size_t row = 0; row < parameters.height; ++row ) {

        // How wide (in genome coordinates) is each window in the current row?
        auto const window_width = cathedral_window_width( record, row );
        assert( std::isfinite( window_width ) && window_width > 0.0 );
        record.window_widths[row] = window_width;

        // Per row, we have a lot of overlap between the windows, up until the very last few
        // rows where windows tend to overlap less. Using this gives massive speedup,
        // as we only need to add entries once, and then remove them again once,
        // instead of computing their accumulated sums over and over again.
        // We use a deque for the entries that are in the current window,
        // and keep track of the next index in the entry vector that needs to be enqueued.
        std::deque<typename Record::Entry> queue;
        size_t entry_idx = 0;

        // Start a new accumulation of values for the row.
        accumulator.reset();

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
        for( size_t col = 0; col < parameters.width; ++col ) {
            assert( cur_gen_pos + window_width >= 0.0 );
            assert( cur_gen_pos <= static_cast<double>( record.chromosome_length ));

            // Find the genome positions that correspond to the boundaries of the current window,
            // limited to their possible ranges, and making sure to include the last one
            // (can be a bit off due to rounding?! might need to check).
            auto l_gen_pos = static_cast<size_t>( std::max( cur_gen_pos, 0.0 ));
            auto r_gen_pos = static_cast<size_t>( std::min( cur_gen_pos + window_width, chr_len ));
            if( col == parameters.width - 1 ) {
                r_gen_pos = record.chromosome_length;
            }
            cur_gen_pos += advance;

            // Some checks that hold true if this function is called with correct data.
            assert( l_gen_pos <= r_gen_pos );
            assert( r_gen_pos <= record.chromosome_length );

            // Remove entries in the beginning of the queue that are not part of the window any more.
            while( ! queue.empty() && queue.front().position < l_gen_pos ) {
                ++diss_cnt;
                accumulator.dissipate( queue.front() );
                queue.pop_front();
            }

            // Now accumulate entries that need to be added for the current window.
            // In cases where due to rounding the windows do not overlap and leave entries
            // between their boundaries, this here also incluse those entries in the later window.
            // This is good, as that way, every entry is used at least once.
            while(
                entry_idx < record.entries.size() &&
                record.entries[ entry_idx ].position <= r_gen_pos
            ) {

                // Assert that the entries are in order.
                assert( queue.empty() || queue.back().position < record.entries[entry_idx].position );
                assert( record.entries[entry_idx].position <= record.chromosome_length );

                // Accumulate the values and add the entry to the queue.
                ++accu_cnt;
                accumulator.accumulate( record.entries[entry_idx] );
                queue.push_back( record.entries[entry_idx] );

                // Move to the next entry to be enqueued.
                ++entry_idx;
            }
            assert(
                entry_idx == record.entries.size() ||
                record.entries[entry_idx].position > r_gen_pos
            );

            // The queue contains entries that are exactly within the window.
            assert( queue.empty() || queue.front().position >= l_gen_pos );
            assert( queue.empty() || queue.back().position  <= r_gen_pos );

            // Now we have processed everything for this pixel, and can store the result.
            record.value_matrix( row, col ) = accumulator.aggregate();
        }

        // Assert that for one row, we accumulate and dissipate each value exactly once.
        // The dissipated ones are not including the remainder of the queue in the last window,
        // so we need to account for those here.
        assert( record.entries.size() == accu_cnt );
        assert( record.entries.size() == diss_cnt + queue.size() );
    }
}

// =================================================================================================
//     Storage Functions
// =================================================================================================

/**
 * @brief Get a user-readable description of a CathedralPlotParameters as a
 * @link genesis::utils::JsonDocument JsonDocument@endlink.
 *
 * @see cathedral_plot_parameters_to_json_document()
 */
genesis::utils::JsonDocument cathedral_plot_parameters_to_json_document(
    CathedralPlotParameters const& parameters
);

/**
 * @brief Get a user-readable description of the data of a CathedralPlotRecord as a
 * @link genesis::utils::JsonDocument JsonDocument@endlink.
 *
 * This is meant for user output, so that cathedral plots can be generated from a data matrix,
 * without having to recompute the matrix.
 *
 * @see fst_cathedral_plot_record_to_json_document(), cathedral_plot_parameters_to_json_document(),
 * save_cathedral_plot_record_to_files(), load_cathedral_plot_record_from_files()
 */
genesis::utils::JsonDocument cathedral_plot_record_to_json_document(
    CathedralPlotRecord const& record
);

/**
 * @brief Save the record of a cathedral plot in a set of files.
 *
 * To allow for flexibility, the CathedralPlotRecord, or its derived classes such as
 * FstCathedralPlotRecord, are expected to be converted to a Json document first, with
 * cathedral_plot_record_to_json_document() and related functions.
 *
 * We then store that meta-data, as well as the value matrix computed with compute_cathedral_matrix()
 * in two files, which use the given @p base_path, and append extensions `.json` and `.csv`,
 * respectively. The resulting files can be loaded again with
 * load_cathedral_plot_record_from_files().
 */
void save_cathedral_plot_record_to_files(
    genesis::utils::JsonDocument const& record_document,
    genesis::utils::Matrix<double> const& record_value_matrix,
    std::string const& base_path
);

/**
 * @brief Load the record of a cathedral plot from a set of files.
 *
 * See save_cathedral_plot_record_to_files(). This reads a json and a csv file using the
 * @p base_path with the extensions `.json` and `.csv`. For convenience, it is also possible to
 * specify one of the two file paths directly, and the respective other will be inferred.
 */
std::pair<genesis::utils::JsonDocument, genesis::utils::Matrix<double>>
load_cathedral_plot_record_from_files(
    std::string const& base_path
);

} // namespace population
} // namespace genesis

#endif // include guard
