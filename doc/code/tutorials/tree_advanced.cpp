/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
    using namespace genesis::utils;

    {
        // Get an object, so that the rest compiles.
        Tree tree;

        // -------------------------------------------------------------------------
        //     Iterating Nodes and Edges
        // -------------------------------------------------------------------------

        // Prepare a vector of all node names.
        auto names = std::vector<std::string>( tree.node_count() );

        // Traverse the Nodes of a Tree, and fill the vector.
        for( auto const& node : tree.nodes() ) {
            names[ node.index() ] = node.data<CommonNodeData>().name;
        }
    }

    {
        // -------------------------------------------------------------------------
        //     Iterating Nodes and Edges
        // -------------------------------------------------------------------------

        // Read a Newick file into a Tree object.
        Tree tree = CommonTreeNewickReader().read( from_file( "path/to/tree.newick" ));
    }
}
