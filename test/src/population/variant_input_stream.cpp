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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_parallel_input_stream.hpp"
#include "genesis/population/filter/variant_filter_numerical.hpp"
#include "genesis/population/filter/variant_filter_positional.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/population/function/variant_input_stream.hpp"
#include "genesis/utils/text/string.hpp"

#include <unordered_set>

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     SAM/BAM/CRAM
// =================================================================================================

#ifdef GENESIS_HTSLIB

TEST( VariantInputStream, SamInputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/ex1.sam.gz";
    auto it = make_variant_input_stream_from_sam_file( infile );
    EXPECT_EQ( "ex1", it.data().source_name );

    // Add a filter that limits it to a region, and then skips a region inside.
    it.add_filter(
        make_variant_filter_by_region_excluding( GenomeRegion( "seq1", 272, 279 ))
    );
    it.add_filter(
        make_variant_filter_by_region_excluding( GenomeRegion( "seq1", 274, 277 ), true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        make_variant_filter_by_region_excluding( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        // LOG_DBG << variant.chromosome << ":" << variant.position;
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 272 273 278 279", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_stream_from_sam_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_stream_from_sam_file( "/path/to/nowhere.sam.gz" ));
}

TEST( VariantInputStream, SamInputStreamSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/ex1.sam.gz";

    // Filter empty. Both samples are there, as this is equivalent to no filtering.
    {
        SamVariantInputStream sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({  });
        auto it = make_variant_input_stream_from_sam_file( infile, sam_it );
        EXPECT_EQ( 2, it.begin()->samples.size() );
    }

    // Filter S1.
    {
        SamVariantInputStream sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({ "S1" });
        auto it = make_variant_input_stream_from_sam_file( infile, sam_it );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter S2.
    {
        SamVariantInputStream sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({ "S2" });
        auto it = make_variant_input_stream_from_sam_file( infile, sam_it );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter invalid.
    {
        SamVariantInputStream sam_it;
        sam_it.split_by_rg( true );
        sam_it.rg_tag_filter({ "XYZ" });
        EXPECT_ANY_THROW( make_variant_input_stream_from_sam_file( infile, sam_it ));
    }
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Pileup
// =================================================================================================

TEST( VariantInputStream, PileupInputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.pileup";
    auto it = make_variant_input_stream_from_pileup_file( infile );
    EXPECT_EQ( "example", it.data().source_name );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "seq1", 274, 277 );
    // it.add_filter(
    //     make_variant_filter_by_region_excluding( region )
    // );
    it.add_filter(
        make_variant_filter_by_region_excluding( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        make_variant_filter_by_region_excluding( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 272 273 278 279", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_stream_from_pileup_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_stream_from_pileup_file( "/path/to/nowhere.pileup" ));

    // Test case for wrong number of filter indices: file only contains one sample, with index zero.
    // So, first works, second does not.
    EXPECT_NO_THROW(
        make_variant_input_stream_from_pileup_file( infile, std::vector<size_t>{ 0 })
    );
    EXPECT_ANY_THROW(
        make_variant_input_stream_from_pileup_file( infile, std::vector<size_t>{ 1 })
    );
}

TEST( VariantInputStream, PileupInputStreamSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example3.pileup";

    // The first sample in the file contains either 1 or 0 bases.
    // The second sample in the file contains either 2 or 0 bases.

    // No samples. This shall result in no filtering.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{}
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 2, variant.samples.size() );
        }
    }

    // Flipped, no samples. This shall result in no filtering.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{}, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 2, variant.samples.size() );
        }
    }

    // Normal, sample 1.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 0 }
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant, SampleCountsFilterPolicy::kAll );
            EXPECT_TRUE( sum == 0 || sum == 1 );
        }
    }

    // Normal, sample 2.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 1 }
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant, SampleCountsFilterPolicy::kAll );
            EXPECT_TRUE( sum == 0 || sum == 2 );
        }
    }

    // Flipped, sample 1.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 1 }, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant, SampleCountsFilterPolicy::kAll );
            EXPECT_TRUE( sum == 0 || sum == 1 );
        }
    }

    // Flipped, sample 2.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 0 }, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 1, variant.samples.size() );
            auto const sum = total_nucleotide_sum( variant, SampleCountsFilterPolicy::kAll );
            EXPECT_TRUE( sum == 0 || sum == 2 );
        }
    }

    // Both samples.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 0, 1 }
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 2, variant.samples.size() );
        }
    }

    // Flipped, both samples. This is a special case, as clearly a sample filter is given,
    // so we take this into account, and so it results in no samples at all.
    {
        auto it = make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 0, 1 }, true
        );
        for( auto const& variant : it ) {
            EXPECT_EQ( 0, variant.samples.size() );
        }
    }

    // Fail due to asking for indices that are larger that the number of samples in the file.
    {
        EXPECT_ANY_THROW( make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 2 }, false
        ));
        EXPECT_ANY_THROW( make_variant_input_stream_from_pileup_file(
            infile, std::vector<size_t>{ 2 }, true
        ));
    }
}

// =================================================================================================
//     Sync
// =================================================================================================

TEST( VariantInputStream, SyncInputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";
    auto it = make_variant_input_stream_from_sync_file( infile );
    EXPECT_EQ( "test", it.data().source_name );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "2R", 2302, 2302 );
    it.add_filter(
        make_variant_filter_by_region_excluding( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        make_variant_filter_by_region_excluding( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 2303 2304 2305", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_stream_from_sync_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_stream_from_sync_file( "/path/to/nowhere.sync" ));
}

TEST( VariantInputStream, SyncInputStreamSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";

    // No samples. This shall result in no filtering.
    {
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{}
        );
        auto beg = it.begin();
        EXPECT_EQ( 2, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
        EXPECT_EQ( 6, beg->samples[1].t_count );
    }

    // Flipped, No samples. This shall result in no filtering.
    {
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{}, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 2, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
        EXPECT_EQ( 6, beg->samples[1].t_count );
    }

    // Normal, sample 1.
    {
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 0 }
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
    }

    // Normal, sample 2.
    {
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 1 }
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 6, beg->samples[0].t_count );
    }

    // Flipped, sample 1.
    {
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 1 }, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 7, beg->samples[0].t_count );
    }

    // Flipped, sample 2.
    {
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 0 }, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 1, beg->samples.size() );
        EXPECT_EQ( 6, beg->samples[0].t_count );
    }

    // Both samples.
    {
        auto it = make_variant_input_stream_from_sync_file(
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
        auto it = make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 0, 1 }, true
        );
        auto beg = it.begin();
        EXPECT_EQ( 0, beg->samples.size() );
    }

    // Fail due to asking for indices that are larger that the number of samples in the file.
    {
        EXPECT_ANY_THROW( make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 2 }, false
        ));
        EXPECT_ANY_THROW( make_variant_input_stream_from_sync_file(
            infile, std::vector<size_t>{ 2 }, true
        ));
    }
}

// =================================================================================================
//     VCF
// =================================================================================================

#ifdef GENESIS_HTSLIB

TEST( VariantInputStream, VcfInputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";
    auto it = make_variant_input_stream_from_pool_vcf_file( infile );
    EXPECT_EQ( "example_ad", it.data().source_name );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "20", 17000, 1120000 );
    it.add_filter(
        make_variant_filter_by_region_excluding( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        make_variant_filter_by_region_excluding( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
        // LOG_DBG << variant.chromosome << ":" << variant.position;
        // for( auto const& s : variant.samples ) {
        //     LOG_DBG1 << s;
        // }
    }
    EXPECT_EQ( " 14370 1230237 1230238", result );

    // Test cases for missing file.
    EXPECT_ANY_THROW( make_variant_input_stream_from_pool_vcf_file( "" ));
    EXPECT_ANY_THROW( make_variant_input_stream_from_pool_vcf_file( "/path/to/nowhere.vcf" ));
}

TEST( VariantInputStream, VcfInputStreamSampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";

    // Filter empty. All samples are there, as this is equivalent to no filtering.
    {
        VariantInputStreamFromVcfParams params;
        params.sample_names = std::vector<std::string>{};
        auto it = make_variant_input_stream_from_pool_vcf_file(
            infile, params
        );
        EXPECT_EQ( 3, it.begin()->samples.size() );
    }

    // Filter empty, inversed. All samples are there, as this is equivalent to no filtering.
    {
        VariantInputStreamFromVcfParams params;
        params.sample_names = std::vector<std::string>{};
        params.inverse_sample_names = true;
        auto it = make_variant_input_stream_from_pool_vcf_file(
            infile, params
        );
        EXPECT_EQ( 3, it.begin()->samples.size() );
    }

    // Filter NA00002.
    {
        VariantInputStreamFromVcfParams params;
        params.sample_names = std::vector<std::string>{ "NA00002" };
        auto it = make_variant_input_stream_from_pool_vcf_file(
            infile, params
        );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter NA00002, inversed. Two samples remain.
    {
        VariantInputStreamFromVcfParams params;
        params.sample_names = std::vector<std::string>{ "NA00002" };
        params.inverse_sample_names = true;
        auto it = make_variant_input_stream_from_pool_vcf_file(
            infile, params
        );
        EXPECT_EQ( 2, it.begin()->samples.size() );
    }

    // Filter invalid.
    {
        VariantInputStreamFromVcfParams params;
        params.sample_names = std::vector<std::string>{ "XYZ" };
        EXPECT_ANY_THROW(
             make_variant_input_stream_from_pool_vcf_file(
                infile, params
            )
        );
    }
    {
        VariantInputStreamFromVcfParams params;
        params.sample_names = std::vector<std::string>{ "XYZ" };
        params.inverse_sample_names = true;
        EXPECT_ANY_THROW(
             make_variant_input_stream_from_pool_vcf_file(
                infile, params
            )
        );
    }
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Parallel Input
// =================================================================================================

TEST( VariantInputStream, ParallelInputStream1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Only those that do not depend on htslib here.
    // See below for a version of this test that does all file types that we currently support.

    // Sync in.
    std::string const snc_infile = environment->data_dir + "population/test.sync";
    auto snc_it = make_variant_input_stream_from_sync_file( snc_infile );

    // Pileup in.
    std::string const plp_infile = environment->data_dir + "population/example.pileup";
    auto plp_it = make_variant_input_stream_from_pileup_file( plp_infile );

    // Make parallel iterator from all source.
    VariantParallelInputStream parallel;
    parallel.add_variant_input_stream(
        snc_it, VariantParallelInputStream::ContributionType::kCarrying
    );
    parallel.add_variant_input_stream(
        plp_it, VariantParallelInputStream::ContributionType::kCarrying
    );

    // Make the iterator.
    auto it = make_variant_input_stream_from_variant_parallel_input_stream( parallel );
    EXPECT_EQ( "", it.data().source_name );

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

TEST( VariantInputStream, ParallelInputStream2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // SAM in.
    std::string const sam_infile = environment->data_dir + "population/ex1.sam.gz";
    auto sam_it = make_variant_input_stream_from_sam_file( sam_infile );

    // Sync in.
    std::string const snc_infile = environment->data_dir + "population/test.sync";
    auto snc_it = make_variant_input_stream_from_sync_file( snc_infile );

    // Pileup in.
    std::string const plp_infile = environment->data_dir + "population/example.pileup";
    auto plp_it = make_variant_input_stream_from_pileup_file( plp_infile );

    // VCF in.
    std::string const vcf_infile = environment->data_dir + "population/example_ad.vcf";
    auto vcf_it = make_variant_input_stream_from_pool_vcf_file( vcf_infile );

    // Make parallel iterator from all source.
    VariantParallelInputStream parallel;
    parallel.add_variant_input_stream(
        sam_it, VariantParallelInputStream::ContributionType::kCarrying
    );
    parallel.add_variant_input_stream(
        snc_it, VariantParallelInputStream::ContributionType::kCarrying
    );
    parallel.add_variant_input_stream(
        plp_it, VariantParallelInputStream::ContributionType::kCarrying
    );
    parallel.add_variant_input_stream(
        vcf_it, VariantParallelInputStream::ContributionType::kCarrying
    );

    // Make the iterator.
    auto it = make_variant_input_stream_from_variant_parallel_input_stream( parallel );

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

// =================================================================================================
//     Unordered Chromosomes
// =================================================================================================

void test_variant_input_stream_unordered_chromosomes_(
    VariantInputStream& it,
    size_t expected_positions,
    bool with_observers,
    bool with_dict,
    bool good_sequence_lengths
) {
    // All files are named the same, for simplicity.
    EXPECT_EQ( "unordered", it.data().source_name );

    // Make a sequence dict, or not.
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict;
    if( with_dict ) {
        sequence_dict = std::make_shared<genesis::sequence::SequenceDict>();
        if( good_sequence_lengths ) {
            sequence_dict->add( "XYZ", 2000 );
            sequence_dict->add( "ABC", 2000 );
        } else {
            sequence_dict->add( "XYZ", 10 );
            sequence_dict->add( "ABC", 10 );
        }
    }

    // Add a check observer to the iterator.
    if( with_observers ) {
        it.add_on_enter_observer(
            make_variant_input_stream_sequence_order_observer( sequence_dict )
        );
        it.add_on_leave_observer(
            make_variant_input_stream_sequence_order_observer( sequence_dict )
        );
    }

    // Just test the number of positions
    size_t cnt = 0;
    for( auto const& variant : it ) {
        (void) variant;
        ++cnt;
    }
    EXPECT_EQ( expected_positions, cnt );
}

#ifdef GENESIS_HTSLIB

TEST( VariantInputStream, UnorderedChromosomesSam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/unordered.sam.gz";

    {
        auto it = make_variant_input_stream_from_sam_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 3136, true, false, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_sam_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 3136, true, true, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_sam_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 3136, true, true, true );
    }
    {
        auto it = make_variant_input_stream_from_sam_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 3136, false, false, false );
    }
}

#endif // GENESIS_HTSLIB

TEST( VariantInputStream, UnorderedChromosomesPileup )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/unordered.pileup";

    {
        auto it = make_variant_input_stream_from_pileup_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 16, true, false, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_pileup_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 16, true, true, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_pileup_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 16, true, true, true );
    }
    {
        auto it = make_variant_input_stream_from_pileup_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 16, false, false, false );
    }
}

TEST( VariantInputStream, UnorderedChromosomesSync )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/unordered.sync";

    {
        auto it = make_variant_input_stream_from_sync_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 12, true, false, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_sync_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 12, true, true, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_sync_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 12, true, true, true );
    }
    {
        auto it = make_variant_input_stream_from_sync_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 12, false, false, false );
    }
}

TEST( VariantInputStream, UnorderedChromosomesFrequencyTable )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/unordered.csv";

    {
        auto it = make_variant_input_stream_from_frequency_table_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 8, true, false, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_frequency_table_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 8, true, true, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_frequency_table_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 8, true, true, true );
    }
    {
        auto it = make_variant_input_stream_from_frequency_table_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 8, false, false, false );
    }
}

#ifdef GENESIS_HTSLIB

TEST( VariantInputStream, UnorderedChromosomesVcf )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/unordered.vcf";

    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 10, true, false, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        EXPECT_ANY_THROW(
            test_variant_input_stream_unordered_chromosomes_( it, 10, true, true, false )
        );
    }
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 10, true, true, true );
    }
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        test_variant_input_stream_unordered_chromosomes_( it, 10, false, false, false );
    }
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Sample Filter
// =================================================================================================

TEST( VariantInputStream, MakeSampleFilter )
{
    auto sample_names = std::vector<std::string>{
        "A", "B", "C", "D", "E", "F", "G", "H"
    };
    auto names_filter = std::vector<std::string>{
        "C", "D", "G"
    };

    auto const f1 = make_sample_name_filter( sample_names, names_filter, false );
    ASSERT_EQ( 8, f1.size() );
    EXPECT_FALSE( f1[0] );
    EXPECT_FALSE( f1[1] );
    EXPECT_TRUE(  f1[2] );
    EXPECT_TRUE(  f1[3] );
    EXPECT_FALSE( f1[4] );
    EXPECT_FALSE( f1[5] );
    EXPECT_TRUE(  f1[6] );
    EXPECT_FALSE( f1[7] );

    auto const f2 = make_sample_name_filter( sample_names, names_filter, true );
    ASSERT_EQ( 8, f2.size() );
    EXPECT_TRUE(  f2[0] );
    EXPECT_TRUE(  f2[1] );
    EXPECT_FALSE( f2[2] );
    EXPECT_FALSE( f2[3] );
    EXPECT_TRUE(  f2[4] );
    EXPECT_TRUE(  f2[5] );
    EXPECT_FALSE( f2[6] );
    EXPECT_TRUE(  f2[7] );

    EXPECT_ANY_THROW( make_sample_name_filter({ "A", "B" }, { "X" }));
    EXPECT_ANY_THROW( make_sample_name_filter({ "A", "A" }, { "A" }));
    EXPECT_ANY_THROW( make_sample_name_filter({ "A", "B" }, { "A", "A" }));
}

#ifdef GENESIS_HTSLIB

TEST( VariantInputStream, SampleFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";

    // Sample names: NA00001 NA00002 NA00003

    // Filter empty. No samples are there.
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        it.add_transform(
            make_variant_input_stream_sample_name_filter_transform(
                make_sample_name_filter( it.data().sample_names, {}, false )
            )
        );
        EXPECT_EQ( 0, it.begin()->samples.size() );
    }

    // Filter empty, inversed. All samples are there, as this is equivalent to no filtering.
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        it.add_transform(
            make_variant_input_stream_sample_name_filter_transform(
                make_sample_name_filter( it.data().sample_names, {}, true )
            )
        );
        EXPECT_EQ( 3, it.begin()->samples.size() );
    }

    // Filter NA00002.
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        it.add_transform(
            make_variant_input_stream_sample_name_filter_transform(
                make_sample_name_filter( it.data().sample_names, { "NA00002" }, false )
            )
        );
        EXPECT_EQ( 1, it.begin()->samples.size() );
    }

    // Filter NA00002, inversed. Two samples remain.
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        it.add_transform(
            make_variant_input_stream_sample_name_filter_transform(
                make_sample_name_filter( it.data().sample_names, { "NA00002" }, true )
            )
        );
        EXPECT_EQ( 2, it.begin()->samples.size() );
    }

    // Filter invalid.
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        EXPECT_ANY_THROW(
            it.add_transform(
                make_variant_input_stream_sample_name_filter_transform(
                    make_sample_name_filter( it.data().sample_names, { "XYZ" }, false )
                )
            );
        );
    }
    {
        auto it = make_variant_input_stream_from_pool_vcf_file( infile );
        EXPECT_ANY_THROW(
            it.add_transform(
                make_variant_input_stream_sample_name_filter_transform(
                    make_sample_name_filter( it.data().sample_names, { "XYZ" }, true )
                )
            );
        );
    }
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Sample Group Merging
// =================================================================================================

TEST( VariantInputStream, SampleGroupMerging )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/sample-names.sync";

    auto sync_it = make_variant_input_stream_from_sync_file( infile );

    // Sample names: sample_1	sample_2	sample_3	sample_4
    auto const group_assignment = std::unordered_map<std::string, std::string>{
        { "sample_1", "group_a" },
        { "sample_2", "group_a" },
        { "sample_3", "group_b" },
        { "sample_4", "group_b" },
        // { "sample_5", "group_c" }
    };

    auto merged_it = make_variant_merging_input_stream( sync_it, group_assignment );
    auto const exp_group_names = std::vector<std::string>{ "group_a", "group_b" };
    EXPECT_EQ( exp_group_names, merged_it.data().sample_names );

    size_t cnt = 0;
    for( auto const& var : merged_it ) {
        ASSERT_EQ( 2, var.samples.size() );
        EXPECT_EQ( 1, var.samples[0].a_count );
        EXPECT_EQ( 2, var.samples[0].t_count );
        EXPECT_EQ( 3, var.samples[1].c_count );
        EXPECT_EQ( 4, var.samples[1].g_count );
        ++cnt;
    }
    EXPECT_EQ( 6, cnt );
}
