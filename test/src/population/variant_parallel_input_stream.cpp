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

#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_parallel_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/function/genome_locus.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <set>
#include <unordered_map>
#include <vector>

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     Simple Tests
// =================================================================================================

void test_parallel_input_stream_(
    VariantParallelInputStream::ContributionType p_sel,
    VariantParallelInputStream::ContributionType s_sel,
    VariantParallelInputStream::ContributionType v_sel,
    std::set<size_t> expected_positions,
    std::set<size_t> additional_loci = {},
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = {}
) {
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const p_infile = environment->data_dir + "population/parallel.pileup";
    std::string const s_infile = environment->data_dir + "population/parallel_a.sync";

    #ifdef GENESIS_HTSLIB

        std::string const v_infile = environment->data_dir + "population/parallel.vcf";

    #else

        std::string const v_infile = environment->data_dir + "population/parallel_b.sync";

    #endif

    // Init with the desired settings
    auto pit = VariantParallelInputStream();
    pit.add_variant_input_stream(
        make_variant_input_stream_from_pileup_file( p_infile ),
        p_sel
    );
    pit.add_variant_input_stream(
        make_variant_input_stream_from_sync_file( s_infile ),
        s_sel
    );

    #ifdef GENESIS_HTSLIB

        pit.add_variant_input_stream(
            make_variant_input_stream_from_pool_vcf_file( v_infile ),
            v_sel
        );

    #else

        pit.add_variant_input_stream(
            make_variant_input_stream_from_sync_file( v_infile ),
            v_sel
        );

    #endif

    // Add the addtional loci, if present. If all of the input sources are following, but
    // we have additional carrying loci, we do only visit those, and hence need to clear the
    // list of expected postions from the intersection of the input sources first.
    if(
        p_sel == VariantParallelInputStream::ContributionType::kFollowing &&
        s_sel == VariantParallelInputStream::ContributionType::kFollowing &&
        v_sel == VariantParallelInputStream::ContributionType::kFollowing &&
        ! additional_loci.empty()
    ) {
        expected_positions.clear();
    }
    for( auto const& al : additional_loci ) {
        pit.add_carrying_locus( GenomeLocus( "XYZ", al ));
        expected_positions.insert( al );
    }

    // Set the seq dict. If nullptr, this works as well
    pit.sequence_dict( sequence_dict );

    std::set<size_t> found_positions;
    for( auto it = pit.begin(); it != pit.end(); ++it ) {
        // LOG_DBG2 << it.locus().to_string();
        found_positions.insert( it.locus().position );

        // Make sure that all inputs have either no data, or the correct number of samples.
        EXPECT_TRUE( ! it.variants()[0] || it.variants()[0]->samples.size() == 1 );
        EXPECT_TRUE( ! it.variants()[1] || it.variants()[1]->samples.size() == 2 );
        EXPECT_TRUE( ! it.variants()[2] || it.variants()[2]->samples.size() == 3 );

        // Same with a differnt way of access.
        EXPECT_TRUE( ! it.variant_at(0) || it.variant_at(0)->samples.size() == 1 );
        EXPECT_TRUE( ! it.variant_at(1) || it.variant_at(1)->samples.size() == 2 );
        EXPECT_TRUE( ! it.variant_at(2) || it.variant_at(2)->samples.size() == 3 );

        // // Yet another way. Here we test that if the iterator is type following,
        // // we must have data, as otherwise we would not have stopped here.
        // // Nope, not true, all can have all.
        // if( p_sel == VariantParallelInputStream::ContributionType::kFollowing ) {
        //     EXPECT_TRUE( it.variant_at(0) );
        // }
        // if( s_sel == VariantParallelInputStream::ContributionType::kFollowing ) {
        //     EXPECT_TRUE( it.variant_at(1) );
        // }
        // if( v_sel == VariantParallelInputStream::ContributionType::kFollowing ) {
        //     EXPECT_TRUE( it.variant_at(2) );
        // }

        // Traverse the data. Test from the class documentation.
        for( auto const& var : *it ) {
            if( var ) {
                // Dummy test
                EXPECT_TRUE( var->samples.size() <= 3 );
            }
        }


        // Also test this using the merged variants.
        // We need to ignore alt bases here, as not all input sources have them.
        EXPECT_EQ( 6, it.joined_variant().samples.size() );

        // We also test with move as the very last thing, to make sure that this
        // does not give any weird segfaults.
        VariantParallelInputStream::JoinedVariantParams joined_variant_params;
        joined_variant_params.move_samples = true;
        EXPECT_EQ( 6, it.joined_variant( joined_variant_params ).samples.size() );
    }
    EXPECT_EQ( expected_positions, found_positions );
}

void test_parallel_input_stream_all_(
    std::set<size_t> additional_loci = {},
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = {}
) {
    //  Key to which input file has which positions
    //  Pos   P   S   V
    //  5     x       x
    //  8     x
    //  10    x   x   x
    //  12        x
    //  15    x       x
    //  17    x   x
    //  20            x
    //  22    x
    //  25        x   x
    //  28    x   x
    //  30        x

    // Test out all compbinations of carrying and following iterators.

    // LOG_DBG1 << "UUU";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kCarrying,
        std::set<size_t>{ 5, 8, 10, 12, 15, 17, 20, 22, 25, 28, 30 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "UUI";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kFollowing,
        std::set<size_t>{ 5, 8, 10, 12, 15, 17, 22, 25, 28, 30 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "UIU";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kCarrying,
        std::set<size_t>{ 5, 8, 10, 15, 17, 20, 22, 25, 28 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "IUU";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kCarrying,
        std::set<size_t>{ 5, 10, 12, 15, 17, 20, 25, 28, 30 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "UII";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kFollowing,
        std::set<size_t>{ 5, 8, 10, 15, 17, 22, 28 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "IUI";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kCarrying,
        VariantParallelInputStream::ContributionType::kFollowing,
        std::set<size_t>{ 10, 12, 17, 25, 28, 30 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "IIU";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kCarrying,
        std::set<size_t>{ 5, 10, 15, 20, 25 },
        additional_loci,
        sequence_dict
    );
    // LOG_DBG1 << "III";
    test_parallel_input_stream_(
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kFollowing,
        VariantParallelInputStream::ContributionType::kFollowing,
        std::set<size_t>{ 10 },
        additional_loci,
        sequence_dict
    );
}

TEST( ParallelInputStream, Basics )
{
    // Test without additional loci.
    // LOG_DBG << "Normal";
    test_parallel_input_stream_all_();

    // Test with different positions and numbers of additional loci.
    // LOG_DBG << "1";
    test_parallel_input_stream_all_({ 1 });
    // LOG_DBG << "15";
    test_parallel_input_stream_all_({ 15 });
    // LOG_DBG << "16";
    test_parallel_input_stream_all_({ 16 });
    // LOG_DBG << "32";
    test_parallel_input_stream_all_({ 32 });
    // LOG_DBG << "1, 15, 32";
    test_parallel_input_stream_all_({ 1, 15, 32 });
    // LOG_DBG << "15, 32";
    test_parallel_input_stream_all_({ 15, 32 });
}

TEST( ParallelInputStream, SequenceDict )
{
    auto seq_dict = std::make_shared<genesis::sequence::SequenceDict>();
    seq_dict->add( "XYZ", 50 );
    seq_dict->add( "ABC", 50 );

    // Test without additional loci.
    // LOG_DBG << "Normal";
    test_parallel_input_stream_all_({}, seq_dict );

    // // Test with different positions and numbers of additional loci.
    // // LOG_DBG << "1";
    // test_parallel_input_stream_all_({ 1 }, seq_dict );
    // // LOG_DBG << "15";
    // test_parallel_input_stream_all_({ 15 }, seq_dict );
    // // LOG_DBG << "16";
    // test_parallel_input_stream_all_({ 16 }, seq_dict );
    // // LOG_DBG << "32";
    // test_parallel_input_stream_all_({ 32 }, seq_dict );
    // // LOG_DBG << "1, 15, 32";
    // test_parallel_input_stream_all_({ 1, 15, 32 }, seq_dict );
    // // LOG_DBG << "15, 32";
    // test_parallel_input_stream_all_({ 15, 32 }, seq_dict );
}

TEST( ParallelInputStream, UnorderedChromosomes )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const s_infile = environment->data_dir + "population/parallel_chrs_1.sync";

    // Init with the desired settings
    auto pit = VariantParallelInputStream();
    pit.add_variant_input_stream(
        make_variant_input_stream_from_sync_file( s_infile ),
        VariantParallelInputStream::ContributionType::kCarrying
    );

    // Get the expected list of positions per chromosome.
    std::unordered_map<std::string, std::set<size_t>> expected_positions = {
        { "XYZ", { 10, 12, 17, 25, 28, 30 }},
        { "ABC", { 10, 12, 17, 25, 28, 30 }}
    };

    // Get the expected order of chromsomes.
    auto seq_dict = std::make_shared<genesis::sequence::SequenceDict>();
    seq_dict->add( "XYZ", 50 );
    seq_dict->add( "ABC", 50 );
    pit.sequence_dict( seq_dict );

    // Execute the test.
    std::unordered_map<std::string, std::set<size_t>> found_positions;
    for( auto it = pit.begin(); it != pit.end(); ++it ) {
        // LOG_DBG2 << it.locus();
        found_positions[ it.locus().chromosome ].insert( it.locus().position );
    }
    EXPECT_EQ( expected_positions, found_positions );
}

// =================================================================================================
//     Randomized Tests
// =================================================================================================

void test_parallel_input_stream_random_() {

    // While building the test case, we keep track of which chr:pos we expect to see.
    std::unordered_map<std::string, std::set<size_t>> expected_chr_pos;

    // Per chromsome, we generate random positions up to a max position (not their count).
    size_t const max_positions = 100;

    // Generate a random number of sources, between 1 and 5 inclusive.
    auto const num_sources = 1 + permuted_congruential_generator() % 5;
    assert( 1 <= num_sources && num_sources <= 5 );
    auto variants = std::vector<std::vector<Variant>>( num_sources );

    // We also randomly decide which ones are carrying (1) and which ones are following (0).
    auto carrying = std::vector<bool>( num_sources );
    size_t carrying_cnt = 0;
    for( size_t si = 0; si < num_sources; ++si ) {
        carrying[si] = permuted_congruential_generator() % 2;
        carrying_cnt += carrying[si];
        // LOG_DBG1 << "Source " << si << " carry " << std::boolalpha << carrying[si];
    }

    // Generate a random number of chromosomes, between 1 and 5 inclusive.
    // We name them A-E, for simplicity.
    auto const num_chroms = 1 + permuted_congruential_generator() % 5;
    std::vector<std::string> chrs;
    for( size_t ci = 0; ci < num_chroms; ++ci ) {
        chrs.push_back( std::string( 1, 'A' + ci ));
    }

    // We randomly decide whether to use a seq dict, or not.
    // When using a seq dict, we do reverse order, just to test that.
    bool const use_seq_dict = permuted_congruential_generator() % 2;
    auto seq_dict = std::make_shared<genesis::sequence::SequenceDict>();
    if( use_seq_dict ) {
        std::reverse( chrs.begin(), chrs.end() );
        for( auto const& chr : chrs ) {
            seq_dict->add( chr, max_positions );
        }
    }

    // Generate data for the sources
    assert( 1 <= num_chroms && num_chroms <= 5 );
    for( size_t ci = 0; ci < num_chroms; ++ci ) {

        // For following sources, we need to make sure to know the positions where they all
        // just happen to have the same position, so that we still count them.
        // We mark all of them as true for now, and then set to false unless there is a following
        // locus. Whatever remains is a position shared by all following sources.
        auto following_pos = std::vector<bool>( max_positions + 1, true );
        following_pos[0] = false;

        for( size_t si = 0; si < num_sources; ++si ) {
            // We randomly decide wheather to add the chromosome to a variant or not,
            // to test absences.
            if( permuted_congruential_generator() % 2 == 0 ) {
                // If a source is following, and skipped, there will be no visited loci on the chr.
                if( ! carrying[si] ) {
                    following_pos = std::vector<bool>( max_positions + 1, false );
                }
                continue;
            }
            // LOG_DBG1 << "Source " << si << " add " << chrs[ci];

            // Randomly (0.5 chance, to maximize coverage of cases) add positions
            for( size_t pos = 1; pos <= max_positions; ++pos ) {
                if( permuted_congruential_generator() % 2 == 0 ) {
                    if( ! carrying[si] ) {
                        following_pos[pos] = false;
                    }
                    continue;
                }

                // Make the variant
                auto var = Variant();
                var.chromosome = chrs[ci];
                var.position = pos;
                variants[si].push_back(var);

                // For carrying sources, we expect the position to be in the output.
                if( carrying[si] ) {
                    expected_chr_pos[chrs[ci]].insert(pos);
                }
            }
        }

        // Now add all psoitions that were not reset
        if( carrying_cnt == 0 ) {
            for( size_t fi = 1; fi < following_pos.size(); ++fi ) {
                if( following_pos[fi] ) {
                    expected_chr_pos[chrs[ci]].insert(fi);
                }
            }
        }
    }

    // Set up the iterator
    auto parallel_it = VariantParallelInputStream();
    for( size_t si = 0; si < num_sources; ++si ) {
        parallel_it.add_variant_input_stream(
            make_variant_input_stream_from_vector( variants[si] ),
            carrying[si]
                ? VariantParallelInputStream::ContributionType::kCarrying
                : VariantParallelInputStream::ContributionType::kFollowing
        );
    }
    if( use_seq_dict ) {
        parallel_it.sequence_dict( seq_dict );
    }

    // Additional loci.
    size_t added_chrs = 0;
    bool have_cleared_following_only_loci = false;
    for( size_t ci = 0; ci < num_chroms; ++ci ) {
        // Currenlty, we cannot have extra carrying loci with a dict,
        // as the ordering of chromsomes gets out of control...
        if( use_seq_dict ) {
            break;
        }

        // Make a random 0.5 selection of chromosomes.
        if( permuted_congruential_generator() % 2 == 0 ) {
            continue;
        }
        ++added_chrs;
        // LOG_DBG1 << "Chr " << chrs[ci] << " add";

        // We add ~10 random positions on the chr.
        for( size_t pos = 1; pos <= max_positions; ++pos ) {
            if( permuted_congruential_generator() % 10 != 0 ) {
                continue;
            }

            // We have a complicated extra condition: if we add additional loci to an iterator
            // that only contains following sources, but no carrying ones, then these additional loci
            // will be the ones visited. So in case of that, we first need to clear all the following
            // loci. But only if we are actually adding at least one additional locus...
            // So here, we are at the point where we are about to do that.
            // We only clear the list on the first chr where we add loci, of course.
            if( carrying_cnt == 0 && ! have_cleared_following_only_loci ) {
                expected_chr_pos.clear();
                have_cleared_following_only_loci = true;
            }

            parallel_it.add_carrying_locus( GenomeLocus( chrs[ci], pos ));
            expected_chr_pos[chrs[ci]].insert(pos);
            // LOG_DBG1 << chrs[ci] << ":" << pos;
        }
    }

    // Now turn this into an exact list of loci that we want to visit.
    std::vector<GenomeLocus> expected_list;
    for( size_t ci = 0; ci < num_chroms; ++ci ) {
        if( expected_chr_pos.count( chrs[ci] ) == 0 ) {
            continue;
        }
        for( auto pos : expected_chr_pos[ chrs[ci] ] ) {
            expected_list.emplace_back( chrs[ci], pos );
            // LOG_DBG1 << "Exp " <<  chrs[ci] << ":" << pos;
        }
    }

    // Reporting
    LOG_DBG << "Test:" << std::boolalpha
            << " num_sources=" << num_sources
            << " num_chroms=" << num_chroms
            << " carrying_cnt=" << carrying_cnt
            << " added_chrs=" << added_chrs
            << " use_seq_dict=" << use_seq_dict
            << " expected_list.size()=" << expected_list.size()
    ;

    // Execute the test.
    size_t it_cnt = 0;
    for( auto it = parallel_it.begin(); it != parallel_it.end(); ++it ) {
        // LOG_DBG2 << it.locus();
        ASSERT_LT( it_cnt, expected_list.size() );
        EXPECT_EQ( expected_list[it_cnt], it.locus() );
        ++it_cnt;
    }
    EXPECT_EQ( it_cnt, expected_list.size() );
}

TEST( ParallelInputStream, Random )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    // 0.5s runtime, our default for normal tests.
    size_t const max_tests = 1000;

    // Run tests while we have time.
    for( size_t test_num = 0; test_num < max_tests; ++test_num ) {
        // LOG_DBG << "Test " << test_num;
        test_parallel_input_stream_random_();
    }
}
