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

class  Tree;
struct TreeBrokerNode;
class  TreeLink;

// =============================================================================
//     TreeNode
// =============================================================================

class TreeNode
{
    friend Tree;

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

protected:

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name_;

    TreeLink* link_;
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
    inline bool TreeNode::IsLeaf()
    {
        return link_->IsLeaf();
    }

    /**
     * @brief True iff the node is an inner node.
     */
    inline bool TreeNode::IsInner()
    {
        return link_->IsInner();
    }

} // namespace genesis

#endif // include guard
