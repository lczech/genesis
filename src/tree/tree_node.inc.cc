/**
 * @brief Implementation of TreeNode functions.
 *
 * For reasons of readability, in this implementation file the template data types
 * NodeDataType and BranchDataType are abbreviated NDT and BDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_broker.hh"
#include "tree/tree_link.hh"

namespace genesis {

/**
 * @brief Rank of the node, i.e. how many immediate children it has.
 */
template <class NDT, class BDT>
int TreeNode<NDT, BDT>::Rank()
{
    int rank = -1;
    TreeLink<NDT, BDT>* link = link_;

    do {
        ++rank;
        link = link->next();
    } while (link != link_);

    return rank;
}

/**
 * @brief Fills the node with data from a TreeBrokerNode.
 */
template <class NDT, class BDT>
void TreeNode<NDT, BDT>::FromTreeBrokerNode (TreeBrokerNode* node)
{
    data.FromTreeBrokerNode(node);
}

/**
 * @brief Returns a one-line dump summary of the data of this node.
 */
template <class NDT, class BDT>
std::string TreeNode<NDT, BDT>::Dump()
{
    return data.Dump() + " \t Rank: " + std::to_string(Rank());
}

} // namespace genesis
