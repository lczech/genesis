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
#include "genesis/population/format/simple_pileup_reader.hpp"
#include "genesis/population/filter/sample_counts_filter_numerical.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter_numerical.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Pileup, VariantReader )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.pileup";

    auto reader = SimplePileupReader();
    auto variants = reader.read_variants( from_file( infile ));

    std::vector<char> ref_bases = { 'T', 'T', 'T', 'A', 'G', 'T', 'G', 'C' };
    SampleCountsFilterNumericalParams filter;
    filter.only_snps = true;
    filter.only_biallelic_snps = true;
    SampleCountsFilterStats stats;

    ASSERT_EQ( 8, variants.size() );
    for( size_t i = 0; i < variants.size(); ++i ) {
        EXPECT_EQ( "seq1", variants[i].chromosome );
        EXPECT_EQ( 272 + i, variants[i].position );
        EXPECT_EQ( ref_bases[i], variants[i].reference_base );

        ASSERT_EQ( 1, variants[i].samples.size() );
    }

    // Record 0, Sample 0
    auto& pool_0 = variants[0].samples[0];
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
    auto& pool_1 = variants[1].samples[0];
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
    auto& pool_2 = variants[2].samples[0];
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
    auto& pool_3 = variants[3].samples[0];
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
    auto& pool_4 = variants[4].samples[0];
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
    auto& pool_5 = variants[5].samples[0];
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
    auto& pool_6 = variants[6].samples[0];
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
    auto& pool_7 = variants[7].samples[0];
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
