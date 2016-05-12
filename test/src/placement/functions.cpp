/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include <memory>

#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/io/jplace_reader.hpp"
#include "lib/placement/io/newick_reader.hpp"
#include "lib/placement/sample.hpp"
#include "lib/tree/formats/newick/reader.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(SampleFunctions, Filter)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Check before merging.
    EXPECT_EQ( 10, total_placement_count(smp) );

    // Filter everything below a threshold and check result.
    filter_min_weight_threshold( smp, 0.5 );
    EXPECT_EQ( 8, total_placement_count(smp) );

    // Re-read the file.
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Filter max number of placements and check result.
    filter_n_max_weight_placements( smp, 1 );
    EXPECT_EQ( 7, total_placement_count(smp) );

    // Re-read the file.
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Filter max number of placements and check result.
    filter_min_accumulated_weight( smp, 0.6 );
    EXPECT_EQ( 8, total_placement_count(smp) );
}
