#ifndef GENESIS_TREE_TREE_CASTS_H_
#define GENESIS_TREE_TREE_CASTS_H_

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

#include "tree/tree/node.hpp"
#include "tree/tree/edge.hpp"
#include "tree/tree/node_data.hpp"
#include "tree/tree/edge_data.hpp"

#include <memory>

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Casts
// =================================================================================================

template< class NodeDataType >
NodeDataType& node_data_cast( std::unique_ptr< TreeNode >& node )
{
    return dynamic_cast< NodeDataType& >( node->data() );
}

template< class NodeDataType >
NodeDataType const& node_data_cast( std::unique_ptr< TreeNode > const& node )
{
    return dynamic_cast< NodeDataType const& >( node->data() );
}

template< class NodeDataType >
NodeDataType& node_data_cast( TreeNode& node )
{
    return dynamic_cast< NodeDataType& >( node.data() );
}

template< class NodeDataType >
NodeDataType const& node_data_cast( TreeNode const& node )
{
    return dynamic_cast< NodeDataType const& >( node.data() );
}

template< class NodeDataType >
NodeDataType& node_data_cast( BaseNodeData& data )
{
    return dynamic_cast< NodeDataType& >( data );
}

template< class NodeDataType >
NodeDataType const& node_data_cast( BaseNodeData const& data )
{
    return dynamic_cast< NodeDataType const& >( data );
}

// =================================================================================================
//     Edge Casts
// =================================================================================================

template< class EdgeDataType >
EdgeDataType& edge_data_cast( std::unique_ptr< TreeEdge >& edge )
{
    return dynamic_cast< EdgeDataType& >( edge->data() );
}

template< class EdgeDataType >
EdgeDataType const& edge_data_cast( std::unique_ptr< TreeEdge > const& edge )
{
    return dynamic_cast< EdgeDataType const& >( edge->data() );
}

template< class EdgeDataType >
EdgeDataType& edge_data_cast( TreeEdge& edge )
{
    return dynamic_cast< EdgeDataType& >( edge.data() );
}

template< class EdgeDataType >
EdgeDataType const& edge_data_cast( TreeEdge const& edge )
{
    return dynamic_cast< EdgeDataType const& >( edge.data() );
}

template< class EdgeDataType >
EdgeDataType& edge_data_cast( BaseEdgeData& data )
{
    return dynamic_cast< EdgeDataType& >( data );
}

template< class EdgeDataType >
EdgeDataType const& edge_data_cast( BaseEdgeData const& data )
{
    return dynamic_cast< EdgeDataType const& >( data );
}

} // namespace tree
} // namespace genesis

#endif // include guard
