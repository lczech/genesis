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

#include "tree/default/distances.hpp"
#include "tree/default/tree.hpp"
#include "tree/function/distances.hpp"
#include "tree/function/functions.hpp"
#include "tree/iterator/eulertour.hpp"
#include "tree/iterator/preorder.hpp"
#include "tree/iterator/postorder.hpp"

#include "utils/formats/svg/svg.hpp"

#include "utils/core/logging.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

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
    // set_node_x_phylogram_( tree );

    set_node_x_cladogram_( tree );

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

        node.name = it.node().data<DefaultNodeData>().name;
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

void RectangularLayout::set_node_x_phylogram_( Tree const& tree )
{
    auto node_dists = node_branch_length_distance_vector( tree );
    assert( node_dists.size() == nodes_.size() );

    for( size_t i = 0; i < node_dists.size(); ++i ) {
        nodes_[i].x = node_dists[i] * scaler_x_;
    }
}

void RectangularLayout::set_node_x_cladogram_( Tree const& tree )
{
    // Set root x to 0.
    nodes_[ tree.root_node().index() ].x = 0.0;

    // Get the height of the tree, i.e. longest path from root to any leaf.
    auto root_path_lengths = node_path_length_vector( tree );
    auto root_height       = *std::max_element( root_path_lengths.begin(), root_path_lengths.end() );

    for( auto it : preorder( tree )) {
        // The subtree height calculation does not work for the root, so skip it.
        // Also, we already set the leaf.
        if( it.is_first_iteration() ) {
            continue;
        }

        // Get the height of the subtree starting at the current node.
        auto height = subtree_max_path_height( tree, it.link().outer() );
        assert( height <= root_height );

        // Set the x position.
        nodes_[ it.node().index() ].x = ( root_height - height ) * scaler_x_;
    }
}


} // namespace tree
} // namespace genesis
