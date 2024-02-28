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

#include "genesis/population/formats/frequency_table_input_stream.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/population/streams/variant_input_stream.hpp"

#include <cmath>
#include <limits>

using namespace genesis::population;
using namespace genesis::utils;

TEST( FrequencyTableInputStream, Read )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/freq1.csv";

    auto const exp_names = std::vector<std::string>{ "Smp1", "SmpA", "SmpC", "smp2" };
    std::vector<Variant> variants;

    // auto reader = FrequencyTableInputStream( from_file( infile ));
    // auto beg = reader.begin();
    // auto end = reader.end();
    //
    // for( ; beg != end; ++beg ) {
    //     variants.emplace_back( *beg );
    // }
    // EXPECT_EQ( exp_names, beg.sample_names() );

    // Test custom header field names.
    auto reader = FrequencyTableInputStream();
    reader.header_chromosome_string( "chr" );
    reader.header_position_string( "pos" );
    reader.header_reference_base_string( "ref" );
    reader.header_alternative_base_string( "alt" );

    // Our example table contains different types of header field styles,
    // so we cannot cover all of them here.
    // reader.header_sample_reference_count_substring( "refcnt" );
    // reader.header_sample_alternative_count_substring( "altcount" );
    // reader.header_sample_frequency_substring( ".freq" );
    // reader.header_sample_coverage_substring( "coverage-" );
    auto it = make_variant_input_stream_from_frequency_table_file( infile, '\t', reader );
    for( auto const& var : it ) {
        variants.emplace_back( var );
    }

    EXPECT_EQ( infile,    it.data().file_path );
    EXPECT_EQ( "freq1",   it.data().source_name );
    EXPECT_EQ( exp_names, it.data().sample_names );
    ASSERT_EQ( 2, variants.size() );

    // First line
    EXPECT_EQ( "1", variants[0].chromosome );
    EXPECT_EQ(   1, variants[0].position );
    EXPECT_EQ( 'T', variants[0].reference_base );
    EXPECT_EQ( 'C', variants[0].alternative_base );
    ASSERT_EQ(   4, variants[0].samples.size() );

    // First line, first sample
    EXPECT_EQ(                0, variants[0].samples[0].a_count );
    EXPECT_EQ( 5404319552844595, variants[0].samples[0].c_count );
    EXPECT_EQ(                0, variants[0].samples[0].g_count );
    EXPECT_EQ( 3602879701896397, variants[0].samples[0].t_count );
    EXPECT_EQ(                0, variants[0].samples[0].n_count );
    EXPECT_EQ(                0, variants[0].samples[0].d_count );

    // First line, second sample
    EXPECT_EQ(  0, variants[0].samples[1].a_count );
    EXPECT_EQ(  4, variants[0].samples[1].c_count );
    EXPECT_EQ(  0, variants[0].samples[1].g_count );
    EXPECT_EQ( 16, variants[0].samples[1].t_count );
    EXPECT_EQ(  0, variants[0].samples[1].n_count );
    EXPECT_EQ(  0, variants[0].samples[1].d_count );

    // First line, third sample
    EXPECT_EQ(  0, variants[0].samples[2].a_count );
    EXPECT_EQ(  5, variants[0].samples[2].c_count );
    EXPECT_EQ(  0, variants[0].samples[2].g_count );
    EXPECT_EQ(  5, variants[0].samples[2].t_count );
    EXPECT_EQ(  0, variants[0].samples[2].n_count );
    EXPECT_EQ(  0, variants[0].samples[2].d_count );

    // First line, fourth sample
    EXPECT_EQ(  0, variants[0].samples[3].a_count );
    EXPECT_EQ(  4, variants[0].samples[3].c_count );
    EXPECT_EQ(  0, variants[0].samples[3].g_count );
    EXPECT_EQ(  8, variants[0].samples[3].t_count );
    EXPECT_EQ(  0, variants[0].samples[3].n_count );
    EXPECT_EQ(  0, variants[0].samples[3].d_count );

    // Second line
    EXPECT_EQ( "1", variants[1].chromosome );
    EXPECT_EQ(   5, variants[1].position );
    EXPECT_EQ( 'C', variants[1].reference_base );
    EXPECT_EQ( 'G', variants[1].alternative_base );
    ASSERT_EQ(   4, variants[1].samples.size() );

    // Second line, first sample
    EXPECT_EQ(                0, variants[1].samples[0].a_count );
    EXPECT_EQ( 4503599627370496, variants[1].samples[0].c_count );
    EXPECT_EQ( 4503599627370496, variants[1].samples[0].g_count );
    EXPECT_EQ(                0, variants[1].samples[0].t_count );
    EXPECT_EQ(                0, variants[1].samples[0].n_count );
    EXPECT_EQ(                0, variants[1].samples[0].d_count );

    // Second line, second sample
    EXPECT_EQ(  0, variants[1].samples[1].a_count );
    EXPECT_EQ(  3, variants[1].samples[1].c_count );
    EXPECT_EQ( 12, variants[1].samples[1].g_count );
    EXPECT_EQ(  0, variants[1].samples[1].t_count );
    EXPECT_EQ(  0, variants[1].samples[1].n_count );
    EXPECT_EQ(  0, variants[1].samples[1].d_count );

    // Second line, third sample
    EXPECT_EQ(  0, variants[1].samples[2].a_count );
    EXPECT_EQ(  8, variants[1].samples[2].c_count );
    EXPECT_EQ(  4, variants[1].samples[2].g_count );
    EXPECT_EQ(  0, variants[1].samples[2].t_count );
    EXPECT_EQ(  0, variants[1].samples[2].n_count );
    EXPECT_EQ(  0, variants[1].samples[2].d_count );

    // Second line, fourth sample
    EXPECT_EQ(  0, variants[1].samples[3].a_count );
    EXPECT_EQ(  5, variants[1].samples[3].c_count );
    EXPECT_EQ( 10, variants[1].samples[3].g_count );
    EXPECT_EQ(  0, variants[1].samples[3].t_count );
    EXPECT_EQ(  0, variants[1].samples[3].n_count );
    EXPECT_EQ(  0, variants[1].samples[3].d_count );
}
