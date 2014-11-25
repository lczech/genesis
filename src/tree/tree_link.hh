#ifndef GNS_TREE_TREELINK_H_
#define GNS_TREE_TREELINK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/tree_branch.hh"
#include "tree/tree_node.hh"

namespace genesis {

class TreeLink
{
public:
    TreeLink();

protected:
    TreeLink* outer_;
    TreeLink* next_;

    TreeNode* node_;
    TreeBranch* branch_;
};

} // namespace genesis

#endif // include guard
