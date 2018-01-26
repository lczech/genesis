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

#include "genesis/tree/drawing/layout_base.hpp"

#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/eulertour.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/function/manipulation.hpp"

#include <cassert>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree
// =================================================================================================

void LayoutBase::tree( Tree const& orig_tree, bool ladderize_tree )
{
    // We first copy the tree, then ladderize it before init, so that all positions
    // are initialized correlty. This is possible because ladderize() only changes link pointers
    // of the tree, but not and indices or node array positions.
    tree_ = orig_tree.clone_topology();
    if( ladderize_tree ) {
        ladderize( tree_ );
    }
    init_tree_( orig_tree );
}

Tree& LayoutBase::tree()
{
    return tree_;
}

Tree const& LayoutBase::tree() const
{
     return tree_;
}

void LayoutBase::set_edge_strokes( utils::SvgStroke const& stroke )
{
    set_edge_spreading_strokes( stroke );
    set_edge_distance_strokes( stroke );
}

void LayoutBase::set_edge_strokes( std::vector< utils::SvgStroke > const& strokes )
{
    set_edge_spreading_strokes( strokes );
    set_edge_distance_strokes( strokes );
}

void LayoutBase::set_edge_spreading_strokes( utils::SvgStroke const& stroke )
{
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().spreading_stroke = stroke;
    }
}

void LayoutBase::set_edge_spreading_strokes( std::vector< utils::SvgStroke > const& strokes )
{
    // Empty: Reset to default.
    if( strokes.empty() ) {
        set_edge_spreading_strokes( utils::SvgStroke() );
        return;
    }

    // Non-empty case.
    if( strokes.size() != tree_.edge_count() ) {
        throw std::runtime_error( "Edge stroke vector has wrong size." );
    }
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().spreading_stroke = strokes[ i ];
    }
}

void LayoutBase::set_edge_distance_strokes( utils::SvgStroke const& stroke )
{
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().distance_stroke = stroke;
    }
}

void LayoutBase::set_edge_distance_strokes( std::vector< utils::SvgStroke > const& strokes )
{
    // Empty: Reset to default.
    if( strokes.empty() ) {
        set_edge_distance_strokes( utils::SvgStroke() );
        return;
    }

    // Non-empty case.
    if( strokes.size() != tree_.edge_count() ) {
        throw std::runtime_error( "Edge stroke vector has wrong size." );
    }
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().distance_stroke = strokes[ i ];
    }
}

void LayoutBase::set_edge_shapes( utils::SvgGroup const& shape )
{
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().shape = shape;
    }
}

void LayoutBase::set_edge_shapes( std::vector< utils::SvgGroup> const& shapes )
{
    // Empty: Reset to default.
    if( shapes.empty() ) {
        set_edge_shapes( utils::SvgGroup() );
        return;
    }

    // Non-empty case.
    if( shapes.size() != tree_.edge_count() ) {
        throw std::runtime_error( "Edge shape vector has wrong size." );
    }
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().shape = shapes[ i ];
    }
}

void LayoutBase::set_node_shapes( utils::SvgGroup const& shape )
{
    for( size_t i = 0; i < tree_.node_count(); ++i ) {
        tree_.node_at(i).data<LayoutNodeData>().shape = shape;
    }
}

void LayoutBase::set_node_shapes( std::vector< utils::SvgGroup> const& shapes )
{
    // Empty: Reset to default.
    if( shapes.empty() ) {
        set_node_shapes( utils::SvgGroup() );
        return;
    }

    // Non-empty case.
    if( shapes.size() != tree_.node_count() ) {
        throw std::runtime_error( "Node shape vector has wrong size." );
    }
    for( size_t i = 0; i < tree_.node_count(); ++i ) {
        tree_.node_at(i).data<LayoutNodeData>().shape = shapes[ i ];
    }
}

// =================================================================================================
//     Init
// =================================================================================================

void LayoutBase::init_tree_( Tree const& orig_tree )
{
    // Init nodes.
    for( size_t i = 0; i < tree().node_count(); ++i ) {
        auto& node = tree().node_at(i);
        auto const& orig_node = orig_tree.node_at(i);

        // Safety: correct indices.
        assert( node.index() == i && orig_node.index() == i );

        // Set the tree node data.
        node.reset_data( LayoutNodeData::create() );

        // If the original tree has node names, use them.
        auto orig_node_data_ptr = orig_node.data_cast<DefaultNodeData>();
        if( orig_node_data_ptr ) {
            node.data<DefaultNodeData>().name = orig_node_data_ptr->name;
        }
    }

    // Init edges.
    for( size_t i = 0; i < tree().edge_count(); ++i ) {
        auto& edge = tree().edge_at(i);
        auto const& orig_edge = orig_tree.edge_at(i);

        // Safety: correct indices.
        assert( edge.index() == i && orig_edge.index() == i );

        // Set the tree edge data.
        edge.reset_data( LayoutEdgeData::create() );

        // If the original tree has edge branch lengths, use them.
        auto orig_edge_data_ptr = orig_edge.data_cast<DefaultEdgeData>();
        if( orig_edge_data_ptr ) {
            edge.data<DefaultEdgeData>().branch_length = orig_edge_data_ptr->branch_length;
        }
    }

    // Layout
    init_layout_();
}

void LayoutBase::init_layout_()
{
    if( tree().empty() ) {
        return;
    }

    // Set distances of nodes.
    if( type() == LayoutType::kCladogram ) {
        set_node_distances_cladogram_();
    } else if( type() == LayoutType::kPhylogram ) {
        set_node_distances_phylogram_();
    } else {
        assert( false );
    }

    // Set spreadings of nodes.
    set_node_spreadings_();
}

void LayoutBase::set_node_spreadings_()
{
    auto const num_leaves = static_cast<double>( leaf_node_count( tree() ));

    // Set node parents and spreading of leaves.
    size_t leaf_count = 0;
    size_t parent = 0;
    for( auto it : eulertour( tree() )) {
        auto& node_data = tree().node_at( it.node().index() ).data<LayoutNodeData>();

        if( node_data.parent_index == -1 ) {
            node_data.parent_index = parent;
        }
        if( it.node().is_leaf() ) {
            node_data.spreading = static_cast<double>(leaf_count) / num_leaves;
            leaf_count++;
        }

        parent = it.node().index();
    }
    assert( leaf_count == static_cast<size_t>( num_leaves ));

    // Min and max spreading of the children of each node.
    // Init to -1.0 so that we can check which ones are done already.
    auto chrn_min_s = std::vector<double>( tree().node_count(), -1.0 );
    auto chrn_max_s = std::vector<double>( tree().node_count(), -1.0 );

    // Set remaining spreading of inner nodes to mid-points of their children.
    for( auto it : postorder( tree() )) {
        auto const node_index = it.node().index();
        auto& node_data   = tree().node_at( node_index ).data<LayoutNodeData>();
        auto const prnt_index = node_data.parent_index;

        if( node_data.spreading < 0.0 ) {
            // We already have done those nodes because of the postorder.
            assert( chrn_min_s[ node_index ] > -1.0 );
            assert( chrn_max_s[ node_index ] > -1.0 );

            auto min_max_diff = chrn_max_s[ node_index ] - chrn_min_s[ node_index ];
            node_data.spreading = chrn_min_s[ node_index ] + min_max_diff / 2.0;
        }

        if( chrn_min_s[ prnt_index ] < 0.0 || chrn_min_s[ prnt_index ] > node_data.spreading ) {
            chrn_min_s[ prnt_index ] = node_data.spreading;
        }
        if( chrn_max_s[ prnt_index ] < 0.0 || chrn_max_s[ prnt_index ] < node_data.spreading ) {
            chrn_max_s[ prnt_index ] = node_data.spreading;
        }
    }
}

void LayoutBase::set_node_distances_phylogram_()
{
    // Get distnace from root to every node.
    auto node_dists = node_branch_length_distance_vector( tree() );

    // We already check the size in init_layout_()
    assert( ! node_dists.empty() );
    auto const max_d = *std::max_element( node_dists.begin(), node_dists.end() );

    for( size_t i = 0; i < node_dists.size(); ++i ) {
        tree().node_at(i).data<LayoutNodeData>().distance = node_dists[i] / max_d;
    }
}

void LayoutBase::set_node_distances_cladogram_()
{
    // Set root distance to 0.
    tree().root_node().data<LayoutNodeData>().distance = 0.0;

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

        // Set the distance.
        auto const distance = ( root_height - height ) / root_height;
        tree().node_at( it.node().index() ).data<LayoutNodeData>().distance = distance;
    }
}

// =================================================================================================
//     Options
// =================================================================================================

void LayoutBase::text_template( utils::SvgText const& tt )
{
    text_template_ = tt;
}

utils::SvgText& LayoutBase::text_template()
{
    return text_template_;
}

utils::SvgText const& LayoutBase::text_template() const
{
    return text_template_;
}

void LayoutBase::type( LayoutType const drawing_type )
{
    type_ = drawing_type;
    if( ! tree_.empty() ) {
        init_layout_();
    }
}

LayoutType LayoutBase::type() const
{
    return type_;
}

} // namespace tree
} // namespace genesis
