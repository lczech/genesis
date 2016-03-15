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

#include "lib/placement/io/newick_processor.hpp"
#include "lib/placement/sample.hpp"
#include "lib/placement/simulator.hpp"
#include "lib/tree/io/newick/processor.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(PlacementSimulator, TwoStepSimple)
{
    auto tree = std::make_shared<PlacementTree>();
    EXPECT_TRUE( PlacementTreeNewickProcessor().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
        *tree
    ));

    Sample map(tree);
    EXPECT_EQ   (0, map.placement_count());
    EXPECT_TRUE (map.validate(true, false));

    PlacementSimulatorTwostep sim(map);

    size_t n = 100;
    sim.generate(n);
    EXPECT_EQ   (n, map.placement_count());
    EXPECT_TRUE (map.validate(true, false));
}

TEST(PlacementSimulator, TwoStepLeavesOnly)
{
    // Build a simple tree.
    auto tree = std::make_shared<PlacementTree>();
    EXPECT_TRUE( PlacementTreeNewickProcessor().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
        *tree
    ));

    Sample map(tree);
    PlacementSimulatorTwostep sim(map);

    // Set weight so that onlye edges adjacet to leaves are populated.
    std::vector<int> edge_weights = {1};
    sim.edge_distribution().set_depths_distributed_weights(edge_weights);

    // Generate placements.
    size_t n = 100;
    sim.generate(n);
    EXPECT_EQ   (n, map.placement_count());
    EXPECT_TRUE (map.validate(true, false));

    // Check whether all placements are next to leaf nodes.
    for (auto& pqry : map.pqueries()) {
        auto edge = pqry->placements[0]->edge;
        EXPECT_TRUE (edge->primary_node()->is_leaf() || edge->secondary_node()->is_leaf());
    }
}
