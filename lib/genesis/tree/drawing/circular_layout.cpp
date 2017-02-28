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
//     Circular Layout
// =================================================================================================

CircularLayout::CircularLayout( Tree const& orig_tree )
{
    if( orig_tree.empty() ) {
        tree_ = orig_tree;
        return;
    }

    // Get a copy of the tree topology.
    tree_ = orig_tree.clone_topology();

    // Init nodes.
    for( size_t i = 0; i < orig_tree.node_count(); ++i ) {
        // Safety: correct indices.
        assert( tree_.node_at(i).index() == i && orig_tree.node_at(i).index() == i );

        // Set the tree node data.
        tree_.node_at(i).reset_data( CircularNodeData::create() );
        auto& node_data = tree_.node_at(i).data<CircularNodeData>();

        // Set defaults needed to later distinguish whether those values have already been set.
        node_data.x = -1.0;
        node_data.y = -1.0;

        // If the original tree has node names, use them.
        auto orig_node_data_ptr = orig_tree.node_at(i).data_cast<DefaultNodeData>();
        if( orig_node_data_ptr ) {
            node_data.name = orig_node_data_ptr->name;
        }
    }

    // Init edges.
    for( size_t i = 0; i < orig_tree.edge_count(); ++i ) {
        // Safety: correct indices.
        assert( tree_.edge_at(i).index() == i && orig_tree.edge_at(i).index() == i );

        // Set the tree edge data.
        tree_.edge_at(i).reset_data( CircularEdgeData::create() );
        auto& edge_data = tree_.edge_at(i).data<CircularEdgeData>();

        // If the original tree has edge branch lengths, use them.
        auto orig_edge_data_ptr = orig_tree.edge_at(i).data_cast<DefaultEdgeData>();
        if( orig_edge_data_ptr ) {
            edge_data.branch_length = orig_edge_data_ptr->branch_length;
        }
    }

    // Set node x-coords according to branch lengths (distance from root).
    // set_node_r_phylogram_();
    set_node_r_cladogram_();

    auto num_leaves = static_cast<double>( leaf_node_count( orig_tree ));

    // Set node parents and angles of leaves.
    size_t leaf_count = 0;
    size_t parent = 0;
    for( auto it : eulertour( orig_tree )) {
        auto& node_data = tree_.node_at( it.node().index() ).data<CircularNodeData>();

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
    for( auto it : postorder( orig_tree )) {
        auto& node_data   = tree_.node_at( it.node().index()      ).data<CircularNodeData>();
        auto& parent_data = tree_.node_at( node_data.parent_index ).data<CircularNodeData>();

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

// -------------------------------------------------------------
//     Drawing
// -------------------------------------------------------------

Tree& CircularLayout::tree()
{
    return tree_;
}

void CircularLayout::set_edge_strokes( std::vector< utils::SvgStroke > strokes )
{
    if( strokes.size() != tree_.edge_count() ) {
        throw std::runtime_error( "Edge stroke vector has wrong size." );
    }
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<CircularEdgeData>().stroke = strokes[ i ];
    }
}

std::string svg_arc(
    double center_x, double center_y, double radius, double start_angle, double end_angle
) {
    std::string large_arc;
    if( start_angle > end_angle ) {
        large_arc = ( end_angle - start_angle <= utils::PI ? "1" : "0" );
    } else {
        large_arc = ( end_angle - start_angle <= utils::PI ? "0" : "1" );
    }

    double start_x = center_x + ( radius * cos( end_angle ));
    double start_y = center_y + ( radius * sin( end_angle ));
    double end_x   = center_x + ( radius * cos( start_angle ));
    double end_y   = center_y + ( radius * sin( start_angle ));

    std::ostringstream os;
    os << "M " << start_x << " " << start_y << " ";
    os << "A " << radius << " " << radius << " " << 0 << " " << large_arc << " " << 0 << " ";
    os << end_x << " " << end_y;
    return os.str();
}

utils::SvgDocument CircularLayout::to_svg_document() const
{
    using namespace utils;
    SvgDocument doc;
    SvgGroup    tree_lines;
    SvgGroup    taxa_names;

    for( auto const& node_it : tree_.nodes() ) {
        auto const& node = *node_it;

        auto const& node_data   = node.data<CircularNodeData>();
        auto const& parent_data = tree_.node_at( node_data.parent_index ).data<CircularNodeData>();

        // Get the edge between the node and its parent.
        auto edge_data_ptr = edge_between( node, tree_.node_at( node_data.parent_index ) );

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
                { svg_arc( 0, 0, parent_data.r, start_a, end_a ) },
                stroke,
                SvgFill( SvgFill::Type::kNone )
            );

            tree_lines << SvgLine(
                parent_data.r * cos( node_data.a ), parent_data.r * sin( node_data.a ),
                node_data.r * cos( node_data.a ), node_data.r * sin( node_data.a ),
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

            auto label = SvgText( node_data.name );
            label.transform.append( SvgTransform::Translate(
                ( node_data.r + 10 ) * cos( node_data.a ),
                ( node_data.r + 10 ) * sin( node_data.a )
            ));
            label.transform.append( SvgTransform::Rotate(
                360 * node_data.a / ( 2.0 * utils::PI )
            ));
            label.font.size /= 2;
            taxa_names << label;
        }
    }

    // We are sure that we won't use the groups again, so let's move them!
    doc << std::move( tree_lines );
    doc << std::move( taxa_names );
    return doc;
}

void CircularLayout::set_node_r_phylogram_()
{
    auto node_dists = node_branch_length_distance_vector( tree_ );

    for( size_t i = 0; i < node_dists.size(); ++i ) {
        tree_.node_at(i).data<CircularNodeData>().r = node_dists[i] * scaler_r_ * 20.0;
    }
}

void CircularLayout::set_node_r_cladogram_()
{
    // Set root r to 0.
    tree_.root_node().data<CircularNodeData>().r = 0.0;

    // Get the heights of all subtrees starting from the root.
    auto heights = subtree_max_path_heights( tree_ );

    // Get the height of the tree, i.e. longest path from root to any leaf.
    auto root_height = heights[ tree_.root_node().index() ];

    for( auto it : preorder( tree_ )) {
        // The subtree height calculation does not work for the root, so skip it.
        // Also, we already set the leaf.
        if( it.is_first_iteration() ) {
            continue;
        }

        // Get the height of the subtree starting at the current node.
        auto height = heights[ it.node().index() ];
        assert( height <= root_height );

        // Set the radius.
        auto r = ( root_height - height ) * scaler_r_;
        tree_.node_at( it.node().index() ).data<CircularNodeData>().r = r;
    }
}


} // namespace tree
} // namespace genesis
