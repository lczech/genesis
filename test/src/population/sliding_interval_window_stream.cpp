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

#include "genesis/population/formats/simple_pileup_input_stream.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/streams/variant_input_stream.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/sliding_interval_window_stream.hpp"
#include "genesis/population/window/variant_window_stream.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

#include <unordered_map>

using namespace genesis::population;
using namespace genesis::utils;

template<class WindowStream>
void test_sliding_interval_stream_( WindowStream& win_it )
{
    bool found_first_win = false;
    bool found_last_win = false;

    // DBG  2R : 7790001 7790001-7800000 # 1
    // DBG  2R : 7800001 7800001-7810000 # 9874
    // DBG  2R : 7810001 7810001-7820000 # 9972
    // DBG  2R : 7820001 7820001-7830000 # 9909
    // DBG  2R : 7830001 7830001-7840000 # 10000
    // DBG  2R : 7840001 7840001-7850000 # 9997
    // DBG  2R : 7850001 7850001-7860000 # 247

    // Map from starting positions to sizes of the windows.
    std::unordered_map<size_t, size_t> const window_sizes = {
        { 7790001, 1 },
        { 7800001, 9874 },
        { 7810001, 9972 },
        { 7820001, 9909 },
        { 7830001, 10000 },
        { 7840001, 9997 },
        { 7850001, 247 }
    };

    // Also test that the observer functions get executed once per window.
    size_t observe_cnt = 0;
    using ValueType = typename WindowStream::InputStreamType::value_type;
    win_it.add_observer( [&observe_cnt]( Window<ValueType> const& ){
        // LOG_DBG << "at " << observe_cnt;
        ++observe_cnt;
    });

    size_t window_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        // LOG_DBG << window.chromosome() << " : "
        //         << anchor_position( window ) << " "
        //         << window.first_position() << "-" << window.last_position()
        //         << " # " << window.entry_count();

        if( it.is_first_window() ) {
            EXPECT_EQ( 7790001, window.first_position() );
            EXPECT_FALSE( found_first_win );
            found_first_win = true;
        }
        if( it.is_last_window() ) {
            EXPECT_EQ( 7850001, window.first_position() );
            EXPECT_FALSE( found_last_win );
            found_last_win = true;
        }

        EXPECT_TRUE( window.first_position() >= 7790001 );
        EXPECT_TRUE( window.first_position() <= 7850001 );
        EXPECT_TRUE( window.last_position() >= 7800000 );
        EXPECT_TRUE( window.last_position() <= 7860000 );

        EXPECT_TRUE( window_sizes.count( window.first_position() ));
        EXPECT_EQ( window_sizes.at( window.first_position() ), window.size() );

        ++window_cnt;
    }
    EXPECT_EQ( 7, window_cnt );
    EXPECT_EQ( 7, observe_cnt );

    EXPECT_TRUE( found_first_win );
    EXPECT_TRUE( found_last_win );
}

TEST( WindowStream, SlidingIntervalDirect )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make an underlying data iterator over some variants.
    auto pileup_begin = SimplePileupInputStream<>( from_file( infile ));
    auto pileup_end = SimplePileupInputStream<>();

    // Set up the window iterator. Rename to `win_it` to use it with the below test code.
    auto win_it = make_default_sliding_interval_window_stream(
        pileup_begin, pileup_end, 10000
    );
    win_it.emit_leading_empty_windows( false );

    // Run the tests.
    test_sliding_interval_stream_( win_it );

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

TEST( WindowStream, SlidingIntervalLambda )
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
    auto win_it = make_default_sliding_interval_window_stream(
        pileup_begin, pileup_end, 10000
    );
    win_it.emit_leading_empty_windows( false );

    test_sliding_interval_stream_( win_it );
}

void run_sliding_interval_window_view_variant_test_( VariantWindowViewStream& win_it )
{
    size_t window_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        ++window_cnt;
    }
    EXPECT_EQ( 7, window_cnt );
}

TEST( WindowStream, SlidingIntervalWindowView )
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
    auto win_it = make_default_sliding_interval_window_view_stream(
        pileup_begin, pileup_end, 10000
    );
    // win_it.emit_leading_empty_windows( false );

    // Also test that the observer functions get executed once per window.
    size_t observe_cnt = 0;
    win_it.add_observer( [&observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "at " << observe_cnt;
        ++observe_cnt;
    });

    // We use a test function that takes our abstract type, to see if we set this up correctly.
    run_sliding_interval_window_view_variant_test_( win_it );
    EXPECT_EQ( 7, observe_cnt );

    // test_sliding_interval_stream_( win_it );
    // size_t window_cnt = 0;
    // for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
    //     ++window_cnt;
    // }
    // EXPECT_EQ( 786, window_cnt );
}

TEST( WindowStream, SlidingIntervalEmpty )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/empty.pileup";

    // Make a Generic Input Stream over the data stream.
    auto data_gen = make_variant_input_stream_from_pileup_file( infile );
    auto pileup_begin = data_gen.begin();
    auto pileup_end   = data_gen.end();

    // Create a window iterator based on the Generic Input Stream.
    auto win_it = make_default_sliding_interval_window_stream(
        pileup_begin, pileup_end, 10000
    );

    // Also test that the observer functions get executed once per window.
    size_t observe_cnt = 0;
    win_it.add_observer( [&observe_cnt]( Window<Variant> const& ){
        // LOG_DBG << "at " << observe_cnt;
        ++observe_cnt;
    });

    size_t window_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {

        // This body should never be exectuted.
        // We access the iterator still, to make sure that this thing gets compiled properly,
        // without optimizing anything away by accident.
        // EXPECT_ANY_THROW( it->chromosome() );
        EXPECT_TRUE( it.is_first_window() );
        EXPECT_TRUE( it.is_last_window() );
        LOG_DBG << "-" << it->chromosome() << "-";

        ++window_cnt;
    }
    EXPECT_EQ( 0, window_cnt );
    EXPECT_EQ( 0, observe_cnt );
}
