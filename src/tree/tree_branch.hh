#ifndef GNS_TREE_TREEBRANCH_H_
#define GNS_TREE_TREEBRANCH_H_

/**
 * @brief This class represents a branch inside of a tree.
 *
 * For more information, see TreeBranch class.
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/tree_broker.hh"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class BranchDataType>
class  Tree;

template <class NodeDataType, class BranchDataType>
class  TreeLink;

template <class NodeDataType, class BranchDataType>
class  TreeNode;

// =============================================================================
//     DefaultBranchData
// =============================================================================

class DefaultBranchData
{
public:
    typedef double BranchLength;
    BranchLength branch_length;

    inline void FromTreeBrokerNode (TreeBrokerNode* node)
    {
        branch_length = node->branch_length;
    }

    inline void ToTreeBrokerNode (TreeBrokerNode* node)
    {
        node->branch_length = branch_length;
    }

    inline std::string Dump()
    {
        return "Length: " + std::to_string(branch_length);
    }
};

// =============================================================================
//     TreeBranch
// =============================================================================

template <class NodeDataType, class BranchDataType>
class TreeBranch
{
    friend class Tree<NodeDataType, BranchDataType>;

public:
    TreeBranch() : link_p_(nullptr), link_s_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the link of this branch that points towards the root.
     */
    inline TreeLink<NodeDataType, BranchDataType>* PrimaryLink()
    {
        return link_p_;
    }

    /**
     * @brief Returns the link of this branch that points away from the root.
     */
    inline TreeLink<NodeDataType, BranchDataType>* SecondaryLink()
    {
        return link_s_;
    }

    TreeNode<NodeDataType, BranchDataType>* PrimaryNode();
    TreeNode<NodeDataType, BranchDataType>* SecondaryNode();

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    void FromTreeBrokerNode (TreeBrokerNode* node);

    std::string Dump();

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    BranchDataType data;

protected:

    TreeLink<NodeDataType, BranchDataType>* link_p_;
    TreeLink<NodeDataType, BranchDataType>* link_s_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_branch.inc.cc"

#endif // include guard
