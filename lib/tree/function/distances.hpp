#ifndef GENESIS_TREE_FUNCTION_DISTANCES_H_
#define GENESIS_TREE_FUNCTION_DISTANCES_H_

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
namespace tree {

// =================================================================================================
//     Distance Measures
// =================================================================================================

template <class Tree>
utils::Matrix<int>         node_depth_matrix    (
    const Tree& tree
);

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

template <class Tree>
std::vector< std::pair<const typename Tree::NodeType*, int> >  closest_leaf_depth_vector(
    const Tree& tree
);

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

#include "tree/function/distances.tpp"

#endif // include guard
