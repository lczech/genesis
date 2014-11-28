#ifndef GNS_TREE_TREELINK_H_
#define GNS_TREE_TREELINK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class Tree;
class TreeBranch;
class TreeNode;

// =============================================================================
//     TreeLink
// =============================================================================

class TreeLink
{
    friend class Tree;

public:
    TreeLink() : next_(nullptr), outer_(nullptr), node_(nullptr), branch_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /** @brief Returns a pointer to the next link within the node. */
    inline TreeLink* next()
    {
        return next_;
    }

    /** @brief Returns a pointer to the link of the adjacent node. */
    inline TreeLink* outer()
    {
        return outer_;
    }

    /** @brief Returns a pointer to the TreeBranch containing the data of this link's branch. */
    inline TreeBranch* branch()
    {
        return branch_;
    }

    /** @brief Returns a pointer to the TreeNode containing the data of this link's node. */
    inline TreeNode* node()
    {
        return node_;
    }

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    /** @brief Returns true iff the node of this link is a leaf node. */
    inline bool IsLeaf()
    {
        return next_ == this;
    }

    /** @brief Returns true iff the node of this link is an inner node. */
    inline bool IsInner()
    {
        return next_ != this;
    }

    /**
     * @brief Returns a string containing dump information about this link.
     *
     * At the moment, a link does not contain any information, so an empty string is returned.
     * This might change in the future, in case links also contain data.
     */
    inline std::string Dump()
    {
        return "";
    }

protected:

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    TreeLink* next_;
    TreeLink* outer_;

    TreeNode* node_;
    TreeBranch* branch_;
};

} // namespace genesis

#endif // include guard
