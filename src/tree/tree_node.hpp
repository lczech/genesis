#ifndef GNS_TREE_TREENODE_H_
#define GNS_TREE_TREENODE_H_

/**
 * @brief This class represents a node inside of a tree.
 *
 * For more information, see TreeNode class.
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/tree_broker.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class TreeNodeIteratorLinks;

// =============================================================================
//     DefaultNodeData
// =============================================================================

class DefaultNodeData
{
public:
    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name;

    inline void FromTreeBrokerNode (TreeBrokerNode* node)
    {
        name = node->name;
    }

    inline void ToTreeBrokerNode (TreeBrokerNode* node) const
    {
        node->name = name;
    }

    inline std::string Dump() const
    {
        return "Name: '" + name + "'";
    }
};

// =============================================================================
//     TreeNode
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeNode
{
    friend Tree<NodeDataType, EdgeDataType>;

public:
    TreeNode() : link_(nullptr) {}

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    /**
     * @brief Returns the link of this node that points towards the root.
     */
    inline TreeLink<NodeDataType, EdgeDataType>* PrimaryLink() const
    {
        return link_;
    }

    /**
     * @brief Returns the link of this node that points towards the root.
     *
     * This is just an alias for PrimaryLink(), that is shorter to use when needed
     * frequently in an algorithm.
     */
    inline TreeLink<NodeDataType, EdgeDataType>* Link() const
    {
        return link_;
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

    typedef TreeNodeIteratorLinks<NodeDataType, EdgeDataType> IteratorLinks;

    inline IteratorLinks BeginLinks()
    {
        return IteratorLinks(link_);
    }

    inline IteratorLinks EndLinks()
    {
        return IteratorLinks(nullptr);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    int  Rank() const;
    bool IsLeaf() const;
    bool IsInner() const;

    //~ /** True if the node is the root, false otherwise. */
    //~ bool IsRoot();

    //~ /** Depth of the node in the tree, i.e. its distance from root. */
    //~ int Depth();

    void FromTreeBrokerNode (TreeBrokerNode* node);

    std::string Dump() const;

    // -----------------------------------------------------
    //     Member Variables
    // -----------------------------------------------------

    NodeDataType data;

protected:
    TreeLink<NodeDataType, EdgeDataType>* link_;
};

// =============================================================================
//     Iterator Links
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeNodeIteratorLinks
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeNodeIteratorLinks<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                         iterator_category;
    typedef TreeLink<NodeDataType, EdgeDataType>              value_type;
    typedef TreeLink<NodeDataType, EdgeDataType>&             reference;
    typedef TreeLink<NodeDataType, EdgeDataType>*             pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeNodeIteratorLinks (pointer link) :
    link_(link), start_(link)
    {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        link_ = link_->Next();
        if (link_ == start_) {
            link_ = nullptr;
        }
        return *this;
    }

    inline self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    inline bool operator == (const self_type &other) const
    {
        return other.link_ == link_;
    }

    inline bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    inline reference operator * ()
    {
        return *link_;
    }

    inline pointer operator -> ()
    {
        return link_;
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline TreeLink<NodeDataType, EdgeDataType>* Link()
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node()
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge()
    {
        return link_->Edge();
    }

    inline TreeLink<NodeDataType, EdgeDataType>* StartLink()
    {
        return start_;
    }

protected:
    pointer link_;
    pointer start_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_node.tpp"

#endif // include guard
