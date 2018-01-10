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

#include <cassert>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree
// =================================================================================================

void LayoutBase::tree( Tree const& orig_tree )
{
    tree_ = orig_tree.clone_topology();
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

void LayoutBase::set_edge_strokes( std::vector< utils::SvgStroke > const& strokes )
{
    if( strokes.size() != tree_.edge_count() ) {
        throw std::runtime_error( "Edge stroke vector has wrong size." );
    }
    for( size_t i = 0; i < tree_.edge_count(); ++i ) {
        tree_.edge_at(i).data<LayoutEdgeData>().stroke = strokes[ i ];
    }
}

void LayoutBase::set_node_shapes( std::vector< utils::SvgGroup> const& shapes )
{
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
        init_node_( node, orig_node );

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
        init_edge_( edge, orig_edge );

        // If the original tree has edge branch lengths, use them.
        auto orig_edge_data_ptr = orig_edge.data_cast<DefaultEdgeData>();
        if( orig_edge_data_ptr ) {
            edge.data<DefaultEdgeData>().branch_length = orig_edge_data_ptr->branch_length;
        }
    }

    // Layout
    init_layout_();
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

void LayoutBase::type( Type const drawing_type )
{
    type_ = drawing_type;
    if( ! tree_.empty() ) {
        init_layout_();
    }
}

LayoutBase::Type LayoutBase::type() const
{
    return type_;
}

} // namespace tree
} // namespace genesis
