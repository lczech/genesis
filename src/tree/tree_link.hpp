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

template <class NodeDataType, class EdgeDataType>
class Tree;

template <class NodeDataType, class EdgeDataType>
class TreeEdge;

template <class NodeDataType, class EdgeDataType>
class TreeNode;

// =============================================================================
//     TreeLink
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeLink
{
    friend class Tree<NodeDataType, EdgeDataType>;

public:
    TreeLink() : next_(nullptr), outer_(nullptr), node_(nullptr), edge_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /** @brief Returns a pointer to the next link within the node. */
    inline TreeLink<NodeDataType, EdgeDataType>* Next()
    {
        return next_;
    }

    /** @brief Returns a pointer to the link of the adjacent node. */
    inline TreeLink<NodeDataType, EdgeDataType>* Outer()
    {
        return outer_;
    }

    /** @brief Returns a pointer to the TreeEdge containing the data of this link's edge. */
    inline TreeEdge<NodeDataType, EdgeDataType>* Edge()
    {
        return edge_;
    }

    /** @brief Returns a pointer to the TreeNode containing the data of this link's node. */
    inline TreeNode<NodeDataType, EdgeDataType>* Node()
    {
        return node_;
    }

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the index of this Link.
     */
    inline int Index()
    {
        return index_;
    }

    /**
     * @brief Returns true iff the node of this link is a leaf node.
     */
    inline bool IsLeaf()
    {
        return next_ == this;
    }

    /**
     * @brief Returns true iff the node of this link is an inner node.
     */
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

    int index_;

    TreeLink<NodeDataType, EdgeDataType>* next_;
    TreeLink<NodeDataType, EdgeDataType>* outer_;

    TreeNode<NodeDataType, EdgeDataType>* node_;
    TreeEdge<NodeDataType, EdgeDataType>* edge_;
};

} // namespace genesis

#endif // include guard
