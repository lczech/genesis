#ifndef GENESIS_TREE_FUNCTIONS_H_
#define GENESIS_TREE_FUNCTIONS_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "tree/tree.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Count Properties
// =================================================================================================

// TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

template <class Tree>
int max_rank( Tree const& tree );

template <class Tree>
bool is_bifurcating( Tree const& tree );

template <class Tree>
size_t leaf_nodes_count( Tree const& tree );

template <class Tree>
size_t inner_nodes_count( Tree const& tree );

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

#include "tree/functions.tpp"

#endif // include guard
