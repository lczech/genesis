#ifndef GENESIS_TREE_DEFAULT_FUNCTIONS_H_
#define GENESIS_TREE_DEFAULT_FUNCTIONS_H_

/**
 * @brief Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include<string>
#include<vector>

namespace genesis {

// =================================================================================================
//     Node Names
// =================================================================================================

template <class TreeType>
std::vector<std::string> node_names(
    TreeType& tree,
    bool leaves_only = false
);

template <class TreeType>
typename TreeType::NodeType* find_node(
    TreeType& tree,
    const std::string& name,
    bool replace_underscores = false
);

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/default/functions.tpp"

#endif // include guard
