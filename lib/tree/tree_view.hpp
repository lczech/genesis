#ifndef GENESIS_TREE_TREE_VIEW_H_
#define GENESIS_TREE_TREE_VIEW_H_

/**
 * @brief Header of TreeView class.
 *
 * @file
 * @ingroup tree
 */

#include <functional>
#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     TreeView
// =================================================================================================

/**
 * @brief
 */
class TreeView
{
public:

    // -------------------------------------------------------------------------
    //     Viewing Methods
    // -------------------------------------------------------------------------

    template <typename TreeType>
    std::string compact (
        const TreeType& tree,
        const std::function<std::string (typename TreeType::ConstIteratorPreorder& it)> print_line
    );

    template <typename TreeType>
    std::string compact (const TreeType& tree);

};

} // namespace tree
} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/tree_view.tpp"

#endif // include guard
