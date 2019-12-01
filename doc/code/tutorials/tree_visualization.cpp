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

#include "genesis/genesis.hpp"

#include <stdexcept>

int main()
{
    using namespace genesis::tree;
    using namespace genesis::utils;

    // Get a tree
    Tree tree = CommonTreeNewickReader().read( from_file( "path/to/tree.newick" ));
    LOG_INFO << "Tree has " << tree.edge_count() << " edges.";

    // Make all edges red
    auto edge_colors = std::vector<Color>( tree.edge_count(), Color( 1.0, 0.0, 0.0 ) );

    // Make tips blue
    for( auto const& edge : tree.edges() ) {
        if( is_leaf( edge )) {
            edge_colors[ edge.index() ] = Color( 0.0, 1.0, 0.0 );
        }
    }

    // Write to some formats
    write_tree_to_newick_file( tree, "path/to/new_tree.newick" );
    write_color_tree_to_phyloxml_file( tree, edge_colors, "path/to/new_tree.phyloxml" );
    write_color_tree_to_nexus_file( tree, edge_colors, "path/to/new_tree.nexus" );

    // Prepare the layout parameters
    LayoutParameters params;
    params.shape = LayoutShape::kCircular;
    params.stroke.width = 3.0;

    // Use them to write an svg file
    write_color_tree_to_svg_file( tree, params, edge_colors, "path/to/new_tree.svg" );

    // Assume we have some per-edge values that we want to visualize
    auto edge_values = std::vector<double>{ 739.2, 25.1, 934.2, 47.5, 9.3, 86.6 };
    assert( edge_values.size() == tree.edge_count() );

    // Make a color map and norm and use it to create a legend
    auto color_map = ColorMap( color_list_viridis() );
    auto color_norm = ColorNormalizationLogarithmic( 1.0, 1000.0);
    write_color_tree_to_svg_file( tree, params, edge_values, color_map, color_norm, "path/to/new_tree_2.svg" );
}
