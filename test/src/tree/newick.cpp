/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief Testing Newick class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/tree/default/newick_reader.hpp"
#include "genesis/tree/default/newick_writer.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/formats/newick/color_writer_plugin.hpp"
#include "genesis/tree/formats/newick/input_iterator.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace tree;

TEST(Newick, FromAndToString)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    // Wead
    Tree tree = DefaultTreeNewickReader().from_string( input );

    // Write
    auto writer = DefaultTreeNewickWriter();
    writer.enable_branch_lengths( false );
    std::string output = writer.to_string( tree );

    EXPECT_EQ(input, output);
}

TEST(Newick, NewickVariants)
{
    Tree tree;
    std::string newick_string;
    auto writer = DefaultTreeNewickWriter();

    EXPECT_TRUE( tree.empty() );

    // First, no branch lengths.
    writer.enable_branch_lengths( false );

    // Stupid tree.
    newick_string = "();";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( 2, tree.node_count() );
    EXPECT_FALSE( tree.empty() );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // No nodes are named.
    newick_string = "(,,(,));";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // Leaf nodes are named.
    newick_string = "(A,B,(C,D));";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // All nodes are named.
    newick_string = "(A,B,(C,D)E)F;";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // Now, test with branch lengths.
    writer.enable_branch_lengths( true );

    // All but root node have a distance to parent.
    newick_string = "(:0.1,:0.2,(:0.3,:0.4):0.5);";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // All have a distance to parent.
    // We never write out the root branch length, so this test is skipped.
    newick_string = "(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    // EXPECT_EQ( newick_string, writer.to_string( tree ));

    // Distances and leaf names (popular).
    newick_string = "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // Distances and all names.
    newick_string = "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // A tree rooted on a leaf node (rare).
    newick_string = "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;";
    tree = DefaultTreeNewickReader().from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string, writer.to_string( tree ));

    // All mixed, with comments and tags. Need to activate tags first.
    // We here only test the reading, and check for a simple standard written output,
    // instead of the hassle of using custom plugins to also write tags etc...
    newick_string = "( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};";
    auto const newick_string2 = "((Ant:0.2,Bee:0.09)Inner:0.7,Coyote:0.5);";
    auto reader = DefaultTreeNewickReader();
    reader.enable_tags( true );
    tree = reader.from_string( newick_string );
    EXPECT_TRUE( validate_topology(tree) );
    EXPECT_EQ( newick_string2, writer.to_string( tree ));
}

TEST(Newick, ColorPlugin)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    Tree tree;

    // Prepare a Newick writer with color plugin functions.
    auto writer = DefaultTreeNewickWriter();
    auto color_plugin = NewickColorWriterPlugin();
    color_plugin.register_with( writer );

    // Make sure that the plugin does not interfere with other Newick functionality. If it does, the
    // following line would hopefully crash.
    tree = DefaultTreeNewickReader().from_string(input);

    // Create a color vector for all edges that marks edges leading to a leaf node in red.
    auto color_vector = std::vector<utils::Color>( tree.edge_count() );
    for( auto it = tree.begin_edges(); it != tree.end_edges(); ++it ) {
        if( (*it)->primary_node().is_leaf() || (*it)->secondary_node().is_leaf() ) {
            color_vector[(*it)->index()] = utils::Color(1, 0, 0);
        }
    }

    // Use the color vector to produce a newick string with color tags.
    // We set ignored color to fuchsia ("magic pink") in order to also print out the black colored
    // inner edges.
    color_plugin.edge_colors(color_vector);
    color_plugin.ignored_color(utils::Color(1, 0, 1));
    std::string output = writer.to_string(tree);

    // Check if we actually got the right number of red color tag comments.
    auto count_red = utils::count_substring_occurrences( output, "[&!color=#ff0000]" );
    EXPECT_EQ( leaf_node_count(tree), count_red );

    // Check if we also got the right number of black color tag comments.
    // This is one fewer than the number of nodes, as no color tag is written for the root.
    auto count_black = utils::count_substring_occurrences( output, "[&!color=#000000]" );
    EXPECT_EQ( inner_node_count(tree) - 1, count_black );
}

TEST( Newick, MultipleTrees )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Open a file stream.
    std::string infile = environment->data_dir + "tree/multiple.newick";
    utils::InputStream instream( utils::make_unique< utils::FileInputSource >( infile ));

    // Get the iterator and start reading.
    auto tree_iter = NewickInputIterator( instream );

    while( tree_iter ) {
        EXPECT_EQ( 6, tree_iter->node_count() );
        ++tree_iter;
    }
}

TEST( Newick, MultipleNamedTrees )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Open a file stream.
    std::string infile = environment->data_dir + "tree/multiple_named.newick";
    utils::InputStream instream( utils::make_unique< utils::FileInputSource >( infile ));

    // Get the iterator and start reading.
    auto tree_iter = NewickInputIterator( instream );

    while( tree_iter ) {
        EXPECT_EQ( 6, tree_iter->node_count() );
        ++tree_iter;
    }
}
