/**
 * @brief Testing PlacementMap class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <cstdio>
#include <memory>
#include <string>

#include "lib/placement/io/jplace_processor.hpp"
#include "lib/placement/placement_map.hpp"
#include "lib/placement/io/serializer.hpp"
#include "lib/tree/io/newick_processor.hpp"

using namespace genesis;

TEST(PlacementMapSerializer, SaveAndLoad)
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA

    // In and out files.
    std::string infile  = environment->data_dir + "placement/test_a.jplace";
    std::string tmpfile = environment->data_dir + "placement/test_a.bplace";

    // Prepare a PlacementMap with data.
    PlacementMap map_save;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map_save));
    EXPECT_EQ   (5, map_save.placement_count());
    EXPECT_TRUE (map_save.validate(true, false));

    // Save it to a file.
    EXPECT_TRUE (PlacementMapSerializer::save(map_save, tmpfile));

    // Load again.
    PlacementMap map_load;
    EXPECT_TRUE (PlacementMapSerializer::load(tmpfile, map_load));

    // Check for correctly read data.
    EXPECT_EQ   (5, map_load.placement_count());
    EXPECT_TRUE (map_load.validate(true, false));

    // Make sure the file is deleted.
    ASSERT_EQ (0, std::remove(tmpfile.c_str()));
}
