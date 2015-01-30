#ifndef GNS_TREE_TREEEDGE_H_
#define GNS_TREE_TREEEDGE_H_

/**
 * @brief This class represents an edge inside of a tree.
 *
 * For more information, see TreeEdge class.
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
class  TreeNode;

// =============================================================================
//     DefaultEdgeData
// =============================================================================

class DefaultEdgeData
{
public:
    typedef double BranchLength;
    BranchLength branch_length;

    inline void FromTreeBrokerNode (TreeBrokerNode* node)
    {
        branch_length = node->branch_length;
    }

    inline void ToTreeBrokerNode (TreeBrokerNode* node) const
    {
        node->branch_length = branch_length;
    }

    inline std::string Dump() const
    {
        return "Length: " + std::to_string(branch_length);
    }
};

// =============================================================================
//     TreeEdge
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeEdge
{
    friend class Tree<NodeDataType, EdgeDataType>;

public:
    TreeEdge() : link_p_(nullptr), link_s_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the link of this edge that points towards the root.
     */
    inline TreeLink<NodeDataType, EdgeDataType>* PrimaryLink() const
    {
        return link_p_;
    }

    /**
     * @brief Returns the link of this edge that points away from the root.
     */
    inline TreeLink<NodeDataType, EdgeDataType>* SecondaryLink() const
    {
        return link_s_;
    }

    TreeNode<NodeDataType, EdgeDataType>* PrimaryNode() const;
    TreeNode<NodeDataType, EdgeDataType>* SecondaryNode() const;

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

    void FromTreeBrokerNode (TreeBrokerNode* node);

    std::string Dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    EdgeDataType data;

protected:

    int index_;

    TreeLink<NodeDataType, EdgeDataType>* link_p_;
    TreeLink<NodeDataType, EdgeDataType>* link_s_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_edge.tpp"

#endif // include guard
