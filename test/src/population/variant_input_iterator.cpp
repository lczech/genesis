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
#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/formats/variant_parallel_input_iterator.hpp"
#include "genesis/population/functions/filter_transform.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/text/string.hpp"

#include <unordered_set>

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     SAM/BAM/CRAM
// =================================================================================================

#ifdef GENESIS_HTSLIB

TEST( VariantInputIterator, SamInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/ex1.sam.gz";
    auto it = make_variant_input_iterator_from_sam_file( infile );

    // Add a filter that limits it to a region, and then skips a region inside.
    it.add_filter(
        filter_by_region( GenomeRegion( "seq1", 272, 279 ))
    );
    it.add_filter(
        filter_by_region( GenomeRegion( "seq1", 274, 277 ), true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        // LOG_DBG << variant.chromosome << ":" << variant.position;
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 272 273 278 279", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_iterator_from_sam_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_iterator_from_sam_file( "/path/to/nowhere.sam.gz" ));
}

TEST( VariantInputIterator, SamInputIteratorSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/ex1.sam.gz";

    // Filter empty. Both samples are there, as this is equivalent to no filtering.
    {
        SamVariantInputIterator sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({  });
        auto it = make_variant_input_iterator_from_sam_file( infile, sam_it );
        EXPECT_EQ( 2, it.begin()->samples.size() );
    }

    // Filter S1.
    {
        SamVariantInputIterator sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({ "S1" });
        auto it = make_variant_input_iterator_from_sam_file( infile, sam_it );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter S2.
    {
        SamVariantInputIterator sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({ "S2" });
        auto it = make_variant_input_iterator_from_sam_file( infile, sam_it );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter invalid.
    {
        SamVariantInputIterator sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({ "XYZ" });
        EXPECT_ANY_THROW( make_variant_input_iterator_from_sam_file( infile, sam_it ));
    }
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Pileup
// =================================================================================================

TEST( VariantInputIterator, PileupInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.pileup";
    auto it = make_variant_input_iterator_from_pileup_file( infile );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "seq1", 274, 277 );
    // it.add_filter(
    //     filter_by_region( region )
    // );
    it.add_filter(
        filter_by_region( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 272 273 278 279", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_iterator_from_pileup_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_iterator_from_pileup_file( "/path/to/nowhere.pileup" ));

    // Test case for wrong number of filter indices: file only contains one sample, with index zero.
    // So, first works, second does not.
    EXPECT_NO_THROW(
        make_variant_input_iterator_from_pileup_file( infile, std::vector<size_t>{ 0 })
    );
    EXPECT_ANY_THROW(
        make_variant_input_iterator_from_pileup_file( infile, std::vector<size_t>{ 1 })
    );
}

TEST( VariantInputIterator, PileupInputIteratorSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example3.pileup";

    // The first sample in the file contains either 1 or 0 bases.
    // The second sample in the file contains either 2 or 0 bases.

    // No samples. This shall result in no filtering.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{}
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 2, variant.samples.size() );
        }
    }

    // Flipped, no samples. This shall result in no filtering.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{}, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 2, variant.samples.size() );
        }
    }

    // Normal, sample 1.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 0 }
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant );
            EXPECT_TRUE( sum == 0 || sum == 1 );
        }
    }

    // Normal, sample 2.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 1 }
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant );
            EXPECT_TRUE( sum == 0 || sum == 2 );
        }
    }

    // Flipped, sample 1.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 1 }, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant );
            EXPECT_TRUE( sum == 0 || sum == 1 );
        }
    }

    // Flipped, sample 2.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 0 }, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant );
            EXPECT_TRUE( sum == 0 || sum == 2 );
        }
    }

    // Both samples.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 0, 1 }
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 2, variant.samples.size() );
        }
    }

    // Flipped, both samples. This is a special case, as clearly a sample filter is given,
    // so we take this into account, and so it results in no samples at all.
    {
        auto it = make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 0, 1 }, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 0, variant.samples.size() );
        }
    }

    // Fail due to asking for indices that are larger that the number of samples in the file.
    {
        EXPECT_ANY_THROW( make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 2 }, false
        ));
        EXPECT_ANY_THROW( make_variant_input_iterator_from_pileup_file(
            infile, std::vector<size_t>{ 2 }, true
        ));
    }
}

// =================================================================================================
//     Sync
// =================================================================================================

TEST( VariantInputIterator, SyncInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";
    auto it = make_variant_input_iterator_from_sync_file( infile );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "2R", 2302, 2302 );
    it.add_filter(
        filter_by_region( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 2303 2304 2305", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_iterator_from_sync_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_iterator_from_sync_file( "/path/to/nowhere.sync" ));
}

TEST( VariantInputIterator, SyncInputIteratorSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";

    // No samples. This shall result in no filtering.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{}
        );
        auto beg = it.begin();
        EXPECT_EQ( 2, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
        EXPECT_EQ( 6, beg->samples[1].t_count );
    }

    // Flipped, No samples. This shall result in no filtering.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{}, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 2, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
        EXPECT_EQ( 6, beg->samples[1].t_count );
    }

    // Normal, sample 1.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 0 }
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
    }

    // Normal, sample 2.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 1 }
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 6, beg->samples[0].t_count );
    }

    // Flipped, sample 1.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 1 }, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
    }

    // Flipped, sample 2.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 0 }, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 6, beg->samples[0].t_count );
    }

    // Both samples.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 0, 1 }
        );
        auto beg = it.begin();
        EXPECT_EQ( 2, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
        EXPECT_EQ( 6, beg->samples[1].t_count );
    }

    // Flipped, both samples. This is a special case, as clearly a sample filter is given,
    // so we take this into account, and so it results in no samples at all.
    {
        auto it = make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 0, 1 }, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 0, beg->samples.size() );
    }

    // Fail due to asking for indices that are larger that the number of samples in the file.
    {
        EXPECT_ANY_THROW( make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 2 }, false
        ));
        EXPECT_ANY_THROW( make_variant_input_iterator_from_sync_file(
            infile, std::vector<size_t>{ 2 }, true
        ));
    }
}

// =================================================================================================
//     VCF
// =================================================================================================

#ifdef GENESIS_HTSLIB

TEST( VariantInputIterator, VcfInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";
    auto it = make_variant_input_iterator_from_pool_vcf_file( infile, false, false );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "20", 17000, 1120000 );
    it.add_filter(
        filter_by_region( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 14370 1230237", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_iterator_from_pool_vcf_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_iterator_from_pool_vcf_file( "/path/to/nowhere.vcf" ));
}


TEST( VariantInputIterator, VcfInputIteratorSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";

    // Filter empty. All samples are there, as this is equivalent to no filtering.
    {
        auto it = make_variant_input_iterator_from_pool_vcf_file(
            infile, std::vector<std::string>{}
        );
        EXPECT_EQ( 3, it.begin()->samples.size() );
    }

    // Filter empty, inversed. All samples are there, as this is equivalent to no filtering.
    {
        auto it = make_variant_input_iterator_from_pool_vcf_file(
            infile, std::vector<std::string>{}, true
        );
        EXPECT_EQ( 3, it.begin()->samples.size() );
    }

    // Filter NA00002.
    {
        auto it = make_variant_input_iterator_from_pool_vcf_file(
            infile, std::vector<std::string>{ "NA00002" }
        );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter NA00002, inversed. Two samples remain.
    {
        auto it = make_variant_input_iterator_from_pool_vcf_file(
            infile, std::vector<std::string>{ "NA00002" }, true
        );
        EXPECT_EQ( 2, it.begin()->samples.size() );
    }

    // Filter invalid.
    {
        EXPECT_ANY_THROW(
             make_variant_input_iterator_from_pool_vcf_file(
                infile, std::vector<std::string>{ "XYZ" }
            )
        );
        EXPECT_ANY_THROW(
             make_variant_input_iterator_from_pool_vcf_file(
                infile, std::vector<std::string>{ "XYZ" }, true
            )
        );
    }
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Parallel Input
// =================================================================================================

TEST( VariantInputIterator, ParallelInputIterator1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Only those that do not depend on htslib here.
    // See below for a version of this test that does all file types that we currently support.

    // Sync in.
    std::string const snc_infile = environment->data_dir + "population/test.sync";
    auto snc_it = make_variant_input_iterator_from_sync_file( snc_infile );

    // Pileup in.
    std::string const plp_infile = environment->data_dir + "population/example.pileup";
    auto plp_it = make_variant_input_iterator_from_pileup_file( plp_infile );

    // Make parallel iterator from all source.
    VariantParallelInputIterator parallel;
    parallel.add_variant_input_iterator(
        snc_it, VariantParallelInputIterator::ContributionType::kCarrying
    );
    parallel.add_variant_input_iterator(
        plp_it, VariantParallelInputIterator::ContributionType::kCarrying
    );

    // Make the iterator.
    auto it = make_variant_input_iterator_from_variant_parallel_input_iterator( parallel );

    // We expect to find all chromosomes that appear in all of the input files.
    std::unordered_set<std::string> exp_chromosomes = {
        "2R", "seq1"
    };

    // Simple test that the correct region is filtered out.
    std::unordered_set<std::string> tst_chromosomes;
    for( auto const& variant : it ) {
        tst_chromosomes.insert( variant.chromosome );
        // LOG_DBG << variant.chromosome << ":" << variant.position;
    }
    EXPECT_EQ( exp_chromosomes, tst_chromosomes );
}

#ifdef GENESIS_HTSLIB

TEST( VariantInputIterator, ParallelInputIterator2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // SAM in.
    std::string const sam_infile = environment->data_dir + "population/ex1.sam.gz";
    auto sam_it = make_variant_input_iterator_from_sam_file( sam_infile );

    // Sync in.
    std::string const snc_infile = environment->data_dir + "population/test.sync";
    auto snc_it = make_variant_input_iterator_from_sync_file( snc_infile );

    // Pileup in.
    std::string const plp_infile = environment->data_dir + "population/example.pileup";
    auto plp_it = make_variant_input_iterator_from_pileup_file( plp_infile );

    // VCF in.
    std::string const vcf_infile = environment->data_dir + "population/example_ad.vcf";
    auto vcf_it = make_variant_input_iterator_from_pool_vcf_file( vcf_infile, false, false );

    // Make parallel iterator from all source.
    VariantParallelInputIterator parallel;
    parallel.add_variant_input_iterator(
        sam_it, VariantParallelInputIterator::ContributionType::kCarrying
    );
    parallel.add_variant_input_iterator(
        snc_it, VariantParallelInputIterator::ContributionType::kCarrying
    );
    parallel.add_variant_input_iterator(
        plp_it, VariantParallelInputIterator::ContributionType::kCarrying
    );
    parallel.add_variant_input_iterator(
        vcf_it, VariantParallelInputIterator::ContributionType::kCarrying
    );

    // Make the iterator.
    auto it = make_variant_input_iterator_from_variant_parallel_input_iterator( parallel );

    // We expect to find all chromosomes that appear in all of the input files.
    std::unordered_set<std::string> exp_chromosomes = {
        "20", "2R", "seq1", "seq2"
    };

    // Simple test that the correct region is filtered out.
    std::unordered_set<std::string> tst_chromosomes;
    for( auto const& variant : it ) {
        tst_chromosomes.insert( variant.chromosome );
        // LOG_DBG << variant.chromosome << ":" << variant.position;
    }
    EXPECT_EQ( exp_chromosomes, tst_chromosomes );
}

#endif // GENESIS_HTSLIB
