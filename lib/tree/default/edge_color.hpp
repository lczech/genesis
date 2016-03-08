#ifndef GENESIS_TREE_DEFAULT_EDGE_COLOR_H_
#define GENESIS_TREE_DEFAULT_EDGE_COLOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class Color;
}

// =================================================================================================
//     Edge Color Functions
// =================================================================================================

template<class TreeType>
std::vector<utils::Color> edge_color_branch_length_gradient(
    TreeType const& tree,
    bool zero_based = false
);

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

#include "tree/default/edge_color.tpp"

#endif // include guard
