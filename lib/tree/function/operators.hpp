#ifndef GENESIS_TREE_FUNCTION_OPERATORS_H_
#define GENESIS_TREE_FUNCTION_OPERATORS_H_

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

template <class TreeTypeL, class TreeTypeR>
bool equal(const TreeTypeL& lhs, const TreeTypeR& rhs);

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
#include "tree/function/operators.tpp"

#endif // include guard
