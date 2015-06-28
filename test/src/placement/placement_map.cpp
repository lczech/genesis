/**
 * @brief Testing PlacementMap class.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "common.hpp"

// #include "lib/placement/jplace_processor.hpp"
#include "lib/placement/placement_map.hpp"
#include "lib/tree/newick_processor.hpp"

using namespace genesis;

TEST(PlacementMapTest, WithTree)
{
    auto tree = std::make_shared<PlacementTree>();
    EXPECT_TRUE (NewickProcessor().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
    *tree));

    PlacementMap map(tree);
	EXPECT_EQ   (0, map.placement_count());
    EXPECT_TRUE (map.validate());
}
