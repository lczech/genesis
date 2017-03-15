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
 * @brief Testing Tree Iterators.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>
#include <utility>

#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/default/newick_reader.hpp"
#include "genesis/tree/default/tree.hpp"
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

    Tree tree = DefaultTreeNewickReader().from_string( input );

    auto node = find_node( tree, node_name );
    ASSERT_NE( nullptr, node );

    for( auto it : levelorder( *node )) {
        nodes += std::to_string( it.depth() )
              +  it.node().data<DefaultNodeData>().name
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
