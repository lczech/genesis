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
    label_  = node->comment;
}

/**
 * @brief Returns a one-line dump summary of the data of this branch.
 */
std::string TreeBranch::Dump()
{
    return "Length: " + std::to_string(length_) + " \t Label: '" + label_ + "'";
}

} // namespace genesis
