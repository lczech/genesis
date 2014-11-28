/**
 * @brief Implementation of TreeNode functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_node.hh"

#include "tree/tree_broker.hh"
#include "tree/tree_link.hh"

namespace genesis {

/**
 * @brief Rank of the node, i.e. how many immediate children it has.
 */
int TreeNode::Rank()
{
    int rank = -1;
    TreeLink* link = link_;

    do {
        ++rank;
        link = link->next();
    } while (link != link_);

    return rank;
}

/**
 * @brief Fills the node with data from a TreeBrokerNode.
 */
void TreeNode::FromTreeBrokerNode (TreeBrokerNode* node)
{
    name_ = node->name;
}

/**
 * @brief Returns a one-line dump summary of the data of this node.
 */
std::string TreeNode::Dump()
{
    return "Name: '" + name_ + "' \t Rank: " + std::to_string(Rank());
}

} // namespace genesis
