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
#include "genesis/population/format/simple_pileup_common.hpp"
#include "genesis/population/format/simple_pileup_reader.hpp"
#include "genesis/population/filter/sample_counts_filter_numerical.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter_numerical.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/sequence/functions/quality.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Pileup, SimpleReader1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.pileup";

    auto reader = SimplePileupReader();
    auto records = reader.read_records( from_file( infile ));

    std::vector<char> ref_bases = { 'T', 'T', 'T', 'A', 'G', 'T', 'G', 'C' };
    SampleCountsFilterNumericalParams filter;
    filter.only_snps = true;
    filter.only_biallelic_snps = true;
    SampleCountsFilterStats stats;

    ASSERT_EQ( 8, records.size() );
    for( size_t i = 0; i < records.size(); ++i ) {
        EXPECT_EQ( "seq1", records[i].chromosome );
        EXPECT_EQ( 272 + i, records[i].position );
        EXPECT_EQ( ref_bases[i], records[i].reference_base );

        ASSERT_EQ( 1, records[i].samples.size() );

        // LOG_DBG << i;
        // LOG_DBG1 << records[i].samples[0].read_bases;
        // LOG_DBG1 << join( records[i].samples[0].phred_scores );
    }

    EXPECT_EQ( "tTTTTTTttTtTtTTTtttTtTTT", records[0].samples[0].read_bases );
    EXPECT_EQ( "NNTTTTttTtTtTTTtttTtTTA",  records[1].samples[0].read_bases );
    EXPECT_EQ( "tTTT**ttTtTtTTTtttTtTTT",  records[2].samples[0].read_bases );
    EXPECT_EQ( "aAAAAaaAaAaAAAaaaAaAAAA",  records[3].samples[0].read_bases );
    EXPECT_EQ( "GGGTggGgGgGGGgggGgGGGG",   records[4].samples[0].read_bases );
    EXPECT_EQ( "TTTTttTtTtTCTtttTtTTGT",   records[5].samples[0].read_bases );
    EXPECT_EQ( "GGGGggGgGgGGGgggGgGGGGG",  records[6].samples[0].read_bases );
    EXPECT_EQ( "ACCTccCcC<><>cccCcCCCCC",  records[7].samples[0].read_bases );

    EXPECT_EQ(
        std::vector<unsigned char>({
            27, 27, 27, 10, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 27, 26, 27, 26, 22, 27, 5
        }),
        records[0].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            27, 27, 27, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 18, 27, 28, 27, 27, 27, 26, 27, 27, 10
        }),
        records[1].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            22, 27, 22, 26, 27, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 27, 26, 27, 26, 27, 27, 21
        }),
        records[2].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            27, 10, 26, 24, 9, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 27, 27, 25, 26, 27, 27, 27, 27
        }),
        records[3].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            18, 18, 26, 10, 27, 27, 22, 28, 22, 27, 27, 22, 27, 5, 27, 27, 16, 26, 27, 27, 21, 27
        }),
        records[4].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            10, 22, 27, 26, 27, 27, 27, 27, 27, 27, 27, 5, 27, 28, 27, 27, 25, 26, 27, 27, 5, 27
        }),
        records[5].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            4, 18, 23, 9, 27, 27, 26, 27, 22, 27, 27, 22, 27, 28, 27, 27, 27, 26, 27, 27, 27, 27, 27
        }),
        records[6].samples[0].phred_scores
    );
    EXPECT_EQ(
        std::vector<unsigned char>({
            26, 22, 20, 5, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 27, 27, 27, 24, 27, 27, 25, 27, 27
        }),
        records[7].samples[0].phred_scores
    );

    // Record 0, Sample 0
    EXPECT_EQ( 24,  records[0].samples[0].read_depth );
    auto pool_0 = convert_to_sample_counts( records[0].samples[0] );
    EXPECT_EQ(  0,  pool_0.a_count );
    EXPECT_EQ(  0,  pool_0.c_count );
    EXPECT_EQ(  0,  pool_0.g_count );
    EXPECT_EQ( 24,  pool_0.t_count );
    EXPECT_EQ(  0,  pool_0.n_count );
    EXPECT_EQ(  0,  pool_0.d_count );
    EXPECT_EQ( 24,  nucleotide_sum( pool_0 ));
    EXPECT_EQ( 'T', consensus( pool_0 ).first );
    EXPECT_FLOAT_EQ( 1.0, consensus( pool_0 ).second );
    EXPECT_FALSE( apply_sample_counts_filter_numerical( pool_0, filter, stats ));
    EXPECT_EQ( 1, stats[SampleCountsFilterTag::kNotSnp] );
    // EXPECT_TRUE(    status( pool_0 ).is_covered );
    // EXPECT_FALSE(   status( pool_0 ).is_snp );
    // EXPECT_FALSE(   status( pool_0 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_0 ).is_ignored );
    stats.clear();

    // Record 1, Sample 0
    EXPECT_EQ( 23,  records[1].samples[0].read_depth );
    auto pool_1 = convert_to_sample_counts( records[1].samples[0] );
    EXPECT_EQ(  1,  pool_1.a_count );
    EXPECT_EQ(  0,  pool_1.c_count );
    EXPECT_EQ(  0,  pool_1.g_count );
    EXPECT_EQ( 20,  pool_1.t_count );
    EXPECT_EQ(  2,  pool_1.n_count );
    EXPECT_EQ(  0,  pool_1.d_count );
    EXPECT_EQ( 21,  nucleotide_sum( pool_1 ));
    EXPECT_EQ( 'T', consensus( pool_1 ).first );
    EXPECT_FLOAT_EQ( 0.952380952, consensus( pool_1 ).second );
    EXPECT_TRUE( apply_sample_counts_filter_numerical( pool_1, filter, stats ));

    // We cannot directy test the count of passed any more, as we never increment the counter
    // in the filters. So instead we just test that there is no failed filter.
    // EXPECT_EQ( 1, stats[SampleCountsFilterTag::kPassed] );
    EXPECT_EQ( 0, stats.sum() );

    // EXPECT_TRUE(    status( pool_1 ).is_covered );
    // EXPECT_TRUE(    status( pool_1 ).is_snp );
    // EXPECT_TRUE(    status( pool_1 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_1 ).is_ignored );
    stats.clear();

    // Record 2, Sample 0
    EXPECT_EQ( 23,  records[2].samples[0].read_depth );
    auto pool_2 = convert_to_sample_counts( records[2].samples[0] );
    EXPECT_EQ(  0,  pool_2.a_count );
    EXPECT_EQ(  0,  pool_2.c_count );
    EXPECT_EQ(  0,  pool_2.g_count );
    EXPECT_EQ( 21,  pool_2.t_count );
    EXPECT_EQ(  0,  pool_2.n_count );
    EXPECT_EQ(  2,  pool_2.d_count );
    EXPECT_EQ( 21,  nucleotide_sum( pool_2 ));
    EXPECT_EQ( 'N', consensus( pool_2, false ).first );
    EXPECT_FLOAT_EQ( 0.0, consensus( pool_2, false ).second );
    EXPECT_FALSE( apply_sample_counts_filter_numerical( pool_2, filter, stats ));
    EXPECT_EQ( 0, stats[SampleCountsFilterTag::kAboveDeletionsCountLimit] );
    // EXPECT_FALSE(   status( pool_2 ).is_covered );
    // EXPECT_FALSE(   status( pool_2 ).is_snp );
    // EXPECT_FALSE(   status( pool_2 ).is_biallelic );
    // EXPECT_TRUE(    status( pool_2 ).is_ignored );
    stats.clear();

    // Record 3, Sample 0
    EXPECT_EQ( 23,  records[3].samples[0].read_depth );
    auto pool_3 = convert_to_sample_counts( records[3].samples[0] );
    EXPECT_EQ( 23,  pool_3.a_count );
    EXPECT_EQ(  0,  pool_3.c_count );
    EXPECT_EQ(  0,  pool_3.g_count );
    EXPECT_EQ(  0,  pool_3.t_count );
    EXPECT_EQ(  0,  pool_3.n_count );
    EXPECT_EQ(  0,  pool_3.d_count );
    EXPECT_EQ( 23,  nucleotide_sum( pool_3 ));
    EXPECT_EQ( 'A', consensus( pool_3 ).first );
    EXPECT_FLOAT_EQ( 1.0, consensus( pool_3 ).second );
    EXPECT_FALSE( apply_sample_counts_filter_numerical( pool_3, filter, stats ));
    EXPECT_EQ( 1, stats[SampleCountsFilterTag::kNotSnp] );
    // EXPECT_TRUE(    status( pool_3 ).is_covered );
    // EXPECT_FALSE(   status( pool_3 ).is_snp );
    // EXPECT_FALSE(   status( pool_3 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_3 ).is_ignored );
    stats.clear();

    // Record 4, Sample 0
    EXPECT_EQ( 22,  records[4].samples[0].read_depth );
    auto pool_4 = convert_to_sample_counts( records[4].samples[0] );
    EXPECT_EQ(  0,  pool_4.a_count );
    EXPECT_EQ(  0,  pool_4.c_count );
    EXPECT_EQ( 21,  pool_4.g_count );
    EXPECT_EQ(  1,  pool_4.t_count );
    EXPECT_EQ(  0,  pool_4.n_count );
    EXPECT_EQ(  0,  pool_4.d_count );
    EXPECT_EQ( 22,  nucleotide_sum( pool_4 ));
    EXPECT_EQ( 'G', consensus( pool_4 ).first );
    EXPECT_FLOAT_EQ( 0.954545455, consensus( pool_4 ).second );
    EXPECT_TRUE( apply_sample_counts_filter_numerical( pool_4, filter, stats ));

    // We cannot directy test the count of passed any more, as we never increment the counter
    // in the filters. So instead we just test that there is no failed filter.
    // EXPECT_EQ( 1, stats[SampleCountsFilterTag::kPassed] );
    EXPECT_EQ( 0, stats.sum() );

    // EXPECT_TRUE(    status( pool_4 ).is_covered );
    // EXPECT_TRUE(    status( pool_4 ).is_snp );
    // EXPECT_TRUE(    status( pool_4 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_4 ).is_ignored );
    stats.clear();

    // Record 5, Sample 0
    EXPECT_EQ( 22,  records[5].samples[0].read_depth );
    auto pool_5 = convert_to_sample_counts( records[5].samples[0] );
    EXPECT_EQ(  0,  pool_5.a_count );
    EXPECT_EQ(  1,  pool_5.c_count );
    EXPECT_EQ(  1,  pool_5.g_count );
    EXPECT_EQ( 20,  pool_5.t_count );
    EXPECT_EQ(  0,  pool_5.n_count );
    EXPECT_EQ(  0,  pool_5.d_count );
    EXPECT_EQ( 22,  nucleotide_sum( pool_5 ));
    EXPECT_EQ( 'T', consensus( pool_5 ).first );
    EXPECT_FLOAT_EQ( 0.909090909, consensus( pool_5 ).second );
    EXPECT_FALSE( apply_sample_counts_filter_numerical( pool_5, filter, stats ));
    EXPECT_EQ( 1, stats[SampleCountsFilterTag::kNotBiallelicSnp] );
    // EXPECT_TRUE(    status( pool_5 ).is_covered );
    // EXPECT_TRUE(    status( pool_5 ).is_snp );
    // EXPECT_FALSE(   status( pool_5 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_5 ).is_ignored );
    stats.clear();

    // Record 6, Sample 0
    EXPECT_EQ( 23,  records[6].samples[0].read_depth );
    auto pool_6 = convert_to_sample_counts( records[6].samples[0] );
    EXPECT_EQ(  0,  pool_6.a_count );
    EXPECT_EQ(  0,  pool_6.c_count );
    EXPECT_EQ( 23,  pool_6.g_count );
    EXPECT_EQ(  0,  pool_6.t_count );
    EXPECT_EQ(  0,  pool_6.n_count );
    EXPECT_EQ(  0,  pool_6.d_count );
    EXPECT_EQ( 23,  nucleotide_sum( pool_6 ));
    EXPECT_EQ( 'G', consensus( pool_6 ).first );
    EXPECT_FLOAT_EQ( 1.0, consensus( pool_6 ).second );
    EXPECT_FALSE( apply_sample_counts_filter_numerical( pool_6, filter, stats ));
    EXPECT_EQ( 1, stats[SampleCountsFilterTag::kNotSnp] );
    // EXPECT_TRUE(    status( pool_6 ).is_covered );
    // EXPECT_FALSE(   status( pool_6 ).is_snp );
    // EXPECT_FALSE(   status( pool_6 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_6 ).is_ignored );
    stats.clear();

    // Record 7, Sample 0
    EXPECT_EQ( 23,  records[7].samples[0].read_depth );
    auto pool_7 = convert_to_sample_counts( records[7].samples[0] );
    EXPECT_EQ(  1,  pool_7.a_count );
    EXPECT_EQ( 17,  pool_7.c_count );
    EXPECT_EQ(  0,  pool_7.g_count );
    EXPECT_EQ(  1,  pool_7.t_count );
    EXPECT_EQ(  0,  pool_7.n_count );
    EXPECT_EQ(  0,  pool_7.d_count );
    EXPECT_EQ( 19,  nucleotide_sum( pool_7 ));
    EXPECT_EQ( 'C', consensus( pool_7 ).first );
    EXPECT_FLOAT_EQ( 0.894736842, consensus( pool_7 ).second );
    EXPECT_FALSE( apply_sample_counts_filter_numerical( pool_7, filter, stats ));
    EXPECT_EQ( 1, stats[SampleCountsFilterTag::kNotBiallelicSnp] );
    // EXPECT_TRUE(    status( pool_7 ).is_covered );
    // EXPECT_TRUE(    status( pool_7 ).is_snp );
    // EXPECT_FALSE(   status( pool_7 ).is_biallelic );
    // EXPECT_FALSE(   status( pool_7 ).is_ignored );
    stats.clear();
}

TEST( Pileup, SimpleReader2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example2.pileup";

    auto reader = SimplePileupReader();
    auto records = reader.read_records( from_file( infile ));

    std::vector<char> ref_bases = { 'T', 'C', 'C', 'T', 'T', 'T', 'C', 'A', 'A', 'A' };

    ASSERT_EQ( 10, records.size() );
    for( size_t i = 0; i < records.size(); ++i ) {
        EXPECT_EQ( "1", records[i].chromosome );
        EXPECT_EQ( 18149 + i, records[i].position );
        EXPECT_EQ( ref_bases[i], records[i].reference_base );
        ASSERT_EQ( 1, records[i].samples.size() );
    }

    EXPECT_EQ( "T", records[0].samples[0].read_bases );
    EXPECT_EQ( "C", records[1].samples[0].read_bases );
    EXPECT_EQ( "C", records[2].samples[0].read_bases );
    EXPECT_EQ( "T", records[3].samples[0].read_bases );
    EXPECT_EQ( "",  records[4].samples[0].read_bases );
    EXPECT_EQ( "T", records[5].samples[0].read_bases );
    EXPECT_EQ( "C", records[6].samples[0].read_bases );
    EXPECT_EQ( "A", records[7].samples[0].read_bases );
    EXPECT_EQ( "A", records[8].samples[0].read_bases );
    EXPECT_EQ( "A", records[9].samples[0].read_bases );
}

TEST( Pileup, SimpleReader3 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example3.pileup";

    // Read with filtering for only the second sample
    auto reader = SimplePileupReader();
    auto records = reader.read_records( from_file( infile ), std::vector<bool>{ false, true });

    std::vector<char> ref_bases = { 'T', 'C', 'C', 'T', 'T', 'T', 'C', 'A', 'A', 'A' };

    ASSERT_EQ( 10, records.size() );
    for( size_t i = 0; i < records.size(); ++i ) {
        EXPECT_EQ( "1", records[i].chromosome );
        EXPECT_EQ( 18149 + i, records[i].position );
        EXPECT_EQ( ref_bases[i], records[i].reference_base );
        ASSERT_EQ( 1, records[i].samples.size() );
    }

    EXPECT_EQ( "TT", records[0].samples[0].read_bases );
    EXPECT_EQ( "CC", records[1].samples[0].read_bases );
    EXPECT_EQ( "CC", records[2].samples[0].read_bases );
    EXPECT_EQ( "TT", records[3].samples[0].read_bases );
    EXPECT_EQ( "",  records[4].samples[0].read_bases );
    EXPECT_EQ( "TT", records[5].samples[0].read_bases );
    EXPECT_EQ( "CC", records[6].samples[0].read_bases );
    EXPECT_EQ( "AA", records[7].samples[0].read_bases );
    EXPECT_EQ( "AA", records[8].samples[0].read_bases );
    EXPECT_EQ( "AA", records[9].samples[0].read_bases );
}

TEST( Pileup, SimpleReader4 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example3.pileup";

    // Fail with wrong number of filter values.
    auto reader = SimplePileupReader();
    EXPECT_ANY_THROW(
        reader.read_records( from_file( infile ), std::vector<bool>{ false, true, false })
    );
}

TEST( Pileup, QualityEncoding )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    using namespace genesis::sequence;

    {
        std::string const infile = environment->data_dir + "population/example.pileup";
        auto const enc = guess_pileup_quality_encoding( from_file( infile ));
        EXPECT_TRUE( compatible_quality_encodings(
            QualityEncoding::kSanger,
            enc
        )) << quality_encoding_name( enc );
    }
    {
        std::string const infile = environment->data_dir + "population/example2.pileup";
        auto const enc = guess_pileup_quality_encoding( from_file( infile ));
        EXPECT_TRUE( compatible_quality_encodings(
            QualityEncoding::kIllumina13,
            enc
        )) << quality_encoding_name( enc );
    }
    {
        std::string const infile = environment->data_dir + "population/example3.pileup";
        auto const enc = guess_pileup_quality_encoding( from_file( infile ));
        EXPECT_TRUE( compatible_quality_encodings(
            QualityEncoding::kIllumina13,
            enc
        )) << quality_encoding_name( enc );
    }
}
