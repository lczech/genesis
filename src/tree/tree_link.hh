#ifndef GNS_TREE_TREELINK_H_
#define GNS_TREE_TREELINK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// forward declarations
class Tree;
class TreeBranch;
class TreeNode;

class TreeLink
{
    friend class Tree;

public:
    TreeLink() {}

    inline TreeBranch* branch()
    {
        return branch_;
    }

    inline TreeNode* node()
    {
        return node_;
    }

protected:
    TreeLink* outer_;
    TreeLink* next_;

    TreeNode* node_;
    TreeBranch* branch_;
};

} // namespace genesis

#endif // include guard
