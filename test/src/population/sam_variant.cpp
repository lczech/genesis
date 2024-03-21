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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/format/sam_flags.hpp"
#include "genesis/population/format/sam_variant_input_stream.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/text/string.hpp"

#include <cstdlib>

using namespace genesis::population;
using namespace genesis::utils;

#ifdef GENESIS_HTSLIB

void run_sam_bam_cram_test_(
    std::string const& infile,
    bool split_by_rg,
    bool with_unaccounted_rg,
    bool filter_only_s2,
    bool filter_high_map_qual = false
) {
    // We just use any file that comes in here, no matter what the format.
    auto sam_it = SamVariantInputStream( infile );
    size_t exp_smp_size = 1;
    std::vector<std::string> exp_rg_tags;
    sam_it.min_map_qual( filter_high_map_qual ? 200 : 40 );
    if( split_by_rg ) {
        sam_it.split_by_rg( true );
        if( with_unaccounted_rg ) {
            sam_it.with_unaccounted_rg( true );
            if( filter_only_s2 ) {
                sam_it.rg_tag_filter({ "S2" });
                exp_smp_size = 2;
                exp_rg_tags = std::vector<std::string>{ "S2", "unaccounted" };
            } else {
                exp_smp_size = 3;
                exp_rg_tags = std::vector<std::string>{ "S1", "S2", "unaccounted" };
            }
        } else {
            if( filter_only_s2 ) {
                sam_it.rg_tag_filter({ "S2" });
                exp_smp_size = 1;
                exp_rg_tags = std::vector<std::string>{ "S2" };
            } else {
                exp_smp_size = 2;
                exp_rg_tags = std::vector<std::string>{ "S1", "S2" };
            }
        }
    }

    // Get start of the iteration, and check rg tags.
    auto it = sam_it.begin();
    auto const rg_tags = it.rg_tags();
    ASSERT_EQ( exp_rg_tags.size(), rg_tags.size() );
    for( size_t i = 0; i < rg_tags.size(); ++i ) {
        EXPECT_EQ( exp_rg_tags[i], rg_tags[i] );
    }

    // Check that the all function on rg headers also works.
    std::vector<std::string> exp_all_rg_tags{ "S1", "S2" };
    auto const all_rg_tags = it.rg_tags( true );
    ASSERT_EQ( exp_all_rg_tags.size(), all_rg_tags.size() );
    for( size_t i = 0; i < all_rg_tags.size(); ++i ) {
        EXPECT_EQ( exp_all_rg_tags[i], all_rg_tags[i] );
    }

    SampleCounts total_counts;
    auto sample_counts = std::vector<SampleCounts>{ exp_smp_size };
    for( ; it != sam_it.end(); ++it ) {
        auto const& var = *it;

        // Our test files do not have read groups... so, we expect the variants to have only
        // the unaccounted rg base counts object.
        EXPECT_EQ( exp_smp_size, var.samples.size() );

        // std::cout << var.chromosome << "\t" << var.position << "\t";
        for( size_t i = 0; i < var.samples.size(); ++i ) {
            auto const& bs = var.samples[i];

            // std::cout << "\t";
            // to_sync( bs, std::cout );
            merge_inplace( total_counts, bs );
            merge_inplace( sample_counts[i], bs );
        }
        // std::cout << "\n";
    }

    if( filter_high_map_qual ) {
        // Special case: we have filterout out all reads, nothing remains.
        // Mainly a sanity check, to see how the iterator reacts when there is
        // nothing reamining: does it still terminate correctly?
        EXPECT_EQ( 0, total_counts.a_count );
        EXPECT_EQ( 0, total_counts.c_count );
        EXPECT_EQ( 0, total_counts.g_count );
        EXPECT_EQ( 0, total_counts.t_count );
        EXPECT_EQ( 0, total_counts.n_count );
        EXPECT_EQ( 0, total_counts.d_count );
        return;
    }

    // Test total counts, i.e., sum of all reads.
    if( filter_only_s2 ) {
        ASSERT_TRUE( split_by_rg );

        // Only counts of S2 here.
        EXPECT_EQ( 19853, total_counts.a_count );
        EXPECT_EQ( 11605, total_counts.c_count );
        EXPECT_EQ( 10097, total_counts.g_count );
        EXPECT_EQ( 15128, total_counts.t_count );
        EXPECT_EQ(    20, total_counts.n_count );
        EXPECT_EQ(     1, total_counts.d_count );
    } else{
        // Sum of S1 and S2.
        EXPECT_EQ( 39234, total_counts.a_count );
        EXPECT_EQ( 22703, total_counts.c_count );
        EXPECT_EQ( 20181, total_counts.g_count );
        EXPECT_EQ( 30232, total_counts.t_count );
        EXPECT_EQ(    23, total_counts.n_count );
        EXPECT_EQ(     2, total_counts.d_count );
    }

    // Test that the samples were split up by RG properly.
    if( split_by_rg ) {
        if( filter_only_s2 ) {

            // With only S2, these are the counts of S2 itself.
            EXPECT_EQ( 19853, sample_counts[0].a_count );
            EXPECT_EQ( 11605, sample_counts[0].c_count );
            EXPECT_EQ( 10097, sample_counts[0].g_count );
            EXPECT_EQ( 15128, sample_counts[0].t_count );
            EXPECT_EQ(    20, sample_counts[0].n_count );
            EXPECT_EQ(     1, sample_counts[0].d_count );

            // With unaccounted, we have an additonal sample, but it is empty,
            // as all reads have proper RG tags in our test files.
            if( sample_counts.size() == 2 ) {
                EXPECT_EQ( 0, sample_counts[1].a_count );
                EXPECT_EQ( 0, sample_counts[1].c_count );
                EXPECT_EQ( 0, sample_counts[1].g_count );
                EXPECT_EQ( 0, sample_counts[1].t_count );
                EXPECT_EQ( 0, sample_counts[1].n_count );
                EXPECT_EQ( 0, sample_counts[1].d_count );
            }

        } else {

            // Without filtering, we have S1 counts here...
            EXPECT_EQ( 19381, sample_counts[0].a_count );
            EXPECT_EQ( 11098, sample_counts[0].c_count );
            EXPECT_EQ( 10084, sample_counts[0].g_count );
            EXPECT_EQ( 15104, sample_counts[0].t_count );
            EXPECT_EQ(     3, sample_counts[0].n_count );
            EXPECT_EQ(     1, sample_counts[0].d_count );

            // ... and S2 counts here.
            EXPECT_EQ( 19853, sample_counts[1].a_count );
            EXPECT_EQ( 11605, sample_counts[1].c_count );
            EXPECT_EQ( 10097, sample_counts[1].g_count );
            EXPECT_EQ( 15128, sample_counts[1].t_count );
            EXPECT_EQ(    20, sample_counts[1].n_count );
            EXPECT_EQ(     1, sample_counts[1].d_count );

            // With unaccounted, we have an additonal sample, but it is empty,
            // as all reads have proper RG tags in our test files.
            if( sample_counts.size() == 3 ) {
                EXPECT_EQ( 0, sample_counts[2].a_count );
                EXPECT_EQ( 0, sample_counts[2].c_count );
                EXPECT_EQ( 0, sample_counts[2].g_count );
                EXPECT_EQ( 0, sample_counts[2].t_count );
                EXPECT_EQ( 0, sample_counts[2].n_count );
                EXPECT_EQ( 0, sample_counts[2].d_count );
            }
        }
    }
}

TEST( SamBamCram, InputStreamSam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // We run all test combintations that are possible.
    // First, no splitting by RG read group tag, then with splitting.
    // With splitting, we then try all combinations of using unaccounted, and sample filtering.
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", false, false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, true, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, false, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, true, true );

    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", false, false, false, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, false, false, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, true, false, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, false, true, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, true, true, true );
}

TEST( SamBamCram, InputStreamBam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Same as InputStreamSam, see there.
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", false, false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", true, false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", true, true, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", true, false, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", true, true, true );
}

// Cannot use cram at the moment, as it stores the path to the ref fasta/fai files as absolute
// paths, and is hence not portable... see https://github.com/samtools/htslib/issues/1401

// Revision: We can use MD5 hashed reference sequences, and need to set the env path
// for those to be found. See genesis/test/data/population/README.txt for creating the MD5 files.

TEST( SamBamCram, InputStreamCram )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Set the env path so that the MD5-hashed reference sequences for the cram file can be found.
    // See https://stackoverflow.com/a/61806157/4184258 for details on setting the env var.
    // See genesis/test/data/population/README.txt for how we created the cram cache files.

    // Get the absolute path to the cram cache dir, and use it for setting up the env string.
    std::string const env_name  = "REF_PATH";
    std::string const env_value = real_path( environment->data_dir + "/population/cram_cache/" );
    setenv( env_name.c_str(), env_value.c_str(), true );

    // Same as InputStreamSam, see there.
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", false, false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", true, false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", true, true, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", true, false, true );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", true, true, true );
}

TEST( SamBamCram, InputStreamRGFail )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    auto const infile = environment->data_dir + "population/ex1.sam.gz";

    // We just use any file that comes in here, no matter what the format.
    auto sam_it = SamVariantInputStream( infile );
    sam_it.split_by_rg( true );

    // Use an RG tag that does not appear in the file.
    sam_it.rg_tag_filter({ "XYZ" });
    EXPECT_ANY_THROW( sam_it.begin() );
}

TEST( SamBamCram, Flags )
{
    // Number parsing from hex
    EXPECT_EQ( 0x0,  string_to_sam_flag( "0" ));
    EXPECT_EQ( 0x1,  string_to_sam_flag( "1" ));
    EXPECT_EQ( 0x12, string_to_sam_flag( "0x12" ));
    EXPECT_EQ( 0x1234, string_to_sam_flag( "0x1234" ));

    // Number parsing from oct
    EXPECT_EQ( 01,  string_to_sam_flag( "1" ));
    EXPECT_EQ( 012, string_to_sam_flag( "012" ));
    EXPECT_EQ( 01234, string_to_sam_flag( "01234" ));

    // htslib style parsing
    EXPECT_EQ( 0x2, string_to_sam_flag( "PROPER_PAIR" ));
    EXPECT_EQ( 0x2 | 0x20, string_to_sam_flag( "PROPER_PAIR,MREVERSE" ));

    // our improved parsing
    EXPECT_EQ( 0x2, string_to_sam_flag( "ProperPair" ));
    EXPECT_EQ( 0x2 | 1, string_to_sam_flag( "ProperPair | 1" ));
    EXPECT_EQ( 0x2 | 0x20, string_to_sam_flag( "ProperPair + MateReverse" ));
    EXPECT_EQ( 0x2 | 0x20, string_to_sam_flag( "ProperPair | MateReverse" ));
    EXPECT_EQ( 0x2 | 0x20, string_to_sam_flag( "ProperPair | 0x20" ));

    // error cases
    // EXPECT_ANY_THROW( string_to_sam_flag( "0x1234" ));
    EXPECT_ANY_THROW( string_to_sam_flag( "nope" ));
    EXPECT_ANY_THROW( string_to_sam_flag( "ProperPair, nope" ));

    // other direction
    EXPECT_EQ( "", sam_flag_to_string( 0 ));
    EXPECT_EQ( "PROPER_PAIR,UNMAP", sam_flag_to_string( 0x2 | 0x4 ));
}

#endif // GENESIS_HTSLIB
