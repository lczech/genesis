/**
 * @brief Testing Tree class.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

#include <string>

#include "common.hpp"

#include "lib/placement/jplace_processor.hpp"
#include "lib/placement/placement_map.hpp"

using namespace genesis;

TEST(JplaceProcessorTest, FromFile)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA

    std::string infile = environment->data_dir + "test_a.jplace";

    PlacementMap map;
    ASSERT_EQ (true, JplaceProcessor::from_file(infile, map));
	EXPECT_EQ (5, map.placement_count());
}
