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

#include "genesis/population/format/bed_reader.hpp"

#include <cmath>
#include <limits>

using namespace genesis::population;
using namespace genesis::utils;

TEST( BedReader, Read1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/wiki1.bed3";

    auto reader = BedReader();
    auto regions = reader.read_as_genome_region_list( from_file( infile ));
    EXPECT_EQ( 3, regions.total_region_count() );
    EXPECT_FALSE( regions.is_covered( "chr7", 127471196 ));
    EXPECT_TRUE(  regions.is_covered( "chr7", 127471197 ));
    EXPECT_TRUE(  regions.is_covered( "chr7", 127472363 ));
    EXPECT_FALSE( regions.is_covered( "chr7", 127472364 ));
    EXPECT_TRUE(  regions.is_covered( "chr7", 127472365 ));
    EXPECT_TRUE(  regions.is_covered( "chr7", 127473530 ));
    EXPECT_FALSE( regions.is_covered( "chr7", 127473531 ));
    EXPECT_TRUE(  regions.is_covered( "chr7", 127473532 ));
    EXPECT_TRUE(  regions.is_covered( "chr7", 127474697 ));
    EXPECT_FALSE( regions.is_covered( "chr7", 127474698 ));
}

TEST( BedReader, Read2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/wiki2.bed";

    auto reader = BedReader();
    auto regions = reader.read_as_genome_region_list( from_file( infile ));
    EXPECT_EQ( 9, regions.total_region_count() );
    EXPECT_FALSE( regions.is_covered( "chr7", 127471196 ));
    for( size_t i = 127471197; i <= 127481699; ++i ) {
        EXPECT_TRUE(  regions.is_covered( "chr7", i ));
    }
    EXPECT_FALSE( regions.is_covered( "chr7", 127481700 ));
}

TEST( BedReader, Read3 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/ucsc.bed";

    auto reader = BedReader();
    auto regions = reader.read_as_genome_region_list( from_file( infile ));
    EXPECT_EQ( 2, regions.total_region_count() );
    EXPECT_FALSE( regions.is_covered( "chr22", 1000 ));
    EXPECT_TRUE(  regions.is_covered( "chr22", 1001 ));
    EXPECT_TRUE(  regions.is_covered( "chr22", 6000 ));
    EXPECT_FALSE( regions.is_covered( "chr22", 6001 ));
}
