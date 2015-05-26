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

#include "tree/newick_processor.hpp"

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
class TreeNode : public NodeDataType
{
    friend Tree<NodeDataType, EdgeDataType>;

public:
    TreeNode() : link_(nullptr) {}

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
    inline LinkType* PrimaryLink() const
    {
        return link_;
    }

    /**
     * @brief Returns the link of this node that points towards the root.
     *
     * This is just an alias for PrimaryLink(), that is shorter to use when needed
     * frequently in an algorithm.
     */
    inline LinkType* Link() const
    {
        return link_;
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    typedef TreeNodeIteratorLinks<      LinkType*,       NodeType*,       EdgeType*>      IteratorLinks;
    typedef TreeNodeIteratorLinks<const LinkType*, const NodeType*, const EdgeType*> ConstIteratorLinks;

    inline IteratorLinks BeginLinks()
    {
        return IteratorLinks(link_);
    }

    inline IteratorLinks EndLinks()
    {
        return IteratorLinks(nullptr);
    }

    inline ConstIteratorLinks BeginLinks() const
    {
        return ConstIteratorLinks(link_);
    }

    inline ConstIteratorLinks EndLinks() const
    {
        return ConstIteratorLinks(nullptr);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    /**
     * @brief Returns the index of this Link.
     */
    inline size_t Index() const
    {
        return index_;
    }

    int  Rank() const;
    bool IsLeaf() const;
    bool IsInner() const;

    //~ /** True if the node is the root, false otherwise. */
    //~ bool IsRoot();

    //~ /** Depth of the node in the tree, i.e. its distance from root. */
    //~ int Depth();

    std::string Dump() const;

    // -----------------------------------------------------
    //     Member Variables
    // -----------------------------------------------------

    //~ NodeDataType data;

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
