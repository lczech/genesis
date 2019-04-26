/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @brief Testing Tree Iterators.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>
#include <utility>

#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Levelorder
// =================================================================================================

void TestLevelorder(std::string node_name, std::string out_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));

    auto node = find_node( tree, node_name );
    ASSERT_NE( nullptr, node );

    for( auto it : levelorder( *node )) {
        nodes += std::to_string( it.depth() )
              +  it.node().data<CommonNodeData>().name
              +  " ";
    }
    EXPECT_EQ( out_nodes, utils::trim( nodes )) << " with start node " << node_name;
}

TEST (TreeIterator, Levelorder)
{
    // Similar test as in rerooting the tree.
    TestLevelorder( "R", "0R 1A 1F 1G 2B 2C 2H 2I 3D 3E" );
    TestLevelorder( "A", "0A 1R 1B 1C 2F 2G 2D 2E 3H 3I" );
    TestLevelorder( "B", "0B 1A 2C 2R 3D 3E 3F 3G 4H 4I" );
    TestLevelorder( "C", "0C 1A 1D 1E 2R 2B 3F 3G 4H 4I" );
    TestLevelorder( "D", "0D 1C 2E 2A 3R 3B 4F 4G 5H 5I" );
    TestLevelorder( "E", "0E 1C 2A 2D 3R 3B 4F 4G 5H 5I" );
    TestLevelorder( "F", "0F 1R 2G 2A 3H 3I 3B 3C 4D 4E" );
    TestLevelorder( "G", "0G 1R 1H 1I 2A 2F 3B 3C 4D 4E" );
    TestLevelorder( "H", "0H 1G 2I 2R 3A 3F 4B 4C 5D 5E" );
    TestLevelorder( "I", "0I 1G 2R 2H 3A 3F 4B 4C 5D 5E" );
}

void TestLevelorderSubtree( Subtree const& subtree, const std::string expected_nodes )
{
    std::string nodes = "";
    auto const name = subtree.node().data<CommonNodeData>().name + "(" + std::to_string( subtree.link().index() ) + ")";

    // Do a normal traversal.
    for( auto it : levelorder(subtree) ) {
        // nodes += it.node().data<CommonNodeData>().name;

        nodes += std::to_string( it.depth() )
              +  it.node().data<CommonNodeData>().name
              +  " ";
    }
    EXPECT_EQ( expected_nodes, utils::trim( nodes )) << " with start node " << name;

    // Use free function iterator wrapper.
    nodes = "";
    for( auto it = levelorder(subtree).begin(); it != levelorder(subtree).end(); ++it ) {
        // nodes += it.node().data<CommonNodeData>().name;

        nodes += std::to_string( it.depth() )
              +  it.node().data<CommonNodeData>().name
              +  " ";
    }
    EXPECT_EQ( expected_nodes, utils::trim( nodes )) << " with start node " << name;
}

TEST (TreeIterator, LevelorderSubtree)
{
    // Prepare Tree.
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));

    // The following heavily depends on the internal tree structure.
    // If this breaks, we might need a setup that finds nodes,
    // instead of assuming certain link indices. But for now, this is good enough and stable.
    TestLevelorderSubtree( Subtree{ tree.link_at(0) }, "0R 1F 1G 2H 2I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(1) }, "0R 1G 1A 2H 2I 2B 2C 3D 3E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(2) }, "0R 1A 1F 2B 2C 3D 3E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(3) }, "0G 1H 1I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(4) }, "0G 1I 1R 2A 2F 3B 3C 4D 4E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(5) }, "0G 1R 1H 2A 2F 3B 3C 4D 4E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(6) }, "0I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(7) }, "0H" );
    TestLevelorderSubtree( Subtree{ tree.link_at(8) }, "0F" );
    TestLevelorderSubtree( Subtree{ tree.link_at(9) }, "0A 1B 1C 2D 2E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(10) }, "0A 1C 1R 2D 2E 2F 2G 3H 3I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(11) }, "0A 1R 1B 2F 2G 3H 3I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(12) }, "0C 1D 1E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(13) }, "0C 1E 1A 2R 2B 3F 3G 4H 4I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(14) }, "0C 1A 1D 2R 2B 3F 3G 4H 4I" );
    TestLevelorderSubtree( Subtree{ tree.link_at(15) }, "0E" );
    TestLevelorderSubtree( Subtree{ tree.link_at(16) }, "0D" );
    TestLevelorderSubtree( Subtree{ tree.link_at(17) }, "0B" );
}
