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

// =================================================================================================
//     Equality
// =================================================================================================

/**
 * @brief Compares two trees for equality given a binary comparator functional.
 *
 * This function does a preorder traversal of both trees in parallel and calls a comparator
 * functional for each position of the iterator. It returns true iff the comparator is true for
 * every position.
 *
 * The comparator functional can be either a function pointer, function object, or lambda
 * expression.
 *
 * Furthermore, the trees are checked for equal topology: their elements (links, nodes, edges)
 * have to be equal in size and the rank of each node during the traversal has to be identical in
 * both trees. Those assumptions are made because two trees that do not have identical topology
 * are never considered equal for the purposes of this framework.
 */
template <class TreeType>
bool equal(
    const TreeType& lhs,
    const TreeType& rhs,
    const std::function<bool
        (typename TreeType::ConstIteratorPreorder&, typename TreeType::ConstIteratorPreorder&)
    > comparator
);

/**
 * @brief Compares two trees for equality using the respective comparision operators for their nodes
 * and edges.
 *
 * This method is mainly a shortcut for the other equal function, where the comparator functional
 * is instanciated using the default comparision operators of the tree's data.
 */
template <class TreeType>
bool equal(const TreeType& lhs, const TreeType& rhs);

/**
 * @brief Returns true iff both trees have an identical topology.
 *
 * The topology is considered identical only if the order of edges is also the same in both trees.
 * This means, although two trees might have the same number of leaves and branches, they might
 * still be not identical (with respect to this function) when the branches appear in a different
 * order or when the root sits at a different node.
 */
template <class TreeTypeA, class TreeTypeB>
bool identical_topology(const TreeTypeA& lhs, const TreeTypeB& rhs);

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/operators.tpp"

#endif // include guard
