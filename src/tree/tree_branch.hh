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

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class  Tree;
struct TreeBrokerNode;
class  TreeLink;
class  TreeNode;

// =============================================================================
//     TreeBranch
// =============================================================================

class TreeBranch
{
    friend Tree;

public:
    typedef double BranchLength;

    TreeBranch() : length_(0.0), link_p_(nullptr), link_q_(nullptr) {}

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

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

    // ---------------------------------------------------------------------
    //     Mutators
    // ---------------------------------------------------------------------

    void FromTreeBrokerNode (TreeBrokerNode* node);

protected:

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

    BranchLength length_;

    TreeLink* link_p_;
    TreeLink* link_q_;

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
