/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "src/common.hpp"

#include <memory>
#include <string>
#include <unordered_set>

#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/formats/newick_reader.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/function/tree.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/default/newick_writer.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/formats/newick/writer.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST( SampleFunctions, FilterPlacements )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp = JplaceReader().from_file(infile);

    // Check before filtering.
    EXPECT_EQ( 10, total_placement_count(smp) );

    // Filter everything below a threshold and check result.
    filter_min_weight_threshold( smp, 0.5 );
    EXPECT_EQ( 8, total_placement_count(smp) );

    // Re-read the file.
    smp = JplaceReader().from_file(infile);

    // Filter max number of placements and check result.
    filter_n_max_weight_placements( smp, 1 );
    EXPECT_EQ( 7, total_placement_count(smp) );

    // Re-read the file.
    smp = JplaceReader().from_file(infile);

    // Filter max number of placements and check result.
    filter_min_accumulated_weight( smp, 0.6 );
    EXPECT_EQ( 8, total_placement_count(smp) );
}

TEST( SampleFunctions, FilterPqueryNameRegex )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp = JplaceReader().from_file(infile);

    // Check before filtering.
    EXPECT_EQ( 10, total_placement_count(smp) );

    // Keep list.
    filter_pqueries_keeping_names( smp, "[ac]" );
    EXPECT_EQ( 6, total_placement_count(smp) );

    // Re-read the file.
    smp = JplaceReader().from_file(infile);

    // Remove list.
    filter_pqueries_removing_names( smp, "[ac]" );
    EXPECT_EQ( 4, total_placement_count(smp) );
}

TEST( SampleFunctions, FilterPqueryNameLists )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp = JplaceReader().from_file(infile);

    // Check before filtering.
    EXPECT_EQ( 10, total_placement_count(smp) );

    // Keep list.
    std::unordered_set<std::string> keep_list = { "a", "c" };
    filter_pqueries_keeping_names( smp, keep_list );
    EXPECT_EQ( 6, total_placement_count(smp) );

    // Re-read the file.
    smp = JplaceReader().from_file(infile);

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
    std::string const infile_1 = environment->data_dir + "placement/duplicates_a.jplace";
    std::string const infile_2 = environment->data_dir + "placement/duplicates_b.jplace";
    auto sample_1 = JplaceReader().from_file( infile_1 );
    auto sample_2 = JplaceReader().from_file( infile_2 );

    // Checks before filtering.
    EXPECT_EQ(  8, total_placement_count( sample_1 ));
    EXPECT_EQ( 10, total_placement_count( sample_2 ));

    // Intersection.
    filter_pqueries_intersecting_names( sample_1, sample_2 );
    EXPECT_EQ(  8, total_placement_count( sample_1 ));
    EXPECT_EQ(  8, total_placement_count( sample_2 ));

    // Re-read the files.
    auto sample_3 = JplaceReader().from_file( infile_1 );
    auto sample_4 = JplaceReader().from_file( infile_2 );

    // Symmetric difference.
    filter_pqueries_differing_names( sample_3, sample_4 );
    EXPECT_EQ(  0, total_placement_count( sample_3 ));
    EXPECT_EQ(  2, total_placement_count( sample_4 ));
}

TEST( SampleFunctions, ConvertFromDefaultTree )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process a normal newick tree.
    std::string const infile = environment->data_dir + "tree/distances.newick";
    auto const def_tree = tree::DefaultTreeNewickReader().from_file( infile );

    // Convert it to a tree that is usable for samples.
    auto const place_tree = convert_default_tree_to_placement_tree( def_tree );

    // Check if the tree is correct.
    EXPECT_EQ( 13, place_tree.node_count() );
    EXPECT_TRUE( has_correct_edge_nums( place_tree ));
}

TEST( SampleTree, LabelledTree )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Get sample.
    std::string const infile = environment->data_dir + "placement/test_c.jplace";
    auto const sample = JplaceReader().from_file( infile );

    // Pre-checks
    EXPECT_EQ(  7, sample.size() );
    EXPECT_EQ( 10, sample.tree().node_count() );
    EXPECT_EQ(  9, sample.tree().edge_count() );

    // Get and check multifurcating tree.
    auto lm_tree = labelled_tree( sample, false );
    EXPECT_EQ( 25, lm_tree.node_count() );
    EXPECT_EQ( 24, lm_tree.edge_count() );

    // Get and check fully resolved tree.
    auto lf_tree = labelled_tree( sample, true );
    EXPECT_EQ( 26, lf_tree.node_count() );
    EXPECT_EQ( 25, lf_tree.edge_count() );
}
