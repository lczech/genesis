#ifndef GENESIS_TREE_FUNCTION_OPERATORS_H_
#define GENESIS_TREE_FUNCTION_OPERATORS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"

#include <cassert>
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
class Subtree;

// =================================================================================================
//     Data Type Checks
// =================================================================================================

/**
 * @brief Check whether the data of the nodes and edges of the Tree are exactly of the specified
 * data types.
 *
 * This function returns true iff all data types of the nodes and edges match exaclty the
 * specified types. It uses typeid() for this matching. If @p allow_null is set to `true`,
 * data pointers are allowed be empty (`null_ptr`).
 */
template< class NodeDataType, class EdgeDataType >
bool tree_data_is( Tree const& tree, bool allow_null = false )
{
    // Check node data types.
    for( auto const& node : tree.nodes() ) {
        if( ! node.has_data() ) {
            if( allow_null ) {
                continue;
            } else {
                return false;
            }
        }
        assert( node.data_ptr() );
        auto const& ref = *node.data_ptr();
        if( typeid( ref ) != typeid( NodeDataType )) {
            return false;
        }
    }

    // Check edge data types.
    for( auto const& edge : tree.edges() ) {
        if( ! edge.has_data() ) {
            if( allow_null ) {
                continue;
            } else {
                return false;
            }
        }
        assert( edge.data_ptr() );
        auto const& ref = *edge.data_ptr();
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
 * specified types. It uses dynamic_cast() for this. If @p allow_null is set to `true`,
 * data pointers are allowed be empty (`null_ptr`).
 */
template< class NodeDataType, class EdgeDataType >
bool tree_data_is_derived_from( Tree const& tree, bool allow_null = false )
{
    // Check node data types.
    for( auto const& node : tree.nodes() ) {
        if( ! node.has_data() ) {
            if( allow_null ) {
                continue;
            } else {
                return false;
            }
        }
        assert( node.data_ptr() );
        if( dynamic_cast< NodeDataType const* >( node.data_ptr() ) == nullptr ) {
            return false;
        }
    }

    // Check edge data types.
    for( auto const& edge : tree.edges() ) {
        if( ! edge.has_data() ) {
            if( allow_null ) {
                continue;
            } else {
                return false;
            }
        }
        assert( edge.data_ptr() );
        if( dynamic_cast< EdgeDataType const* >( edge.data_ptr() ) == nullptr ) {
            return false;
        }
    }

    return true;
}

// =================================================================================================
//     Conversion
// =================================================================================================

/**
 * @brief Create a tree with the same topology as the source tree, while converting its data.
 *
 * This function takes the given source Tree (possibly with different data types at the nodes and
 * edges), and copies its topology (i.e., all links, nodes and edges, and their structure) to the
 * newly created result tree.
 *
 * The data types are then converted using the two provided functions for the node data type and
 * edge data type, respectively. If a node or an edge does not have data (i.e., the data pointer
 * is a nullptr), the converter functions are not called, but the data of the new tree at that
 * node or edge is also set to a nullptr.
 */
Tree convert(
    Tree const& source,
    std::function< std::unique_ptr<BaseNodeData>( BaseNodeData const& node_data )> node_data_converter,
    std::function< std::unique_ptr<BaseEdgeData>( BaseEdgeData const& edge_data )> edge_data_converter
);

// =================================================================================================
//     Equality and Identity
// =================================================================================================

/**
 * @brief Compare two trees for equality given binary comparator functionals for their nodes and
 * edges.
 *
 * This function does a preorder traversal of both trees in parallel and calls the comparator
 * functionals for each position of the iterator. It returns true iff the comparators are true for
 * every position.
 *
 * The comparator functionals can be either function pointers, function objects, or lambda
 * expressions.
 *
 * As the traversal is done in parallel, the trees are also checked for equal topology:
 * their elements (links, nodes, edges) have to be equal in size and the degree of each node during
 * the traversal has to be identical in both trees. Those assumptions are made because two trees
 * that do not have identical topology are never considered equal.
 */
bool equal(
    Tree const& lhs,
    Tree const& rhs,
    std::function<bool ( TreeNode const&, TreeNode const&) > node_comparator,
    std::function<bool ( TreeEdge const&, TreeEdge const&) > edge_comparator
);

/**
 * @brief Compare all trees for equality given binary comparator functionals for their nodes and
 * edges.
 *
 * @see @link equal() equal( Tree const&, Tree const&, [...] )@endlink for details.
 */
bool equal(
    std::vector<Tree> const& trees,
    std::function<bool ( TreeNode const&, TreeNode const&) > node_comparator,
    std::function<bool ( TreeEdge const&, TreeEdge const&) > edge_comparator
);

// bool equal( Tree const& lhs, Tree const& rhs);

/**
 * @brief Return whether both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of leaves and branches, they might
 * still be not identical (with respect to this function) when the branches appear in a different
 * order or when the root sits at a different node.
 *
 * Futhermore, if @p identical_indices is set to `true` (default), the function also checks
 * whether the indices of nodes and edges are identical at the different nodes and edges of the
 * trees. This is important if mulitple (identical) trees are used in a calculation, where the
 * indices are used as indices into arrays or the like.
 */
bool identical_topology( Tree const& lhs, Tree const& rhs, bool identical_indices = true );

/**
 * @brief Return whether all trees have an identical topology.
 *
 * See identical_topology( Tree const&, Tree const&, bool ) for details.
 */
bool identical_topology( std::vector<Tree> const& trees, bool identical_indices = true );

// =================================================================================================
//     Element Ownership Checks
// =================================================================================================

/**
 * @brief Return whether the TreeNode belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( Tree     const& tree, TreeNode const& node );

/**
 * @brief Return whether the TreeNode belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( TreeNode const& node, Tree     const& tree );

/**
 * @brief Return whether the TreeEdge belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( Tree     const& tree, TreeEdge const& edge );

/**
 * @brief Return whether the TreeEdge belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( TreeEdge const& edge, Tree     const& tree );

/**
 * @brief Return whether the TreeLink belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( Tree     const& tree, TreeLink const& link );

/**
 * @brief Return whether the TreeLink belongs to the Tree, i.e., whether it is owned by the Tree.
 */
bool belongs_to( TreeLink const& link, Tree     const& tree );

/**
 * @brief Return whether the Subtree belongs to the Tree, i.e., whether its link is owned by the Tree.
 */
bool belongs_to( Tree     const& tree, Subtree  const& subt );

/**
 * @brief Return whether the Subtree belongs to the Tree, i.e., whether its link is owned by the Tree.
 */
bool belongs_to( Subtree  const& subt, Tree     const& tree );

/**
 * @brief Return the TreeEdge between two TreeNode&s, if they are neighbours, or `nullptr` otherwise.
 */
TreeEdge*       edge_between( TreeNode& lhs,       TreeNode& rhs );

/**
 * @brief Return the TreeEdge between two TreeNode&s, if they are neighbours, or `nullptr` otherwise.
 */
TreeEdge const* edge_between( TreeNode const& lhs, TreeNode const& rhs );

// =================================================================================================
//     Output
// =================================================================================================

std::string print_info( Tree const& tree );
std::string print_info( TreeEdge const& edge );
std::string print_info( TreeLink const& link );
std::string print_info( TreeNode const& node );

std::string print_gist( Tree const& tree, long items = 10 );

// =================================================================================================
//     Validate
// =================================================================================================

/**
 * @brief Validate that all internal pointers of the Tree elements (TreeLink%s, TreeNode%s,
 * TreeEdge%s) to each other are correct and that some other invariants are met.
 *
 * This check is a bit pedantic, but better safe than sorry.
 */
bool validate_topology( Tree const& tree );

} // namespace tree
} // namespace genesis

#endif // include guard
