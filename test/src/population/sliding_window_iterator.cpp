/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/sliding_window_iterator.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/formats/simple_pileup_input_iterator.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( SlidingWindowIterator, Basics )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    auto pileup_begin = SimplePileupInputIterator<>( from_file( infile ));
    auto pileup_end = SimplePileupInputIterator<>();

    auto win_it = make_default_sliding_window_iterator( pileup_begin, pileup_end, 10000 );
    win_it.emit_leading_empty_windows( false );

    size_t window_cnt = 0;
    for( auto const& window : win_it ) {
        // LOG_DBG << window.chromosome() << " : "
        //         << anchor_position( window ) << " "
        //         << window.first_position() << "-" << window.last_position()
        //         << " # " << window.entry_count();

        EXPECT_TRUE( window.first_position() >= 7790001 );
        EXPECT_TRUE( window.first_position() <= 7850001 );
        EXPECT_TRUE( window.last_position() >= 7800000 );
        EXPECT_TRUE( window.last_position() <= 7860000 );

        ++window_cnt;
    }
    EXPECT_EQ( 7, window_cnt );

    // auto window_range = make_sliding_window_range(
    // auto win_it = make_sliding_window_iterator<SimplePileupReader::Record>(
    //     pileup_begin, pileup_end,
    //     []( SimplePileupReader::Record const& record ) -> SimplePileupReader::Record const& {
    //         return record;
    //     },
    //     []( SimplePileupReader::Record const& record ) -> std::string const& {
    //         return record.chromosome;
    //     },
    //     []( SimplePileupReader::Record const& record ){
    //         return record.position;
    //     },
    //     WindowType::kInterval, 10000, 10000
    // );

    // auto win_it = window_range.begin();
    // while( win_it != window_range.end() ) {
    //     LOG_DBG << win_it->chromosome() << " : " << win_it->anchor_position();
    //     ++win_it;
    // }
}
