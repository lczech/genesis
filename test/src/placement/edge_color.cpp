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
#include "lib/tree/default/functions.hpp"
#include "lib/tree/io/newick/color_mixin.hpp"
#include "lib/tree/io/phyloxml/color_mixin.hpp"
#include "lib/utils/io/nexus/document.hpp"
#include "lib/utils/io/nexus/taxa.hpp"
#include "lib/utils/io/nexus/trees.hpp"
#include "lib/utils/io/nexus/writer.hpp"

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
    EXPECT_TRUE( out.find("<red>255</red>")  != std::string::npos );
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
    EXPECT_TRUE( out.find("color=#ff0000") != std::string::npos );
}

TEST( PlacementTreeEdgeColor, CountGradientNexus )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    PlacementMap map;
    EXPECT_TRUE (JplaceProcessor().from_file(infile, map));

    typedef NewickColorMixin<PlacementTreeNewickProcessor> ColoredPlacementTreeNewickProcessor;

    auto proc = ColoredPlacementTreeNewickProcessor();
    proc.edge_colors( placement_color_count_gradient( map.tree() ));
    proc.enable_edge_nums(false);
    std::string tree_out = proc.to_string(map.tree());

    auto doc = nexus::Document();

    auto taxa = make_unique<nexus::Taxa>();
    taxa->add_taxa(node_names(map.tree()));
    doc.set_block( std::move(taxa) );

    auto trees = make_unique<nexus::Trees>();
    trees->add_tree( "tree1", tree_out );
    doc.set_block( std::move(trees) );

    EXPECT_TRUE( doc.has_block("TREES") );
    EXPECT_TRUE( doc.has_block("TAXA") );

    std::ostringstream buffer;

    auto writer = nexus::Writer();
    writer.to_stream( doc, buffer );
    auto nexus_out = buffer.str();

    EXPECT_TRUE( nexus_out.find("color=#ff0000") != std::string::npos );
}
