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

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/drawing/layout_base.hpp"

#include "genesis/tree/common_tree/distances.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/eulertour.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/function/manipulation.hpp"

#include <cassert>
#include <stdexcept>

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

// =================================================================================================
//     Edge Strokes
// =================================================================================================

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

void LayoutBase::set_label_spacer_strokes( utils::SvgStroke const& stroke, LayoutSpreading spreading )
{
    for( size_t i = 0; i < tree_.node_count(); ++i ) {
        auto& node = tree_.node_at(i);

        bool set_stroke = ( spreading == LayoutSpreading::kAllNodes );
        set_stroke |= (( spreading == LayoutSpreading::kLeafNodesOnly ) && ( is_leaf( node ) ));
        set_stroke |= (( spreading == LayoutSpreading::kAllNodesButRoot ) && ( ! is_root( node ) ));

        if( set_stroke ) {
            node.data<LayoutNodeData>().spacer_stroke = stroke;
        }
    }
}

void LayoutBase::set_label_spacer_strokes( std::vector< utils::SvgStroke > const& strokes )
{
    // Empty: Reset to default.
    if( strokes.empty() ) {
        set_label_spacer_strokes(
            utils::SvgStroke( utils::SvgStroke::Type::kNone ),
            LayoutSpreading::kAllNodes
        );
        return;
    }

    // Non-empty case. We offer all edges or leaves only.
    if( strokes.size() == tree_.node_count() ) {
        // All nodes get a stroke.
        for( size_t i = 0; i < tree_.node_count(); ++i ) {
            tree_.node_at(i).data<LayoutNodeData>().spacer_stroke = strokes[ i ];
        }
    } else if( strokes.size() == tree_.node_count() -1 ) {
        // All nodes but the root get a stroke.
        size_t si = 0;
        for( size_t i = 0; i < tree_.node_count(); ++i ) {
            auto& node = tree_.node_at(i);
            if( ! is_root( node )) {
                node.data<LayoutNodeData>().spacer_stroke = strokes[ si ];
                ++si;
            }
        }
    } else if( strokes.size() == leaf_node_count( tree_ )) {
        // Only leaf nodes get a stroke.
        size_t si = 0;
        for( size_t i = 0; i < tree_.node_count(); ++i ) {
            auto& node = tree_.node_at(i);
            if( is_leaf( node )) {
                node.data<LayoutNodeData>().spacer_stroke = strokes[ si ];
                ++si;
            }
        }
    } else {
        throw std::runtime_error(
            "Edge stroke vector has wrong size. Has to be either tree.node_count(), "
            "tree.node_count() - 1, or leaf_edge_count( tree )."
        );
    }
}

// =================================================================================================
//     Edge and Node Shapes
// =================================================================================================

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
        auto orig_node_data_ptr = orig_node.data_cast<CommonNodeData>();
        if( orig_node_data_ptr ) {
            node.data<CommonNodeData>().name = orig_node_data_ptr->name;
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
        auto orig_edge_data_ptr = orig_edge.data_cast<CommonEdgeData>();
        if( orig_edge_data_ptr ) {
            edge.data<CommonEdgeData>().branch_length = orig_edge_data_ptr->branch_length;
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

    // Set node parent indices.
    size_t parent = tree_.root_node().index();
    for( auto it : eulertour( tree() )) {
        auto& node_data = tree().node_at( it.node().index() ).data<LayoutNodeData>();
        if( node_data.parent_index == -1 ) {
            node_data.parent_index = parent;
        }
        parent = it.node().index();
    }

    // Set distances of nodes.
    if( type() == LayoutType::kCladogram ) {
        set_node_distances_cladogram_();
    } else if( type() == LayoutType::kPhylogram ) {
        set_node_distances_phylogram_();
    } else {
        throw std::runtime_error( "Invalid LayoutType value." );
    }

    // Set spreadings of nodes.
    if( inner_node_spreading_ == LayoutSpreading::kLeafNodesOnly ) {
        set_node_spreadings_leaves_();
    } else {
        set_node_spreadings_all_( inner_node_spreading_ );
    }
}

void LayoutBase::set_node_spreadings_leaves_()
{
    // Maximum number of nodes that we spread. We are actually spreading intervals between nodes,
    // and not nodes themselves, so we need to subtract 1.
    auto const num_leaves = leaf_node_count( tree() ) - 1;

    // Set spreading of leaves.
    size_t leaf_count = 0;
    for( auto it : eulertour( tree() )) {
        auto& node_data = tree().node_at( it.node().index() ).data<LayoutNodeData>();
        if( is_leaf( it.node() )) {
            auto const lcd = static_cast<double>( leaf_count );
            auto const nld = static_cast<double>( num_leaves );
            node_data.spreading = lcd / nld;
            leaf_count++;
        }
    }
    assert( leaf_count == num_leaves + 1 );

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
            // We already have done the following nodes because of the postorder.
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

void LayoutBase::set_node_spreadings_all_( LayoutSpreading spreading )
{
    assert( ! tree_.empty() );
    if( ! is_bifurcating( tree_ )) {
        throw std::invalid_argument(
            "Tree is not bifurcating. Cannot draw with inner node spreading."
        );
    }
    // We now allow unrooted tree (top level trifurcation) as well ;-)
    // if( ! is_rooted( tree_ )) {
    //     throw std::invalid_argument(
    //         "Tree is not rooted. Cannot draw with inner node spreading."
    //     );
    // }

    // Maximum number of nodes that we spread. We are actually spreading intervals between nodes,
    // and not nodes themselves, so we need to subtract 1.
    auto const num_nodes = tree().node_count() - 1 - (
        spreading == LayoutSpreading::kAllNodesButRoot ? 1 : 0
    );
    auto visits = std::vector<size_t>( tree_.node_count(), 0 );
    size_t node_counter = 0;

    for( auto it : eulertour( tree_ )) {
        auto&      node_data  = it.node().data<LayoutNodeData>();
        auto const node_index = it.node().index();

        // Count the how many-th visit this is. As we have a bifurcating tree,
        // it can never surpass 3 visits.
        ++visits[ node_index ];
        assert( visits[ node_index ] <= 3 );

        if( spreading == LayoutSpreading::kAllNodesButRoot && is_root( it.node() )) {
            continue;
        } else if( is_leaf( it.node() ) || visits[ node_index ] == 2 ) {
            auto const ncd = static_cast<double>( node_counter );
            auto const nnd = static_cast<double>( num_nodes );
            node_data.spreading = ncd / nnd;
            ++node_counter;
        }
    }
    assert( node_counter == num_nodes + 1 );

    // Special case for the root if we do not want to spread it:
    // if the root is bifurcating (actual root), set its spread to the middle.
    // if it is a virtual root (top level trifurcation), set its spread the the mid node.
    if( spreading == LayoutSpreading::kAllNodesButRoot ) {
        auto& root = tree_.root_node();
        auto& root_data = root.data<LayoutNodeData>();
        auto const degr = degree( root );
        // assert( root_data.spreading < 0.0 );

        if( degr == 2 ) {
            auto const& l_data = root.link().outer().node().data<LayoutNodeData>();
            auto const& r_data = root.link().next().outer().node().data<LayoutNodeData>();
            root_data.spreading = ( l_data.spreading + r_data.spreading ) / 2.0;
        } else {
            assert( degr == 3 );
            auto const& l_data = root.link().outer().node().data<LayoutNodeData>();
            auto const& r_data = root.link().next().next().outer().node().data<LayoutNodeData>();
            root_data.spreading = ( l_data.spreading + r_data.spreading ) / 2.0;

            // auto const& m_data = root.link().next().outer().node().data<LayoutNodeData>();
            // root_data.spreading = m_data.spreading;
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

void LayoutBase::inner_node_spreading( LayoutSpreading value )
{
    inner_node_spreading_ = value;
    if( ! tree_.empty() ) {
        init_layout_();
    }
}

LayoutSpreading LayoutBase::inner_node_spreading() const
{
    return inner_node_spreading_;
}

void LayoutBase::align_labels( bool value )
{
    align_labels_ = value;
}

bool LayoutBase::align_labels() const
{
    return align_labels_;
}

void LayoutBase::extra_spacer( double value )
{
    extra_spacer_ = value;
}

double LayoutBase::extra_spacer() const
{
    return extra_spacer_;
}

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

} // namespace tree
} // namespace genesis
