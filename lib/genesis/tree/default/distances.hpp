#ifndef GENESIS_TREE_DEFAULT_DISTANCES_H_
#define GENESIS_TREE_DEFAULT_DISTANCES_H_

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
 * @brief Header of Default Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "genesis/tree/default/tree.hpp"
#include "genesis/utils/math/matrix.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

/**
 * @brief Return a distance matrix containing pairwise distances between all Nodes, using the
 * branch_length of the Edges as distance measurement.
 *
 * The elements of the matrix are indexed using node().index().
 */
utils::Matrix<double> node_branch_length_distance_matrix(
    Tree const& tree
);

/**
 * @brief Return a vector containing the distance of all nodes with respect to the given start node,
 * where distance is measured in the sum of branch lengths between the nodes.
 *
 * The vector is indexed using the node().index() for every node. Its elements give the distance of
 * each node with respect to the given start node. The distance is the sum of branch lengths of the
 * edges visited on the path between the two nodes.
 *
 * If no Node pointer is provided, the root is taken as node.
 */
std::vector<double> node_branch_length_distance_vector(
    Tree const& tree,
    TreeNode const* node = nullptr
);

utils::Matrix<double> edge_branch_length_distance_matrix(
    Tree const& tree
);

std::vector<double> edge_branch_length_distance_vector(
    Tree const& tree,
    TreeEdge const* edge = nullptr
);

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Return the longest distance from any point in the tree (on the edges) to any leaf.
 */
double deepest_distance( Tree const& tree );

/**
 * @brief Return a vector containing the closest leaf node for each node, using the branch_length
 * as distance measure.
 *
 * The vector is indexed using the node().index() for every node. Its value contains an std::pair,
 * where the first element is a TreeNode* to the closest leaf node of the node at the index,
 * measured using the branch_length; the second element of the pair is the distance value itself.
 * Thus, leaf nodes will have a pointer to themselves and a distance value of 0.
 *
 * See also furthest_leaf_distance_vector().
 */
std::vector<std::pair< TreeNode const*, double>> closest_leaf_distance_vector(
    Tree const& tree
);

std::vector<std::pair< TreeNode const*, double>> closest_leaf_distance_vector(
    Tree const& tree,
    utils::Matrix<double> const& node_branch_length_distance_mat
);

/**
 * @brief Opposite of closest_leaf_distance_vector().
 */
std::vector<std::pair< TreeNode const*, double>> furthest_leaf_distance_vector(
    Tree const& tree
);

std::vector<std::pair< TreeNode const*, double>> furthest_leaf_distance_vector(
    Tree const& tree,
    utils::Matrix<double> const& node_branch_length_distance_mat
);

} // namespace tree
} // namespace genesis

#endif // include guard
