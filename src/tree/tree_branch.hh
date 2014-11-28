#ifndef GNS_TREE_TREEBRANCH_H_
#define GNS_TREE_TREEBRANCH_H_

/**
 * @brief This class represents a branch inside of a tree.
 *
 * For more information, see Branch class.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// forward declarations
class Tree;
class TreeLink;
class TreeNode;

class TreeBranch
{
    friend Tree;

public:
    typedef double BranchLength;

    TreeBranch() : length_(0.0), link_p_(nullptr), link_q_(nullptr) {}

    inline TreeLink* link_p()
    {
        return link_p_;
    }

    inline TreeLink* link_q()
    {
        return link_q_;
    }

    TreeNode* node_p();
    TreeNode* node_q();

protected:
    BranchLength length_;

    TreeLink* link_p_;
    TreeLink* link_q_;

};

} // namespace genesis

#include "tree/tree_link.hh"

namespace genesis {

    inline TreeNode* TreeBranch::node_p()
    {
        return link_p_->node();
    }

    inline TreeNode* TreeBranch::node_q()
    {
        return link_q_->node();
    }

} // namespace genesis

#endif // include guard
