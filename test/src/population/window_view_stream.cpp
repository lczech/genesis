/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/format/simple_pileup_input_stream.hpp"
#include "genesis/population/format/simple_pileup_reader.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/window/base_window.hpp"
#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/interval_window_stream.hpp"
#include "genesis/population/window/window_view_stream.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( WindowStream, EmptyWindowViewStream )
{
    // Make a Generic Input Stream over the data stream.
    std::vector<Variant> data;
    auto data_gen = make_variant_input_stream_from_vector( data );

    // Create a window iterator based on the Generic Input Stream.
    auto win_it = make_window_view_stream(
        make_default_interval_window_stream(
            data_gen.begin(), data_gen.end(), 10000
        )
    );

    // Also test that the observer functions get executed once per window.
    size_t enter_observe_cnt = 0;
    size_t leave_observe_cnt = 0;
    win_it.add_on_enter_observer( [&enter_observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "enter at " << enter_observe_cnt;
        ++enter_observe_cnt;
    });
    win_it.add_on_leave_observer( [&leave_observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "leave at " << leave_observe_cnt;
        ++leave_observe_cnt;
    });

    // Same for the callbacks
    size_t begin_cnt = 0;
    size_t end_cnt = 0;
    win_it.add_begin_callback( [&begin_cnt](){
        // LOG_DBG << "begin";
        ++begin_cnt;
    });
    win_it.add_end_callback( [&end_cnt](){
        // LOG_DBG << "end";
        ++end_cnt;
    });

    size_t window_cnt = 0;
    size_t total_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;
        LOG_DBG << "loop";

        for( auto const& elem : window ) {
            (void) elem;
            ++total_cnt;
        }
        ++window_cnt;
    }
    EXPECT_EQ( 0, window_cnt );
    EXPECT_EQ( 0, enter_observe_cnt );
    EXPECT_EQ( 0, leave_observe_cnt );
    EXPECT_EQ( 1, begin_cnt );
    EXPECT_EQ( 1, end_cnt );
    EXPECT_EQ( 0, total_cnt );
}

TEST( WindowStream, WindowViewStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make a Generic Input Stream over the data stream.
    auto data_gen = make_variant_input_stream_from_pileup_file( infile );
    // data_gen.block_size( 1024 * 1024 );
    data_gen.block_size(0);
    auto pileup_begin = data_gen.begin();
    auto pileup_end   = data_gen.end();

    // Create a window iterator based on the Generic Input Stream.
    auto win_it = make_window_view_stream(
        make_default_interval_window_stream(
            pileup_begin, pileup_end, 10000
        ).emit_leading_empty_windows( false )
    );

    // Also test that the observer functions get executed once per window.
    size_t enter_observe_cnt = 0;
    size_t leave_observe_cnt = 0;
    win_it.add_on_enter_observer( [&enter_observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "enter at " << enter_observe_cnt;
        ++enter_observe_cnt;
    });
    win_it.add_on_leave_observer( [&leave_observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "leave at " << leave_observe_cnt;
        ++leave_observe_cnt;
    });

    // Same for the callbacks
    size_t begin_cnt = 0;
    size_t end_cnt = 0;
    win_it.add_begin_callback( [&begin_cnt](){
        // LOG_DBG << "begin";
        ++begin_cnt;
    });
    win_it.add_end_callback( [&end_cnt](){
        // LOG_DBG << "end";
        ++end_cnt;
    });

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
    EXPECT_EQ( 7, enter_observe_cnt );
    EXPECT_EQ( 7, leave_observe_cnt );
    EXPECT_EQ( 1, begin_cnt );
    EXPECT_EQ( 1, end_cnt );
    EXPECT_EQ( 50000, total_cnt );
}

TEST( Window, Anchor )
{
    // Make some data.
    Window<int> w;
    w.first_position( 100 );
    w.last_position( 400 );
    w.entries().push_back( Window<int>::Entry( 0, 200, 42 ));
    w.entries().push_back( Window<int>::Entry( 1, 300, 420 ));
    WindowView<int> v{ w };

    // Get anchors for the window.
    EXPECT_EQ( 100, anchor_position( w, WindowAnchorType::kIntervalBegin ));
    EXPECT_EQ( 400, anchor_position( w, WindowAnchorType::kIntervalEnd ));
    EXPECT_EQ( 250, anchor_position( w, WindowAnchorType::kIntervalMidpoint ));
    EXPECT_EQ( 200, anchor_position( w, WindowAnchorType::kVariantFirst ));
    EXPECT_EQ( 300, anchor_position( w, WindowAnchorType::kVariantLast ));
    EXPECT_EQ( 300, anchor_position( w, WindowAnchorType::kVariantMedian ));
    EXPECT_EQ( 250, anchor_position( w, WindowAnchorType::kVariantMean ));
    EXPECT_EQ( 250, anchor_position( w, WindowAnchorType::kVariantMidpoint ));

    // Same for the window view. Some should fail.
    EXPECT_EQ(   100, anchor_position( v, WindowAnchorType::kIntervalBegin ));
    EXPECT_EQ(   400, anchor_position( v, WindowAnchorType::kIntervalEnd ));
    EXPECT_EQ(   250, anchor_position( v, WindowAnchorType::kIntervalMidpoint ));
    EXPECT_ANY_THROW( anchor_position( v, WindowAnchorType::kVariantFirst ));
    EXPECT_ANY_THROW( anchor_position( v, WindowAnchorType::kVariantLast ));
    EXPECT_ANY_THROW( anchor_position( v, WindowAnchorType::kVariantMedian ));
    EXPECT_ANY_THROW( anchor_position( v, WindowAnchorType::kVariantMean ));
    EXPECT_ANY_THROW( anchor_position( v, WindowAnchorType::kVariantMidpoint ));

    // Access window through a base window and test again.
    BaseWindow<int> const& bw = w;
    EXPECT_EQ( 100, anchor_position( bw, WindowAnchorType::kIntervalBegin ));
    EXPECT_EQ( 400, anchor_position( bw, WindowAnchorType::kIntervalEnd ));
    EXPECT_EQ( 250, anchor_position( bw, WindowAnchorType::kIntervalMidpoint ));
    EXPECT_EQ( 200, anchor_position( bw, WindowAnchorType::kVariantFirst ));
    EXPECT_EQ( 300, anchor_position( bw, WindowAnchorType::kVariantLast ));
    EXPECT_EQ( 300, anchor_position( bw, WindowAnchorType::kVariantMedian ));
    EXPECT_EQ( 250, anchor_position( bw, WindowAnchorType::kVariantMean ));
    EXPECT_EQ( 250, anchor_position( bw, WindowAnchorType::kVariantMidpoint ));

    // Access window view through a base window and test again.
    BaseWindow<int> const& bv = v;
    EXPECT_EQ(   100, anchor_position( bv, WindowAnchorType::kIntervalBegin ));
    EXPECT_EQ(   400, anchor_position( bv, WindowAnchorType::kIntervalEnd ));
    EXPECT_EQ(   250, anchor_position( bv, WindowAnchorType::kIntervalMidpoint ));
    EXPECT_ANY_THROW( anchor_position( bv, WindowAnchorType::kVariantFirst ));
    EXPECT_ANY_THROW( anchor_position( bv, WindowAnchorType::kVariantLast ));
    EXPECT_ANY_THROW( anchor_position( bv, WindowAnchorType::kVariantMedian ));
    EXPECT_ANY_THROW( anchor_position( bv, WindowAnchorType::kVariantMean ));
    EXPECT_ANY_THROW( anchor_position( bv, WindowAnchorType::kVariantMidpoint ));
}
