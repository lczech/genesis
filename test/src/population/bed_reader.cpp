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
using namespace genesis::sequence;
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

template<class R>
void test_bed_reader_mask_coverage_( R const& regions )
{
    // First region boundaries
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 3380 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 3381 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 3390 ));
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 3391 ));

    // Second region boundaries
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 1200 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 1201 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 1700 ));
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 1701 ));

    // Third region boundaries
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 4000 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 4001 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 5000 ));
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 5001 ));

    // Fourth region boundaries
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 6782 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 6783 ));
    EXPECT_TRUE(  regions.is_covered( "agouti_scaf_1001", 6790 ));
    EXPECT_FALSE( regions.is_covered( "agouti_scaf_1001", 6791 ));
}

TEST( BedReader, ReadMask )
{
    // Debugging for https://github.com/lczech/grenedalf/issues/29
    // using the `mask.txt` file from there for testing.

    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/mask.bed";
    auto reader = BedReader();

    // Regions in the file (in bed format, i.e., 0-based half-open intervals):
    // agouti_scaf_1001    3380    3390
    // agouti_scaf_1001    1200    1700
    // agouti_scaf_1001    4000    5000
    // agouti_scaf_1001    6782    6790

    // Test the four regions in the file, as a region list:
    auto const regions = reader.read_as_genome_region_list( from_file( infile ));
    EXPECT_EQ( 4, regions.total_region_count() );
    test_bed_reader_mask_coverage_( regions );

    // Now test them again as genome locus set.
    // We want to see that the length of the bitvector matches the last position of the input,
    // plus one for the zero-th bit, as we are using 1-based positions.
    {
        auto const locus_set = reader.read_as_genome_locus_set( from_file( infile ));
        EXPECT_EQ( 1, locus_set.chromosome_count() );
        EXPECT_EQ( 6791, locus_set.chromosome_positions( "agouti_scaf_1001" ).size() );
        test_bed_reader_mask_coverage_( locus_set );
    }

    // Repeat the above, but this time using a seq dict to set the length.
    {
        auto seq_dict = std::make_shared<SequenceDict>();
        seq_dict->add( "agouti_scaf_1001", 10000 );
        auto const locus_set = reader.read_as_genome_locus_set(
            from_file( infile ), seq_dict
        );
        EXPECT_EQ( 1, locus_set.chromosome_count() );
        EXPECT_EQ( 10001, locus_set.chromosome_positions( "agouti_scaf_1001" ).size() );
        test_bed_reader_mask_coverage_( locus_set );
    }

    // And one more time, with a seq dict that just exactly fits, to test the boundary.
    {
        auto seq_dict = std::make_shared<SequenceDict>();
        seq_dict->add( "agouti_scaf_1001", 6790 );
        auto const locus_set = reader.read_as_genome_locus_set(
            from_file( infile ), seq_dict
        );
        EXPECT_EQ( 1, locus_set.chromosome_count() );
        EXPECT_EQ( 6791, locus_set.chromosome_positions( "agouti_scaf_1001" ).size() );
        test_bed_reader_mask_coverage_( locus_set );
    }

    // Last time, this time testing the error case, where the dict is too short for the positions.
    {
        auto seq_dict = std::make_shared<SequenceDict>();
        seq_dict->add( "agouti_scaf_1001", 6789 );
        EXPECT_ANY_THROW( reader.read_as_genome_locus_set(
            from_file( infile ), seq_dict
        ));
    }
}
