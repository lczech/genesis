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
#include "genesis/tree/iterator/eulertour.hpp"
#include "genesis/tree/tree.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Eulertour
// =================================================================================================

TEST (TreeIterator, EulertourNew)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";

    std::string node_name = "R";
    std::string expected_nodes = "RABACDCECARFRGHGIG";

    // Prepare Tree.
    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));

    // Find the Node for this test run.
    auto const& ttr = tree;
    const auto node = find_node(ttr, node_name);
    ASSERT_NE(nullptr, node);

    TreeNode const& nn = *node;

    std::string resulting_nodes = "";

    // for (auto it = eulertour(node).begin(); it != eulertour(node).end(); ++it) {
    //     resulting_nodes += it.node()->data.name;
    //     // it.node()->data.name = "bla";
    // }

    // for( auto const& it : TreeIteratorEulertourRange<CommonTree>(tree) ) {
    //     resulting_nodes += it.node()->data.name;
    // }

    // for( auto const& it : eulertour(tree) ) {
    //     resulting_nodes += it->node().data.name;
    //
    //     // it->node()->data.name = "bla";
    // }

    // for( auto it : eulertour_from_link(nn.link()) ) {
    // for( auto it : eulertour( ttr ) ) {
    for( auto it : eulertour( nn ) ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
        // it.node().data.name = "bla";
        // resulting_nodes += it->node().data.name;
    }

    // for( auto const& it : eulertour(tree) ) {
    //     resulting_nodes += it.node()->data.name;
    // }

    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;
}

/*
class Eulertour : public ::testing::TestWithParam< std::pair<std::string, std::string> >
{};

TEST_P (Eulertour, Test)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    CommonTree tree;
    CommonTreeNewickProcessor().read( utils::from_string(input, tree));

    auto node = tree.find_node(GetParam().first);
    ASSERT_NE(nullptr, node);

    for (auto it = tree.begin_eulertour(node); it != tree.end_eulertour(); ++it) {
        nodes += it.node()->name;
    }
    EXPECT_EQ(GetParam().second, nodes) << " with start node " << GetParam().first;
}

INSTANTIATE_TEST_CASE_P (TreeIterator, Eulertour, ::testing::Values(
    std::make_pair("R", "RABACDCECARFRGHGIG"),
    std::make_pair("A", "ARFRGHGIGRABACDCEC"),
    std::make_pair("B", "BACDCECARFRGHGIGRA"),
    std::make_pair("C", "CARFRGHGIGRABACDCE"),
    std::make_pair("D", "DCECARFRGHGIGRABAC"),
    std::make_pair("E", "ECARFRGHGIGRABACDC"),
    std::make_pair("F", "FRGHGIGRABACDCECAR"),
    std::make_pair("G", "GRABACDCECARFRGHGI"),
    std::make_pair("H", "HGIGRABACDCECARFRG"),
    std::make_pair("I", "IGRABACDCECARFRGHG")
));
*/

/*

    TODO this is a test for const correctness.
    currently, it is not const correct, for many reasons:
      * the free function and iterator wrapper do not handle constness
      * find_node takes a const tree, but returns a non const node
      * this again is possible since the unique ptr used in tree to store the nodes
        allows for this. thus, we need a wrapper iterator around the vec of uniq ptr
        for the tree members that does not allow this kind of conversion.
      * on the other hand, that might result in need for two versions for each
        free function like find_node: a const and non const version. this is ugly.
      * so far, i don't see a proper clean solution for that. need to consult someone
        who knows more about this issue...

    so for now, we leave it in this dirty state. be warned!

 */
void do_test(const std::string node_name, const std::string expected_nodes, Tree const& tree)
{
    std::string resulting_nodes = "";

    // Find the Node for this test run.
    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    // Do a normal traversal.
    for( auto it : eulertour(*node) ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
        // it.node()->data.name = "bla";
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // Use free function iterator wrapper.
    resulting_nodes = "";
    for (auto it = eulertour(*node).begin(); it != eulertour(*node).end(); ++it) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
        // it.node()->data.name = "bla";
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // Do range-based for loop traversal.
    resulting_nodes = "";
    // for (auto& node : eulertour(tree)) {
    for (auto const& node_it : eulertour(*node)) {
        resulting_nodes += node_it.node().data<CommonNodeData>().name;
        // node.data.name = "bla";
    }
    // for (auto& node_it : eulertour(node)) {
    //     resulting_nodes += node_it.data.name;
    //     // node.data.name = "bla";
    // }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // LOG_DBG << "did it\n=============";
}

void TestEulertour(const std::string node_name, const std::string expected_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    // std::string resulting_nodes = "";

    // Prepare Tree.
    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));

    do_test(node_name, expected_nodes, tree);
}

TEST (TreeIterator, Eulertour)
{
    TestEulertour("R", "RABACDCECARFRGHGIG");
    TestEulertour("A", "ARFRGHGIGRABACDCEC");
    TestEulertour("B", "BACDCECARFRGHGIGRA");
    TestEulertour("C", "CARFRGHGIGRABACDCE");
    TestEulertour("D", "DCECARFRGHGIGRABAC");
    TestEulertour("E", "ECARFRGHGIGRABACDC");
    TestEulertour("F", "FRGHGIGRABACDCECAR");
    TestEulertour("G", "GRABACDCECARFRGHGI");
    TestEulertour("H", "HGIGRABACDCECARFRG");
    TestEulertour("I", "IGRABACDCECARFRGHG");
}

void TestEulertourSubtree( Subtree const& subtree, const std::string expected_nodes )
{
    std::string resulting_nodes = "";
    auto const name = subtree.node().data<CommonNodeData>().name + "(" + std::to_string( subtree.link().index() ) + ")";

    // Do a normal traversal.
    for( auto it : eulertour(subtree) ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ( expected_nodes, resulting_nodes ) << " with start node " << name;

    // Use free function iterator wrapper.
    resulting_nodes = "";
    for( auto it = eulertour(subtree).begin(); it != eulertour(subtree).end(); ++it ) {
        resulting_nodes += it.node().data<CommonNodeData>().name;
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << name;
}

TEST (TreeIterator, EulertourSubtree)
{
    // Prepare Tree.
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));

    // The following heavily depends on the internal tree structure.
    // If this breaks, we might need a setup that finds nodes,
    // instead of assuming certain link indices. But for now, this is good enough and stable.
    TestEulertourSubtree( Subtree{ tree.link_at(0) }, "RFRGHGIG" );
    TestEulertourSubtree( Subtree{ tree.link_at(1) }, "RGHGIGRABACDCECA" );
    TestEulertourSubtree( Subtree{ tree.link_at(2) }, "RABACDCECARF" );
    TestEulertourSubtree( Subtree{ tree.link_at(3) }, "GHGI" );
    TestEulertourSubtree( Subtree{ tree.link_at(4) }, "GIGRABACDCECARFR" );
    TestEulertourSubtree( Subtree{ tree.link_at(5) }, "GRABACDCECARFRGH" );
    TestEulertourSubtree( Subtree{ tree.link_at(6) }, "I" );
    TestEulertourSubtree( Subtree{ tree.link_at(7) }, "H" );
    TestEulertourSubtree( Subtree{ tree.link_at(8) }, "F" );
    TestEulertourSubtree( Subtree{ tree.link_at(9) }, "ABACDCEC" );
    TestEulertourSubtree( Subtree{ tree.link_at(10) }, "ACDCECARFRGHGIGR" );
    TestEulertourSubtree( Subtree{ tree.link_at(11) }, "ARFRGHGIGRAB" );
    TestEulertourSubtree( Subtree{ tree.link_at(12) }, "CDCE" );
    TestEulertourSubtree( Subtree{ tree.link_at(13) }, "CECARFRGHGIGRABA" );
    TestEulertourSubtree( Subtree{ tree.link_at(14) }, "CARFRGHGIGRABACD" );
    TestEulertourSubtree( Subtree{ tree.link_at(15) }, "E" );
    TestEulertourSubtree( Subtree{ tree.link_at(16) }, "D" );
    TestEulertourSubtree( Subtree{ tree.link_at(17) }, "B" );
}
