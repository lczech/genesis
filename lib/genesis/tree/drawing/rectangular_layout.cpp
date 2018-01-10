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

RectangularLayout& RectangularLayout::width( double const value )
{
    width_ = value;
    return *this;
}

double RectangularLayout::width() const
{
    return width_;
}

RectangularLayout& RectangularLayout::height( double const value )
{
    height_ = value;
    return *this;
}

double RectangularLayout::height() const
{
    return height_;
}

// =================================================================================================
//     Virtual Functions
// =================================================================================================

void RectangularLayout::init_node_( TreeNode& node, TreeNode const& orig_node )
{
    (void) orig_node;
    node.reset_data( RectangularNodeData::create() );
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

    // If no width and/or height is set, use automatic ones:
    // The height is at least 100, or depends on the node count, so that it scales well.
    // The factor of six is chosen based on the default svg font on our test system.
    // Might need to look into this for other systems.
    // Also, circular trees use node count without a factor as the default radius.
    // Because circumference is 2*pi*r, our factor of 6 is close to 2*pi,
    // which makes the font spacing similar for circular and rectangular trees ;-)
    // Furthermore, width is chosen to be half the height,
    // which usually is a good aspect ratio for tree figures.
    auto width = width_;
    auto height = height_;
    if( height <= 0.0 ) {
        height = std::max( 100.0, 6.0 * static_cast<double>( tree().node_count() ));
    }
    if( width <= 0.0 ) {
        width = height / 2.0;
    }

    for( auto const& node_it : tree().nodes() ) {
        auto const& node = *node_it;

        auto const& node_data = node.data<RectangularNodeData>();
        auto const& prnt_data = tree().node_at( node_data.parent_index ).data<RectangularNodeData>();

        // Get the edge between the node and its parent.
        auto edge_data_ptr = edge_between( node, tree().node_at( node_data.parent_index ) );

        // If there is an edge (i.e., we are not at the root), draw lines between the nodes.
        if( edge_data_ptr ) {
            auto stroke = edge_data_ptr->data<RectangularEdgeData>().stroke;
            // stroke.line_cap = utils::SvgStroke::LineCap::kRound;

            tree_lines << SvgLine(
                node_data.x * width, node_data.y * height,
                prnt_data.x * width, node_data.y * height,
                stroke
            );
            tree_lines << SvgLine(
                prnt_data.x * width, node_data.y * height,
                prnt_data.x * width, prnt_data.y * height,
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
            //     SvgPoint( node_data.x * width + 5, node_data.y * height )
            // );
            // label.dy = "0.4em";

            auto label = text_template();
            label.text = node_data.name;
            label.transform.append( SvgTransform::Translate(
                node_data.x * width + 5,
                node_data.y * height
            ));
            taxa_names << std::move( label );
        }

        // If there is a node shape, draw it.
        if( ! node_data.shape.empty() ) {
            auto ns = node_data.shape;
            ns.transform.append(
                SvgTransform::Translate( node_data.x * width, node_data.y * height )
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
    auto const num_leaves = static_cast<double>( leaf_node_count( tree() ));

    // Set node parents and y-coord of leaves.
    size_t leaf_count = 0;
    size_t parent = 0;
    for( auto it : eulertour( tree() )) {
        auto& node_data = tree().node_at( it.node().index() ).data<RectangularNodeData>();

        if( node_data.parent_index == -1 ) {
            node_data.parent_index = parent;
        }
        if( it.node().is_leaf() ) {
            node_data.y = static_cast<double>(leaf_count) / num_leaves;
            leaf_count++;
        }

        parent = it.node().index();
    }
    assert( leaf_count == num_leaves );

    auto children_min_y = std::vector<double>( tree().node_count(), -1.0 );
    auto children_max_y = std::vector<double>( tree().node_count(), -1.0 );

    // Set remaining y-coords of inner nodes to mid-points of their children.
    for( auto it : postorder( tree() )) {
        auto const node_index = it.node().index();
        auto& node_data   = tree().node_at( node_index ).data<RectangularNodeData>();
        auto const prnt_index = node_data.parent_index;

        if( node_data.y < 0.0 ) {
            // We already have done those nodes because of the postorder.
            assert( children_min_y[ node_index ] > -1.0 );
            assert( children_max_y[ node_index ] > -1.0 );

            auto min_max_diff = children_max_y[ node_index ] - children_min_y[ node_index ];
            node_data.y = children_min_y[ node_index ] + min_max_diff / 2.0;
        }

        if( children_min_y[ prnt_index ] < 0.0 || children_min_y[ prnt_index ] > node_data.y ) {
            children_min_y[ prnt_index ] = node_data.y;
        }
        if( children_max_y[ prnt_index ] < 0.0 || children_max_y[ prnt_index ] < node_data.y ) {
            children_max_y[ prnt_index ] = node_data.y;
        }
    }
}

void RectangularLayout::set_node_x_phylogram_()
{
    // Get distnace from root to every node.
    auto node_dists = node_branch_length_distance_vector( tree() );

    // We already check the size in init_layout_()
    assert( ! node_dists.empty() );
    auto const max_d = *std::max_element( node_dists.begin(), node_dists.end() );

    for( size_t i = 0; i < node_dists.size(); ++i ) {
        tree().node_at(i).data<RectangularNodeData>().x = node_dists[i] / max_d;
    }
}

void RectangularLayout::set_node_x_cladogram_()
{
    // Set root x to 0.
    tree().root_node().data<RectangularNodeData>().x = 0.0;

    // Get the heights of all subtrees starting from the root.
    auto const heights = subtree_max_path_heights( tree() );

    // Get the height of the tree, i.e. longest path from root to any leaf.
    auto const root_height = static_cast<double>( heights[ tree().root_node().index() ] );

    for( auto it : preorder( tree() )) {
        // The subtree height calculation does not work for the root, so skip it.
        // Also, we already set the leaf.
        if( it.is_first_iteration() ) {
            continue;
        }

        // Get the height of the subtree starting at the current node.
        auto const height = static_cast<double>( heights[ it.node().index() ]);
        assert( height <= root_height );

        // Set the x position.
        auto const x = ( root_height - height ) / root_height;
        tree().node_at( it.node().index() ).data<RectangularNodeData>().x = x;
    }
}

} // namespace tree
} // namespace genesis
