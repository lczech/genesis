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

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

struct TreeBrokerNode;

template <class NodeDataType, class BranchDataType>
class  Tree;

template <class NodeDataType, class BranchDataType>
class  TreeLink;

// =============================================================================
//     TreeNode
// =============================================================================

template <class NodeDataType, class BranchDataType>
class TreeNode
{
    friend Tree<NodeDataType, BranchDataType>;

public:
    TreeNode() : name_(""), link_(nullptr) {}
    TreeNode(const std::string& name) : name_(name), link_(nullptr) {};

    inline std::string name() const
    {
        return name_;
    }

    int  Rank();
    bool IsLeaf();
    bool IsInner();

    //~ /** True if the node is the root, false otherwise. */
    //~ bool IsRoot();

    //~ /** Depth of the node in the tree, i.e. its distance from root. */
    //~ int Depth();

    void FromTreeBrokerNode (TreeBrokerNode* node);

    std::string Dump();

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    NodeDataType data;

protected:

    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name_;

    TreeLink<NodeDataType, BranchDataType>* link_;
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

    /**
     * @brief True iff the node is a leaf/tip.
     */
    template <class NodeDataType, class BranchDataType>
    inline bool TreeNode<NodeDataType, BranchDataType>::IsLeaf()
    {
        return link_->IsLeaf();
    }

    /**
     * @brief True iff the node is an inner node.
     */
    template <class NodeDataType, class BranchDataType>
    inline bool TreeNode<NodeDataType, BranchDataType>::IsInner()
    {
        return link_->IsInner();
    }

} // namespace genesis

// include the template class implementation
#include "tree/tree_node.inc.cc"

#endif // include guard
