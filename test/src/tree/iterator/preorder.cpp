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

    Tree tree = DefaultTreeNewickReader().from_string(input);

    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    for( auto it : tree::preorder(*node) ) {
        nodes += it.node().data<tree::DefaultNodeData>().name;
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
