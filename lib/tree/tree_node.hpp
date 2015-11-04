#ifndef GENESIS_TREE_TREE_NODE_H_
#define GENESIS_TREE_TREE_NODE_H_

/**
 * @brief This class represents a node inside of a tree.
 *
 * For more information, see TreeNode class.
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
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeNodeIteratorLinks;

// =============================================================================
//     TreeNode
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeNode
{
    friend Tree<NodeDataType, EdgeDataType>;

public:
    TreeNode() : link_(nullptr) {}

    // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!! delete all copy and move stuff!!!

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.
    TreeNode (const TreeNode&) = delete;
    TreeNode& operator = (const TreeNode&) = delete;

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef Tree    <NodeDataType, EdgeDataType> TreeType;
    typedef TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef TreeNode<NodeDataType, EdgeDataType> NodeType;
    typedef TreeEdge<NodeDataType, EdgeDataType> EdgeType;

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    /**
     * @brief Returns the link of this node that points towards the root.
     */
    inline LinkType* primary_link() const
    {
        return link_;
    }

    /**
     * @brief Returns the link of this node that points towards the root.
     *
     * This is just an alias for primary_link(), that is shorter to use when needed
     * frequently in an algorithm.
     */
    inline LinkType* link() const
    {
        return link_;
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    typedef TreeNodeIteratorLinks<      LinkType*,       NodeType*,       EdgeType*>      IteratorLinks;
    typedef TreeNodeIteratorLinks<const LinkType*, const NodeType*, const EdgeType*> ConstIteratorLinks;

    inline IteratorLinks begin_links()
    {
        return IteratorLinks(link_);
    }

    inline IteratorLinks end_links()
    {
        return IteratorLinks(nullptr);
    }

    inline ConstIteratorLinks begin_links() const
    {
        return ConstIteratorLinks(link_);
    }

    inline ConstIteratorLinks end_links() const
    {
        return ConstIteratorLinks(nullptr);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    /**
     * @brief Returns the index of this link.
     */
    inline size_t index() const
    {
        return index_;
    }

    size_t rank() const;
    bool   is_leaf() const;
    bool   is_inner() const;

    //~ /** True if the node is the root, false otherwise. */
    //~ bool IsRoot();

    //~ /** Depth of the node in the tree, i.e. its distance from root. */
    //~ int Depth();

    std::string dump() const;

    // -----------------------------------------------------
    //     Member Variables
    // -----------------------------------------------------

    NodeDataType data;

// TODO !!! make protected again, and use some other mechanism for setting the members !!!
//~ protected:

    size_t index_;

    LinkType* link_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the iterator
// =============================================================================

#include "tree/tree_node_iterator.hpp"

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

#include "tree/tree_node.tpp"

#endif // include guard
