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

#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/window/position_window_stream.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"
#include "genesis/utils/math/random.hpp"

#include <unordered_map>
#include <unordered_set>

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     Empty
// =================================================================================================

TEST( PositionWindowStream, Empty )
{
    using namespace genesis::sequence;

    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    // LOG_DBG << "empty";
    std::string const infile = environment->data_dir + "population/empty.pileup";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make a generic input stream over the data stream,
    // and set up the window iterator. Rename to `win_it` to use it with the below test code.
    auto data_gen = make_variant_input_stream_from_pileup_file( infile );
    auto win_it = make_default_position_window_stream( data_gen.begin(), data_gen.end() );

    // Run the tests.
    std::vector<size_t> pos_per_chr;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        EXPECT_EQ( 1, it->first_position() );
        EXPECT_EQ( 1, it->last_position() );

        pos_per_chr.push_back(0);
        // LOG_DBG << "chr " << it->chromosome();
        for( auto const& elem : *it ) {
            // LOG_DBG1 << "at " << elem.position;
            (void) elem;
            ++pos_per_chr.back();
        }
        // LOG_DBG << "done " << cnt;

        EXPECT_EQ( 1, it->first_position() );
        EXPECT_EQ( 1, it->last_position() );
    }

    // Tests
    EXPECT_EQ( 0, pos_per_chr.size() );
}

// =================================================================================================
//     Random Fuzzy
// =================================================================================================

std::vector<Variant> test_position_window_stream_fuzzy_make_data_()
{
    // Create some variants, for some random number of chromosomes.
    std::vector<Variant> data;
    auto const num_chr = permuted_congruential_generator( 3 );
    for( size_t c = 0; c < num_chr; ++c ) {
        auto const n_positions = 100;
        auto const first_position = data.size();
        for( size_t n_var = 0; n_var < n_positions; ++n_var ) {
            data.emplace_back();
            auto& variant = data.back();
            variant.chromosome = '1' + c;
            variant.position = n_var + 1;
            // We do not need to fill the variant with sample data here,
            // as we are not going to use that anyway...
        }
        auto const last_position = data.size();

        // Now we set the status randomly to passing or not passing here.
        // We want a couple of configurations here just to make sure that this works under all
        // conditions, so we select from: none passing, some passing, and all passing,
        // at equal probability. This will fill the whole chromosome with this configuration,
        // so that we end up testing all combinations of types as well.
        auto const type = permuted_congruential_generator( 2 );
        switch( type ) {
            case 0: {
                // None passing.
                for( size_t n_var = first_position; n_var < n_positions; ++n_var ) {
                    data[n_var].status.set( 1 );
                }
                break;
            }
            case 1: {
                // Some passing. 50:50 of making that happen.
                for( size_t n_var = first_position; n_var < n_positions; ++n_var ) {
                    if( permuted_congruential_generator_bool() ) {
                        data[n_var].status.set( 1 );
                    }
                }
                break;
            }
            case 2: {
                // All passing.
                // Nothing to do.
                break;
            }
            default: {
                // Can't happen by the above logic... just here as an assertion.
                EXPECT_TRUE( false );
            }
        }

        LOG_DBG << std::string( 1, '1' + c ) << ":" << first_position << "-" << last_position
                << " with type " << type
        ;
    }

    return data;
}

void test_position_window_stream_fuzzy_run_( std::vector<Variant> const& data )
{
    // Make a generic input stream over the data stream, and set up the window iterator.
    auto data_gen = make_variant_input_stream_from_vector( data );
    auto win_it = make_passing_variant_position_window_stream( data_gen.begin(), data_gen.end() );

    // Run the tests.
    std::unordered_map<std::string, size_t> got_visiting;
    std::unordered_set<std::string> first_chr;
    std::unordered_set<std::string> last_chr;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        // Test the basic setup of the window and its content.
        ASSERT_EQ( 1, it->entry_count() );
        auto const& var = window[0].data;
        EXPECT_EQ( var.chromosome, it->chromosome() );
        EXPECT_EQ( var.position, it->first_position() );
        EXPECT_EQ( var.position, it->last_position() );
        EXPECT_EQ( var.position, window[0].position );
        EXPECT_TRUE( var.status.passing() );

        // Test the first and last counters per chromosome
        if( it.is_first_window() ) {
            EXPECT_EQ( 0, first_chr.count( window.chromosome() ));
            first_chr.insert( window.chromosome() );
        }
        if( it.is_last_window() ) {
            EXPECT_EQ( 0, last_chr.count( window.chromosome() ));
            last_chr.insert( window.chromosome() );
        }

        // Increment the number of visited variants for this chromosome
        ++got_visiting[window.chromosome()];
    }

    // For the test expectation, we do a manual count of how many positions
    // on each chromosome have which status.
    std::unordered_map<std::string, size_t> exp_visiting;
    for( auto const& var : data ) {
        if( var.status.passing() ) {
            ++exp_visiting[var.chromosome];
        }
    }

    // We need to have found as many first and last window chr entries
    // as we have different chromosomes.
    EXPECT_EQ( exp_visiting.size(), first_chr.size() );
    EXPECT_EQ( exp_visiting.size(), last_chr.size() );

    // Finally, test that we got the exact right amount of visited entries in each chr.
    EXPECT_EQ( exp_visiting, got_visiting );
}

TEST( PositionWindowStream, RandomFuzzy )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 2000;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        auto const data = test_position_window_stream_fuzzy_make_data_();
        test_position_window_stream_fuzzy_run_( data );
    }
}
