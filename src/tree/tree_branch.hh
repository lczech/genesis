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

//~ #include "tree/tree_node.hh"

namespace genesis {

// forward declare node
class TreeNode;

class TreeBranch
{
public:
    typedef double BranchLength;

    TreeBranch();

protected:
    BranchLength length_;

    TreeNode* node_p_;
    TreeNode* node_q_;

};

} // namespace genesis

#endif // include guard
