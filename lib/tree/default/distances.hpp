#ifndef GENESIS_TREE_DEFAULT_DISTANCES_H_
#define GENESIS_TREE_DEFAULT_DISTANCES_H_

/**
 * @brief Header of Default Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "tree/default_tree.hpp"
#include "utils/math/matrix.hpp"

namespace genesis {

// =================================================================================================
//     Distance Related Propoerties
// =================================================================================================

/**
 * @brief Returns the length of the tree (sum of all branch lengths).
 */
template <class Tree>
double length(const Tree& tree);

/**
 * @brief Returns the height of the tree (longest distance from root to a leaf).
 */
template <class Tree>
double height(const Tree& tree);

template <class Tree>
double depth(const Tree& tree);

/**
 * @brief Returns the longest distance from any point in the tree (on the edges) to any leaf.
 */
template <class Tree>
double deepest_distance(const Tree& tree);

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

/**
 * @brief Returns a distance matrix containing pairwise distances between all Nodes, using the
 * branch_length of the Edges as distance measurement.
 *
 * The elements of the matrix are indexed using node()->index().
 */
template <class Tree>
Matrix<double>      node_distance_matrix (
    const Tree& tree
);

/**
 * @brief Returns a vector containing the distance of all nodes with respect to the given start node.
 *
 * The vector is indexed using the node()->index() for every node. Its elements give the distance of
 * each node with respect to the given start node. The distance is the sum of branch lengths of the
 * edges visited on the path between the two nodes.
 *
 * If no Node pointer is provided, the root is taken as node.
 */
template <class Tree>
std::vector<double> node_distance_vector (
    const Tree& tree,
    const typename Tree::NodeType* node = nullptr
);

template <class Tree>
Matrix<double>      edge_distance_matrix (
    const Tree& tree
);

template <class Tree>
std::vector<double> edge_distance_vector (
    const Tree& tree,
    const typename Tree::EdgeType* edge = nullptr
);

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Returns a vector containing the closest leaf node for each node, using the branch_length
 * as distance measure.
 *
 * The vector is indexed using the node()->index() for every node. Its value contains an std::pair,
 * where the first element is a NodeType* to the closest leaf node of the node at the index,
 * measured using the branch_length; the second element of the pair is the distance value itself.
 * Thus, leaf nodes will have a pointer to themselves and a distance value of 0.
 */
template <class Tree>
std::vector<std::pair<const typename Tree::NodeType*, double>> closest_leaf_distance_vector(
    const Tree& tree
);

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

#include "tree/default/distances.tpp"

#endif // include guard
