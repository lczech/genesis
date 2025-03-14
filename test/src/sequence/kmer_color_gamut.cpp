/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/sequence/kmer/canonical_encoding.hpp"
#include "genesis/sequence/kmer/color_gamut_functions.hpp"
#include "genesis/sequence/kmer/color_gamut.hpp"
#include "genesis/sequence/kmer/extractor.hpp"
#include "genesis/sequence/kmer/function.hpp"
#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/taxonomy/functions/kmer.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/printers/nested.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/random.hpp"

// The KmerColorGamut class is only available from C++17 onwards.
#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <future>
#include <iomanip>
#include <ios>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::utils;

// =================================================================================================
//     Functionality Tests
// =================================================================================================

// -------------------------------------------------------------------------
//     Basics
// -------------------------------------------------------------------------

TEST( KmerColorGamut, Basics )
{
    // We test up one more than a power of two, which are the points
    // where the binary reduction algorithm is interesting.
    for( size_t p = 1; p <= 17; ++p ) {
        // LOG_DBG << "============================================";
        auto gamut = KmerColorGamut( p );

        // LOG_DBG << "init_primary_colors";
        // gamut.init_primary_colors( p );
        // print_kmer_color_list( gamut );

        // LOG_DBG << "init_secondary_colors_with_binary_reduction";
        add_secondary_colors_with_binary_reduction( gamut );
        auto const initial_gamut_size = gamut.get_color_list().size();

        // Now we find all existing colors. The find function
        // should return exctly the color that we started with.
        for( size_t i = 0; i < initial_gamut_size; ++i ) {
            auto const& color = gamut.get_color_at(i);
            EXPECT_EQ( i, gamut.find_existing_color( color.elements ));
        }

        // For more than two elements, we did not add a color
        // that contains exaclty the first and last element.
        // Use that as a test for non-existing colors.
        if( p > 2 ) {
            auto non_existing = Bitvector( p );
            non_existing.set( 0 );
            non_existing.set( p-1 );
            EXPECT_EQ( 0, gamut.find_existing_color( non_existing ));
        }

        // Test that all primary colors can be retrieved.
        // That should also not change our color gamut size.
        for( size_t i = 0; i < p; ++i ) {
            EXPECT_EQ( i + 1, gamut.get_joined_color_index( 0, i ));
        }
        EXPECT_EQ( initial_gamut_size, gamut.get_color_list().size() );
        EXPECT_EQ( 2 * p, gamut.get_color_list().size() );

        // Print results
        verify_unique_colors( gamut );
        // LOG_DBG << print_kmer_color_list( gamut );
        // LOG_DBG << print_kmer_color_lookup( gamut );
        // LOG_DBG << print_kmer_color_gamut( gamut );
        // LOG_DBG << print_kmer_color_gamut_summary( gamut );
    }
}

// -------------------------------------------------------------------------
//     Example
// -------------------------------------------------------------------------

TEST( KmerColorGamut, Example )
{
    NEEDS_TEST_DATA;
    auto const p = 10;
    auto const r = 60;

    // Set up a manual example that tests every code path of the get_joined_color_index function
    auto gamut = KmerColorGamut( p, r );
    add_secondary_colors_with_binary_reduction( gamut );
    auto const initial_gamut_size = gamut.get_color_list().size();
    EXPECT_EQ( 2 * p, initial_gamut_size );

    // Initial list of colors and their bitvectors
    //   0  E   0  00000000 00
    //   1  P   1  10000000 00
    //   2  P   2  01000000 00
    //   3  P   3  00100000 00
    //   4  P   4  00010000 00
    //   5  P   5  00001000 00
    //   6  P   6  00000100 00
    //   7  P   7  00000010 00
    //   8  P   8  00000001 00
    //   9  P   9  00000000 10
    //  10  P  10  00000000 01
    //  11  S  11  11000000 00
    //  12  S  12  00110000 00
    //  13  S  13  00001100 00
    //  14  S  14  00000011 00
    //  15  S  15  00000000 11
    //  16  S  16  11110000 00
    //  17  S  17  00001111 00
    //  18  S  18  11111111 00
    //  19  S  19  11111111 11

    // Test special case of existing color index 0
    for( size_t i = 0; i < p; ++i ) {
        EXPECT_EQ( i + 1, gamut.get_joined_color_index( 0, i ));
    }

    // For every color that already exists in the set, look up that color and
    // each of its bits as the new target. As the bits are coming from the color,
    // this should always return the color itself.
    for( size_t i = 0; i < gamut.get_color_list().size(); ++i ) {
        auto const& color = gamut.get_color_at(i);
        for( size_t b = 0; b < p; ++b ) {
            if( color.elements.get(b) ) {
                EXPECT_EQ( i, gamut.get_joined_color_index( i, b ));
            }
        }
    }

    // Now we get a bit more creative. We want to lookup up colors where for a given color,
    // we ask for one additional bit being set, in a way such that this color with one extra
    // bit already exists. With the above setup, that is only the case for the two-bit secondary
    // colors: We use the primary colors as existing colors, and ask for the color with their
    // respecitive second bit being set, which gives colors 11 to 15 above.
    for( size_t i = 0; i < p; ++i ) {
        // The primary colors have an offset of one, and one bit set.
        auto const& color = gamut.get_color_at( i + 1 );
        EXPECT_EQ( 1, pop_count( color.elements ));

        // For the even-indexed primary numbers, we ask for the odd bit, and vice versa.
        if( i % 2 == 0 ) {
            EXPECT_EQ( 11 + i / 2, gamut.get_joined_color_index( i + 1, i + 1 ));
        } else {
            EXPECT_EQ( 11 + i / 2, gamut.get_joined_color_index( i + 1, i - 1 ));
        }
    }

    // Up until now, we have only asked for existing colors.
    // Check that all the above did not add any extra colors yet.
    EXPECT_EQ( initial_gamut_size, gamut.get_color_list().size() );

    // Now let's ask for colors that do not exist yet, and need to be created.
    // We do this by going through all two-bit colors, and ask for a third bit,
    // of which none exist in the above init.
    for( size_t i = 11; i <= 15; ++i ) {
        auto color = gamut.get_color_at(i);
        EXPECT_EQ( 2, pop_count( color.elements ));

        for( size_t b = 0; b < p; ++b ) {
            // If this is an existing bit, we expect the color index to be the same as before.
            if( color.elements.get( b )) {
                EXPECT_EQ( i, gamut.get_joined_color_index( i, b ));
                continue;
            }

            // Each of the colors that we look up here is about to be created,
            // so the size of the color list before will be its index.
            // We run the lookup twice, as the second time should do nothing,
            // which we want to check here.
            auto const exp_idx = gamut.get_color_list().size();
            EXPECT_EQ( exp_idx, gamut.get_joined_color_index( i, b ));
            EXPECT_EQ( exp_idx, gamut.get_joined_color_index( i, b ));
        }
    }

    // There are 5 secondary colors with two bits that we iterated above,
    // and for each of them, we asked for the color with each of the eight missing bits.
    EXPECT_EQ( initial_gamut_size + 5 * 8, gamut.get_color_list().size() );

    // At this point, we have exactly 60 colors: 1 empty, 10 primary, 9 initial secondary,
    // and 40 new secondary. We have set the real color limit to 60, so that we have exactly
    // exhausted that now. We now test imaginary colors by doing the same as we did with the
    // two-bit colors, but this time with the four-bit colors. There are two of them, at
    // indices 16 and 17 (see above), and adding another bit to either of them yields new colors.
    // These will be imaginary and in the gamut, and the closest fit will either be color 18 or 19.
    for( size_t i = 16; i <= 17; ++i ) {
        auto color = gamut.get_color_at(i);
        EXPECT_EQ( 4, pop_count( color.elements ));

        for( size_t b = 0; b < p; ++b ) {
            // Skip if this is an existing bit. We only want to ask for the missing ones.
            if( color.elements.get( b )) {
                continue;
            }

            // We expect color 18 if b is one of the first 8 bits, and color 19 otherwise.
            // Again, we run the lookup twice, as this shall not add new colors the second time,
            // but give the same result both times.
            auto const exp_idx = ( b < 8 ? 18 : 19 );
            EXPECT_EQ( exp_idx, gamut.get_joined_color_index( i, b ));
            EXPECT_EQ( exp_idx, gamut.get_joined_color_index( i, b ));
        }
    }

    // We added 12 imaginary colors now, and have switched to gamut now.
    EXPECT_EQ( initial_gamut_size + 5 * 8, gamut.get_color_list().size() );
    EXPECT_EQ( r, gamut.get_gamut_matrix().rows() );
    EXPECT_EQ( p, gamut.get_gamut_matrix().cols() );
    EXPECT_EQ(  0, gamut.get_gamut_statistics().real_color_count );
    EXPECT_EQ( 12, gamut.get_gamut_statistics().imag_color_count );

    // Finally check that we did not get any duplicates.
    verify_unique_colors( gamut );
    // LOG_DBG << print_kmer_color_list( gamut );
    // LOG_DBG << print_kmer_color_lookup( gamut );
    // LOG_DBG << print_kmer_color_gamut( gamut );
    // LOG_DBG << print_kmer_color_gamut_summary( gamut );

    // Test the serialization
    auto const colors_file = environment->data_dir + "sequence/gamut_example_colors.bin";
    auto const matrix_file = environment->data_dir + "sequence/gamut_example_matrix.bin";
    serialize_kmer_color_gamut_colors( gamut, to_file( colors_file ));
    serialize_kmer_color_gamut_matrix( gamut, to_file( matrix_file ));
    auto const deser_colors = deserialize_kmer_color_gamut_colors( from_file( colors_file ));
    auto const deser_matrix = deserialize_kmer_color_gamut_matrix( from_file( matrix_file ));
    EXPECT_EQ( 0, std::remove( colors_file.c_str() ));
    EXPECT_EQ( 0, std::remove( matrix_file.c_str() ));
    EXPECT_EQ( initial_gamut_size + 5 * 8, deser_colors.size() );
    EXPECT_EQ( gamut.get_color_list().size(), deser_colors.size() );
    for( size_t i = 0; i < deser_colors.size(); ++i ) {
        EXPECT_EQ( p, deser_colors[i].size() );
        EXPECT_EQ( gamut.get_color_at(i).elements, deser_colors[i] );
    }
    EXPECT_EQ( r, deser_matrix.rows() );
    EXPECT_EQ( p, deser_matrix.cols() );
    EXPECT_EQ( gamut.get_gamut_matrix(), deser_matrix );
}

// -------------------------------------------------------------------------
//     Random
// -------------------------------------------------------------------------

TEST( KmerColorGamut, Random )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_DBG << "Seed: " << seed;

    // Params of the color gamut.
    // We are only allowing the initial secondary colors;
    // everything after that (in the loop) will be an imaginary color.
    auto const p = 8;
    auto const r = 16;

    // Params of the loops.
    auto const n = 1000000;

    auto gamut = KmerColorGamut( p, r );
    add_secondary_colors_with_binary_reduction( gamut );
    // auto const initial_gamut_size = gamut.get_color_list().size();

    //   0  E   0  00000000
    //   1  P   1  10000000
    //   2  P   2  01000000
    //   3  P   3  00100000
    //   4  P   4  00010000
    //   5  P   5  00001000
    //   6  P   6  00000100
    //   7  P   7  00000010
    //   8  P   8  00000001
    //   9  S   9  11000000
    //  10  S  10  00110000
    //  11  S  11  00001100
    //  12  S  12  00000011
    //  13  S  13  11110000
    //  14  S  14  00001111
    //  15  S  15  11111111

    for( size_t i = 0; i < n; ++i ) {
        // Pick a random entry, and a random bit, and look it up. As we have limited
        // the max colors  to the inital ones, we immedately go into gamut mode.
        auto const e = permuted_congruential_generator( gamut.get_color_list().size() - 1 );
        // auto const e = permuted_congruential_generator( r - 1 );
        auto const b = permuted_congruential_generator( p - 1 );
        gamut.get_joined_color_index( e, b );
    }
    // LOG_DBG << print_kmer_color_list( gamut );

    // We have run the loop enough that we can be virtually sure to have requested
    // every color in the 8 bit range that we are using here, so the gamut should be full.
    EXPECT_EQ( 16, gamut.get_color_list().size() );
    EXPECT_EQ( 16, gamut.get_gamut_matrix().rows() );
    EXPECT_EQ(  8, gamut.get_gamut_matrix().cols() );
    auto img_idx_counts = std::vector<size_t>( 16, 0 );
    for( auto c : gamut.get_gamut_matrix() ) {
        ++img_idx_counts[c];
    }

    // Most of the imaginary colors will point to the all-set color at index 15,
    // while there will be eight each for indices 13 and 14.
    EXPECT_EQ( 16, img_idx_counts[13] );
    EXPECT_EQ( 16, img_idx_counts[14] );
    EXPECT_EQ( 64, img_idx_counts[15] );

    verify_unique_colors( gamut );
    // LOG_DBG << print_kmer_color_list( gamut );
    // LOG_DBG << print_kmer_color_lookup( gamut );
    // LOG_DBG << print_kmer_color_gamut( gamut );
    // LOG_DBG << print_kmer_color_gamut_summary( gamut );
}

// -------------------------------------------------------------------------
//     Taxonomy
// -------------------------------------------------------------------------

void test_kmer_color_taxonomy_(
    std::string const& infile,
    size_t exp_num_groups,
    size_t exp_secondary_colors,
    size_t exp_total_colors
) {
    // Read the test taxonomy
    auto const tax = taxonomy::read_kmer_taxonomy_from_json( from_file( infile ));
    auto const num_groups = taxonomy::count_taxon_groups( tax );
    EXPECT_EQ( exp_num_groups, num_groups );

    // Debugging output
    // auto printer = PrinterNested();
    // printer.indent_string( "    " );
    // LOG_DBG << printer( tax );

    // auto const bvs = make_secondary_colors_from_taxonomy( tax, 10, true );
    auto bvs = make_secondary_colors_from_taxonomy( tax );
    EXPECT_EQ( exp_secondary_colors, bvs.size() );

    // Debugging output
    // LOG_DBG << "secondary colors: " << bvs.size();
    // LOG_DBG << indent( bit_string_header( bvs[0].size() ), 8 );
    // for( size_t i = 0; i < bvs.size(); ++i ) {
    //     LOG_DBG << std::setw( 4 ) << i << "    " << to_bit_string( bvs[i], false );
    // }

    // Use the colors to construct a gamut
    auto gamut = KmerColorGamut( num_groups, std::move( bvs ));
    verify_unique_colors( gamut );
    // LOG_DBG << print_kmer_color_gamut_summary( gamut );
    EXPECT_EQ( exp_total_colors, gamut.get_color_list_size() );
}

TEST( KmerColorGamut, Taxonomy )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // We are testing with two types of files,
    // both made with the taxonomy grouping,
    // one being the full, and one being the trunk,
    // on a relatively recent NCBI taxonomy.

    // Expectation for both input files:
    // DBG  secondary colors: 1309
    // DBG  Elements:    512
    //      Colors:      1822
    //      Max colors:  1822
    //      Unique keys: 1818
    //      Gamut size:  0 x 0
    //      Gamut real:  0 (0.0%)
    //      Gamut imag:  0 (0.0%)
    //      Gamut empty: 0 (0.0%)

    test_kmer_color_taxonomy_(
        environment->data_dir + "sequence/grouped_taxonomy.json.gz",
        512,
        1309,
        1822
    );
    test_kmer_color_taxonomy_(
        environment->data_dir + "sequence/grouped_taxonomy_trunk.json.gz",
        512,
        1309,
        1822
    );
}

// =================================================================================================
//     Concurrency Tests
// =================================================================================================

void kmer_color_gamut_concurrency_test_(
    size_t p, // number of elements
    size_t r, // max number of colors
    size_t n  // number of requests per thread
) {
    size_t num_threads = 8;

    // Init the color gamut
    auto gamut = KmerColorGamut( p, r );
    add_secondary_colors_with_binary_reduction( gamut );
    // auto const initial_gamut_size = gamut.get_color_list().size();

    // Debugging and benchmarking output
    std::atomic<size_t> total_calls{0};
    gamut.set_on_gamut_start_callback( [&total_calls, n, num_threads](){
        auto const tcp = 100.0 * total_calls.load() / (n * num_threads);
        LOG_DBG << "starting gamut with total calls: " << total_calls.load() << " = " << tcp << "%";
    });
    gamut.set_on_gamut_filled_callback( [&total_calls, n, num_threads](){
        auto const tcp = 100.0 * total_calls.load() / (n * num_threads);
        LOG_DBG << "filled gamut with total calls: " << total_calls.load() << " = " << tcp << "%";
    });

    // Prepare async tasks that will run in parallel accessing the set
    std::atomic<size_t> worker_ready{0};
    std::promise<void> go;
    std::shared_future<void> ready(go.get_future());
    auto worker_done = std::vector<std::future<void>>(num_threads);
    // std::mutex mtx;

    try
    {
        // Run workers, all waiting for the signal to start, then running in parallel.
        for( size_t i = 0; i < num_threads; ++i ) {
            worker_done[i] = std::async(
                std::launch::async,
                [ready, &worker_ready, &gamut, p, r, n, &total_calls]() {
                    // Wait for all workers to be ready
                    ++worker_ready;
                    ready.wait();

                    // Then run some async stress on the color gamut!
                    for( size_t i = 0; i < n; ++i ) {
                        // Pick a random entry, and a random bit, and look it up. This time,
                        // we only pick secondary colors, and ignore existing imaginary colors.
                        auto max_color_index = std::min( gamut.get_color_list_size() - 1, r - 1 );
                        auto const e = permuted_congruential_generator( max_color_index );
                        auto const b = permuted_congruential_generator( p - 1 );

                        // Protect the data - no longer needed, as the color gamut
                        // now has built-in locking for extra fast concurrency.
                        // std::lock_guard<std::mutex> lock( mtx );

                        ++total_calls;
                        gamut.get_joined_color_index( e, b );
                    }
                }
            );
        }

        // Set up all threads to wait for the signal, then go!
        // Busy wait is okay here for test purposes
        while( worker_ready.load() != num_threads ) {
            std::this_thread::yield();
        }
        go.set_value();

        // Signal was given, now we wait for results
        for( size_t i = 0; i < num_threads; ++i ) {
            worker_done[i].get();
        }

        // Test our results
    } catch(...) {
        // Clean up such that all threads join again
        go.set_value();
        throw;
    }

    // Let's see what we got!
    verify_unique_colors( gamut );
    // LOG_DBG << print_kmer_color_list( gamut );
    // LOG_DBG << print_kmer_color_lookup( gamut );
    // LOG_DBG << print_kmer_color_gamut( gamut );
    LOG_DBG << print_kmer_color_gamut_summary( gamut );
}

TEST( KmerColorGamut, Concurrency )
{
    // Deactivate logging output for regular tests.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_DBG << "Seed: " << seed;

    // Params of the color gamut.
    // size_t const p = 16;
    // size_t const r = 1048576;
    // auto const n = 10000;

    // Run test that does not saturate the colors
    kmer_color_gamut_concurrency_test_( 16, 1024 * 1024, 10000 );

    // Run a test that satures the colors and starts the gamut,
    // but does not fill it completely.
    kmer_color_gamut_concurrency_test_( 16, 1024, 5000 );

    // Run a test that saturates the colors, and (very likely) fills the gamut.
    kmer_color_gamut_concurrency_test_( 16, 256, 20000 );
}

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17
