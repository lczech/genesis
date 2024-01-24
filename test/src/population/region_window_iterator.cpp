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

#include "genesis/population/variant.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/region_window_iterator.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/lambda_iterator.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace genesis::population;
using namespace genesis::utils;

// -------------------------------------------------------------------------
//     Run test case
// -------------------------------------------------------------------------

void run_region_window_test_(
    std::vector<Variant> const& variants,
    std::shared_ptr<GenomeRegionList> const region_list,
    bool skip_empty_regions
) {
    // Test output
    std::stringstream ss;
    ss << "Variants:";
    for( auto const& var : variants ) {
        ss << " " << var.chromosome << ":" << var.position;
    }
    ss << "\nRegions:";
    for( auto const& chr : region_list->chromosome_names() ) {
        for( auto const& reg : region_list->chromosome_regions(chr) ) {
            ss << " " << chr << ":" << reg.low() << "-" << reg.high();
        }
    }
    ss << "\nSkip: " << std::boolalpha << skip_empty_regions << "\n";
    LOG_DBG << ss.str();

    // Keep track of which chromosomes we have seen in the iteration, and for which
    // we have had the first and last window property set, to check that those are correct.
    std::unordered_set<std::string> chr_names;
    std::unordered_set<std::string> found_first_win;
    std::unordered_set<std::string> found_last_win;

    // Build a Bitvector for each chr, noting its variants, so that we can quickly check them.
    std::unordered_map<std::string, Bitvector> chr_vars;
    for( auto const& var : variants ) {
        // We make it 50 bits long, to give us space for all test cases.
        if( chr_vars.count( var.chromosome ) == 0 ) {
            chr_vars[ var.chromosome ] = Bitvector( 50 );
        }
        EXPECT_FALSE( chr_vars[ var.chromosome ][ var.position ] );
        chr_vars[ var.chromosome ].set( var.position );
    }

    // Additinally, build the same strucuture the other way round: at each position where
    // vars and regions overlap, store how many regions there are. Then, we delete them one by
    // one while iterating the regions, and in the end, all should be zero, as then we have seen
    // each position as often as it is covered.
    // We also count the sum of coverages, which is how many variants we expect to see in total,
    // with possible duplicates for overlapping regions.
    std::unordered_map<std::string, std::vector<size_t>> exp_chr_reg_cov;
    size_t exp_var_total_cnt_1 = 0;
    for( auto const& var : variants ) {
        // We make it 50 entries long, to give us space for all test cases.
        if( exp_chr_reg_cov.count( var.chromosome ) == 0 ) {
            exp_chr_reg_cov[ var.chromosome ] = std::vector<size_t>( 50, 0 );
        }
        EXPECT_EQ( 0, exp_chr_reg_cov[ var.chromosome ][ var.position ] );
        auto const cov_cnt = region_list->cover_count( var.chromosome, var.position );
        exp_chr_reg_cov[ var.chromosome ][ var.position ] = cov_cnt;
        exp_var_total_cnt_1 += cov_cnt;
    }

    // Also, do this the other way round as well: Check that all regions see their variants.
    // For this, we check for all regions which variants they have, and increment a count here.
    // Then, while iterating the regions, we decrement again, and each should end at zero then.
    // At the same time, we also keep track of how many regions have variants at all,
    // and use that for a total window count. Lastly, keep track of how many variants in total
    // we will see, with possible duplicates for overlapping regions. Same as before,
    // but differnt way of assertaining this number, to be sure.
    std::unordered_map<std::string, std::vector<size_t>> exp_chr_reg_var_cov_cnt;
    std::unordered_map<std::string, size_t> exp_chr_win_cnt;
    size_t regions_with_vars = 0;
    size_t window_cnt_target = 0;
    size_t exp_var_total_cnt_2 = 0;
    for( auto const& chr : region_list->chromosome_names() ) {
        for( auto const& reg : region_list->chromosome_regions(chr) ) {
            // We make it 50 entries long, to give us space for all test cases.
            if( exp_chr_reg_var_cov_cnt.count( chr ) == 0 ) {
                exp_chr_reg_var_cov_cnt[ chr ] = std::vector<size_t>( 50, 0 );
            }

            // Find all variants in the region, and increment them.
            bool found_var = false;
            for( size_t i = reg.low(); i <= reg.high(); ++i ) {
                if( chr_vars.count( chr ) > 0 && chr_vars[chr].get( i )) {
                    ++exp_chr_reg_var_cov_cnt[ chr ][ i ];
                    ++exp_var_total_cnt_2;
                    found_var = true;
                }
            }

            // If the region has at least a variant, we expect to iterate over that region.
            // If we do not skip empty regions, we also expect to iteratie it.
            (void) exp_chr_win_cnt[chr];
            if( found_var ) {
                ++exp_chr_win_cnt[chr];
                ++regions_with_vars;
                ++window_cnt_target;
            } else if( ! skip_empty_regions ) {
                ++exp_chr_win_cnt[chr];
                ++window_cnt_target;
            }
        }
    }
    if( ! skip_empty_regions ) {
        EXPECT_EQ( region_list->total_region_count(), window_cnt_target );
    }
    EXPECT_EQ( exp_var_total_cnt_1, exp_var_total_cnt_2 );

    // We want to make sure that the indices of the entries per window are what they are supposed
    // to be. So we create a list for each variant telling us by its position the how many'th it is.
    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> index_map;
    for( auto const& var : variants ) {
        // We make it 50 entries long, to give us space for all test cases.
        if( index_map.count( var.chromosome ) == 0 ) {
            index_map[ var.chromosome ] = std::unordered_map<size_t, size_t>();
        }
        // The inner map is from positin to its index. No position should be set multiple times.
        // Then, we set it to the current size of the map on the chromosome, meaning that the first
        // var gets index 0, and increasing from there, which is what we want.
        EXPECT_EQ( 0, index_map[ var.chromosome ].count( var.position ));
        auto const idx_sz = index_map[ var.chromosome ].size();
        index_map[ var.chromosome ][ var.position ] = idx_sz;
    }

    // Count how many windows we have seen in total. Needs to match the number of regions with
    // data or, all of them, depending on whether we skip empty regions.
    // Also count how many we have seen with variants in them.
    size_t window_tot_cnt = 0;
    size_t window_var_cnt = 0;
    size_t var_total_cnt = 0;

    // Make the iterator
    auto win_it = make_default_region_window_iterator(
        variants.begin(), variants.end(), region_list
    );

    // Also test that the observer functions get executed once per window.
    size_t observe_cnt = 0;
    win_it.add_observer( [&observe_cnt]( Window<Variant> const& ){
        // LOG_DBG << "at " << observe_cnt;
        ++observe_cnt;
    });

    // ...and go through it.
    win_it.skip_empty_regions( skip_empty_regions );
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;
        auto const& chr = window.chromosome();

        LOG_DBG << "At " << chr << ":" << window.first_position() << "-" << window.last_position()
                << " with " << window.size() << " entries";

        // Window should match the region we are at. We could do a full check of this, by marking
        // regions that we have seen, which would take care of duplicate regions (that can happen
        // randomly), but would also be a bit tricky given that we might skip empty regions, so
        // we would have to scan through the unmarked ones and see if they are indeed empty...
        // So instead we simply assert that a region exists with the exact window positions,
        // and by the nature of our randomness, that will hopefully cover enough test cases.
        auto reg_win = region_list->chromosome_regions( chr ).find({
            window.first_position(), window.last_position()
        });
        EXPECT_NE( region_list->chromosome_regions( chr ).end(), reg_win );

        // Check that the first and last only occurr once and in the correct order.
        // We check all combinations individually, to make sure that each case is covered
        // without interference of having inserted chrs in the sets already.
        // This is a bit repetetive, but better save than sorry.
        EXPECT_TRUE( exp_chr_win_cnt.count( chr ) > 0 );
        if( it.is_first_window() && it.is_last_window() ) {
            // Window that is the only region on its chromosome.
            EXPECT_TRUE( region_list->region_count( chr ) == 1 || skip_empty_regions );
            EXPECT_TRUE( exp_chr_win_cnt.at( chr ) == 1 );
            EXPECT_TRUE( chr_names.count( chr ) == 0 );
            EXPECT_TRUE( found_first_win.count( chr ) == 0 );
            EXPECT_TRUE( found_last_win.count( chr ) == 0 );
            found_first_win.insert( chr );
            found_last_win.insert( chr );

        } else if( it.is_first_window() && ! it.is_last_window() ) {
            // First window, but there are more regions on this chromosome.
            EXPECT_TRUE( region_list->region_count( chr ) > 1 );
            EXPECT_TRUE( exp_chr_win_cnt.at( chr ) > 1 );
            EXPECT_TRUE( chr_names.count( chr ) == 0 );
            EXPECT_TRUE( found_first_win.count( chr ) == 0 );
            EXPECT_TRUE( found_last_win.count( chr ) == 0 );
            found_first_win.insert( chr );

        } else if( ! it.is_first_window() && it.is_last_window() ) {
            // Last window on this chromosome, after having seen others.
            EXPECT_TRUE( region_list->region_count( chr ) > 1 );
            EXPECT_TRUE( exp_chr_win_cnt.at( chr ) == 1 );
            EXPECT_TRUE( chr_names.count( chr ) > 0 );
            EXPECT_TRUE( found_first_win.count( chr ) > 0 );
            EXPECT_TRUE( found_last_win.count( chr ) == 0 );
            found_last_win.insert( chr );

        } else if( ! it.is_first_window() && ! it.is_last_window() ) {
            // We are somewhere in the middle of the regions on this chromosome.
            EXPECT_TRUE( region_list->region_count( chr ) > 1 );
            EXPECT_TRUE( exp_chr_win_cnt.at( chr ) > 1 );
            EXPECT_TRUE( chr_names.count( chr ) > 0 );
            EXPECT_TRUE( found_first_win.count( chr ) > 0 );
            EXPECT_TRUE( found_last_win.count( chr ) == 0 );

        } else {
            // We covered all cases above.
            ASSERT_FALSE( true );
        }

        // Decrement the windows that we have seen for this region. Should be 0 in the end.
        EXPECT_TRUE( exp_chr_win_cnt.count( chr ) > 0 );
        --exp_chr_win_cnt[chr];

        // Keep track of all that we have seen, so that we always collect a chr in chr_names,
        // independently of whether it had its first property set or not. That ensures that
        // for each chr that we see, we can check whether its first property was set at all,
        // after the iteration loop.
        EXPECT_TRUE( found_first_win.count( chr ) > 0 );
        chr_names.insert( chr );

        // Test that all variants in the window exist, and that all variants that we have for
        // that window are actually in there, and none are where they shouldn't be.
        if( chr_vars.count( chr ) == 0 ) {
            // Chromosome is not in the variants bitvectors. We can only have that
            // if we don't skip empty regions, and have an empty region here.
            EXPECT_FALSE( skip_empty_regions );
            EXPECT_EQ( 0, window.size() );
        } else {
            // Test that all variants are in the window, and that the window only contains variants
            // that exist. For this, we make a copy of the bitvector, and unset the bits for which
            // we have vars, after unsetting all that are outside of the window anyway.
            // At the end, none should remain.
            auto bv = chr_vars.at( chr );
            EXPECT_FALSE( bv[0] );
            for( size_t i = 0; i < window.first_position(); ++i ) {
                bv.unset(i);
            }
            for( size_t i = window.last_position() + 1; i < bv.size(); ++i ) {
                bv.unset(i);
            }
            for( auto const& var : window ) {
                // We expect the position to be present. If not, the window contains a variant
                // that was not in the data.
                EXPECT_TRUE( bv[ var.position ] );
                bv.unset( var.position );
            }
            // We expect the result to be empty.
            // If not, there are variants that were not in the window.
            EXPECT_EQ( 0, bv.count() );
        }

        // For each variant in the window, decrement its cover count.
        if( exp_chr_reg_cov.count( chr ) == 0 ) {
            // Same as above, can only occurr for empty windows that are not skipped.
            EXPECT_FALSE( skip_empty_regions );
            EXPECT_EQ( 0, window.size() );
        } else {
            EXPECT_TRUE( exp_chr_reg_cov[ chr ].size() == 50 );
            for( auto const& var : window ) {
                EXPECT_TRUE( exp_chr_reg_cov[ chr ][ var.position ] > 0 );
                --exp_chr_reg_cov[ chr ][ var.position ];
            }
        }

        // Do this in the other direction as well, and drement the cover count obtained from regions.
        // These entries all need to exist, as they were created by iteration the regions.
        EXPECT_TRUE( exp_chr_reg_var_cov_cnt.count( chr ) > 0 );
        EXPECT_TRUE( exp_chr_reg_var_cov_cnt[ chr ].size() == 50 );
        for( auto const& var : window ) {
            EXPECT_TRUE( exp_chr_reg_var_cov_cnt[ chr ][ var.position ] > 0 );
            --exp_chr_reg_var_cov_cnt[ chr ][ var.position ];
        }

        // Check that the indices are correct.
        if( ! window.empty() ) {
            EXPECT_TRUE( index_map.count( chr ) > 0 );
            for( auto const& var : window ) {
                EXPECT_TRUE( index_map[chr].count( var.position ) > 0 );
                EXPECT_EQ( var.index, index_map[chr][ var.position ] );
            }
        } else {
            EXPECT_TRUE( ! skip_empty_regions );
        }

        // Increment our window counts: how many with data, and how many in total.
        var_total_cnt += window.size();
        if( window.size() > 0 ) {
            ++window_var_cnt;
        }
        ++window_tot_cnt;
    }

    // Check that first and last occurred for each chr that we saw, and vice versa.
    EXPECT_EQ( chr_names.size(), found_first_win.size() );
    EXPECT_EQ( chr_names.size(), found_last_win.size() );
    for( auto const& chr : chr_names ) {
        EXPECT_TRUE( found_first_win.count( chr ) > 0 );
        EXPECT_TRUE( found_last_win.count( chr ) > 0 );
    }
    for( auto const& chr : found_first_win ) {
        EXPECT_TRUE( chr_names.count( chr ) > 0 );
        EXPECT_TRUE( found_last_win.count( chr ) > 0 );
    }
    for( auto const& chr : found_last_win ) {
        EXPECT_TRUE( chr_names.count( chr ) > 0 );
        EXPECT_TRUE( found_first_win.count( chr ) > 0 );
    }

    // We should have seen each variant exactly as often as it is covered by a region.
    // Each time that happend, we decremented the coverage count, so now they all need to be zero.
    for( auto const& chr_reg : exp_chr_reg_cov ) {
        for( auto const& entry : chr_reg.second ) {
            EXPECT_EQ( 0, entry );
        }
    }

    // Same for the map that was created in the other direction, from regions.
    for( auto const& chr_reg : exp_chr_reg_var_cov_cnt ) {
        for( auto const& entry : chr_reg.second ) {
            EXPECT_EQ( 0, entry );
        }
    }

    // Check that we have seen as many windows as we expected from the number of regions with data
    // (or also without, if we dont't skip empty regions).
    for( auto const& reg_cnt : exp_chr_win_cnt ) {
        EXPECT_EQ( 0, reg_cnt.second );
    }
    EXPECT_EQ( exp_var_total_cnt_1, var_total_cnt );
    EXPECT_EQ( exp_var_total_cnt_2, var_total_cnt );
    EXPECT_EQ( regions_with_vars, window_var_cnt );
    EXPECT_EQ( window_cnt_target, window_tot_cnt );
    EXPECT_EQ( window_cnt_target, observe_cnt );
    if( ! skip_empty_regions ) {
        EXPECT_EQ( region_list->total_region_count(), window_tot_cnt );
    }
}

// -------------------------------------------------------------------------
//     Build random target test cases
// -------------------------------------------------------------------------

void test_add_var_( std::vector<Variant>& variants, std::string const& chr, size_t pos )
{
    Variant var;
    var.chromosome = chr;
    var.position = pos;
    variants.push_back( var );
}

void test_add_target_variants_(
    size_t target,
    size_t bit,
    std::vector<Variant>& variants,
    std::string const& chr,
    size_t offset
) {
    if( target & ( 1 << bit )) {
        // We select random positions and create variants there.
        // We use the first 10 bit of a random number for this, making sure that it's not 0.
        uint32_t t = 0;
        while( t == 0 ) {
            t = permuted_congruential_generator() & (( 1 << 10 ) - 1 );
        }

        // Now add variants at the position when the bit is set.
        // This also ensures that no variant is duplicated, which we do not want to allow.
        for( size_t i = 0; i < 10; ++i ) {
            if( t & ( 1 << i )) {
                test_add_var_( variants, chr, offset + i );
            }
        }
    }
}

void test_add_target_regions_(
    size_t target,
    size_t bit,
    std::shared_ptr<GenomeRegionList> region_list,
    std::string const& chr,
    size_t offset
) {
    if( target & ( 1 << bit )) {

        // We generate 1-3 regions randomly here,
        // and hope that this eventually covers all cases that we want to test.
        auto const t = 1 + permuted_congruential_generator() % 3;
        assert( 1 <= t && t <= 3 );

        for( size_t i = 0; i < t; ++i ) {
            // Get random start and end, sort ascending, add as region with the offset.
            auto s = permuted_congruential_generator() % 10;
            auto e = permuted_congruential_generator() % 10;
            ascending( s, e );
            assert( s < 10 );
            assert( e < 10 );
            assert( s <= e );
            region_list->add( chr, offset + s, offset + e );
        }
    }
}

void test_make_target_(
    size_t target,
    std::vector<Variant>& variants,
    std::shared_ptr<GenomeRegionList> region_list,
    bool& skip_empty_regions
) {
    // Test the first 9 bits, and create variants in the positions they represent.
    test_add_target_variants_( target, 0, variants, "1", 10 );
    test_add_target_variants_( target, 1, variants, "1", 20 );
    test_add_target_variants_( target, 2, variants, "1", 30 );
    test_add_target_variants_( target, 3, variants, "2", 10 );
    test_add_target_variants_( target, 4, variants, "2", 20 );
    test_add_target_variants_( target, 5, variants, "2", 30 );
    test_add_target_variants_( target, 6, variants, "3", 10 );
    test_add_target_variants_( target, 7, variants, "3", 20 );
    test_add_target_variants_( target, 8, variants, "3", 30 );

    // Test the next 9 bits, and create regions in their positions.
    test_add_target_regions_( target,  9, region_list, "1", 10 );
    test_add_target_regions_( target, 10, region_list, "1", 20 );
    test_add_target_regions_( target, 11, region_list, "1", 30 );
    test_add_target_regions_( target, 12, region_list, "2", 10 );
    test_add_target_regions_( target, 13, region_list, "2", 20 );
    test_add_target_regions_( target, 14, region_list, "2", 30 );
    test_add_target_regions_( target, 15, region_list, "3", 10 );
    test_add_target_regions_( target, 16, region_list, "3", 20 );
    test_add_target_regions_( target, 17, region_list, "3", 30 );

    // Also, set the skip propery randomly, using yet another bit
    skip_empty_regions = ( target & ( 1 << 18 ));
}

std::string test_get_target_representation_( size_t target )
{
    // Print out the target pattern, with letters stand for whether there is data, regions,
    // or both, in one of the three intervals (low, mid, high), in each of the three chromosomes.
    std::string str = "[---][---][---] (" + std::to_string(target) + ")";
    auto set_char_ = [&]( size_t var_bit, size_t reg_bit, size_t str_pos ){
        if( target & ( 1 << var_bit )) {
            if( target & ( 1 << reg_bit )) {
                str[str_pos] = 'X';
            } else {
                str[str_pos] = 'I';
            }
        } else {
            if( target & ( 1 << reg_bit )) {
                str[str_pos] = 'O';
            }
        }
    };

    set_char_( 0,  9,  1 );
    set_char_( 1, 10,  2 );
    set_char_( 2, 11,  3 );
    set_char_( 3, 12,  6 );
    set_char_( 4, 13,  7 );
    set_char_( 5, 14,  8 );
    set_char_( 6, 15, 11 );
    set_char_( 7, 16, 12 );
    set_char_( 8, 17, 13 );
    return str;
}

// -------------------------------------------------------------------------
//     Main test functions
// -------------------------------------------------------------------------

TEST( WindowIterator, RegionWindowIterator )
{
    // We want all kinds of combinations:
    //
    //  - Chromosomes with and without data, in all possible orders,
    //    that is, also empty chromosomes surrounded by those with data, and vice versa.
    //  - Same for positions on the same chromosome, we want regions with and without positions,
    //    and surrounded by those with data, and vice versa.
    //
    // We use chr 1, 2, 3 with either combination of data and regions, for all permutations (?).
    // Then, we do the same with positions, we use intervals low=[10,19], mid=[20,29], high=[30,39],
    // with and without data, and with and without regions.
    // Then, we double all of that again for skipping or not skipping empty.
    //
    // We encode all of this in an integer, were bits stand for whether we have data or regions:
    //
    //  - bit 0: data in chr 1 low or not
    //  - bit 1: data in chr 1 mid
    //  - ...
    //  - bit 8: data in chr 3 high
    //  - bit 9..17: same for regions
    //  - bit 18: skip or no skip empty regions
    //
    // Where the data and regions within those intervals are is generated randomly, as an
    // exhaustive test over all possible combintaions of regions and variants would be infeasible.
    // With this, we hope to generate all kinds of combinations of different types of regions:
    // (none, implicit if bit off), single, separated, adjacent, overlapping, duplicate, nested.
    // As this is too much to comprehensively test, we just chose at random, and by the sheer number
    // of tests, will hit these cases from time to time.

    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    // We have 2^9 = 512 possible combinations of variants, and the same for regions.
    // So we want at least that many tests, to have (on average across test runs) a good enough
    // chance to encounter each combination regularly, and in particular the combinations where there
    // are no regions at all, or no variants at all, which are probably the most interesting ones.
    // See below for some numbers of test cases that fulfill these requirements.

    // This tests all possible combinations as explained above. Each one of them is randomly
    // filled with data and regions, so it's not fully exhaustive, but at least each type
    // of combination is present once.
    // Takes ~30s on my machine.
    // size_t const max_tests = ( 1 << 19 );

    // ~1h runtime (on my machine), for some crazy random testing. If used with random_targets = false,
    // this tests all combinations of variants and regions again and again, but each time with a
    // different random filling of them.
    // size_t const max_tests = ( 1 << 26 );

    // 0.5s runtime, our default for normal tests.
    size_t const max_tests = 10000;

    // Debug testing.
    // size_t const max_tests = 10;

    // Do we want random tests, or enumerate all? The latter only makes sense when we actually
    // have a high enough max_tests number, e.g., max_tests > ( 1 << 19 )
    bool const random_targets = true;
    // bool const random_targets = false;

    // Run tests while we have time.
    for( size_t test_num = 0; test_num < max_tests; ++test_num ) {
        // Create a random combination of positions and regions, or just enumerate,
        // using the test number as the target, that is, with each test case we have a new target
        // representing a new combination of regions and variants.
        auto const target = random_targets
            ? permuted_congruential_generator() & (( 1 << 19 ) - 1 )
            : test_num
        ;

        // Fill our iterables with data according to the target.
        std::vector<Variant> variants;
        auto region_list = std::make_shared<GenomeRegionList>();
        bool skip_empty_regions = false;
        test_make_target_( target, variants, region_list, skip_empty_regions );

        // Run the test.
        LOG_DBG << "Test " << test_num << ": " << test_get_target_representation_( target ) << "\n";
        run_region_window_test_( variants, region_list, skip_empty_regions );
    }
}
