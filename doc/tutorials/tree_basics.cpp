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

    // -------------------------------------------------------------------------
    //     Reading and Writing
    // -------------------------------------------------------------------------

    // Read a Newick file into a Tree object.
    Tree tree = DefaultTreeNewickReader().from_file( "path/to/tree.newick" );

    // Given a string containing a Newick tree, read it into a Tree object.
    std::string newick = "(((A:0.2,B:0.3)C:0.3,(D:0.4,E:0.45,F:0.3,G:0.35)H:0.25)I:0.8,J:0.4,K:0.5)L;";
    tree = DefaultTreeNewickReader().from_string( newick );

    // Write a Tree to a Newick file and a PhyloXML file.
    DefaultTreeNewickWriter().to_file( tree, "path/to/tree.nw" );
    DefaultTreePhyloxmlWriter().to_file( tree, "path/to/tree.phyloxml" );

    // -------------------------------------------------------------------------
    //     Inspecting a Tree
    // -------------------------------------------------------------------------

    // Print the tree topology including node names and branch lengths.
    std::cout << PrinterCompact().print( tree );

    // -------------------------------------------------------------------------
    //     Iterating Nodes and Edges
    // -------------------------------------------------------------------------

    // Print all node names.
    for( auto const& node : tree.nodes() ) {
        auto const& name = node->data<DefaultNodeData>().name;
        if( ! name.empty() ) {
            std::cout << name << std::endl;
        }
    }

    // Print all branch lenghts.
    for( auto const& edge : tree.edges() ) {
        auto const& branch_length = edge->data<DefaultEdgeData>().branch_length;
        std::cout << branch_length << std::endl;
    }

    // Iterate nodes using their index and fill a vector with them.
    auto names = std::vector<std::string>( tree.node_count(), "" );
    for( size_t i = 0; i < tree.node_count(); ++i ) {
        auto const& node = tree.node_at( i );
        auto const& name = node.data<DefaultNodeData>().name;
        names[ i ] = name;
    }

    // Iterate edges using their index and fill a vector with them.
    auto branch_lengths =  std::vector<double>( tree.edge_count(), 0.0 );
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto const& edge = tree.edge_at( i );
        auto const& branch_length = edge.data<DefaultEdgeData>().branch_length;
        branch_lengths[ i ] = branch_length;
    }

    // -------------------------------------------------------------------------
    //     Eulertour Traversal
    // -------------------------------------------------------------------------

    // Do an Eulertour around the Tree and print Node names.
    for( auto it : eulertour( tree ) ) {
        std::cout << it.node().data<DefaultNodeData>().name << " ";
    }
    std::cout << std::endl;

    // Find Node "C" and start an Eulertour traversal from there.
    auto node_C = find_node( tree, "C" );
    for( auto it : eulertour( *node_C ) ) {
        std::cout << it.node().data<DefaultNodeData>().name << " ";
    }
    std::cout << std::endl;

    // -------------------------------------------------------------------------
    //     Preorder and Postorder Traversal
    // -------------------------------------------------------------------------

    // Traverse the Tree in preorder fashion, starting from the root Node "L".
    for( auto it : preorder( tree ) ) {
        std::cout << it.node().data<DefaultNodeData>().name << " ";
    }
    std::cout << std::endl;

    // Traverse the Tree in postorder fashion, starting from the root Node "L".
    for( auto it : postorder( tree ) ) {
        std::cout << it.node().data<DefaultNodeData>().name << " ";
    }
    std::cout << std::endl;

    // Find Node "H" and start traversals from there.
    auto node_H = find_node( tree, "H" );
    for( auto it : preorder( *node_H ) ) {
        // ...
    }
    for( auto it : postorder( *node_H ) ) {
        // ...
    }

    // Do a preorder traversal of the Edges, and print their branch lengths.
    for( auto it : preorder( tree ) ) {
        if( ! it.is_first_iteration() ) {
            std::cout << it.edge().data<DefaultEdgeData>().branch_length << " ";
        }
    }
    std::cout << std::endl;

    // Do a postorder traversal of the Edges, and print their branch lengths.
    for( auto it : postorder( tree ) ) {
        if( ! it.is_last_iteration() ) {
            std::cout << it.edge().data<DefaultEdgeData>().branch_length << " ";
        }
    }
    std::cout << std::endl;

}
