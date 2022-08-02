/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/formats/genome_region_reader.hpp"
#include "genesis/population/functions/genome_region.hpp"
#include "genesis/population/genome_region.hpp"

using namespace genesis::population;
using namespace genesis::utils;

GenomeRegion make_genome_region( std::string const& chr, size_t s, size_t e )
{
    GenomeRegion result;
    result.chromosome = chr;
    result.start = s;
    result.end = e;
    return result;
}

TEST( GenomeRegion, Basics )
{
    GenomeRegionList list;

    // Add some regions, in different orders.
    list.add(make_genome_region( "X", 15, 20 ));
    list.add(make_genome_region( "X", 1, 5 ));
    list.add(make_genome_region( "X", 10, 15 ));
    list.add(make_genome_region( "X", 25, 35 ));

    // Add some overlapping and invalid regions.
    // EXPECT_ANY_THROW( list.add(make_genome_region( "X", 1, 5 )); );
    // EXPECT_ANY_THROW( list.add(make_genome_region( "X", 3, 18 )); );
    EXPECT_ANY_THROW( list.add(make_genome_region( "X", 45, 40 )); );
    // EXPECT_ANY_THROW( list.add(make_genome_region( "X", 50, 50 )); );

    // Test all existing regions.
    EXPECT_FALSE( list.is_covered( "X", 0 ));
    EXPECT_TRUE(  list.is_covered( "X", 1 ));
    EXPECT_TRUE(  list.is_covered( "X", 4 ));
    EXPECT_TRUE(  list.is_covered( "X", 5 ));
    EXPECT_FALSE( list.is_covered( "X", 9 ));
    EXPECT_TRUE(  list.is_covered( "X", 10 ));
    EXPECT_TRUE(  list.is_covered( "X", 15 ));
    EXPECT_TRUE(  list.is_covered( "X", 19 ));
    EXPECT_TRUE(  list.is_covered( "X", 20 ));
    EXPECT_FALSE( list.is_covered( "X", 22 ));
    EXPECT_FALSE( list.is_covered( "X", 24 ));
    EXPECT_TRUE(  list.is_covered( "X", 25 ));
    EXPECT_TRUE(  list.is_covered( "X", 34 ));
    EXPECT_TRUE(  list.is_covered( "X", 35 ));
    EXPECT_FALSE( list.is_covered( "X", 36 ));

    // Test coverage
    EXPECT_EQ( 0, list.cover_count( "X", 0 ));
    EXPECT_EQ( 1, list.cover_count( "X", 1 ));
    EXPECT_EQ( 1, list.cover_count( "X", 5 ));
    EXPECT_EQ( 1, list.cover_count( "X", 14 ));
    EXPECT_EQ( 2, list.cover_count( "X", 15 ));
    EXPECT_EQ( 1, list.cover_count( "X", 16 ));
    EXPECT_EQ( 1, list.cover_count( "X", 20 ));
    EXPECT_EQ( 0, list.cover_count( "X", 21 ));

    // Test some other stuff.
    EXPECT_FALSE( list.is_covered( "Y", 5 ));
    EXPECT_EQ( 1, list.chromosome_count() );
    EXPECT_EQ( 4, list.region_count( "X" ));
    EXPECT_EQ( 4, list.total_region_count());
}

TEST( GenomeRegion, Parse )
{
    // The Good.
    EXPECT_EQ( GenomeRegion( "A", 0, 0 ),   parse_genome_region( "A" ));
    EXPECT_EQ( GenomeRegion( "A", 10, 10 ), parse_genome_region( "A:10" ));
    EXPECT_EQ( GenomeRegion( "A", 5, 10 ),  parse_genome_region( "A:5-10" ));
    EXPECT_EQ( GenomeRegion( "A", 5, 10 ),  parse_genome_region( "A:5..10" ));

    // The Bad.
    EXPECT_ANY_THROW( parse_genome_region( "A:B" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:1:2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:B:C" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:B-C" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:B..C" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:2..0" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0--2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:-1-2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0-1-" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0-1-2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0-1..2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0..1-2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0..1..2" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0.." ));
    EXPECT_ANY_THROW( parse_genome_region( "A:..1" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:0-" ));
    EXPECT_ANY_THROW( parse_genome_region( "A:-1" ));

    // The Ugly.
    EXPECT_ANY_THROW( parse_genome_region( "" ));
    EXPECT_ANY_THROW( parse_genome_region( ":" ));
    EXPECT_ANY_THROW( parse_genome_region( "-" ));
    EXPECT_ANY_THROW( parse_genome_region( ".." ));
    EXPECT_ANY_THROW( parse_genome_region( ":-" ));
    EXPECT_ANY_THROW( parse_genome_region( ":.." ));
}

TEST( GenomeRegion, ParseFile )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/regions_1.txt";

    GenomeRegionList list;
    GenomeRegionReader().read_as_genome_region_list( from_file( infile ), list );

    EXPECT_TRUE(  list.is_covered( "A" ));
    EXPECT_TRUE(  list.is_covered( "A", 0 ));
    EXPECT_TRUE(  list.is_covered( "A", 10 ));

    EXPECT_EQ( 0, list.cover_count( "A", 1 ));
    EXPECT_EQ( 1, list.cover_count( "A", 1, true ));

    EXPECT_FALSE( list.is_covered( "B" ));
    EXPECT_FALSE( list.is_covered( "B", 0 ));
    EXPECT_FALSE( list.is_covered( "B", 9 ));
    EXPECT_TRUE(  list.is_covered( "B", 10 ));
    EXPECT_TRUE(  list.is_covered( "B", 20 ));
    EXPECT_FALSE( list.is_covered( "B", 21 ));

    EXPECT_FALSE( list.is_covered( "C" ));
    EXPECT_FALSE( list.is_covered( "C", 0 ));
    EXPECT_FALSE( list.is_covered( "C", 9 ));
    EXPECT_TRUE(  list.is_covered( "C", 10 ));
    EXPECT_FALSE( list.is_covered( "C", 11 ));
}

TEST( GenomeList, ParseFile )
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

void test_genome_list_set_operators_( GenomeLocusSet const& list_1, GenomeLocusSet const& list_2 )
{
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

TEST( GenomeList, SetOperators )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile_1 = environment->data_dir + "population/regions_1.txt";
    std::string const infile_2 = environment->data_dir + "population/regions_2.txt";

    // Get lists.
    auto const list_1 = GenomeRegionReader().read_as_genome_locus_set( from_file( infile_1 ));
    auto const list_2 = GenomeRegionReader().read_as_genome_locus_set( from_file( infile_2 ));

    // The operators are symmetric, so test both directions.
    test_genome_list_set_operators_( list_1, list_2 );
    test_genome_list_set_operators_( list_2, list_1 );
}
