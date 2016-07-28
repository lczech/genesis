/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include <string>

#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/default/newick_writer.hpp"
#include "lib/tree/function/functions.hpp"
#include "lib/tree/function/operators.hpp"
#include "lib/tree/formats/newick/color_writer_mixin.hpp"
#include "lib/tree/formats/newick/reader.hpp"
#include "lib/tree/formats/newick/writer.hpp"
#include "lib/tree/tree.hpp"
#include "lib/utils/text/string.hpp"

using namespace genesis;
using namespace tree;

TEST(Newick, FromAndToString)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    Tree tree;
    EXPECT_TRUE(DefaultTreeNewickReader().from_string(input, tree));
    std::string output = DefaultTreeNewickWriter().to_string(tree);

    EXPECT_EQ(input, output);
}

TEST(Newick, NewickVariants)
{
    Tree tree;

    // No nodes are named.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(,,(,));",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // Leaf nodes are named.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(A,B,(C,D));",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // All nodes are named.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(A,B,(C,D)E)F;",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // All but root node have a distance to parent.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(:0.1,:0.2,(:0.3,:0.4):0.5);",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // All have a distance to parent.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // Distances and leaf names (popular).
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // Distances and all names.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // A tree rooted on a leaf node (rare).
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );

    // All mixed, with comments and tags.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(
        "( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};",
        tree
    ));
    EXPECT_TRUE( validate_topology(tree) );
}

TEST(Newick, ColorMixin)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    Tree tree;
    typedef DefaultTreeNewickWriterMixin<NewickColorWriterMixin<NewickWriter>> ColorTreeNewickWriter;

    // Make sure that the mixin does not interfere with other Newick functionality. If it does, the
    // following line would hopefully crash.
    EXPECT_TRUE( DefaultTreeNewickReader().from_string(input, tree) );

    // Create a color vector for all edges that marks edges leading to a leaf node in red.
    auto color_vector = std::vector<utils::Color>( tree.edge_count() );
    for( auto it = tree.begin_edges(); it != tree.end_edges(); ++it ) {
        if( (*it)->primary_node().is_leaf() || (*it)->secondary_node().is_leaf() ) {
            color_vector[(*it)->index()] = utils::Color(255, 0, 0);
        }
    }

    // Use the color vector to produce a newick string with color tags.
    // We set ignored color to fuchsia ("magic pink") in order to also print out the black colored
    // inner edges.
    auto proc = ColorTreeNewickWriter();
    proc.edge_colors(color_vector);
    proc.ignored_color(utils::Color(255, 0, 255));
    std::string output = proc.to_string(tree);

    // Check if we actually got the right number of red color tag comments.
    auto count_red = utils::count_substring_occurrences( output, "[&!color=#ff0000]" );
    EXPECT_EQ( leaf_node_count(tree), count_red );

    // Check if we also got the right number of black color tag comments.
    // This is one fewer than the number of nodes, as no color tag is written for the root.
    auto count_black = utils::count_substring_occurrences( output, "[&!color=#000000]" );
    EXPECT_EQ( inner_node_count(tree) - 1, count_black );
}
