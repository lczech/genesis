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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/sam_variant_input_iterator.hpp"
#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/functions/variant.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/text/string.hpp"

#include <cstdlib>

using namespace genesis::population;
using namespace genesis::utils;

#ifdef GENESIS_HTSLIB

void run_sam_bam_cram_test_( std::string const& infile, bool split_by_rg, bool with_unaccounted_rg )
{
    // We just use any file that comes in here, no matter what the format.
    auto sam_it = SamVariantInputIterator( infile );
    size_t exp_smp_size = 1;
    std::vector<std::string> exp_rg_tags;
    sam_it.min_map_qual( 40 );
    if( split_by_rg ) {
        sam_it.split_by_rg( true );
        if( with_unaccounted_rg ) {
            sam_it.with_unaccounted_rg( true );
            exp_smp_size = 3;
            exp_rg_tags = std::vector<std::string>{ "S1", "S2", "unaccounted" };
        } else {
            // With splitting by rg, but without unaccounted, no reads are left,
            // as our test files do not have rg tags... so, we expect no data at all.
            exp_smp_size = 2;
            exp_rg_tags = std::vector<std::string>{ "S1", "S2" };
        }
    }

    // Get start of the iteration, and check rg tags.
    auto it = sam_it.begin();
    auto const rg_tags = it.rg_tags();
    ASSERT_EQ( exp_rg_tags.size(), rg_tags.size() );
    for( size_t i = 0; i < rg_tags.size(); ++i ) {
        EXPECT_EQ( exp_rg_tags[i], rg_tags[i] );
    }

    BaseCounts total_counts;
    auto sample_counts = std::vector<BaseCounts>{ exp_smp_size };
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

    EXPECT_EQ( 39234, total_counts.a_count );
    EXPECT_EQ( 22703, total_counts.c_count );
    EXPECT_EQ( 20181, total_counts.g_count );
    EXPECT_EQ( 30232, total_counts.t_count );
    EXPECT_EQ(    23, total_counts.n_count );
    EXPECT_EQ(     2, total_counts.d_count );

    // Test that the samples were split up by RG properly.
    if( split_by_rg ) {
        EXPECT_EQ( 19381, sample_counts[0].a_count );
        EXPECT_EQ( 11098, sample_counts[0].c_count );
        EXPECT_EQ( 10084, sample_counts[0].g_count );
        EXPECT_EQ( 15104, sample_counts[0].t_count );
        EXPECT_EQ(     3, sample_counts[0].n_count );
        EXPECT_EQ(     1, sample_counts[0].d_count );

        EXPECT_EQ( 19853, sample_counts[1].a_count );
        EXPECT_EQ( 11605, sample_counts[1].c_count );
        EXPECT_EQ( 10097, sample_counts[1].g_count );
        EXPECT_EQ( 15128, sample_counts[1].t_count );
        EXPECT_EQ(    20, sample_counts[1].n_count );
        EXPECT_EQ(     1, sample_counts[1].d_count );

        // The unaccounted is empty, as all reads have proper RG tags in our test files.
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

TEST( SamBamCram, InputIteratorSam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.sam.gz", true, true );
}

TEST( SamBamCram, InputIteratorBam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", true, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.bam", true, true );
}

// Cannot use cram at the moment, as it stores the path to the ref fasta/fai files as absolute
// paths, and is hence not portable... see https://github.com/samtools/htslib/issues/1401

// Revision: We can use MD5 hashed reference sequences, and need to set the env path
// for those to be found. See genesis/test/data/population/README.txt for creating the MD5 files.

TEST( SamBamCram, InputIteratorCram )
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

    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", false, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", true, false );
    run_sam_bam_cram_test_( environment->data_dir + "population/ex1.cram", true, true );
}

#endif // GENESIS_HTSLIB
