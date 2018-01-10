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

#include "genesis/tree/drawing/circular_layout.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/eulertour.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/iterator/postorder.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Options
// =================================================================================================

CircularLayout& CircularLayout::radius_scaler( double const value )
{
    scaler_r_ = value;
    return *this;
}

double CircularLayout::radius_scaler() const
{
    return scaler_r_;
}

// =================================================================================================
//     Virtual Functions
// =================================================================================================

void CircularLayout::init_node_( TreeNode& node, TreeNode const& orig_node )
{
    (void) orig_node;
    node.reset_data( CircularNodeData::create() );
}

void CircularLayout::init_edge_( TreeEdge& edge, TreeEdge const& orig_edge )
{
    (void) orig_edge;
    edge.reset_data( CircularEdgeData::create() );
}

void CircularLayout::init_layout_()
{
    if( tree().empty() ) {
        return;
    }

    // Set radiuses of nodes.
    if( type() == Type::kCladogram ) {
        set_node_r_cladogram_();
    } else if( type() == Type::kPhylogram ) {
        set_node_r_phylogram_();
    } else {
        assert( false );
    }

    // Set angles of nodes.
    set_node_a_();
}

utils::SvgDocument CircularLayout::to_svg_document_() const
{
    using namespace utils;
    SvgDocument doc;
    SvgGroup    tree_lines;
    SvgGroup    taxa_names;
    SvgGroup    node_shapes;

    for( auto const& node_it : tree().nodes() ) {
        auto const& node = *node_it;

        auto const& node_data   = node.data<CircularNodeData>();
        auto const& parent_data = tree().node_at( node_data.parent_index ).data<CircularNodeData>();

        auto const node_x = node_data.r * scaler_r_ * cos( node_data.a );
        auto const node_y = node_data.r * scaler_r_ * sin( node_data.a );

        // Get the edge between the node and its parent.
        auto edge_data_ptr = edge_between( node, tree().node_at( node_data.parent_index ) );

        // If there is an edge (i.e., we are not at the root), draw lines between the nodes.
        if( edge_data_ptr ) {
            auto stroke = edge_data_ptr->data<CircularEdgeData>().stroke;
            stroke.line_cap = utils::SvgStroke::LineCap::kRound;

            auto start_a = parent_data.a;
            auto end_a   = node_data.a;
            if( parent_data.a > node_data.a ) {
                std::swap( start_a, end_a );
            }

            tree_lines << SvgPath(
                { svg_arc( 0, 0, parent_data.r * scaler_r_, start_a, end_a ) },
                stroke,
                SvgFill( SvgFill::Type::kNone )
            );

            tree_lines << SvgLine(
                parent_data.r * scaler_r_ * cos( node_data.a ),
                parent_data.r * scaler_r_ * sin( node_data.a ),
                node_x, node_y,
                stroke
            );

        } else {

            // If there is no edge, it must be the root.
            assert( node.is_root() );
        }

        // If the node has a name, print it.
        if( node_data.name != "" ) {
            // auto label = SvgText( node_data.name, SvgPoint( node_data.x + 5, node_data.y ) );
            // label.dy = "0.4em";

            auto label = text_template();
            label.text = node_data.name;
            label.transform.append( SvgTransform::Translate(
                ( node_data.r * scaler_r_ + 10 ) * cos( node_data.a ),
                ( node_data.r * scaler_r_ + 10 ) * sin( node_data.a )
            ));
            label.transform.append( SvgTransform::Rotate(
                360 * node_data.a / ( 2.0 * utils::PI )
            ));
            taxa_names << std::move( label );
        }

        // If there is a node shape, draw it.
        if( ! node_data.shape.empty() ) {
            auto ns = node_data.shape;
            ns.transform.append( SvgTransform::Translate( node_x, node_y ));
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

void CircularLayout::set_node_a_()
{
    auto const num_leaves = static_cast<double>( leaf_node_count( tree() ));

    // Set node parents and angles of leaves.
    size_t leaf_count = 0;
    size_t parent = 0;
    for( auto it : eulertour( tree() )) {
        auto& node_data = tree().node_at( it.node().index() ).data<CircularNodeData>();

        if( node_data.parent_index == -1 ) {
            node_data.parent_index = parent;
        }
        if( it.node().is_leaf() ) {
            node_data.a = 2.0 * utils::PI * static_cast<double>(leaf_count) / num_leaves;
            ++leaf_count;
        }

        parent = it.node().index();
    }

    // Set remaining angles of inner nodes to mid-points of their children.
    for( auto it : postorder( tree() )) {
        auto& node_data   = tree().node_at( it.node().index()      ).data<CircularNodeData>();
        auto& parent_data = tree().node_at( node_data.parent_index ).data<CircularNodeData>();

        if( node_data.a < 0.0 ) {
            auto min_max_diff = node_data.children_max_a - node_data.children_min_a;
            node_data.a = node_data.children_min_a + min_max_diff / 2.0;
        }

        if( parent_data.children_min_a < 0.0 || parent_data.children_min_a > node_data.a ) {
            parent_data.children_min_a = node_data.a;
        }
        if( parent_data.children_max_a < 0.0 || parent_data.children_max_a < node_data.a ) {
            parent_data.children_max_a = node_data.a;
        }
    }
}

void CircularLayout::set_node_r_phylogram_()
{
    auto node_dists = node_branch_length_distance_vector( tree() );

    for( size_t i = 0; i < node_dists.size(); ++i ) {
        tree().node_at(i).data<CircularNodeData>().r = node_dists[i] * 20.0;
    }
}

void CircularLayout::set_node_r_cladogram_()
{
    // Set root r to 0.
    tree().root_node().data<CircularNodeData>().r = 0.0;

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

        // Set the radius.
        auto r = ( root_height - height );
        tree().node_at( it.node().index() ).data<CircularNodeData>().r = r;
    }
}

} // namespace tree
} // namespace genesis
