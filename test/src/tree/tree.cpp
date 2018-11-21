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
 * @brief Testing Tree class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/tree.hpp"

using namespace genesis;
using namespace tree;

TEST(Tree, Basics)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    Tree const tree = CommonTreeNewickReader().read( utils::from_string(input));

    EXPECT_EQ( 2, degree( tree.root_node() ));
    EXPECT_EQ( 7, leaf_node_count( tree ));
    EXPECT_EQ( 6, inner_node_count( tree ));
    EXPECT_EQ( 13, tree.node_count());
    EXPECT_TRUE( is_bifurcating( tree ));

    EXPECT_EQ("R", tree.root_node().data<CommonNodeData>().name);
    EXPECT_TRUE( validate_topology( tree ));

    // Copy constructor.
    Tree copy_a{ tree };
    EXPECT_TRUE( validate_topology( copy_a ));

    // Copy assignment.
    auto const copy_b = tree;
    EXPECT_TRUE( validate_topology( copy_b ));
}
