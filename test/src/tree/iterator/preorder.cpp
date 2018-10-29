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
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Preorder
// =================================================================================================

void TestPreorder(std::string node_name, std::string out_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree = CommonTreeNewickReader().from_string(input);

    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    for( auto it : tree::preorder(*node) ) {
        nodes += it.node().data<tree::CommonNodeData>().name;
    }
    EXPECT_EQ(out_nodes, nodes) << " with start node " << node_name;
}

TEST (TreeIterator, Preorder)
{
    TestPreorder("R", "RABCDEFGHI");
    TestPreorder("A", "ARFGHIBCDE");
    TestPreorder("B", "BACDERFGHI");
    TestPreorder("C", "CARFGHIBDE");
    TestPreorder("D", "DCEARFGHIB");
    TestPreorder("E", "ECARFGHIBD");
    TestPreorder("F", "FRGHIABCDE");
    TestPreorder("G", "GRABCDEFHI");
    TestPreorder("H", "HGIRABCDEF");
    TestPreorder("I", "IGRABCDEFH");
}

void TestPreorderSubtree( Subtree const& subtree, const std::string expected_nodes )
{
    std::string resulting_nodes = "";
    auto const name = subtree.node().data<CommonNodeData>().name + "(" + std::to_string( subtree.link().index() ) + ")";

    // Do a normal traversal.
    for( auto it : preorder(subtree) ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ( expected_nodes, resulting_nodes ) << " with start node " << name;

    // Use free function iterator wrapper.
    resulting_nodes = "";
    for( auto it = preorder(subtree).begin(); it != preorder(subtree).end(); ++it ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << name;
}

TEST (TreeIterator, PreorderSubtree)
{
    // Prepare Tree.
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    Tree tree = CommonTreeNewickReader().from_string( input );

    // The following heavily depends on the internal tree structure.
    // If this breaks, we might need a setup that finds nodes,
    // instead of assuming certain link indices. But for now, this is good enough and stable.
    TestPreorderSubtree( { tree.link_at(0) }, "RFGHI" );
    TestPreorderSubtree( { tree.link_at(1) }, "RGHIABCDE" );
    TestPreorderSubtree( { tree.link_at(2) }, "RABCDEF" );
    TestPreorderSubtree( { tree.link_at(3) }, "GHI" );
    TestPreorderSubtree( { tree.link_at(4) }, "GIRABCDEF" );
    TestPreorderSubtree( { tree.link_at(5) }, "GRABCDEFH" );
    TestPreorderSubtree( { tree.link_at(6) }, "I" );
    TestPreorderSubtree( { tree.link_at(7) }, "H" );
    TestPreorderSubtree( { tree.link_at(8) }, "F" );
    TestPreorderSubtree( { tree.link_at(9) }, "ABCDE" );
    TestPreorderSubtree( { tree.link_at(10) }, "ACDERFGHI" );
    TestPreorderSubtree( { tree.link_at(11) }, "ARFGHIB" );
    TestPreorderSubtree( { tree.link_at(12) }, "CDE" );
    TestPreorderSubtree( { tree.link_at(13) }, "CEARFGHIB" );
    TestPreorderSubtree( { tree.link_at(14) }, "CARFGHIBD" );
    TestPreorderSubtree( { tree.link_at(15) }, "E" );
    TestPreorderSubtree( { tree.link_at(16) }, "D" );
    TestPreorderSubtree( { tree.link_at(17) }, "B" );
}
