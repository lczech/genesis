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
 * @brief Testing PlacementSimulator class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>
#include <string>
#include <vector>

#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/io/newick_reader.hpp"
#include "lib/placement/sample.hpp"
#include "lib/placement/simulator/functions.hpp"
#include "lib/placement/simulator/simulator.hpp"
#include "lib/tree/io/newick/reader.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(PlacementSimulator, TwoStepSimple)
{
    auto tree = PlacementTree();
    EXPECT_TRUE( PlacementTreeNewickReader().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
        tree
    ));

    Sample smp(tree);
    EXPECT_EQ   (0, total_placement_count(smp));
    EXPECT_TRUE (validate(smp, true, false));

    Simulator sim;

    size_t n = 100;
    sim.generate(smp, n);
    EXPECT_EQ   (n, total_placement_count(smp));
    EXPECT_TRUE (validate(smp, true, false));
}

TEST(PlacementSimulator, TwoStepLeavesOnly)
{
    // Build a simple tree.
    auto tree = PlacementTree();
    EXPECT_TRUE( PlacementTreeNewickReader().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
        tree
    ));

    Sample smp(tree);
    Simulator sim;

    // Set weight so that onlye edges adjacet to leaves are populated.
    std::vector<int> edge_weights = {1};
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
