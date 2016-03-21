#ifndef GENESIS_TREE_TREE_LINK_H_
#define GENESIS_TREE_TREE_LINK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

namespace genesis {
namespace tree {

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
    LinkType* next() const
    {
        return next_;
    }

    /**
     * @brief Returns a pointer to the previous link within the node.
     *
     * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
     * As this link first has to be found, this function is not as cheap as next().
     */
    LinkType* prev()
    {
        LinkType* res = this;
        while (res->next() != this) {
            res = res->next();
        }
        return res;
    }

    /** @brief Returns a pointer to the link of the adjacent node. */
    LinkType* outer() const
    {
        return outer_;
    }

    /** @brief Returns a pointer to the TreeEdge containing the data of this link's edge. */
    EdgeType* edge() const
    {
        return edge_;
    }

    /** @brief Returns a pointer to the TreeNode containing the data of this link's node. */
    NodeType* node() const
    {
        return node_;
    }

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the index of this Link.
     */
    size_t index() const
    {
        return index_;
    }

    /**
     * @brief Returns true iff the node of this link is a leaf node.
     */
    bool is_leaf() const
    {
        return next_ == this;
    }

    /**
     * @brief Returns true iff the node of this link is an inner node.
     */
    bool is_inner() const
    {
        return next_ != this;
    }

    /**
     * @brief Returns a string containing dump information about this link.
     *
     * At the moment, a link does not contain any information, so an empty string is returned.
     * This might change in the future, in case links also contain data.
     */
    std::string dump() const
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

} // namespace tree
} // namespace genesis

#endif // include guard
