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
#include <string>
#include <unordered_set>

#include "lib/placement/formats/jplace_reader.hpp"
#include "lib/placement/formats/newick_reader.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/sample.hpp"
#include "lib/tree/default/tree.hpp"
#include "lib/tree/formats/newick/reader.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST( SampleFunctions, FilterPlacements )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Check before filtering.
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

TEST( SampleFunctions, FilterPqueryNames )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Check before filtering.
    EXPECT_EQ( 10, total_placement_count(smp) );

    // Keep list.
    std::unordered_set<std::string> keep_list = { "a", "c" };
    filter_pqueries_keeping_names( smp, keep_list );
    EXPECT_EQ( 6, total_placement_count(smp) );

    // Re-read the file.
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Remove list.
    std::unordered_set<std::string> remove_list = { "a", "c" };
    filter_pqueries_removing_names( smp, remove_list );
    EXPECT_EQ( 4, total_placement_count(smp) );
}

TEST( SampleFunctions, FilterPqueryNameSets )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read files.
    std::string infile_1 = environment->data_dir + "placement/duplicates_a.jplace";
    std::string infile_2 = environment->data_dir + "placement/duplicates_b.jplace";
    Sample sample_1;
    Sample sample_2;
    EXPECT_NO_THROW( JplaceReader().from_file( infile_1, sample_1 ));
    EXPECT_NO_THROW( JplaceReader().from_file( infile_2, sample_2 ));

    // Checks before filtering.
    EXPECT_EQ(  8, total_placement_count( sample_1 ));
    EXPECT_EQ( 10, total_placement_count( sample_2 ));

    // Intersection.
    filter_pqueries_intersecting_names( sample_1, sample_2 );
    EXPECT_EQ(  8, total_placement_count( sample_1 ));
    EXPECT_EQ(  8, total_placement_count( sample_2 ));

    // Re-read the files.
    EXPECT_NO_THROW( JplaceReader().from_file( infile_1, sample_1 ));
    EXPECT_NO_THROW( JplaceReader().from_file( infile_2, sample_2 ));

    // Symmetric difference.
    filter_pqueries_differing_names( sample_1, sample_2 );
    EXPECT_EQ(  0, total_placement_count( sample_1 ));
    EXPECT_EQ(  2, total_placement_count( sample_2 ));
}

TEST( SampleFunctions, ConvertFromDefaultTree )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process a normal newick tree.
    tree::DefaultTree def_tree;
    std::string infile = environment->data_dir + "tree/distances.newick";
    tree::DefaultTreeNewickReader().from_file(infile, def_tree);

    // Convert it to a tree that is usable for samples.
    auto place_tree = convert_to_placement_tree( def_tree );

    // Check if the tree is correct.
    EXPECT_EQ( 13, place_tree.node_count() );
    EXPECT_TRUE( has_correct_edge_nums( place_tree ));
}
