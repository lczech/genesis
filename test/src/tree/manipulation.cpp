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

#include "lib/tree/default/functions.hpp"
#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/default/tree.hpp"
#include "lib/tree/formats/newick/reader.hpp"
#include "lib/tree/iterator/levelorder.hpp"
#include "lib/tree/tree.hpp"
#include "lib/utils/text/string.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Reroot
// =================================================================================================

void TestReroot( std::string root_node_name, std::string out_nodes, size_t nexts = 0 )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree;
    DefaultTreeNewickReader().from_string( input, tree );

    auto root_node = find_node( tree, root_node_name );
    ASSERT_NE( nullptr, root_node );

    // Move to the 'next' link of the new root link, so that we get different root links
    // of the new root node.
    auto root_link = &root_node->link();
    for( size_t i = 0; i < nexts; ++i ) {
        root_link = &root_link->next();
    }

    // Root the tree at the given link, validate its pointers.
    tree.reroot( *root_link );
    EXPECT_TRUE( validate_topology( tree ));

    // Build a string of the nodes in levelorder, starting from the new root.
    for( auto it : levelorder( tree ) ) {
        nodes += std::to_string( it.depth() )
              +  it.node().data<DefaultNodeData>().name
              +  " ";
    }

    // Check if the levelorder outcome is correct.
    EXPECT_EQ( out_nodes, utils::trim( nodes )) << " with start node " << root_node_name;
}

TEST( TreeManipulation, Reroot )
{
    // Test normal rerooting.
    TestReroot( "R", "0R 1A 1F 1G 2B 2C 2H 2I 3D 3E" );
    TestReroot( "A", "0A 1R 1B 1C 2F 2G 2D 2E 3H 3I" );
    TestReroot( "B", "0B 1A 2C 2R 3D 3E 3F 3G 4H 4I" );
    TestReroot( "C", "0C 1A 1D 1E 2R 2B 3F 3G 4H 4I" );
    TestReroot( "D", "0D 1C 2E 2A 3R 3B 4F 4G 5H 5I" );
    TestReroot( "E", "0E 1C 2A 2D 3R 3B 4F 4G 5H 5I" );
    TestReroot( "F", "0F 1R 2G 2A 3H 3I 3B 3C 4D 4E" );
    TestReroot( "G", "0G 1R 1H 1I 2A 2F 3B 3C 4D 4E" );
    TestReroot( "H", "0H 1G 2I 2R 3A 3F 4B 4C 5D 5E" );
    TestReroot( "I", "0I 1G 2R 2H 3A 3F 4B 4C 5D 5E" );

    // Test rerooting with a specific link.
    TestReroot( "R", "0R 1F 1G 1A 2H 2I 2B 2C 3D 3E", 1 );
    TestReroot( "R", "0R 1G 1A 1F 2H 2I 2B 2C 3D 3E", 2 );
    TestReroot( "A", "0A 1B 1C 1R 2D 2E 2F 2G 3H 3I", 1 );
    TestReroot( "A", "0A 1C 1R 1B 2D 2E 2F 2G 3H 3I", 2 );
    TestReroot( "C", "0C 1D 1E 1A 2R 2B 3F 3G 4H 4I", 1 );
    TestReroot( "C", "0C 1E 1A 1D 2R 2B 3F 3G 4H 4I", 2 );
    TestReroot( "G", "0G 1H 1I 1R 2A 2F 3B 3C 4D 4E", 1 );
    TestReroot( "G", "0G 1I 1R 1H 2A 2F 3B 3C 4D 4E", 2 );
}
