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
#include "genesis/population/formats/sam_variant_input_iterator.hpp"
#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/functions/variant.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::population;
using namespace genesis::utils;

void run_sam_bam_cram_test( std::string const& infile )
{
    // We just use any file that comes in here, no matter what the format.
    auto sam_it = SamVariantInputIterator( infile );
    sam_it.min_map_qual( 40 );

    BaseCounts counts;
    for( auto const& var : sam_it ) {
        // std::cout << var.chromosome << "\t" << var.position << "\t";
        for( auto const& bs : var.samples ) {
            // std::cout << "\t";
            // to_sync( bs, std::cout );
            merge_inplace( counts, bs );
        }
        // std::cout << "\n";
    }

    EXPECT_EQ( 39234, counts.a_count );
    EXPECT_EQ( 22703, counts.c_count );
    EXPECT_EQ( 20181, counts.g_count );
    EXPECT_EQ( 30232, counts.t_count );
    EXPECT_EQ(    23, counts.n_count );
    EXPECT_EQ(     2, counts.d_count );
}

TEST( SamBamCram, InputIteratorSam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_sam_bam_cram_test( environment->data_dir + "population/ex1.sam.gz" );
}

TEST( SamBamCram, InputIteratorBam )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_sam_bam_cram_test( environment->data_dir + "population/ex1.bam" );
}

TEST( SamBamCram, InputIteratorCram )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    run_sam_bam_cram_test( environment->data_dir + "population/ex1.cram" );
}
