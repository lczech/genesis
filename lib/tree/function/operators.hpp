#ifndef GENESIS_TREE_FUNCTION_OPERATORS_H_
#define GENESIS_TREE_FUNCTION_OPERATORS_H_

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
 * @brief Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree.hpp"

#include <functional>
#include <iosfwd>
#include <memory>
#include <typeinfo>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class BaseNodeData;
class BaseEdgeData;

// =================================================================================================
//     Data Type Checks
// =================================================================================================

/**
 * @brief Check whether the data of the nodes and edges of the Tree are exactly of the specified
 * data types.
 *
 * This function returns true iff all data types of the nodes and edges match exaclty the
 * specified types. It uses typeid() for this matching.
 */
template< class NodeDataType, class EdgeDataType >
bool tree_data_is( Tree const& tree )
{
    // Check node data types.
    for( auto const& node : tree.nodes() ) {
        if( ! node->has_data() ) {
            return false;
        }
        auto const& ref = *node->data_ptr();
        if( typeid( ref ) != typeid( NodeDataType )) {
            return false;
        }
    }

    // Check edge data types.
    for( auto const& edge : tree.edges() ) {
        if( ! edge->has_data() ) {
            return false;
        }
        auto const& ref = *edge->data_ptr();
        if( typeid( ref ) != typeid( EdgeDataType )) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Check whether the data of the nodes and edges of the Tree are derived from the specified
 * data types.
 *
 * This function returns true iff all data types of the nodes and edges are derived from the
 * specified types. It uses dynamic_cast() for this.
 */
template< class NodeDataType, class EdgeDataType >
bool tree_data_is_derived_from( Tree const& tree )
{
    // Check node data types.
    for( auto const& node : tree.nodes() ) {
        if( dynamic_cast< NodeDataType const* >( node->data_ptr() ) == nullptr ) {
            return false;
        }
    }

    // Check edge data types.
    for( auto const& edge : tree.edges() ) {
        if( dynamic_cast< EdgeDataType const* >( edge->data_ptr() ) == nullptr ) {
            return false;
        }
    }

    return true;
}

// =================================================================================================
//     Conversion
// =================================================================================================

Tree convert(
    Tree const& source,
    std::function< std::unique_ptr<BaseNodeData>( BaseNodeData const& node_data )> node_data_converter,
    std::function< std::unique_ptr<BaseEdgeData>( BaseEdgeData const& edge_data )> edge_data_converter
);

// =================================================================================================
//     Equality
// =================================================================================================

bool equal(
    Tree const& lhs,
    Tree const& rhs,
    std::function<bool ( TreeNode const&, TreeNode const&) > node_comparator,
    std::function<bool ( TreeEdge const&, TreeEdge const&) > edge_comparator
);

// bool equal( Tree const& lhs, Tree const& rhs);

bool identical_topology( Tree const& lhs, Tree const& rhs);

bool belongs_to( Tree const& tree, TreeNode const& node );
bool belongs_to( Tree const& tree, TreeEdge const& edge );
bool belongs_to( Tree const& tree, TreeLink const& link );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Tree const& tree );

// =================================================================================================
//     Validate
// =================================================================================================

bool validate_topology( Tree const& tree );

} // namespace tree
} // namespace genesis

#endif // include guard
