/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @brief Testing Placement Tree Phyloxml Processor class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/placement/formats/edge_color.hpp"
#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/formats/newick_writer.hpp"
#include "genesis/tree/formats/phyloxml/writer.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/default/phyloxml_writer.hpp"
#include "genesis/tree/formats/newick/color_writer_plugin.hpp"
#include "genesis/tree/formats/phyloxml/color_writer_plugin.hpp"
#include "genesis/utils/formats/nexus/document.hpp"
#include "genesis/utils/formats/nexus/taxa.hpp"
#include "genesis/utils/formats/nexus/trees.hpp"
#include "genesis/utils/formats/nexus/writer.hpp"

using namespace genesis;
using namespace genesis::placement;
using namespace genesis::tree;
using namespace genesis::utils;

TEST( PlacementTreeEdgeColor, CountGradientPhyloxml )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    Sample map = JplaceReader().from_file(infile);

    auto writer = tree::DefaultTreePhyloxmlWriter();
    auto color_plugin = PhyloxmlColorWriterPlugin();
    color_plugin.register_with( writer );

    color_plugin.edge_colors( placement_color_count_gradient( map ));
    std::string out = writer.to_string(map.tree());

    // At least one element in the output should have the color for the edge with most placements.
    EXPECT_TRUE( out.find("<red>255</red>")  != std::string::npos );
    // EXPECT_NE(std::string::npos, out.find("<red>255</red>")) << "Red edge not found.";
}

TEST( PlacementTreeEdgeColor, CountGradientNewick )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    Sample map = JplaceReader().from_file(infile);

    // Create a writer and add a color plugin.
    auto writer = PlacementTreeNewickWriter();
    auto color_plugin = NewickColorWriterPlugin();
    color_plugin.register_with( writer );

    color_plugin.edge_colors( placement_color_count_gradient( map ));
    std::string out = writer.to_string(map.tree());

    // At least one element in the output should have the color for the edge with most placements.
    EXPECT_TRUE( out.find("color=#ff0000") != std::string::npos );
}

TEST( PlacementTreeEdgeColor, CountGradientNexus )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    Sample map = JplaceReader().from_file(infile);

    // Create a writer and add a color plugin.
    auto writer = PlacementTreeNewickWriter();
    auto color_plugin = NewickColorWriterPlugin();
    color_plugin.register_with( writer );

    color_plugin.edge_colors( placement_color_count_gradient( map ));
    writer.enable_edge_nums(false);
    std::string tree_out = writer.to_string(map.tree());

    auto doc = NexusDocument();

    auto taxa = make_unique<NexusTaxa>();
    taxa->add_taxa(node_names(map.tree()));
    doc.set_block( std::move(taxa) );

    auto trees = make_unique<NexusTrees>();
    trees->add_tree( "tree1", tree_out );
    doc.set_block( std::move(trees) );

    EXPECT_TRUE( doc.has_block("TREES") );
    EXPECT_TRUE( doc.has_block("TAXA") );

    std::ostringstream buffer;

    auto nexus_writer = NexusWriter();
    nexus_writer.to_stream( doc, buffer );
    auto nexus_out = buffer.str();

    EXPECT_TRUE( nexus_out.find("color=#ff0000") != std::string::npos );
}
