/**
 * @brief Testing PlacementMap class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>

#include "lib/placement/newick_adapter.hpp"
#include "lib/placement/jplace_processor.hpp"
#include "lib/placement/placement_map.hpp"
#include "lib/tree/newick_processor.hpp"

using namespace genesis;

TEST(PlacementMap, WithTree)
{
    auto tree = std::make_shared<PlacementTree>();
    EXPECT_TRUE (PlacementTreeNewickProcessor().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
    *tree));

    PlacementMap map(tree);
	EXPECT_EQ   (0, map.placement_count());
    EXPECT_TRUE (map.validate(true, false));
}

TEST(PlacementMap, MergeDuplicates)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA

    std::string infile = environment->data_dir + "duplicates.jplace";

    PlacementMap map;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map));
	EXPECT_EQ   (7, map.pquery_size());
	EXPECT_EQ   (8, map.placement_count());
    EXPECT_TRUE (map.validate(true, false));

    LOG_DBG << map.dump();

    merge_duplicates(map);
    EXPECT_EQ   (3, map.pquery_size());
	EXPECT_EQ   (8, map.placement_count());

    LOG_DBG << map.dump();
}
