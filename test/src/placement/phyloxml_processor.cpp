/**
 * @brief Testing Placement Tree Phyloxml Processor class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/placement/io/edge_color.hpp"
#include "lib/placement/io/jplace_processor.hpp"
#include "lib/placement/io/phyloxml_processor.hpp"
#include "lib/placement/placement_map.hpp"
#include "lib/utils/core/std.hpp"

using namespace genesis;

TEST(PlacementTreePhyloxmlProcessor, CountGradient)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    PlacementMap map;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map));

    auto proc = PlacementTreePhyloxmlProcessor();
    proc.edge_color_vector( placement_color_count_gradient( map.tree() ));
    std::string pxml = proc.to_string(map.tree());

    // At least one element in the output should have the color for the edge with most placements.
    EXPECT_TRUE(contains(pxml, std::string("<red>255</red>")));
    // EXPECT_NE(std::string::npos, pxml.find("<red>255</red>")) << "Red edge not found.";
}
