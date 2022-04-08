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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/functions/filter_transform.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::population;
using namespace genesis::utils;

#ifdef GENESIS_HTSLIB

TEST( Variant, SamInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/ex1.sam.gz";
    auto it = make_variant_input_iterator_from_sam_file( infile );

    // Add a filter that limits it to a region, and then skips a region inside.
    it.add_filter(
        filter_by_region( GenomeRegion( "seq1", 272, 279 ))
    );
    it.add_filter(
        filter_by_region( GenomeRegion( "seq1", 274, 277 ), true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        // LOG_DBG << variant.chromosome << ":" << variant.position;
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 272 273 278 279", result );
}

#endif // GENESIS_HTSLIB

TEST( Variant, PileupInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.pileup";
    auto it = make_variant_input_iterator_from_pileup_file( infile );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "seq1", 274, 277 );
    // it.add_filter(
    //     filter_by_region( region )
    // );
    it.add_filter(
        filter_by_region( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 272 273 278 279", result );
}

TEST( Variant, SyncInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/test.sync";
    auto it = make_variant_input_iterator_from_sync_file( infile );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "2R", 2302, 2302 );
    it.add_filter(
        filter_by_region( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 2303 2304 2305", result );
}

#ifdef GENESIS_HTSLIB

TEST( Variant, VcfInputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example_ad.vcf";
    auto it = make_variant_input_iterator_from_pool_vcf_file( infile, false, false );

    // Add a filter that skips the specified region.
    auto region = GenomeRegion( "20", 17000, 1120000 );
    it.add_filter(
        filter_by_region( region, true )
    );

    // Add a filter that doesn't do anything
    it.add_filter(
        filter_by_region( GenomeRegion("not_a_chr", 100, 200), true )
    );

    // Simple test that the correct region is filtered out.
    std::string result;
    for( auto const& variant : it ) {
        result += " " + std::to_string( variant.position );
    }
    EXPECT_EQ( " 14370 1230237", result );
}

#endif // GENESIS_HTSLIB
