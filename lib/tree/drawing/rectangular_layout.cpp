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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/drawing/rectangular_layout.hpp"

#include "tree/default/tree.hpp"

#include "utils/formats/svg/svg.hpp"

#include <algorithm>

namespace genesis {
namespace tree {

// =================================================================================================
//     Rectangular Layout
// =================================================================================================

// -------------------------------------------------------------
//     Drawing
// -------------------------------------------------------------

void RectangularLayout::from_tree( Tree const& tree )
{
    if( tree.empty() ) {
        throw std::runtime_error( "Cannot draw an empty tree." );
    }

    nodes_ = std::vector< Node >( tree.node_count() );

    // Set node x-coords according to branch lengths (distance from root).
    auto node_dists = node_branch_length_distance_vector( tree );
    set_node_x_phylogram_( node_dists );
    // auto node_dists = node_path_length_vector( tree );
    // set_node_x_cladogram_( node_dists );

    // Set node parents and y-coord of leaves.
    size_t leaf_count = 0;
    size_t parent = 0;
    for( auto it : eulertour( tree )) {
        auto& node = nodes_[ it.node().index() ];

        if( node.parent == -1 ) {
            node.parent = parent;
            // edge_to_layout[it.edge()] = &node;
        }
        if( it.node().is_leaf() ) {
            node.y = scaler_y_ * leaf_count++;
        }

        node.name = node_data_cast< DefaultNodeData >( it.node() ).name;
        parent = it.node().index();
    }

    // Set remaining y-coords of inner nodes to mid-points of their children.
    for( auto it : postorder( tree )) {
        auto& node = nodes_[ it.node().index() ];
        auto& parent_node = nodes_[ node.parent ];

        if( node.y < 0.0 ) {
            node.y = node.children_min_y + ( node.children_max_y - node.children_min_y ) / 2;
        }

        if( parent_node.children_min_y < 0.0 || parent_node.children_min_y > node.y ) {
            parent_node.children_min_y = node.y;
        }
        if( parent_node.children_max_y < 0.0 || parent_node.children_max_y < node.y ) {
            parent_node.children_max_y = node.y;
        }
    }
}

utils::SvgDocument RectangularLayout::to_svg_document() const
{
    using namespace utils;
    SvgDocument doc;

    auto stroke = SvgStroke();
    stroke.line_cap = SvgStroke::LineCap::kRound;

    for( auto const& node : nodes_ ) {
        doc << SvgLine(
            node.x, node.y,
            nodes_[ node.parent ].x, node.y,
            stroke
        );
        doc << SvgLine(
            nodes_[ node.parent ].x, node.y,
            nodes_[ node.parent ].x, nodes_[ node.parent ].y,
            stroke
        );
    }

    return doc;
}

void RectangularLayout::set_node_x_phylogram_( std::vector<double> node_dists )
{
    assert( node_dists.size() == nodes_.size() );
    for( size_t i = 0; i < node_dists.size(); ++i ) {
        nodes_[i].x = node_dists[i] * scaler_x_;
    }
}

void RectangularLayout::set_node_x_cladogram_( std::vector<int> node_dists )
{
    assert( node_dists.size() == nodes_.size() );

    (void) node_dists;

    // int max = *std::max_element( node_dists.begin(), node_dists.end() );
    //
    // for( size_t i = 0; i < node_dists.size(); ++i ) {
    //     nodes_[i].x = node_dists[i] * scaler_x_;
    // }
}


} // namespace tree
} // namespace genesis
