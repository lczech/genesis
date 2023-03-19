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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/formats/simple_pileup_input_iterator.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/window/base_window_iterator.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/sliding_interval_window_iterator.hpp"
#include "genesis/population/window/window_view_iterator.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/lambda_iterator.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( WindowIterator, WindowViewIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make a Lambda Iterator over the data stream.
    auto data_gen = make_variant_input_iterator_from_pileup_file( infile );
    // data_gen.block_size( 1024 * 1024 );
    data_gen.block_size(0);
    auto pileup_begin = data_gen.begin();
    auto pileup_end   = data_gen.end();

    // Create a window iterator based on the lambda iterator.
    auto win_it = make_window_view_iterator(
        make_default_sliding_interval_window_iterator(
            pileup_begin, pileup_end, 10000
        ).emit_leading_empty_windows( false )
    );

    size_t window_cnt = 0;
    size_t total_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        for( auto const& elem : window ) {
            (void) elem;
            ++total_cnt;
        }
        ++window_cnt;
    }
    EXPECT_EQ( 7, window_cnt );
    EXPECT_EQ( 50000, total_cnt );
}
