#ifndef GENESIS_TREE_FUNCTION_DISTANCES_H_
#define GENESIS_TREE_FUNCTION_DISTANCES_H_

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
 * @brief Header of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "genesis/utils/containers/matrix.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Foward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Node Distance Measures
// =================================================================================================

/**
 * @brief Return a matrix containing the pairwise depth of all nodes of the tree.
 *
 * See @link node_path_length_vector( Tree const& tree, TreeNode const& node ) node_path_length_vector(...)@endlink
 * for more information.
 *
 * The vector is indexed using the node().index() for every node.
 */
utils::Matrix<size_t> node_path_length_matrix( Tree const& tree );

/**
 * @brief Return a vector containing the depth of all nodes with respect to the given start node.
 *
 * The vector is indexed using the node().index() for every node. Its elements give the depth of
 * each node with respect to the given start node. The depth is the number of edges visited on the
 * path between two nodes (0 for itself, 1 for immediate neighbours, etc).
 */
std::vector<size_t> node_path_length_vector(
    Tree     const& tree,
    TreeNode const& node
);

/**
 * @brief Return a vector containing the depth of all nodes with respect to the root node.
 *
 * This function calls and returns the value of
 * @link node_path_length_vector( Tree const& tree, TreeNode const& node ) node_path_length_vector(...)@endlink
 * using the root node of the tree.
 */
std::vector<size_t> node_path_length_vector(
    Tree     const& tree
);

// =================================================================================================
//     Edge Distance Measures
// =================================================================================================

utils::Matrix<size_t> edge_path_length_matrix(
    Tree const& tree
);

std::vector<size_t> edge_path_length_vector(
    Tree     const& tree,
    TreeEdge const& edge
);

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Returns a vector containing the closest leaf node for each node, measured in number of
 * edges between them and its depth (number of edges between them).
 *
 * The vector is indexed using the node().index() for every node. Its value contains an std::pair,
 * where the first element is a NodeType* to the closest leaf node (with respect to its depth) and
 * the second element its depth with respect to the node at the given index of the vector. The depth
 * is the number of edges visited on the path between two nodes (0 for itself, 1 for immediate
 * neighbours, etc).
 *
 * Thus, leaf nodes will have a pointer to themselves and a depth value of 0, and for all other
 * nodes the depth will be the number of edges between it and the closest leaf node.
 *
 * There might be more than one leaf with the same depth to a given node. In this case, an
 * arbitrary one is used.
 */
std::vector< std::pair< TreeNode const*, size_t >>  closest_leaf_depth_vector(
    Tree const& tree
);

} // namespace tree
} // namespace genesis

#endif // include guard
