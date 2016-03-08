#ifndef GENESIS_TREE_DISTANCES_H_
#define GENESIS_TREE_DISTANCES_H_

/**
 * @brief Header of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "tree/tree.hpp"
#include "tree/tree_edge.hpp"
#include "utils/math/matrix.hpp"

namespace genesis {

// =================================================================================================
//     Distance Measures
// =================================================================================================

/**
 * @brief
 *
 * The vector is indexed using the node()->index() for every node.
 */
template <class Tree>
utils::Matrix<int>         node_depth_matrix    (
    const Tree& tree
);

/**
 * @brief Returns a vector containing the depth of all nodes with respect to the given start node.
 *
 * The vector is indexed using the node()->index() for every node. Its elements give the depth of
 * each node with respect to the given start node. The depth is the number of edges visited on the
 * path between two nodes (0 for itself, 1 for immediate neighbours, etc).
 *
 * If no start node pointer is provided, the root is taken as node.
 */
template <class Tree>
std::vector<int>    node_depth_vector    (
    const Tree& tree,
    const typename Tree::NodeType* node = nullptr
);

template <class Tree>
utils::Matrix<int>         edge_depth_matrix    (
    const Tree& tree
);

template <class Tree>
std::vector<int>    edge_depth_vector    (
    const Tree& tree,
    const typename Tree::EdgeType* edge = nullptr
);

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Returns a vector containing the closest leaf node for each node, measured in number of
 * edges between them and its depth (number of edges between them).
 *
 * The vector is indexed using the node()->index() for every node. Its value contains an std::pair,
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
template <class Tree>
std::vector< std::pair<const typename Tree::NodeType*, int> >  closest_leaf_depth_vector(
    const Tree& tree
);

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

#include "tree/distances.tpp"

#endif // include guard
