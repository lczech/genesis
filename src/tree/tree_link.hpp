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

template <class NodeDataType, class BranchDataType>
class Tree;

template <class NodeDataType, class BranchDataType>
class TreeBranch;

template <class NodeDataType, class BranchDataType>
class TreeNode;

// =============================================================================
//     TreeLink
// =============================================================================

template <class NodeDataType, class BranchDataType>
class TreeLink
{
    friend class Tree<NodeDataType, BranchDataType>;

public:
    TreeLink() : next_(nullptr), outer_(nullptr), node_(nullptr), branch_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /** @brief Returns a pointer to the next link within the node. */
    inline TreeLink<NodeDataType, BranchDataType>* Next()
    {
        return next_;
    }

    /** @brief Returns a pointer to the link of the adjacent node. */
    inline TreeLink<NodeDataType, BranchDataType>* Outer()
    {
        return outer_;
    }

    /** @brief Returns a pointer to the TreeBranch containing the data of this link's branch. */
    inline TreeBranch<NodeDataType, BranchDataType>* Branch()
    {
        return branch_;
    }

    /** @brief Returns a pointer to the TreeNode containing the data of this link's node. */
    inline TreeNode<NodeDataType, BranchDataType>* Node()
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

    TreeLink<NodeDataType, BranchDataType>* next_;
    TreeLink<NodeDataType, BranchDataType>* outer_;

    TreeNode<NodeDataType, BranchDataType>*   node_;
    TreeBranch<NodeDataType, BranchDataType>* branch_;
};

} // namespace genesis

#endif // include guard
