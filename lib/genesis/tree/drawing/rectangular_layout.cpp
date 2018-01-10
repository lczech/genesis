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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/drawing/rectangular_layout.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/eulertour.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/iterator/postorder.hpp"

#include "genesis/utils/core/logging.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Options
// =================================================================================================

RectangularLayout& RectangularLayout::scaler_x( double const sx )
{
    scaler_x_ = sx;
    return *this;
}

double RectangularLayout::scaler_x() const
{
    return scaler_x_;
}

RectangularLayout& RectangularLayout::scaler_y( double const sy )
{
    scaler_y_ = sy;
    return *this;
}

double RectangularLayout::scaler_y() const
{
    return scaler_y_;
}

// =================================================================================================
//     Virtual Functions
// =================================================================================================

void RectangularLayout::init_node_( TreeNode& node, TreeNode const& orig_node )
{
    (void) orig_node;
    node.reset_data( RectangularNodeData::create() );
    auto& node_data = node.data<RectangularNodeData>();

    // Set defaults needed to later distinguish whether those values have already been set.
    node_data.x = -1.0;
    node_data.y = -1.0;
}

void RectangularLayout::init_edge_( TreeEdge& edge, TreeEdge const& orig_edge )
{
    (void) orig_edge;
    edge.reset_data( RectangularEdgeData::create() );
}

void RectangularLayout::init_layout_()
{
    if( tree().empty() ) {
        return;
    }

    // Set node x-coords according to branch lengths (distance from root).
    if( type() == Type::kCladogram ) {
        set_node_x_cladogram_();
    } else if( type() == Type::kPhylogram ) {
        set_node_x_phylogram_();
    } else {
        assert( false );
    }

    // Set node y-coords.
    set_node_y_();
}

utils::SvgDocument RectangularLayout::to_svg_document_() const
{
    using namespace utils;
    SvgDocument doc;
    SvgGroup    tree_lines;
    SvgGroup    taxa_names;
    SvgGroup    node_shapes;

    for( auto const& node_it : tree().nodes() ) {
        auto const& node = *node_it;

        auto const& node_data   = node.data<RectangularNodeData>();
        auto const& parent_data = tree().node_at( node_data.parent_index ).data<RectangularNodeData>();

        // Get the edge between the node and its parent.
        auto edge_data_ptr = edge_between( node, tree().node_at( node_data.parent_index ) );

        // If there is an edge (i.e., we are not at the root), draw lines between the nodes.
        if( edge_data_ptr ) {
            auto stroke = edge_data_ptr->data<RectangularEdgeData>().stroke;
            // stroke.line_cap = utils::SvgStroke::LineCap::kRound;

            tree_lines << SvgLine(
                node_data.x * scaler_x_, node_data.y * scaler_y_,
                parent_data.x * scaler_x_, node_data.y * scaler_y_,
                stroke
            );
            tree_lines << SvgLine(
                parent_data.x * scaler_x_, node_data.y * scaler_y_,
                parent_data.x * scaler_x_, parent_data.y * scaler_y_,
                stroke
            );
        } else {

            // If there is no edge, it must be the root.
            assert( node.is_root() );
        }

        // If the node has a name, print it.
        if( node_data.name != "" ) {
            // auto label = SvgText(
            //     node_data.name,
            //     SvgPoint( node_data.x * scaler_x_ + 5, node_data.y * scaler_y_ )
            // );
            // label.dy = "0.4em";

            auto label = text_template();
            label.text = node_data.name;
            label.transform.append( SvgTransform::Translate(
                node_data.x * scaler_x_ + 5,
                node_data.y * scaler_y_
            ));
            taxa_names << std::move( label );
        }

        // If there is a node shape, draw it.
        if( ! node_data.shape.empty() ) {
            auto ns = node_data.shape;
            ns.transform.append(
                SvgTransform::Translate( node_data.x * scaler_x_, node_data.y * scaler_y_ )
            );
            node_shapes << std::move( ns );
        }
    }

    // Make sure that the drawing is done from outside to inside,
    // so that the overlapping parts look nice.
    tree_lines.reverse();

    // We are sure that we won't use the groups again, so let's move them!
    doc << std::move( tree_lines );
    if( ! taxa_names.empty() ) {
        doc << std::move( taxa_names );
    }
    if( ! node_shapes.empty() ) {
        doc << std::move( node_shapes );
    }
    return doc;
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

void RectangularLayout::set_node_y_()
{
    // Set node parents and y-coord of leaves.
    size_t leaf_count = 0;
    size_t parent = 0;
    for( auto it : eulertour( tree() )) {
        auto& node_data = tree().node_at( it.node().index() ).data<RectangularNodeData>();

        if( node_data.parent_index == -1 ) {
            node_data.parent_index = parent;
        }
        if( it.node().is_leaf() ) {
            node_data.y = leaf_count;
            leaf_count++;
        }

        parent = it.node().index();
    }

    // Set remaining y-coords of inner nodes to mid-points of their children.
    for( auto it : postorder( tree() )) {
        auto& node_data   = tree().node_at( it.node().index()      ).data<RectangularNodeData>();
        auto& parent_data = tree().node_at( node_data.parent_index ).data<RectangularNodeData>();

        if( node_data.y < 0.0 ) {
            auto min_max_diff = node_data.children_max_y - node_data.children_min_y;
            node_data.y = node_data.children_min_y + min_max_diff / 2.0;
        }

        if( parent_data.children_min_y < 0.0 || parent_data.children_min_y > node_data.y ) {
            parent_data.children_min_y = node_data.y;
        }
        if( parent_data.children_max_y < 0.0 || parent_data.children_max_y < node_data.y ) {
            parent_data.children_max_y = node_data.y;
        }
    }
}

void RectangularLayout::set_node_x_phylogram_()
{
    auto node_dists = node_branch_length_distance_vector( tree() );

    for( size_t i = 0; i < node_dists.size(); ++i ) {
        tree().node_at(i).data<RectangularNodeData>().x = node_dists[i];
    }
}

void RectangularLayout::set_node_x_cladogram_()
{
    // Set root x to 0.
    tree().root_node().data<RectangularNodeData>().x = 0.0;

    // Get the heights of all subtrees starting from the root.
    auto heights = subtree_max_path_heights( tree() );

    // Get the height of the tree, i.e. longest path from root to any leaf.
    auto root_height = heights[ tree().root_node().index() ];

    for( auto it : preorder( tree() )) {
        // The subtree height calculation does not work for the root, so skip it.
        // Also, we already set the leaf.
        if( it.is_first_iteration() ) {
            continue;
        }

        // Get the height of the subtree starting at the current node.
        auto height = heights[ it.node().index() ];
        assert( height <= root_height );

        // Set the x position.
        auto x = ( root_height - height );
        tree().node_at( it.node().index() ).data<RectangularNodeData>().x = x;
    }
}

} // namespace tree
} // namespace genesis
