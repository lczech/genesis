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

#include "genesis/genesis.hpp"

int main()
{
    using namespace genesis::tree;

    // Read a Newick file into a Tree object.
    Tree tree = DefaultTreeNewickReader().from_file( "path/to/tree.newick" );

    // Given a string containing a Newick tree, read it into a Tree object.
    std::string newick = "(((A:0.2,B:0.3)C:0.3,(D:0.4,E:0.45,F:0.3,G:0.35)H:0.25)I:0.8,J:0.4,K:0.5)L;";
    Tree tree_2 = DefaultTreeNewickReader().from_string( newick );

    // Write a Tree to a Newick file and a PhyloXML file.
    DefaultTreeNewickWriter().to_file( tree, "path/to/tree.nw" );
    DefaultTreePhyloxmlWriter().to_file( tree, "path/to/tree.phyloxml" );

    // Print the tree topology including node names and branch lengths.
    std::cout << PrinterCompact().print( tree );
}
