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
#include "genesis/tree/iterator/path_set.hpp"
#include "genesis/tree/tree.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Postorder
// =================================================================================================

void TestPathSet(
    std::string start_node_name,
    std::string finish_node_name,
    std::string lca_node_name,
    std::string out_nodes
) {
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree = DefaultTreeNewickReader().from_string( input );

    auto start_node  = find_node( tree, start_node_name  );
    auto finish_node = find_node( tree, finish_node_name );
    auto lca_node    = find_node( tree, lca_node_name );
    ASSERT_NE(nullptr, start_node);
    ASSERT_NE(nullptr, finish_node);
    ASSERT_NE(nullptr, lca_node);

    for( auto it : path_set( *start_node, *finish_node, *lca_node )) {
        nodes += it.node().data<DefaultNodeData>().name;
    }

    EXPECT_EQ( out_nodes, nodes)
        << " with start node " << start_node_name << " and finish node " << finish_node_name;
}

TEST (TreeIterator, PathSet)
{
    TestPathSet( "A", "A", "A", "A" );
    TestPathSet( "A", "B", "A", "BA" );
    TestPathSet( "A", "C", "A", "CA" );
    TestPathSet( "A", "D", "A", "DCA" );
    TestPathSet( "A", "E", "A", "ECA" );
    TestPathSet( "A", "F", "R", "AFR" );
    TestPathSet( "A", "G", "R", "AGR" );
    TestPathSet( "A", "H", "R", "AHGR" );
    TestPathSet( "A", "I", "R", "AIGR" );
    TestPathSet( "A", "R", "R", "AR" );

    TestPathSet( "B", "A", "A", "BA" );
    TestPathSet( "B", "B", "B", "B" );
    TestPathSet( "B", "C", "A", "BCA" );
    TestPathSet( "B", "D", "A", "BDCA" );
    TestPathSet( "B", "E", "A", "BECA" );
    TestPathSet( "B", "F", "R", "BAFR" );
    TestPathSet( "B", "G", "R", "BAGR" );
    TestPathSet( "B", "H", "R", "BAHGR" );
    TestPathSet( "B", "I", "R", "BAIGR" );
    TestPathSet( "B", "R", "R", "BAR" );

    TestPathSet( "C", "A", "A", "CA" );
    TestPathSet( "C", "B", "A", "CBA" );
    TestPathSet( "C", "C", "C", "C" );
    TestPathSet( "C", "D", "C", "DC" );
    TestPathSet( "C", "E", "C", "EC" );
    TestPathSet( "C", "F", "R", "CAFR" );
    TestPathSet( "C", "G", "R", "CAGR" );
    TestPathSet( "C", "H", "R", "CAHGR" );
    TestPathSet( "C", "I", "R", "CAIGR" );
    TestPathSet( "C", "R", "R", "CAR" );

    TestPathSet( "D", "A", "A", "DCA" );
    TestPathSet( "D", "B", "A", "DCBA" );
    TestPathSet( "D", "C", "C", "DC" );
    TestPathSet( "D", "D", "D", "D" );
    TestPathSet( "D", "E", "C", "DEC" );
    TestPathSet( "D", "F", "R", "DCAFR" );
    TestPathSet( "D", "G", "R", "DCAGR" );
    TestPathSet( "D", "H", "R", "DCAHGR" );
    TestPathSet( "D", "I", "R", "DCAIGR" );
    TestPathSet( "D", "R", "R", "DCAR" );

    TestPathSet( "E", "A", "A", "ECA" );
    TestPathSet( "E", "B", "A", "ECBA" );
    TestPathSet( "E", "C", "C", "EC" );
    TestPathSet( "E", "D", "C", "EDC" );
    TestPathSet( "E", "E", "E", "E" );
    TestPathSet( "E", "F", "R", "ECAFR" );
    TestPathSet( "E", "G", "R", "ECAGR" );
    TestPathSet( "E", "H", "R", "ECAHGR" );
    TestPathSet( "E", "I", "R", "ECAIGR" );
    TestPathSet( "E", "R", "R", "ECAR" );

    TestPathSet( "F", "A", "R", "FAR" );
    TestPathSet( "F", "B", "R", "FBAR" );
    TestPathSet( "F", "C", "R", "FCAR" );
    TestPathSet( "F", "D", "R", "FDCAR" );
    TestPathSet( "F", "E", "R", "FECAR" );
    TestPathSet( "F", "F", "F", "F" );
    TestPathSet( "F", "G", "R", "FGR" );
    TestPathSet( "F", "H", "R", "FHGR" );
    TestPathSet( "F", "I", "R", "FIGR" );
    TestPathSet( "F", "R", "R", "FR" );

    TestPathSet( "G", "A", "R", "GAR" );
    TestPathSet( "G", "B", "R", "GBAR" );
    TestPathSet( "G", "C", "R", "GCAR" );
    TestPathSet( "G", "D", "R", "GDCAR" );
    TestPathSet( "G", "E", "R", "GECAR" );
    TestPathSet( "G", "F", "R", "GFR" );
    TestPathSet( "G", "G", "G", "G" );
    TestPathSet( "G", "H", "G", "HG" );
    TestPathSet( "G", "I", "G", "IG" );
    TestPathSet( "G", "R", "R", "GR" );

    TestPathSet( "H", "A", "R", "HGAR" );
    TestPathSet( "H", "B", "R", "HGBAR" );
    TestPathSet( "H", "C", "R", "HGCAR" );
    TestPathSet( "H", "D", "R", "HGDCAR" );
    TestPathSet( "H", "E", "R", "HGECAR" );
    TestPathSet( "H", "F", "R", "HGFR" );
    TestPathSet( "H", "G", "G", "HG" );
    TestPathSet( "H", "H", "H", "H" );
    TestPathSet( "H", "I", "G", "HIG" );
    TestPathSet( "H", "R", "R", "HGR" );

    TestPathSet( "I", "A", "R", "IGAR" );
    TestPathSet( "I", "B", "R", "IGBAR" );
    TestPathSet( "I", "C", "R", "IGCAR" );
    TestPathSet( "I", "D", "R", "IGDCAR" );
    TestPathSet( "I", "E", "R", "IGECAR" );
    TestPathSet( "I", "F", "R", "IGFR" );
    TestPathSet( "I", "G", "G", "IG" );
    TestPathSet( "I", "H", "G", "IHG" );
    TestPathSet( "I", "I", "I", "I" );
    TestPathSet( "I", "R", "R", "IGR" );

    TestPathSet( "R", "A", "R", "AR" );
    TestPathSet( "R", "B", "R", "BAR" );
    TestPathSet( "R", "C", "R", "CAR" );
    TestPathSet( "R", "D", "R", "DCAR" );
    TestPathSet( "R", "E", "R", "ECAR" );
    TestPathSet( "R", "F", "R", "FR" );
    TestPathSet( "R", "G", "R", "GR" );
    TestPathSet( "R", "H", "R", "HGR" );
    TestPathSet( "R", "I", "R", "IGR" );
    TestPathSet( "R", "R", "R", "R" );
}

TEST (TreeIterator, PathSetExceptions)
{
    // The iterator is able to find some cases of wrong LCAs, but not all of them.
    // If the provided LCA is somewhere on the path between the actual LCA and the root, this
    // is not detected. Thus, we only test cases where the LCA is not on this path.

    EXPECT_THROW( TestPathSet( "A", "A", "G", "" ), std::runtime_error );
    EXPECT_THROW( TestPathSet( "A", "B", "B", "" ), std::runtime_error );
    EXPECT_THROW( TestPathSet( "A", "B", "G", "" ), std::runtime_error );
    EXPECT_THROW( TestPathSet( "G", "R", "G", "" ), std::runtime_error );
    EXPECT_THROW( TestPathSet( "H", "I", "I", "" ), std::runtime_error );
}
