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

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/tree.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Postorder
// =================================================================================================

void TestPostorder(std::string node_name, std::string out_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree = CommonTreeNewickReader().read( utils::from_string(input));

    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    for( auto it : postorder(*node) ) {
        nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ(out_nodes, nodes) << " with start node " << node_name;
}

TEST (TreeIterator, Postorder)
{
    TestPostorder("R", "BDECAFHIGR");
    TestPostorder("A", "FHIGRBDECA");
    TestPostorder("B", "DECFHIGRAB");
    TestPostorder("C", "FHIGRBADEC");
    TestPostorder("D", "EFHIGRBACD");
    TestPostorder("E", "FHIGRBADCE");
    TestPostorder("F", "HIGBDECARF");
    TestPostorder("G", "BDECAFRHIG");
    TestPostorder("H", "IBDECAFRGH");
    TestPostorder("I", "BDECAFRHGI");
}

void TestPostorderSubtree( Subtree const& subtree, const std::string expected_nodes )
{
    std::string resulting_nodes = "";
    auto const name = subtree.node().data<CommonNodeData>().name + "(" + std::to_string( subtree.link().index() ) + ")";

    // Do a normal traversal.
    for( auto it : postorder(subtree) ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ( expected_nodes, resulting_nodes ) << " with start node " << name;

    // Use free function iterator wrapper.
    resulting_nodes = "";
    for( auto it = postorder(subtree).begin(); it != postorder(subtree).end(); ++it ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << name;
}

TEST (TreeIterator, PostorderSubtree)
{
    // Prepare Tree.
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));

    // The following heavily depends on the internal tree structure.
    // If this breaks, we might need a setup that finds nodes,
    // instead of assuming certain link indices. But for now, this is good enough and stable.
    TestPostorderSubtree( Subtree{ tree.link_at(0) }, "FHIGR" );
    TestPostorderSubtree( Subtree{ tree.link_at(1) }, "HIGBDECAR" );
    TestPostorderSubtree( Subtree{ tree.link_at(2) }, "BDECAFR" );
    TestPostorderSubtree( Subtree{ tree.link_at(3) }, "HIG" );
    TestPostorderSubtree( Subtree{ tree.link_at(4) }, "IBDECAFRG" );
    TestPostorderSubtree( Subtree{ tree.link_at(5) }, "BDECAFRHG" );
    TestPostorderSubtree( Subtree{ tree.link_at(6) }, "I" );
    TestPostorderSubtree( Subtree{ tree.link_at(7) }, "H" );
    TestPostorderSubtree( Subtree{ tree.link_at(8) }, "F" );
    TestPostorderSubtree( Subtree{ tree.link_at(9) }, "BDECA" );
    TestPostorderSubtree( Subtree{ tree.link_at(10) }, "DECFHIGRA" );
    TestPostorderSubtree( Subtree{ tree.link_at(11) }, "FHIGRBA" );
    TestPostorderSubtree( Subtree{ tree.link_at(12) }, "DEC" );
    TestPostorderSubtree( Subtree{ tree.link_at(13) }, "EFHIGRBAC" );
    TestPostorderSubtree( Subtree{ tree.link_at(14) }, "FHIGRBADC" );
    TestPostorderSubtree( Subtree{ tree.link_at(15) }, "E" );
    TestPostorderSubtree( Subtree{ tree.link_at(16) }, "D" );
    TestPostorderSubtree( Subtree{ tree.link_at(17) }, "B" );
}
