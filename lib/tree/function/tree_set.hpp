#ifndef GENESIS_TREE_FUNCTION_TREE_SET_H_
#define GENESIS_TREE_FUNCTION_TREE_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <functional>

namespace genesis {
namespace placement {

// =================================================================================================
//     Tree Set Functions
// =================================================================================================

template <class TreeSetType>
typename TreeSetType::TreeType      * find_tree ( TreeSetType      & tset, std::string const& name);

template <class TreeSetType>
typename TreeSetType::TreeType const* find_tree ( TreeSetType const& tset, std::string const& name);

template <class TreeSetType>
typename TreeSetType::TreeType average_branch_length_tree( TreeSetType const& tset );

// =================================================================================================
//     Comparators
// =================================================================================================

template <class TreeSetType>
bool all_equal(
    TreeSetType const& tset,
    std::function<bool(
        typename TreeSetType::TreeType::NodeType const&,
        typename TreeSetType::TreeType::NodeType const&
    )> node_comparator,
    std::function<bool(
        typename TreeSetType::TreeType::EdgeType const&,
        typename TreeSetType::TreeType::EdgeType const&
    )> edge_comparator
);

template <class TreeSetType>
bool all_equal( TreeSetType const& tset );

template <class TreeSetType>
bool all_identical_topology( TreeSetType const& tset );

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This file contains template functions, so do the inclusion of the implementation here.
#include "tree/function/tree_set.tpp"

#endif // include guard
