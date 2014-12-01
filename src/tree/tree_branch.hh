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

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

struct TreeBrokerNode;

template <class NodeDataType, class BranchDataType>
class  Tree;

template <class NodeDataType, class BranchDataType>
class  TreeLink;

template <class NodeDataType, class BranchDataType>
class  TreeNode;

// =============================================================================
//     TreeBranch
// =============================================================================

template <class NodeDataType, class BranchDataType>
class TreeBranch
{
    friend class Tree<NodeDataType, BranchDataType>;

public:
    typedef double BranchLength;

    TreeBranch() : length_(0.0), label_(""), link_p_(nullptr), link_q_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    inline BranchLength length()
    {
        return length_;
    }

    inline std::string label()
    {
        return label_;
    }

    inline TreeLink<NodeDataType, BranchDataType>* link_p()
    {
        return link_p_;
    }

    inline TreeLink<NodeDataType, BranchDataType>* link_q()
    {
        return link_q_;
    }

    TreeNode<NodeDataType, BranchDataType>* node_p();
    TreeNode<NodeDataType, BranchDataType>* node_q();

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

    BranchLength length_;
    std::string  label_;

    TreeLink<NodeDataType, BranchDataType>* link_p_;
    TreeLink<NodeDataType, BranchDataType>* link_q_;

};

} // namespace genesis

// =============================================================================
//     Inline definitions with dependecies
// =============================================================================

// The following are inline definitions that would create circular dependecies when included in the
// class definition. Thus, they need to be here, after the definition, so that their dependend
// source files can be included without circles.
// See http://www.cplusplus.com/forum/articles/10627/ for more information on this.

#include "tree/tree_link.hh"

namespace genesis {

    template <class NodeDataType, class BranchDataType>
    inline TreeNode<NodeDataType, BranchDataType>* TreeBranch<NodeDataType, BranchDataType>::node_p()
    {
        return link_p_->node();
    }

    template <class NodeDataType, class BranchDataType>
    inline TreeNode<NodeDataType, BranchDataType>* TreeBranch<NodeDataType, BranchDataType>::node_q()
    {
        return link_q_->node();
    }

} // namespace genesis

// include the template class implementation
#include "tree/tree_branch.inc.cc"

#endif // include guard
