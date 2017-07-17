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

#include "genesis/tree/drawing/layout_base.hpp"

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
}

LayoutBase::Type LayoutBase::type() const
{
    return type_;
}

} // namespace tree
} // namespace genesis
