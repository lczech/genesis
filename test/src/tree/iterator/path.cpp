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
 * @brief Testing Tree Iterators.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>
#include <utility>

#include "lib/tree/default/functions.hpp"
#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/formats/newick/reader.hpp"
#include "lib/tree/iterator/path.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Postorder
// =================================================================================================

void TestPath( std::string start_node_name, std::string finish_node_name, std::string out_nodes )
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    Tree tree;
    DefaultTreeNewickReader().from_string(input, tree);

    auto start_node  = find_node( tree, start_node_name  );
    auto finish_node = find_node( tree, finish_node_name );
    ASSERT_NE(nullptr, start_node);
    ASSERT_NE(nullptr, finish_node);

    for( auto it : path( *start_node, *finish_node )) {
        nodes += it.node().data<DefaultNodeData>().name;
    }

    EXPECT_EQ( out_nodes, nodes)
        << " with start node " << start_node_name << " and finish node " << finish_node_name;
}

TEST (TreeIterator, Path)
{
    TestPath( "A", "A", "A" );
    TestPath( "A", "B", "AB" );
    TestPath( "A", "C", "AC" );
    TestPath( "A", "D", "ACD" );
    TestPath( "A", "E", "ACE" );
    TestPath( "A", "F", "ARF" );
    TestPath( "A", "G", "ARG" );
    TestPath( "A", "H", "ARGH" );
    TestPath( "A", "I", "ARGI" );
    TestPath( "A", "R", "AR" );

    TestPath( "B", "A", "BA" );
    TestPath( "B", "B", "B" );
    TestPath( "B", "C", "BAC" );
    TestPath( "B", "D", "BACD" );
    TestPath( "B", "E", "BACE" );
    TestPath( "B", "F", "BARF" );
    TestPath( "B", "G", "BARG" );
    TestPath( "B", "H", "BARGH" );
    TestPath( "B", "I", "BARGI" );
    TestPath( "B", "R", "BAR" );

    TestPath( "C", "A", "CA" );
    TestPath( "C", "B", "CAB" );
    TestPath( "C", "C", "C" );
    TestPath( "C", "D", "CD" );
    TestPath( "C", "E", "CE" );
    TestPath( "C", "F", "CARF" );
    TestPath( "C", "G", "CARG" );
    TestPath( "C", "H", "CARGH" );
    TestPath( "C", "I", "CARGI" );
    TestPath( "C", "R", "CAR" );

    TestPath( "D", "A", "DCA" );
    TestPath( "D", "B", "DCAB" );
    TestPath( "D", "C", "DC" );
    TestPath( "D", "D", "D" );
    TestPath( "D", "E", "DCE" );
    TestPath( "D", "F", "DCARF" );
    TestPath( "D", "G", "DCARG" );
    TestPath( "D", "H", "DCARGH" );
    TestPath( "D", "I", "DCARGI" );
    TestPath( "D", "R", "DCAR" );

    TestPath( "E", "A", "ECA" );
    TestPath( "E", "B", "ECAB" );
    TestPath( "E", "C", "EC" );
    TestPath( "E", "D", "ECD" );
    TestPath( "E", "E", "E" );
    TestPath( "E", "F", "ECARF" );
    TestPath( "E", "G", "ECARG" );
    TestPath( "E", "H", "ECARGH" );
    TestPath( "E", "I", "ECARGI" );
    TestPath( "E", "R", "ECAR" );

    TestPath( "F", "A", "FRA" );
    TestPath( "F", "B", "FRAB" );
    TestPath( "F", "C", "FRAC" );
    TestPath( "F", "D", "FRACD" );
    TestPath( "F", "E", "FRACE" );
    TestPath( "F", "F", "F" );
    TestPath( "F", "G", "FRG" );
    TestPath( "F", "H", "FRGH" );
    TestPath( "F", "I", "FRGI" );
    TestPath( "F", "R", "FR" );

    TestPath( "G", "A", "GRA" );
    TestPath( "G", "B", "GRAB" );
    TestPath( "G", "C", "GRAC" );
    TestPath( "G", "D", "GRACD" );
    TestPath( "G", "E", "GRACE" );
    TestPath( "G", "F", "GRF" );
    TestPath( "G", "G", "G" );
    TestPath( "G", "H", "GH" );
    TestPath( "G", "I", "GI" );
    TestPath( "G", "R", "GR" );

    TestPath( "H", "A", "HGRA" );
    TestPath( "H", "B", "HGRAB" );
    TestPath( "H", "C", "HGRAC" );
    TestPath( "H", "D", "HGRACD" );
    TestPath( "H", "E", "HGRACE" );
    TestPath( "H", "F", "HGRF" );
    TestPath( "H", "G", "HG" );
    TestPath( "H", "H", "H" );
    TestPath( "H", "I", "HGI" );
    TestPath( "H", "R", "HGR" );

    TestPath( "I", "A", "IGRA" );
    TestPath( "I", "B", "IGRAB" );
    TestPath( "I", "C", "IGRAC" );
    TestPath( "I", "D", "IGRACD" );
    TestPath( "I", "E", "IGRACE" );
    TestPath( "I", "F", "IGRF" );
    TestPath( "I", "G", "IG" );
    TestPath( "I", "H", "IGH" );
    TestPath( "I", "I", "I" );
    TestPath( "I", "R", "IGR" );

    TestPath( "R", "A", "RA" );
    TestPath( "R", "B", "RAB" );
    TestPath( "R", "C", "RAC" );
    TestPath( "R", "D", "RACD" );
    TestPath( "R", "E", "RACE" );
    TestPath( "R", "F", "RF" );
    TestPath( "R", "G", "RG" );
    TestPath( "R", "H", "RGH" );
    TestPath( "R", "I", "RGI" );
    TestPath( "R", "R", "R" );
}
