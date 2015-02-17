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

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.
    TreeLink (const TreeLink&) = delete;
    TreeLink& operator = (const TreeLink&) = delete;

    // ---------------------------------------------------------------------
    //     Typedefs
    // ---------------------------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /** @brief Returns a pointer to the next link within the node. */
    inline LinkType* Next() const
    {
        return next_;
    }

    /** @brief Returns a pointer to the link of the adjacent node. */
    inline LinkType* Outer() const
    {
        return outer_;
    }

    /** @brief Returns a pointer to the TreeEdge containing the data of this link's edge. */
    inline EdgeType* Edge() const
    {
        return edge_;
    }

    /** @brief Returns a pointer to the TreeNode containing the data of this link's node. */
    inline NodeType* Node() const
    {
        return node_;
    }

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the index of this Link.
     */
    inline size_t Index() const
    {
        return index_;
    }

    /**
     * @brief Returns true iff the node of this link is a leaf node.
     */
    inline bool IsLeaf() const
    {
        return next_ == this;
    }

    /**
     * @brief Returns true iff the node of this link is an inner node.
     */
    inline bool IsInner() const
    {
        return next_ != this;
    }

    /**
     * @brief Returns a string containing dump information about this link.
     *
     * At the moment, a link does not contain any information, so an empty string is returned.
     * This might change in the future, in case links also contain data.
     */
    inline std::string Dump() const
    {
        return "";
    }

// TODO !!! make protected again, and use some other mechanism for setting the members !!!
//~ protected:

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    size_t index_;

    LinkType* next_;
    LinkType* outer_;

    NodeType* node_;
    EdgeType* edge_;
};

} // namespace genesis

#endif // include guard
