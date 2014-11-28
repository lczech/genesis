/**
 * @brief Implementation of TreeBranch functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_branch.hh"

#include "tree/tree_broker.hh"

namespace genesis {

/**
 * @brief Fills the branch with data from a TreeBrokerNode.
 */
void TreeBranch::FromTreeBrokerNode (TreeBrokerNode* node)
{
    length_ = node->branch_length;
}

} // namespace genesis
