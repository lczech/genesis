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

#include "genesis/population/format/genome_region_reader.hpp"
#include "genesis/population/function/genome_locus_set.hpp"
#include "genesis/population/function/genome_region.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( GenomeLocusSet, ParseFile )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/regions_1.txt";

    // Parse into a genome locus list.
    GenomeLocusSet const list = GenomeRegionReader().read_as_genome_locus_set( from_file( infile ));

    EXPECT_TRUE(  list.is_covered( "A" ));
    EXPECT_TRUE(  list.is_covered( "A", 0 ));
    EXPECT_TRUE(  list.is_covered( "A", 10 ));

    EXPECT_FALSE( list.is_covered( "B" ));
    EXPECT_FALSE( list.is_covered( "B", 0 ));
    EXPECT_FALSE( list.is_covered( "B", 9 ));
    EXPECT_TRUE(  list.is_covered( "B", 10 ));
    EXPECT_TRUE(  list.is_covered( "B", 20 ));
    EXPECT_FALSE( list.is_covered( "B", 21 ));
    EXPECT_FALSE( list.is_covered( "B", 29 ));
    EXPECT_TRUE(  list.is_covered( "B", 30 ));
    EXPECT_TRUE(  list.is_covered( "B", 40 ));
    EXPECT_FALSE( list.is_covered( "B", 41 ));

    EXPECT_FALSE( list.is_covered( "C" ));
    EXPECT_FALSE( list.is_covered( "C", 0 ));
    EXPECT_FALSE( list.is_covered( "C", 9 ));
    EXPECT_TRUE(  list.is_covered( "C", 10 ));
    EXPECT_FALSE( list.is_covered( "C", 11 ));
}

void test_genome_locus_set_operators_(
    GenomeLocusSet const& list_1,
    GenomeLocusSet const& list_2
) {
    // Intersection
    {
        auto result = list_1;
        result.set_intersect( list_2 );

        EXPECT_FALSE( result.is_covered( "A" ));
        EXPECT_FALSE( result.is_covered( "A", 0 ));
        EXPECT_FALSE( result.is_covered( "A", 4 ));
        EXPECT_TRUE(  result.is_covered( "A", 5 ));
        EXPECT_TRUE(  result.is_covered( "A", 10 ));
        EXPECT_FALSE( result.is_covered( "A", 11 ));

        EXPECT_FALSE( result.is_covered( "B" ));
        EXPECT_FALSE( result.is_covered( "B", 0 ));
        EXPECT_FALSE( result.is_covered( "B", 14 ));
        EXPECT_TRUE(  result.is_covered( "B", 15 ));
        EXPECT_TRUE(  result.is_covered( "B", 20 ));
        EXPECT_FALSE( result.is_covered( "B", 21 ));
        EXPECT_FALSE( result.is_covered( "B", 34 ));
        EXPECT_TRUE(  result.is_covered( "B", 35 ));
        EXPECT_TRUE(  result.is_covered( "B", 40 ));
        EXPECT_FALSE( result.is_covered( "B", 41 ));

        EXPECT_FALSE( result.is_covered( "C" ));
        EXPECT_FALSE( result.is_covered( "C", 0 ));
        EXPECT_FALSE( result.is_covered( "C", 10 ));
        EXPECT_TRUE(  result.is_covered( "C", 15 ));
        EXPECT_TRUE(  result.is_covered( "C", 20 ));
        EXPECT_FALSE( result.is_covered( "C", 25 ));
        EXPECT_FALSE( result.is_covered( "C", 29 ));
        EXPECT_TRUE(  result.is_covered( "C", 30 ));

        EXPECT_TRUE(  result.is_covered( "D" ));
        EXPECT_TRUE(  result.is_covered( "D", 0 ));
        EXPECT_TRUE(  result.is_covered( "D", 1 ));

        EXPECT_FALSE( result.is_covered( "E" ));
        EXPECT_FALSE( result.is_covered( "E", 0 ));
        EXPECT_FALSE( result.is_covered( "E", 10 ));
        EXPECT_FALSE( result.is_covered( "E", 20 ));
    }

    // Union
    {
        auto result = list_1;
        result.set_union( list_2 );

        EXPECT_TRUE(  result.is_covered( "A" ));
        EXPECT_TRUE(  result.is_covered( "A", 0 ));
        EXPECT_TRUE(  result.is_covered( "A", 4 ));
        EXPECT_TRUE(  result.is_covered( "A", 5 ));
        EXPECT_TRUE(  result.is_covered( "A", 10 ));
        EXPECT_TRUE(  result.is_covered( "A", 11 ));

        EXPECT_FALSE( result.is_covered( "B" ));
        EXPECT_FALSE( result.is_covered( "B", 0 ));
        EXPECT_FALSE( result.is_covered( "B", 9 ));
        EXPECT_TRUE(  result.is_covered( "B", 10 ));
        EXPECT_TRUE(  result.is_covered( "B", 15 ));
        EXPECT_TRUE(  result.is_covered( "B", 20 ));
        EXPECT_TRUE(  result.is_covered( "B", 25 ));
        EXPECT_FALSE( result.is_covered( "B", 26 ));
        EXPECT_FALSE( result.is_covered( "B", 29 ));
        EXPECT_TRUE(  result.is_covered( "B", 30 ));
        EXPECT_TRUE(  result.is_covered( "B", 40 ));
        EXPECT_TRUE(  result.is_covered( "B", 45 ));
        EXPECT_FALSE( result.is_covered( "B", 46 ));

        EXPECT_FALSE( result.is_covered( "C" ));
        EXPECT_FALSE( result.is_covered( "C", 0 ));
        EXPECT_TRUE(  result.is_covered( "C", 10 ));
        EXPECT_FALSE( result.is_covered( "C", 14 ));
        EXPECT_TRUE(  result.is_covered( "C", 15 ));
        EXPECT_TRUE(  result.is_covered( "C", 20 ));
        EXPECT_TRUE(  result.is_covered( "C", 25 ));
        EXPECT_TRUE(  result.is_covered( "C", 29 ));
        EXPECT_TRUE(  result.is_covered( "C", 30 ));
        EXPECT_FALSE( result.is_covered( "C", 31 ));

        EXPECT_TRUE(  result.is_covered( "D" ));
        EXPECT_TRUE(  result.is_covered( "D", 0 ));
        EXPECT_TRUE(  result.is_covered( "D", 1 ));

        EXPECT_FALSE( result.is_covered( "E" ));
        EXPECT_FALSE( result.is_covered( "E", 0 ));
        EXPECT_FALSE( result.is_covered( "E", 9 ));
        EXPECT_TRUE(  result.is_covered( "E", 10 ));
        EXPECT_TRUE(  result.is_covered( "E", 20 ));
        EXPECT_FALSE( result.is_covered( "E", 21 ));
    }
}

TEST( GenomeLocusSet, SetOperators )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile_1 = environment->data_dir + "population/regions_1.txt";
    std::string const infile_2 = environment->data_dir + "population/regions_2.txt";

    // Get lists.
    auto const list_1 = GenomeRegionReader().read_as_genome_locus_set( from_file( infile_1 ));
    auto const list_2 = GenomeRegionReader().read_as_genome_locus_set( from_file( infile_2 ));

    // The operators are symmetric, so test both directions.
    test_genome_locus_set_operators_( list_1, list_2 );
    test_genome_locus_set_operators_( list_2, list_1 );
}

TEST( GenomeLocusSet, MaskFile )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const mask_file = environment->data_dir + "population/mask.fasta";

    // Defaults
    {
        auto const gls = read_mask_fasta( from_file( mask_file ));
        ASSERT_EQ( 2, gls.chromosome_count() );
        EXPECT_FALSE( gls.is_covered( "1" ));
        EXPECT_FALSE( gls.is_covered( "2" ));
        EXPECT_FALSE( gls.is_covered( "1", 0 ));
        EXPECT_FALSE( gls.is_covered( "1", 1 ));
        EXPECT_FALSE( gls.is_covered( "1", 5 ));
        EXPECT_TRUE(  gls.is_covered( "1", 6 ));
        EXPECT_TRUE(  gls.is_covered( "1", 13 ));
        EXPECT_FALSE( gls.is_covered( "2", 0 ));
        EXPECT_TRUE(  gls.is_covered( "2", 1 ));
        EXPECT_TRUE(  gls.is_covered( "2", 10 ));
        EXPECT_FALSE( gls.is_covered( "2", 11 ));
        EXPECT_FALSE( gls.is_covered( "2", 13 ));

        EXPECT_EQ( Bitvector( "00000011111111" ), gls.chromosome_positions( "1" ));
        EXPECT_EQ( Bitvector( "01111111111000" ), gls.chromosome_positions( "2" ));
    }

    // Invert
    {
        auto const gls = read_mask_fasta( from_file( mask_file ), 0, true );
        ASSERT_EQ( 2, gls.chromosome_count() );
        EXPECT_FALSE( gls.is_covered( "1" ));
        EXPECT_FALSE( gls.is_covered( "2" ));
        EXPECT_FALSE( gls.is_covered( "1", 0 ));
        EXPECT_TRUE(  gls.is_covered( "1", 1 ));
        EXPECT_TRUE(  gls.is_covered( "1", 5 ));
        EXPECT_FALSE( gls.is_covered( "1", 6 ));
        EXPECT_FALSE( gls.is_covered( "1", 13 ));
        EXPECT_FALSE( gls.is_covered( "2", 0 ));
        EXPECT_FALSE( gls.is_covered( "2", 1 ));
        EXPECT_FALSE( gls.is_covered( "2", 10 ));
        EXPECT_TRUE(  gls.is_covered( "2", 11 ));
        EXPECT_TRUE(  gls.is_covered( "2", 13 ));

        EXPECT_EQ( Bitvector( "01111100000000" ), gls.chromosome_positions( "1" ));
        EXPECT_EQ( Bitvector( "00000000000111" ), gls.chromosome_positions( "2" ));
    }

    // Higher mask
    {
        auto const gls = read_mask_fasta( from_file( mask_file ), 1 );
        ASSERT_EQ( 2, gls.chromosome_count() );
        EXPECT_FALSE( gls.is_covered( "1" ));
        EXPECT_FALSE( gls.is_covered( "2" ));
        EXPECT_FALSE( gls.is_covered( "1", 0 ));
        EXPECT_FALSE( gls.is_covered( "1", 1 ));
        EXPECT_FALSE( gls.is_covered( "1", 10 ));
        EXPECT_TRUE(  gls.is_covered( "1", 11 ));
        EXPECT_TRUE(  gls.is_covered( "1", 13 ));
        EXPECT_FALSE( gls.is_covered( "2", 0 ));
        EXPECT_TRUE(  gls.is_covered( "2", 1 ));
        EXPECT_TRUE(  gls.is_covered( "2", 5 ));
        EXPECT_FALSE( gls.is_covered( "2", 6 ));
        EXPECT_FALSE( gls.is_covered( "2", 13 ));

        EXPECT_EQ( Bitvector( "00000000000111" ), gls.chromosome_positions( "1" ));
        EXPECT_EQ( Bitvector( "01111100000000" ), gls.chromosome_positions( "2" ));
    }

    // Higher mask and invert
    {
        auto const gls = read_mask_fasta( from_file( mask_file ), 1, true );
        ASSERT_EQ( 2, gls.chromosome_count() );
        EXPECT_FALSE( gls.is_covered( "1" ));
        EXPECT_FALSE( gls.is_covered( "2" ));
        EXPECT_FALSE( gls.is_covered( "1", 0 ));
        EXPECT_TRUE(  gls.is_covered( "1", 1 ));
        EXPECT_TRUE(  gls.is_covered( "1", 10 ));
        EXPECT_FALSE( gls.is_covered( "1", 11 ));
        EXPECT_FALSE( gls.is_covered( "1", 13 ));
        EXPECT_FALSE( gls.is_covered( "2", 0 ));
        EXPECT_FALSE( gls.is_covered( "2", 1 ));
        EXPECT_FALSE( gls.is_covered( "2", 5 ));
        EXPECT_TRUE(  gls.is_covered( "2", 6 ));
        EXPECT_TRUE(  gls.is_covered( "2", 13 ));

        EXPECT_EQ( Bitvector( "01111111111000" ), gls.chromosome_positions( "1" ));
        EXPECT_EQ( Bitvector( "00000011111111" ), gls.chromosome_positions( "2" ));
    }
}
