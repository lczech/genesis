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

#include "genesis/population/format/map_bim_reader.hpp"

#include <cmath>
#include <limits>

using namespace genesis::population;
using namespace genesis::utils;

void run_map_bim_reader_test_( std::string const& file )
{
    std::string const infile = environment->data_dir + "population/" + file;
    auto const regions = MapBimReader().read_as_genome_region_list( from_file( infile ));

    // Test all positions in the file, and the immediately adjecent ones as well.
    EXPECT_EQ( 5, regions.total_region_count() );

    // First interval.
    EXPECT_FALSE( regions.is_covered( "2", 71209 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71210 ));
    EXPECT_FALSE( regions.is_covered( "2", 71211 ));

    // Second interval.
    EXPECT_FALSE( regions.is_covered( "2", 71227 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71228 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71229 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71230 ));
    EXPECT_FALSE( regions.is_covered( "2", 71231 ));

    // Third interval.
    EXPECT_FALSE( regions.is_covered( "2", 71281 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71282 ));
    EXPECT_FALSE( regions.is_covered( "2", 71283 ));

    // Fourth interval.
    EXPECT_FALSE( regions.is_covered( "2", 71398 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71399 ));
    EXPECT_FALSE( regions.is_covered( "2", 71400 ));

    // Fifth interval.
    EXPECT_FALSE( regions.is_covered( "2", 71530 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71531 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71532 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71533 ));
    EXPECT_TRUE(  regions.is_covered( "2", 71534 ));
    EXPECT_FALSE( regions.is_covered( "2", 71535 ));

    // Test some off-values.
    EXPECT_FALSE( regions.is_covered( "1", 0 ));
    EXPECT_FALSE( regions.is_covered( "1", 1 ));
    EXPECT_FALSE( regions.is_covered( "2", 0 ));
}

TEST( MapBimReader, ReadBim1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_map_bim_reader_test_( "regions_1.bim" );
}

TEST( MapBimReader, ReadBim2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_map_bim_reader_test_( "regions_2.bim" );
}

TEST( MapBimReader, ReadMap1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_map_bim_reader_test_( "regions_1.map" );
}

TEST( MapBimReader, ReadMap2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_map_bim_reader_test_( "regions_2.map" );
}
