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
#include "lib/placement/io/newick_processor.hpp"
#include "lib/placement/io/phyloxml_processor.hpp"
#include "lib/placement/placement_map.hpp"
#include "lib/utils/core/std.hpp"
#include "tree/io/newick/color_mixin.hpp"
#include "tree/io/phyloxml/color_mixin.hpp"

using namespace genesis;

TEST( PlacementTreeEdgeColor, CountGradientPhyloxml )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    PlacementMap map;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map));

    typedef PhyloxmlColorMixin<PlacementTreePhyloxmlProcessor> ColoredPlacementTreePhyloxmlProcessor;

    auto proc = ColoredPlacementTreePhyloxmlProcessor();
    proc.edge_colors( placement_color_count_gradient( map.tree() ));
    std::string out = proc.to_string(map.tree());

    // At least one element in the output should have the color for the edge with most placements.
    EXPECT_TRUE( contains(out, std::string("<red>255</red>")) );
    // EXPECT_NE(std::string::npos, out.find("<red>255</red>")) << "Red edge not found.";
}

TEST( PlacementTreeEdgeColor, CountGradientNewick )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    PlacementMap map;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map));

    typedef NewickColorMixin<PlacementTreeNewickProcessor> ColoredPlacementTreeNewickProcessor;

    auto proc = ColoredPlacementTreeNewickProcessor();
    proc.edge_colors( placement_color_count_gradient( map.tree() ));
    std::string out = proc.to_string(map.tree());

    // At least one element in the output should have the color for the edge with most placements.
    EXPECT_TRUE( contains(out, std::string("color=#ff0000")) );
}
