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

#ifdef GENESIS_HTSLIB

#include "src/common.hpp"

#include "genesis/population/variant.hpp"
#include "genesis/population/functions/variant.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Variant, ConvertFromVcfRecord )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";

    // Load the VCF data and prepare the record
    auto file = HtsFile( infile );
    auto header = VcfHeader( file );
    auto record = VcfRecord( header );

    // Read all lines/records
    std::vector<Variant> samples;
    while( record.read_next( file )) {
        // Typically, we want to ensure record.is_snp() as well (see commented condition below).
        // Here however, we also do a test that allows for "." deletions in the VCF file.
        // That is a bit uncommon, but works for this test. In normal applications, use the
        // condition below instead.
        if( ! record.has_format( "AD" )) {
            continue;
        }
        // if( ! record.is_snp() || ! record.has_format( "AD" )) {
        //     continue;
        // }
        samples.push_back( convert_to_variant( record ));
    }
    ASSERT_EQ( 4, samples.size() );

    // Line 1

    EXPECT_EQ( "20",  samples[0].chromosome );
    EXPECT_EQ( 14370, samples[0].position );
    EXPECT_EQ( 'G',   samples[0].reference_base );
    ASSERT_EQ( 3,     samples[0].samples.size() );

    EXPECT_EQ( 8, samples[0].samples[0].a_count );
    EXPECT_EQ( 0, samples[0].samples[0].c_count );
    EXPECT_EQ( 5, samples[0].samples[0].g_count );
    EXPECT_EQ( 0, samples[0].samples[0].t_count );
    EXPECT_EQ( 0, samples[0].samples[0].n_count );
    EXPECT_EQ( 0, samples[0].samples[0].d_count );

    EXPECT_EQ( 5, samples[0].samples[1].a_count );
    EXPECT_EQ( 0, samples[0].samples[1].c_count );
    EXPECT_EQ( 8, samples[0].samples[1].g_count );
    EXPECT_EQ( 0, samples[0].samples[1].t_count );
    EXPECT_EQ( 0, samples[0].samples[1].n_count );
    EXPECT_EQ( 0, samples[0].samples[1].d_count );

    EXPECT_EQ( 0, samples[0].samples[2].a_count );
    EXPECT_EQ( 0, samples[0].samples[2].c_count );
    EXPECT_EQ( 0, samples[0].samples[2].g_count );
    EXPECT_EQ( 0, samples[0].samples[2].t_count );
    EXPECT_EQ( 0, samples[0].samples[2].n_count );
    EXPECT_EQ( 0, samples[0].samples[2].d_count );

    // Line 2

    EXPECT_EQ( "20",  samples[1].chromosome );
    EXPECT_EQ( 17330, samples[1].position );
    EXPECT_EQ( 'T',   samples[1].reference_base );
    ASSERT_EQ( 3,     samples[1].samples.size() );

    EXPECT_EQ( 2, samples[1].samples[0].a_count );
    EXPECT_EQ( 0, samples[1].samples[0].c_count );
    EXPECT_EQ( 0, samples[1].samples[0].g_count );
    EXPECT_EQ( 1, samples[1].samples[0].t_count );
    EXPECT_EQ( 0, samples[1].samples[0].n_count );
    EXPECT_EQ( 0, samples[1].samples[0].d_count );

    EXPECT_EQ( 4, samples[1].samples[1].a_count );
    EXPECT_EQ( 0, samples[1].samples[1].c_count );
    EXPECT_EQ( 0, samples[1].samples[1].g_count );
    EXPECT_EQ( 3, samples[1].samples[1].t_count );
    EXPECT_EQ( 0, samples[1].samples[1].n_count );
    EXPECT_EQ( 0, samples[1].samples[1].d_count );

    EXPECT_EQ( 6, samples[1].samples[2].a_count );
    EXPECT_EQ( 0, samples[1].samples[2].c_count );
    EXPECT_EQ( 0, samples[1].samples[2].g_count );
    EXPECT_EQ( 5, samples[1].samples[2].t_count );
    EXPECT_EQ( 0, samples[1].samples[2].n_count );
    EXPECT_EQ( 0, samples[1].samples[2].d_count );

    // Line 3

    EXPECT_EQ( "20",    samples[2].chromosome );
    EXPECT_EQ( 1110696, samples[2].position );
    EXPECT_EQ( 'A',     samples[2].reference_base );
    ASSERT_EQ( 3,       samples[2].samples.size() );

    EXPECT_EQ( 0, samples[2].samples[0].a_count );
    EXPECT_EQ( 0, samples[2].samples[0].c_count );
    EXPECT_EQ( 1, samples[2].samples[0].g_count );
    EXPECT_EQ( 2, samples[2].samples[0].t_count );
    EXPECT_EQ( 0, samples[2].samples[0].n_count );
    EXPECT_EQ( 0, samples[2].samples[0].d_count );

    EXPECT_EQ( 3, samples[2].samples[1].a_count );
    EXPECT_EQ( 0, samples[2].samples[1].c_count );
    EXPECT_EQ( 4, samples[2].samples[1].g_count );
    EXPECT_EQ( 5, samples[2].samples[1].t_count );
    EXPECT_EQ( 0, samples[2].samples[1].n_count );
    EXPECT_EQ( 0, samples[2].samples[1].d_count );

    EXPECT_EQ( 6, samples[2].samples[2].a_count );
    EXPECT_EQ( 0, samples[2].samples[2].c_count );
    EXPECT_EQ( 7, samples[2].samples[2].g_count );
    EXPECT_EQ( 8, samples[2].samples[2].t_count );
    EXPECT_EQ( 0, samples[2].samples[2].n_count );
    EXPECT_EQ( 0, samples[2].samples[2].d_count );

    // Line 4

    EXPECT_EQ( "20",    samples[3].chromosome );
    EXPECT_EQ( 1230237, samples[3].position );
    EXPECT_EQ( 'T',     samples[3].reference_base );
    ASSERT_EQ( 3,       samples[3].samples.size() );

    EXPECT_EQ( 0, samples[3].samples[0].a_count );
    EXPECT_EQ( 0, samples[3].samples[0].c_count );
    EXPECT_EQ( 0, samples[3].samples[0].g_count );
    EXPECT_EQ( 5, samples[3].samples[0].t_count );
    EXPECT_EQ( 0, samples[3].samples[0].n_count );
    EXPECT_EQ( 4, samples[3].samples[0].d_count );

    EXPECT_EQ( 0, samples[3].samples[1].a_count );
    EXPECT_EQ( 0, samples[3].samples[1].c_count );
    EXPECT_EQ( 0, samples[3].samples[1].g_count );
    EXPECT_EQ( 3, samples[3].samples[1].t_count );
    EXPECT_EQ( 0, samples[3].samples[1].n_count );
    EXPECT_EQ( 2, samples[3].samples[1].d_count );

    EXPECT_EQ( 0, samples[3].samples[2].a_count );
    EXPECT_EQ( 0, samples[3].samples[2].c_count );
    EXPECT_EQ( 0, samples[3].samples[2].g_count );
    EXPECT_EQ( 1, samples[3].samples[2].t_count );
    EXPECT_EQ( 0, samples[3].samples[2].n_count );
    EXPECT_EQ( 0, samples[3].samples[2].d_count );
}

#endif // htslib guard
