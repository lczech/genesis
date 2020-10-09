/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/functions/pool_sample.hpp"
#include "genesis/population/pool_sample.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Pileup, SimpleReader )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.pileup";

    auto reader = SimplePileupReader();
    auto records = reader.read( from_file( infile ));

    std::vector<char> ref_bases = { 'T', 'T', 'T', 'A', 'G', 'T', 'G', 'C' };

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

    EXPECT_EQ(  0,  records[0].samples[0].a_count );
    EXPECT_EQ(  0,  records[0].samples[0].c_count );
    EXPECT_EQ(  0,  records[0].samples[0].g_count );
    EXPECT_EQ( 24,  records[0].samples[0].t_count );
    EXPECT_EQ(  0,  records[0].samples[0].n_count );
    EXPECT_EQ(  0,  records[0].samples[0].d_count );
    EXPECT_EQ( 24,  records[0].samples[0].read_coverage );
    auto const pool_0 = convert_to_pool_sample( records[0].samples[0] );
    EXPECT_EQ( 24,  nucleotide_sum( pool_0 ));
    EXPECT_TRUE(    status( pool_0 ).is_covered );
    EXPECT_FALSE(   status( pool_0 ).is_snp );
    EXPECT_FALSE(   status( pool_0 ).is_biallelic );
    EXPECT_FALSE(   status( pool_0 ).is_ignored );
    EXPECT_EQ( 'T', consensus( pool_0, status( pool_0 )).first );
    EXPECT_FLOAT_EQ( 1.0, consensus( pool_0, status( pool_0 )).second );

    EXPECT_EQ(  1,  records[1].samples[0].a_count );
    EXPECT_EQ(  0,  records[1].samples[0].c_count );
    EXPECT_EQ(  0,  records[1].samples[0].g_count );
    EXPECT_EQ( 20,  records[1].samples[0].t_count );
    EXPECT_EQ(  2,  records[1].samples[0].n_count );
    EXPECT_EQ(  0,  records[1].samples[0].d_count );
    EXPECT_EQ( 23,  records[1].samples[0].read_coverage );
    auto const pool_1 = convert_to_pool_sample( records[1].samples[0] );
    EXPECT_EQ( 21,  nucleotide_sum( pool_1 ));
    EXPECT_TRUE(    status( pool_1 ).is_covered );
    EXPECT_TRUE(    status( pool_1 ).is_snp );
    EXPECT_TRUE(    status( pool_1 ).is_biallelic );
    EXPECT_FALSE(   status( pool_1 ).is_ignored );
    EXPECT_EQ( 'T', consensus( pool_1, status( pool_1 )).first );
    EXPECT_FLOAT_EQ( 0.952380952, consensus( pool_1, status( pool_1 )).second );

    EXPECT_EQ(  0,  records[2].samples[0].a_count );
    EXPECT_EQ(  0,  records[2].samples[0].c_count );
    EXPECT_EQ(  0,  records[2].samples[0].g_count );
    EXPECT_EQ( 21,  records[2].samples[0].t_count );
    EXPECT_EQ(  0,  records[2].samples[0].n_count );
    EXPECT_EQ(  2,  records[2].samples[0].d_count );
    EXPECT_EQ( 23,  records[2].samples[0].read_coverage );
    auto const pool_2 = convert_to_pool_sample( records[2].samples[0] );
    EXPECT_EQ( 21,  nucleotide_sum( pool_2 ));
    EXPECT_FALSE(   status( pool_2 ).is_covered );
    EXPECT_FALSE(   status( pool_2 ).is_snp );
    EXPECT_FALSE(   status( pool_2 ).is_biallelic );
    EXPECT_TRUE(    status( pool_2 ).is_ignored );
    EXPECT_EQ( 'N', consensus( pool_2, status( pool_2 )).first );
    EXPECT_FLOAT_EQ( 0.0, consensus( pool_2, status( pool_2 )).second );

    EXPECT_EQ( 23,  records[3].samples[0].a_count );
    EXPECT_EQ(  0,  records[3].samples[0].c_count );
    EXPECT_EQ(  0,  records[3].samples[0].g_count );
    EXPECT_EQ(  0,  records[3].samples[0].t_count );
    EXPECT_EQ(  0,  records[3].samples[0].n_count );
    EXPECT_EQ(  0,  records[3].samples[0].d_count );
    EXPECT_EQ( 23,  records[3].samples[0].read_coverage );
    auto const pool_3 = convert_to_pool_sample( records[3].samples[0] );
    EXPECT_EQ( 23,  nucleotide_sum( pool_3 ));
    EXPECT_TRUE(    status( pool_3 ).is_covered );
    EXPECT_FALSE(   status( pool_3 ).is_snp );
    EXPECT_FALSE(   status( pool_3 ).is_biallelic );
    EXPECT_FALSE(   status( pool_3 ).is_ignored );
    EXPECT_EQ( 'A', consensus( pool_3, status( pool_3 )).first );
    EXPECT_FLOAT_EQ( 1.0, consensus( pool_3, status( pool_3 )).second );

    EXPECT_EQ(  0,  records[4].samples[0].a_count );
    EXPECT_EQ(  0,  records[4].samples[0].c_count );
    EXPECT_EQ( 21,  records[4].samples[0].g_count );
    EXPECT_EQ(  1,  records[4].samples[0].t_count );
    EXPECT_EQ(  0,  records[4].samples[0].n_count );
    EXPECT_EQ(  0,  records[4].samples[0].d_count );
    EXPECT_EQ( 22,  records[4].samples[0].read_coverage );
    auto const pool_4 = convert_to_pool_sample( records[4].samples[0] );
    EXPECT_EQ( 22,  nucleotide_sum( pool_4 ));
    EXPECT_TRUE(    status( pool_4 ).is_covered );
    EXPECT_TRUE(    status( pool_4 ).is_snp );
    EXPECT_TRUE(    status( pool_4 ).is_biallelic );
    EXPECT_FALSE(   status( pool_4 ).is_ignored );
    EXPECT_EQ( 'G', consensus( pool_4, status( pool_4 )).first );
    EXPECT_FLOAT_EQ( 0.954545455, consensus( pool_4, status( pool_4 )).second );

    EXPECT_EQ(  0,  records[5].samples[0].a_count );
    EXPECT_EQ(  1,  records[5].samples[0].c_count );
    EXPECT_EQ(  1,  records[5].samples[0].g_count );
    EXPECT_EQ( 20,  records[5].samples[0].t_count );
    EXPECT_EQ(  0,  records[5].samples[0].n_count );
    EXPECT_EQ(  0,  records[5].samples[0].d_count );
    EXPECT_EQ( 22,  records[5].samples[0].read_coverage );
    auto const pool_5 = convert_to_pool_sample( records[5].samples[0] );
    EXPECT_EQ( 22,  nucleotide_sum( pool_5 ));
    EXPECT_TRUE(    status( pool_5 ).is_covered );
    EXPECT_TRUE(    status( pool_5 ).is_snp );
    EXPECT_FALSE(   status( pool_5 ).is_biallelic );
    EXPECT_FALSE(   status( pool_5 ).is_ignored );
    EXPECT_EQ( 'T', consensus( pool_5, status( pool_5 )).first );
    EXPECT_FLOAT_EQ( 0.909090909, consensus( pool_5, status( pool_5 )).second );

    EXPECT_EQ(  0,  records[6].samples[0].a_count );
    EXPECT_EQ(  0,  records[6].samples[0].c_count );
    EXPECT_EQ( 23,  records[6].samples[0].g_count );
    EXPECT_EQ(  0,  records[6].samples[0].t_count );
    EXPECT_EQ(  0,  records[6].samples[0].n_count );
    EXPECT_EQ(  0,  records[6].samples[0].d_count );
    EXPECT_EQ( 23,  records[6].samples[0].read_coverage );
    auto const pool_6 = convert_to_pool_sample( records[6].samples[0] );
    EXPECT_EQ( 23,  nucleotide_sum( pool_6 ));
    EXPECT_TRUE(    status( pool_6 ).is_covered );
    EXPECT_FALSE(   status( pool_6 ).is_snp );
    EXPECT_FALSE(   status( pool_6 ).is_biallelic );
    EXPECT_FALSE(   status( pool_6 ).is_ignored );
    EXPECT_EQ( 'G', consensus( pool_6, status( pool_6 )).first );
    EXPECT_FLOAT_EQ( 1.0, consensus( pool_6, status( pool_6 )).second );

    EXPECT_EQ(  1,  records[7].samples[0].a_count );
    EXPECT_EQ( 17,  records[7].samples[0].c_count );
    EXPECT_EQ(  0,  records[7].samples[0].g_count );
    EXPECT_EQ(  1,  records[7].samples[0].t_count );
    EXPECT_EQ(  0,  records[7].samples[0].n_count );
    EXPECT_EQ(  0,  records[7].samples[0].d_count );
    EXPECT_EQ( 23,  records[7].samples[0].read_coverage );
    auto const pool_7 = convert_to_pool_sample( records[7].samples[0] );
    EXPECT_EQ( 19,  nucleotide_sum( pool_7 ));
    EXPECT_TRUE(    status( pool_7 ).is_covered );
    EXPECT_TRUE(    status( pool_7 ).is_snp );
    EXPECT_FALSE(   status( pool_7 ).is_biallelic );
    EXPECT_FALSE(   status( pool_7 ).is_ignored );
    EXPECT_EQ( 'C', consensus( pool_7, status( pool_7 )).first );
    EXPECT_FLOAT_EQ( 0.894736842, consensus( pool_7, status( pool_7 )).second );
}
