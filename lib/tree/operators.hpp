#ifndef GENESIS_TREE_OPERATORS_H_
#define GENESIS_TREE_OPERATORS_H_

/**
 * @brief Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include <functional>

namespace genesis {
namespace tree {

// =================================================================================================
//     Equality
// =================================================================================================

/**
 * @brief Compares two trees for equality given binary comparator functionals for their nodes and
 * edges.
 *
 * This function does a preorder traversal of both trees in parallel and calls the comparator
 * functionals for each position of the iterator. It returns true iff the comparator is true for
 * every position.
 *
 * The comparator functionals can be either function pointers, function objects, or lambda
 * expressions.
 *
 * As the traversal is done in parallel, the trees are also checked for equal topology:
 * their elements (links, nodes, edges) have to be equal in size and the rank of each node during
 * the traversal has to be identical in both trees. Those assumptions are made because two trees
 * that do not have identical topology are never considered equal.
 */
template <class TreeTypeL, class TreeTypeR>
bool equal(
    const TreeTypeL& lhs,
    const TreeTypeR& rhs,
    std::function<bool
        (const typename TreeTypeL::NodeType&, const typename TreeTypeR::NodeType&)
    > node_comparator,
    std::function<bool
        (const typename TreeTypeL::EdgeType&, const typename TreeTypeR::EdgeType&)
    > edge_comparator
);

/**
 * @brief Compares two trees for equality using the respective comparision operators for their nodes
 * and edges.
 *
 * This method is mainly a shortcut for the other equal function, where the comparator functionals
 * are instanciated using the default comparision operators of the tree's data.
 */
template <class TreeTypeL, class TreeTypeR>
bool equal(const TreeTypeL& lhs, const TreeTypeR& rhs);

/**
 * @brief Returns true iff both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of leaves and branches, they might
 * still be not identical (with respect to this function) when the branches appear in a different
 * order or when the root sits at a different node.
 */
template <class TreeTypeL, class TreeTypeR>
bool identical_topology(const TreeTypeL& lhs, const TreeTypeR& rhs);

// =================================================================================================
//     Validate
// =================================================================================================

template <class TreeType>
bool validate( TreeType const& tree );

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/operators.tpp"

#endif // include guard
