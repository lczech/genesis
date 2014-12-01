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

    inline TreeLink<NodeDataType, BranchDataType>* PrimaryLink()
    {
        return link_p_;
    }

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
//     Inline definitions with dependecies
// =============================================================================

// The following are inline definitions that would create circular dependecies when included in the
// class definition. Thus, they need to be here, after the definition, so that their dependend
// source files can be included without circles.
// See http://www.cplusplus.com/forum/articles/10627/ for more information on this.

// We abbriviate the template types here, in the same way it is done in the implementation file.

#include "tree/tree_link.hh"

namespace genesis {

    template <class NDT, class BDT>
    inline TreeNode<NDT, BDT>* TreeBranch<NDT, BDT>::PrimaryNode()
    {
        return link_p_->node();
    }

    template <class NDT, class BDT>
    inline TreeNode<NDT, BDT>* TreeBranch<NDT, BDT>::SecondaryNode()
    {
        return link_s_->node();
    }

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_branch.inc.cc"

#endif // include guard
