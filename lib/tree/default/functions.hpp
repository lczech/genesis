#ifndef GENESIS_TREE_DEFAULT_FUNCTIONS_H_
#define GENESIS_TREE_DEFAULT_FUNCTIONS_H_

/**
 * @brief Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// =================================================================================================
//     Find
// =================================================================================================

/**
 * @brief Find a Node, given its name.
 */
template <class TreeType>
typename TreeType::NodeType* find_node(TreeType& tree, const std::string& name);

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/default/functions.tpp"

#endif // include guard
