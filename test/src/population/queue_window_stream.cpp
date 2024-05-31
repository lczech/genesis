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
#include "genesis/population/function/genome_region.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/queue_window_stream.hpp"
#include "genesis/population/window/variant_window_stream.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <unordered_map>

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     Basic file based tests
// =================================================================================================

template<class WindowStream>
void test_queue_window_stream_( WindowStream& win_it, size_t count )
{
    bool found_first_win = false;
    bool found_last_win = false;

    // Also test that the observer functions get executed once per window.
    size_t enter_observe_cnt = 0;
    size_t leave_observe_cnt = 0;
    using ValueType = typename WindowStream::InputStreamType::value_type;
    win_it.add_on_enter_observer( [&enter_observe_cnt]( Window<ValueType> const& ){
        // LOG_DBG << "at " << enter_observe_cnt;
        ++enter_observe_cnt;
    });
    win_it.add_on_leave_observer( [&leave_observe_cnt]( Window<ValueType> const& ){
        // LOG_DBG << "at " << leave_observe_cnt;
        ++leave_observe_cnt;
    });

    // DBG  2R : 7790001 7790001-7800000 # 1
    // DBG  2R : 7800001 7800001-7810000 # 9874
    // DBG  2R : 7810001 7810001-7820000 # 9972
    // DBG  2R : 7820001 7820001-7830000 # 9909
    // DBG  2R : 7830001 7830001-7840000 # 10000
    // DBG  2R : 7840001 7840001-7850000 # 9997
    // DBG  2R : 7850001 7850001-7860000 # 247

    std::vector<size_t> window_first_positions;
    std::vector<size_t> window_last_positions;
    std::vector<size_t> window_sizes;

    if( count == 9000 ) {
        window_first_positions = std::vector<size_t>{
            7800000, 7809126, 7818154, 7827245, 7836245, 7845248
        };
        window_last_positions = std::vector<size_t>{
            7809125, 7818153, 7827244, 7836244, 7845247, 7850275
        };
        window_sizes = std::vector<size_t>{
            9000, 9000, 9000, 9000, 9000, 5000
        };
    } else if( count == 10000 ) {
        window_first_positions = std::vector<size_t>{
            7800000, 7810126, 7820154, 7830245, 7840245
        };
        window_last_positions = std::vector<size_t>{
            7810125, 7820153, 7830244, 7840244, 7850275
        };
        window_sizes = std::vector<size_t>{
            10000, 10000, 10000, 10000, 10000
        };
    } else {
        ASSERT_TRUE( false );
    }
    ASSERT_EQ( window_sizes.size(), window_first_positions.size() );
    ASSERT_EQ( window_sizes.size(), window_last_positions.size() );

    size_t window_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        // LOG_DBG << window.chromosome() << " : "
        //         << anchor_position( window ) << " "
        //         << window.first_position() << "-" << window.last_position()
        //         << " # " << window.entry_count();

        // LOG_DBG << "first " << window.first_position()
        //         << " last " << window.last_position()
        //         << " size " << window.size();

        if( it.is_first_window() ) {
            EXPECT_FALSE( found_first_win );
            found_first_win = true;
        }
        if( it.is_last_window() ) {
            EXPECT_FALSE( found_last_win );
            found_last_win = true;
        }

        EXPECT_TRUE( window_cnt < window_first_positions.size() );
        EXPECT_TRUE( window_cnt < window_last_positions.size() );
        EXPECT_TRUE( window_cnt < window_sizes.size() );
        EXPECT_EQ( window_first_positions[window_cnt], window.first_position() );
        EXPECT_EQ( window_last_positions[window_cnt], window.last_position() );
        EXPECT_EQ( window_sizes[window_cnt], window.size() );

        EXPECT_TRUE( window.first_position() >= 7790001 );
        EXPECT_TRUE( window.first_position() <= 7850001 );
        EXPECT_TRUE( window.last_position() >= 7800000 );
        EXPECT_TRUE( window.last_position() <= 7860000 );

        ++window_cnt;
    }
    EXPECT_EQ( window_sizes.size(), window_cnt );
    EXPECT_EQ( window_sizes.size(), enter_observe_cnt );
    EXPECT_EQ( window_sizes.size(), leave_observe_cnt );

    EXPECT_TRUE( found_first_win );
    EXPECT_TRUE( found_last_win );
}

void test_window_stream_queue_direct( size_t count )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make an underlying data iterator over some variants.
    auto pileup_begin = SimplePileupInputStream<>( from_file( infile ));
    auto pileup_end = SimplePileupInputStream<>();

    // Set up the window iterator. Rename to `win_it` to use it with the below test code.
    auto win_it = make_default_queue_window_stream(
        pileup_begin, pileup_end, count
    );

    // Run the tests.
    test_queue_window_stream_( win_it, count );

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

TEST( WindowStream, QueueWindowDirect )
{
    test_window_stream_queue_direct(  9000 );
    test_window_stream_queue_direct( 10000 );
}

void test_window_stream_queue_lambda( size_t count )
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
    auto win_it = make_default_queue_window_stream(
        pileup_begin, pileup_end, count
    );

    test_queue_window_stream_( win_it, count );
}

TEST( WindowStream, QueueWindowLambda )
{
    test_window_stream_queue_lambda(  9000 );
    test_window_stream_queue_lambda( 10000 );
}

void run_queue_window_view_variant_test_( VariantWindowViewStream& win_it )
{
    size_t window_cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        EXPECT_TRUE( window.first_position() >= 7790001 );
        EXPECT_TRUE( window.first_position() <= 7850001 );
        EXPECT_TRUE( window.last_position() >= 7800000 );
        EXPECT_TRUE( window.last_position() <= 7860000 );

        ++window_cnt;
    }
    EXPECT_EQ( 6, window_cnt );
}

TEST( WindowStream, QueueWindowWindowView )
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
    auto win_it = make_default_queue_window_view_stream(
        pileup_begin, pileup_end, 9000
    );

    // Also test that the observer functions get executed once per window.
    size_t enter_observe_cnt = 0;
    size_t leave_observe_cnt = 0;
    win_it.add_on_enter_observer( [&enter_observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "at " << enter_observe_cnt;
        ++enter_observe_cnt;
    });
    win_it.add_on_leave_observer( [&leave_observe_cnt]( WindowView<Variant> const& ){
        // LOG_DBG << "at " << leave_observe_cnt;
        ++leave_observe_cnt;
    });

    // We use a test function that takes our abstract type, to see if we set this up correctly.
    run_queue_window_view_variant_test_( win_it );
    EXPECT_EQ( 6, enter_observe_cnt );
    EXPECT_EQ( 6, leave_observe_cnt );

    // size_t window_cnt = 0;
    // for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
    //     ++window_cnt;
    // }
    // EXPECT_EQ( 6, window_cnt );
}

TEST( WindowStream, QueueWindowEmpty )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/empty.pileup";

    // Make a Generic Input Stream over the data stream.
    auto data_gen = make_variant_input_stream_from_pileup_file( infile );
    auto pileup_begin = data_gen.begin();
    auto pileup_end   = data_gen.end();

    // Create a window iterator based on the Generic Input Stream.
    auto win_it = make_default_queue_window_stream(
        pileup_begin, pileup_end, 10000
    );

    // Also test that the observer functions get executed once per window.
    size_t enter_observe_cnt = 0;
    size_t leave_observe_cnt = 0;
    win_it.add_on_enter_observer( [&enter_observe_cnt]( Window<Variant> const& ){
        // LOG_DBG << "at " << enter_observe_cnt;
        ++enter_observe_cnt;
    });
    win_it.add_on_leave_observer( [&leave_observe_cnt]( Window<Variant> const& ){
        // LOG_DBG << "at " << leave_observe_cnt;
        ++leave_observe_cnt;
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
    EXPECT_EQ( 0, enter_observe_cnt );
    EXPECT_EQ( 0, leave_observe_cnt );
}

// =================================================================================================
//     Randomized fuzzy tests
// =================================================================================================

struct QueueWindowFuzzyTestVariantCounts
{
    size_t missing = 0;
    size_t passing = 0;
    size_t not_passing = 0;
};

struct QueueWindowFuzzyTestVariantData
{
    // We want to test three cases: Variant missing, passing, and not passing.
    // We count how many variants of each type we have per chromosome.
    std::vector<Variant> variants;
    std::vector<QueueWindowFuzzyTestVariantCounts> variant_counts;
};

struct QueueWindowFuzzyTestWindowPositions
{
    std::vector<size_t> first;
    std::vector<size_t> last;
    std::vector<size_t> passing;
    std::vector<size_t> not_passing;
};

QueueWindowFuzzyTestVariantData test_queue_window_fuzzy_make_data_(
    size_t num_chrs, size_t max_pos
) {
    QueueWindowFuzzyTestVariantData data;
    data.variant_counts = std::vector<QueueWindowFuzzyTestVariantCounts>( num_chrs );

    // Make variants for each chr
    for( size_t c = 0; c < num_chrs; ++c ) {
        auto const chr_name = std::string( 1, 'A' + c );
        auto const num_pos = permuted_congruential_generator( 1, max_pos );

        // We want three types of positions, and want to test very different distributions thereof.
        // This can lead to all missing. Also, in the loop below where we draw from these,
        // we could just draw all missing, in particular when num_pos is low. As this would be
        // equivalent to just having drawn a lower num_chrs, but also interfere with our counting
        // here in the test, we want to avoid that case. So below we catch this and fix it.
        auto const frac_passing = permuted_congruential_generator( 0, 10 );
        auto const frac_notpass = permuted_congruential_generator( 0, 10 - frac_passing );
        auto const frac_missing = 10 - ( frac_passing + frac_notpass );
        EXPECT_LE( frac_passing + frac_notpass, 10 );
        EXPECT_EQ( frac_passing + frac_notpass + frac_missing, 10 );

        // Make the variants for the chromosome.
        // std::string pos_log;
        for( size_t p = 1; p <= num_pos; ++p ) {
            Variant var;
            var.chromosome = chr_name;
            var.position = p;

            // If we are in the last position of the chromosome, and have not added any actual
            // data to the variants, we add at least one that is either passing or missing,
            // to avoid having nothing there, which would mess up our chr counting below.
            if(
                p == num_pos &&
                data.variant_counts[c].passing + data.variant_counts[c].not_passing == 0
            ) {
                if( permuted_congruential_generator_bool() ) {
                    // Passing
                    data.variants.push_back( var );
                    // pos_log.push_back('x');
                    ++data.variant_counts[c].passing;
                } else {
                    // Not passing
                    var.status.set( 1 );
                    data.variants.push_back( var );
                    // pos_log.push_back('o');
                    ++data.variant_counts[c].not_passing;
                }
                continue;
            }

            // Make the varaints of each type, randomly.
            // We use the standard approach for drawing from intervals, by accumulating their
            // sizes until our draw falls into an interval.
            auto const var_type_draw = permuted_congruential_generator( 1, 10 );
            if( var_type_draw <= frac_passing ) {
                // Passing
                data.variants.push_back( var );
                // pos_log.push_back('x');
                ++data.variant_counts[c].passing;
            } else if( var_type_draw <= frac_passing + frac_notpass ) {
                // Not passing
                var.status.set( 1 );
                data.variants.push_back( var );
                // pos_log.push_back('o');
                ++data.variant_counts[c].not_passing;
            } else {
                // Missing
                ++data.variant_counts[c].missing;
                // pos_log.push_back('-');
            }
        }

        // Debug print
        // LOG_DBG << chr_name << ": " << pos_log;
    }
    return data;
}

QueueWindowFuzzyTestWindowPositions test_queue_window_fuzzy_get_window_positions_(
    QueueWindowFuzzyTestVariantData  const& data,
    size_t count, size_t stride
) {
    QueueWindowFuzzyTestWindowPositions window_positions;

    // This function goes through all the data and finds which windows we want to visit.
    // As we have full knowledge of the data here, and also in order to not just replicate the
    // online algorithm of the actual QueueWindowStream, we here simply do look-ahead loops
    // to find the end of each window, which keeps the algorithm relatively simple compared
    // to the stream.

    // Helper function that determines how many passing and non passing positions are in there.
    auto add_passing_non_passing_counters_ = [&]( size_t first_p, size_t last_p ){
        // LOG_DBG << "add " << first_p << " to " << last_p;
        size_t passing = 0;
        size_t not_passing = 0;
        for( size_t i = first_p; i <= last_p; ++i ) {
            if( data.variants[i].status.passing() ) {
                ++passing;
            } else {
                ++not_passing;
            }
        }
        window_positions.passing.push_back( passing );
        window_positions.not_passing.push_back( not_passing );
    };

    std::string cur_chr;
    size_t last_selected = 0;
    size_t p = 0;
    while( p < data.variants.size() ) {

        // New chromosome: figure out where the last selected entry is.
        if( cur_chr != data.variants[p].chromosome ) {
            // Find the last selected entry on the chr
            cur_chr = data.variants[p].chromosome;
            bool found_last = false;
            for( size_t j = p; j < data.variants.size(); ++j ) {
                if( data.variants[j].chromosome != cur_chr ) {
                    break;
                }
                if( data.variants[j].status.passing() ) {
                    last_selected = j;
                    found_last = true;
                }
            }

            // If there is no selected entry, we just use the whole chr as a single window,
            // and move p to the next chromosome then. We first add the current position of p
            // as a window start (as we are currently at the beginning of a chromosme), then
            // move to the end of the chromosome onto the next entry. That means, for simplicity
            // of the loop, we go one entry too far, and so we add its previous one as the end
            // of the window. After that, we restart the overall loop.
            if( ! found_last ) {
                size_t const first_p = p;
                window_positions.first.push_back(data.variants[p].position);
                while( p < data.variants.size() && data.variants[p].chromosome == cur_chr ) {
                    ++p;
                }
                EXPECT_TRUE( p > 0 );
                window_positions.last.push_back(data.variants[p-1].position);
                add_passing_non_passing_counters_( first_p, p-1 );
                continue;
            }
        }

        // Here, we have estiablished that we are on a chromosome that has at least one selcted
        // entry, and we now where the last one is, so that we can check and prepare the last
        // window of the chromosome accordingly.

        // We are at the start of a window now. Add it, and then find the end of the window.
        // We move q for as many selected positions as our window width count indicates, or
        // until we are at the last selected position of the window, in which case this is the
        // last window of the chromosome.
        size_t const first_p = p;
        window_positions.first.push_back(data.variants[p].position);
        size_t passing_count = 0;
        size_t q = p;
        for( ; q <= last_selected; ++q ) {
            if( data.variants[q].status.passing() ) {
                ++passing_count;
            }
            if( passing_count == count ) {
                break;
            }
        }

        // If this is the last window of the chromosome, we need to also include the trailing
        // non selected entries on the chromosome for the end of the window.
        if( q == last_selected || q == last_selected + 1 ) {
            while( q < data.variants.size() && data.variants[q].chromosome == cur_chr ) {
                ++q;
            }
            EXPECT_TRUE( q > 0 );
            window_positions.last.push_back(data.variants[q-1].position);
            add_passing_non_passing_counters_( first_p, q-1 );

            // Now that we know that this is the last window of the chromosome, we can skip ahead
            // to the next chromosome, and are done here.
            p = q;
            continue;
        }

        // Now that we are here, we have found a q that is the end of the current window,
        // so we add it to the window list
        window_positions.last.push_back(data.variants[q].position);
        add_passing_non_passing_counters_( first_p, q );

        // Finally, we need to move p by as many selected entries as the stride tells us.
        // We know that this will still leave us within the current chromosome, as otherwise the
        // above boundary condition for q would have fired, so we can just move along without
        // too many extra conditions here.
        passing_count = 0;
        while( passing_count < stride ) {
            EXPECT_TRUE( p < data.variants.size() );
            if( data.variants[p].status.passing() ) {
                ++passing_count;
            }
            ++p;
            EXPECT_TRUE( p < data.variants.size() );
        }
    }
    EXPECT_EQ( window_positions.first.size(), window_positions.last.size() );
    EXPECT_EQ( window_positions.first.size(), window_positions.passing.size() );
    EXPECT_EQ( window_positions.first.size(), window_positions.not_passing.size() );

    // // Debug output of all windows, one per line.
    // for( size_t i = 0; i < window_positions.first.size(); ++i ) {
    //     // We leave a bit of space in the beginning to align the strings.
    //     // The chromosomes are printed as "A: ", which is three characters,
    //     // but our positions here are 1-based, so we always print another space in the loop
    //     // below anyway, which we hence leave out in the indentation padding string.
    //     std::string out = "  ";
    //     for( size_t j = 0; j < window_positions.first[i]; ++j ) {
    //         out += ' ';
    //     }
    //     if( window_positions.first[i] == window_positions.last[i]  ) {
    //         out += "|";
    //     } else {
    //         out += "[";
    //         for( size_t j = window_positions.first[i] + 1; j < window_positions.last[i]; ++j ) {
    //             out += '-';
    //         }
    //         out += "]";
    //     }
    //     LOG_DBG << out << " [" << window_positions.first[i] << ", " << window_positions.last[i] << "]";
    // }

    return window_positions;
}

void test_queue_window_fuzzy_()
{
    // Get our basic params: how many chromosomes, maximally how many positions per chr,
    // and in which window width and stride do we want to process them.
    auto const num_chrs = permuted_congruential_generator( 1, 3 );
    auto const max_pos = permuted_congruential_generator( 1, 400 );
    auto const sel_count = permuted_congruential_generator( 10, 100 );
    auto const sel_stride = permuted_congruential_generator_bool() ? sel_count / 10 : sel_count;

    LOG_DBG << "sel_count==" << sel_count << " sel_stride==" << sel_stride;

    // -------------------------------------------------------------------------
    //     Make Variants
    // -------------------------------------------------------------------------

    // Create random variants for testing
    auto const data = test_queue_window_fuzzy_make_data_( num_chrs, max_pos );

    // Make a list of the positions that we need to visit as starting points of each window.
    // then index into that list in the loop below, checking that we got the correct positions.
    auto const window_exp = test_queue_window_fuzzy_get_window_positions_(
        data, sel_count, sel_stride
    );

    // -------------------------------------------------------------------------
    //     Run stream
    // -------------------------------------------------------------------------

    // Make a Generic Input Stream over the data stream.
    auto data_gen = make_variant_input_stream_from_vector( data.variants );
    auto vec_begin = data_gen.begin();
    auto vec_end   = data_gen.end();

    // Create a window iterator based on the Generic Input Stream.
    // We make either a complete stride, or a partial one.
    auto win_it = make_passing_variant_queue_window_stream(
        vec_begin, vec_end, sel_count, sel_stride
    );
    // LOG_DBG << "sel_stride " << sel_stride;

    // Also test that the observer functions get executed once per window.
    size_t enter_observe_cnt = 0;
    size_t leave_observe_cnt = 0;
    win_it.add_on_enter_observer( [&enter_observe_cnt]( Window<Variant> const& ){
        // LOG_DBG << "at " << enter_observe_cnt;
        ++enter_observe_cnt;
    });
    win_it.add_on_leave_observer( [&leave_observe_cnt]( Window<Variant> const& ){
        // LOG_DBG << "at " << leave_observe_cnt;
        ++leave_observe_cnt;
    });

    // Correct number of first and last windows
    size_t first_cnt = 0;
    size_t last_cnt = 0;

    size_t window_cnt = 0;
    auto chr_win_cnts = std::vector<size_t> ( num_chrs );
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        LOG_DBG << "at " << it->genome_region();

        if( it.is_first_window() ) {
            ++first_cnt;
        }
        if( it.is_last_window() ) {
            ++last_cnt;
        }

        // Test the window boundaries.
        EXPECT_LT( window_cnt, window_exp.first.size() );
        EXPECT_EQ( it->first_position(), window_exp.first[window_cnt] );
        EXPECT_EQ( it->last_position(),  window_exp.last[window_cnt] );

        // Collect info on all variants in the window.
        // We need to have the expected number of passing positions for all but the last window
        // of each chromosome.
        QueueWindowFuzzyTestVariantCounts var_cnt;
        for( auto const& entry : *it ) {
            if( entry.data.status.passing() ) {
                ++var_cnt.passing;
            } else {
                ++var_cnt.not_passing;
            }
        }
        // LOG_DBG << "passing " << var_cnt.passing << " not_passing " << var_cnt.not_passing;
        EXPECT_TRUE(
            var_cnt.passing == sel_count ||
            ( it.is_last_window() && var_cnt.passing <= sel_count )
        );
        EXPECT_EQ( var_cnt.passing, window_exp.passing[window_cnt] );
        EXPECT_EQ( var_cnt.not_passing, window_exp.not_passing[window_cnt] );

        ++chr_win_cnts[ static_cast<size_t>( it->chromosome()[0] - 'A') ];
        ++window_cnt;
    }

    // -------------------------------------------------------------------------
    //     Test post conditions
    // -------------------------------------------------------------------------

    EXPECT_EQ( first_cnt, num_chrs );
    EXPECT_EQ( last_cnt, num_chrs );
    EXPECT_EQ( enter_observe_cnt, window_cnt );
    EXPECT_EQ( leave_observe_cnt, window_cnt );
    EXPECT_EQ( window_exp.first.size(), window_cnt );

    size_t tot_win_cnt = 0;
    ASSERT_EQ( data.variant_counts.size(), chr_win_cnts.size() );
    for( size_t i = 0; i < data.variant_counts.size(); ++i ) {
        // LOG_DBG << "chr " << std::string( 1, 'A' + i );
        auto const& var_cnt = data.variant_counts[i];
        auto const& chr_win_cnt = chr_win_cnts[i];
        tot_win_cnt += chr_win_cnt;

        // LOG_DBG << "missing " << var_cnt.missing << ", passing " << var_cnt.passing << ", not_passing " << var_cnt.not_passing;
        // LOG_DBG << "chr_win_cnt " << chr_win_cnt;

        // auto const calc_tmp = var_cnt.passing - (sel_count - sel_stride);
        // auto const calc_cnt = (calc_tmp + (calc_tmp % sel_stride == 0 ? 0 : 1)) / sel_stride + 1;
        // LOG_DBG << "calc " << calc_cnt;

        // // floor((n-frame_len))/frame_step)+mod(n,frame_step)
        // auto const nc = (( var_cnt.passing - sel_count ) / sel_stride ) + (( (var_cnt.passing-sel_count) % sel_stride ) ? 1 : 0 );
        // LOG_DBG << "nc " << nc;

        size_t exp_num_win = 0;
        if( var_cnt.passing < sel_count ) {
            exp_num_win = 1;
        } else if( (var_cnt.passing - sel_count) % sel_stride == 0 ) {
            exp_num_win = (( var_cnt.passing - sel_count ) / sel_stride ) + 1;
        } else {
            exp_num_win = (( var_cnt.passing - sel_count ) / sel_stride ) + 2;
        }
        // LOG_DBG << "exp_num_win " << exp_num_win;
        EXPECT_EQ( exp_num_win, chr_win_cnt );
    }
    // LOG_DBG << "window_cnt " << window_cnt;
    EXPECT_EQ( tot_win_cnt, window_cnt );
}

TEST( WindowStream, QueueWindowFuzzy )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t const num_tests = 3000;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        test_queue_window_fuzzy_();
    }
}
