/**
 * @brief Implementation of TreeBranch functions.
 *
 * For reasons of readability, in this implementation file the template data types
 * NodeDataType and BranchDataType are abbreviated NDT and BDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_broker.hh"

namespace genesis {

/**
 * @brief Fills the branch with data from a TreeBrokerNode.
 */
template <class NDT, class BDT>
void TreeBranch<NDT, BDT>::FromTreeBrokerNode (TreeBrokerNode* node)
{
    data.FromTreeBrokerNode(node);
}

/**
 * @brief Returns a one-line dump summary of the data of this branch.
 */
template <class NDT, class BDT>
std::string TreeBranch<NDT, BDT>::Dump()
{
    return data.Dump();
}

} // namespace genesis
