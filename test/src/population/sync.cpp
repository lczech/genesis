/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include "genesis/population/formats/sync_input_iterator.hpp"
#include "genesis/population/formats/sync_reader.hpp"
#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/variant.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Sync, SyncReader )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";

    // The allele frequencies are stored in the order `A:T:C:G:N:del`

    auto reader = SyncReader();
    auto data = reader.read( from_file( infile ));
    ASSERT_EQ( 4, data.size() );

    // Line 1

    EXPECT_EQ( "2R", data[0].chromosome );
    EXPECT_EQ( 2302, data[0].position );
    EXPECT_EQ(  'N', data[0].reference_base );
    ASSERT_EQ(    2, data[0].samples.size() );

    EXPECT_EQ( 0, data[0].samples[0].a_count );
    EXPECT_EQ( 7, data[0].samples[0].t_count );
    EXPECT_EQ( 0, data[0].samples[0].c_count );
    EXPECT_EQ( 0, data[0].samples[0].g_count );
    EXPECT_EQ( 0, data[0].samples[0].n_count );
    EXPECT_EQ( 0, data[0].samples[0].d_count );

    EXPECT_EQ( 0, data[0].samples[1].a_count );
    EXPECT_EQ( 7, data[0].samples[1].t_count );
    EXPECT_EQ( 0, data[0].samples[1].c_count );
    EXPECT_EQ( 0, data[0].samples[1].g_count );
    EXPECT_EQ( 0, data[0].samples[1].n_count );
    EXPECT_EQ( 0, data[0].samples[1].d_count );

    // Line 2

    EXPECT_EQ( "2R", data[1].chromosome );
    EXPECT_EQ( 2303, data[1].position );
    EXPECT_EQ(  'N', data[1].reference_base );
    ASSERT_EQ(    2, data[1].samples.size() );

    EXPECT_EQ( 0, data[1].samples[0].a_count );
    EXPECT_EQ( 8, data[1].samples[0].t_count );
    EXPECT_EQ( 0, data[1].samples[0].c_count );
    EXPECT_EQ( 0, data[1].samples[0].g_count );
    EXPECT_EQ( 0, data[1].samples[0].n_count );
    EXPECT_EQ( 0, data[1].samples[0].d_count );

    EXPECT_EQ( 0, data[1].samples[1].a_count );
    EXPECT_EQ( 8, data[1].samples[1].t_count );
    EXPECT_EQ( 0, data[1].samples[1].c_count );
    EXPECT_EQ( 0, data[1].samples[1].g_count );
    EXPECT_EQ( 0, data[1].samples[1].n_count );
    EXPECT_EQ( 0, data[1].samples[1].d_count );

    // Line 3

    EXPECT_EQ( "2R", data[2].chromosome );
    EXPECT_EQ( 2304, data[2].position );
    EXPECT_EQ(  'N', data[2].reference_base );
    ASSERT_EQ(    2, data[2].samples.size() );

    EXPECT_EQ( 0, data[2].samples[0].a_count );
    EXPECT_EQ( 0, data[2].samples[0].t_count );
    EXPECT_EQ( 9, data[2].samples[0].c_count );
    EXPECT_EQ( 0, data[2].samples[0].g_count );
    EXPECT_EQ( 0, data[2].samples[0].n_count );
    EXPECT_EQ( 0, data[2].samples[0].d_count );

    EXPECT_EQ( 0, data[2].samples[1].a_count );
    EXPECT_EQ( 0, data[2].samples[1].t_count );
    EXPECT_EQ( 9, data[2].samples[1].c_count );
    EXPECT_EQ( 0, data[2].samples[1].g_count );
    EXPECT_EQ( 0, data[2].samples[1].n_count );
    EXPECT_EQ( 0, data[2].samples[1].d_count );

    // Line 4

    EXPECT_EQ( "2R", data[3].chromosome );
    EXPECT_EQ( 2305, data[3].position );
    EXPECT_EQ(  'N', data[3].reference_base );
    ASSERT_EQ(    2, data[3].samples.size() );

    EXPECT_EQ( 1, data[3].samples[0].a_count );
    EXPECT_EQ( 0, data[3].samples[0].t_count );
    EXPECT_EQ( 9, data[3].samples[0].c_count );
    EXPECT_EQ( 0, data[3].samples[0].g_count );
    EXPECT_EQ( 0, data[3].samples[0].n_count );
    EXPECT_EQ( 0, data[3].samples[0].d_count );

    EXPECT_EQ( 0, data[3].samples[1].a_count );
    EXPECT_EQ( 0, data[3].samples[1].t_count );
    EXPECT_EQ( 9, data[3].samples[1].c_count );
    EXPECT_EQ( 1, data[3].samples[1].g_count );
    EXPECT_EQ( 0, data[3].samples[1].n_count );
    EXPECT_EQ( 0, data[3].samples[1].d_count );
}

TEST( Sync, SyncInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";

    // Test the while() approach
    size_t cnt_while = 0;
    BaseCounts sum_while[2];
    auto it = SyncInputIterator( from_file( infile ));
    while( it ) {
        EXPECT_EQ( 2, it->samples.size() );
        merge_inplace( sum_while[0], it->samples[0] );
        merge_inplace( sum_while[1], it->samples[1] );

        ++cnt_while;
        ++it;
    }
    EXPECT_EQ( 4, cnt_while );

    // Test first column sum
    EXPECT_EQ(  1, sum_while[0].a_count );
    EXPECT_EQ( 15, sum_while[0].t_count );
    EXPECT_EQ( 18, sum_while[0].c_count );
    EXPECT_EQ(  0, sum_while[0].g_count );
    EXPECT_EQ(  0, sum_while[0].n_count );
    EXPECT_EQ(  0, sum_while[0].d_count );

    // Test second column sum
    EXPECT_EQ(  0, sum_while[1].a_count );
    EXPECT_EQ( 15, sum_while[1].t_count );
    EXPECT_EQ( 18, sum_while[1].c_count );
    EXPECT_EQ(  1, sum_while[1].g_count );
    EXPECT_EQ(  0, sum_while[1].n_count );
    EXPECT_EQ(  0, sum_while[1].d_count );

    // Test the for() approach
    size_t cnt_for = 0;
    BaseCounts sum_for[2];
    for( auto it = SyncInputIterator( from_file( infile )); it; ++it ) {
        EXPECT_EQ( 2, it->samples.size() );
        merge_inplace( sum_for[0], it->samples[0] );
        merge_inplace( sum_for[1], it->samples[1] );

        ++cnt_for;
    }
    EXPECT_EQ( 4, cnt_for );

    // Test first column sum
    EXPECT_EQ(  1, sum_for[0].a_count );
    EXPECT_EQ( 15, sum_for[0].t_count );
    EXPECT_EQ( 18, sum_for[0].c_count );
    EXPECT_EQ(  0, sum_for[0].g_count );
    EXPECT_EQ(  0, sum_for[0].n_count );
    EXPECT_EQ(  0, sum_for[0].d_count );

    // Test second column sum
    EXPECT_EQ(  0, sum_for[1].a_count );
    EXPECT_EQ( 15, sum_for[1].t_count );
    EXPECT_EQ( 18, sum_for[1].c_count );
    EXPECT_EQ(  1, sum_for[1].g_count );
    EXPECT_EQ(  0, sum_for[1].n_count );
    EXPECT_EQ(  0, sum_for[1].d_count );
}
