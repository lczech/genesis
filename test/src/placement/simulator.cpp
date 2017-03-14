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
 * @brief Testing PlacementSimulator class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/formats/newick_reader.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/placement/simulator/functions.hpp"
#include "genesis/placement/simulator/simulator.hpp"
#include "genesis/tree/formats/newick/reader.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(PlacementSimulator, Simple)
{
    auto tree = PlacementTreeNewickReader().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};"
    );

    Sample smp(tree);
    EXPECT_EQ   (0, total_placement_count(smp));
    EXPECT_TRUE (validate(smp, true, false));

    Simulator sim;

    size_t n = 100;
    sim.generate(smp, n);
    EXPECT_EQ   (n, total_placement_count(smp));
    EXPECT_TRUE (validate(smp, true, false));
}

TEST(PlacementSimulator, LeavesOnly)
{
    // Build a simple tree.
    auto tree = PlacementTreeNewickReader().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};"
    );

    Sample smp(tree);
    Simulator sim;

    // Set weight so that onlye edges adjacet to leaves are populated.
    std::vector<double> edge_weights = { 1.0 };
    set_depths_distributed_weights( smp, edge_weights, sim.edge_distribution() );

    // Generate placements.
    size_t n = 100;
    sim.generate(smp, n);
    EXPECT_EQ   (n, total_placement_count(smp));
    EXPECT_TRUE (validate(smp, true, false));

    // Check whether all placements are next to leaf nodes.
    for (auto& pqry : smp.pqueries()) {
        auto const& edge = pqry.placement_at(0).edge();
        EXPECT_TRUE (edge.primary_node().is_leaf() || edge.secondary_node().is_leaf());
    }
}

TEST(PlacementSimulator, Learning)
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // In file.
    std::string infile  = environment->data_dir + "placement/test_a.jplace";

    // Read the Jplace file into a Sample object.
    Sample sample = JplaceReader().from_file( infile );

    // Learn simulation parameters.
    auto sim = Simulator();
    learn_per_edge_weights( sample, sim.edge_distribution() );
    learn_placement_number_weights( sample, sim.extra_placement_distribution() );
    learn_placement_path_length_weights( sample, sim.extra_placement_distribution() );
    learn_like_weight_ratio_distribution( sample, sim.like_weight_ratio_distribution(), 25 );

    // Check edge weight sum.
    double sum_edge_weights = std::accumulate(
        sim.edge_distribution().edge_weights.begin(),
        sim.edge_distribution().edge_weights.end(),
        0.0
    );
    EXPECT_EQ( total_placement_mass( sample ), sum_edge_weights );

    auto& placement_number_weights = sim.extra_placement_distribution().placement_number_weights;

    // Check extra placement weight sum.
    double sum_extra_placement_weights = std::accumulate(
        placement_number_weights.begin(),
        placement_number_weights.end(),
        0
    );
    EXPECT_EQ( sample.size(), sum_extra_placement_weights );

    // Check sum of path lengths weights.
    // This check is a bit more complicatted. We accumulate the weights that denote the distribution
    // of how long the path between two placements of a pquery is. This sum is stored in
    // `sum_path_length_weights`. Furthermore, as the learning function used to get those weights
    // leaves out the paths of length 0 (they are not needed in the simulation), we also add this
    // number, which is the value of total_placement_count(). This is because every placement has
    // a path length of 0 to itself.
    // The resulting value is then compared to the expected value, which is calculated as follows.
    // We expect as many weights as there are combinations of placements within all pqueries.
    // Those combinations come from a pairwise comparison of all placments of a pquery, thus it is
    // quadratic (the i*i component) in the number of placements per pquery (we also need to add 1
    // to the i's, as the numbers actually mean "extra" placements). This is multiplied by the
    // number of pqueries that have `i` many placements.
    // In other words: There are x = placement_number_weights[ i ] many pqueries that have (i+1)
    // many placements. Take this x times the number of combinations of those placements, which is
    // (i+1)*(i+1), and add this up for all pqueries, to get the total expected number of paths
    // between all placements of the pqueries.
    double possible_placement_combinations = 0;
    for( size_t i = 0; i < placement_number_weights.size(); ++i ) {
        possible_placement_combinations += (i+1) * (i+1) * placement_number_weights[ i ];
    }
    double sum_path_length_weights = std::accumulate(
        sim.extra_placement_distribution().placement_path_length_weights.begin(),
        sim.extra_placement_distribution().placement_path_length_weights.end(),
        0
    );
    sum_path_length_weights += total_placement_count( sample );
    EXPECT_EQ( possible_placement_combinations, sum_path_length_weights );

    // Check lwr weight sum.
    size_t sum_lwr_weights = std::accumulate(
        sim.like_weight_ratio_distribution().weights.begin(),
        sim.like_weight_ratio_distribution().weights.end(),
        0
    );
    EXPECT_EQ( total_placement_count( sample ), sum_lwr_weights );
}
