/**
 * @brief Testing PlacementMap class.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

#include <cstdio>
#include <memory>
#include <string>

#include "common.hpp"

#include "lib/placement/jplace_processor.hpp"
#include "lib/placement/placement_map.hpp"
#include "lib/tree/newick_processor.hpp"

using namespace genesis;

TEST(PlacementMap, WithTree)
{
    auto tree = std::make_shared<PlacementTree>();
    EXPECT_TRUE (NewickProcessor().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
    *tree));

    PlacementMap map(tree);
	EXPECT_EQ   (0, map.placement_count());
    EXPECT_TRUE (map.validate());
}

TEST(PlacementMap, SaveAndLoad)
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA

    // In and out files.
    std::string infile  = environment->data_dir + "test_a.jplace";
    std::string tmpfile = environment->data_dir + "test_a.bplace";

    // Prepare a PlacementMap with data.
    PlacementMap map;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map));
    EXPECT_EQ   (5, map.placement_count());
    EXPECT_TRUE (map.validate());

    // Save it to a file.
    EXPECT_TRUE (map.save(tmpfile));

    // Make sure the file is deleted.
    ASSERT_EQ (0, std::remove(tmpfile.c_str()));
}
