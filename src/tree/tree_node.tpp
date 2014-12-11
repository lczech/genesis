/**
 * @brief Implementation of TreeNode functions.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and BranchDataType are abbreviated NDT and BDT, respectively.
 *
 * @file
 * @ingroup tree
 */

// =============================================================================
//     Inline definitions with dependecies
// =============================================================================

// The following are inline definitions that would create circular dependecies when included in the
// class definition. Thus, they need to be here, after the definition, so that their dependend
// source files can be included without circles.
// See http://www.cplusplus.com/forum/articles/10627/ for more information on this.

// Usually, this part would be at the end of the header file, but as this is a class template,
// where the implementation (this file) is included from within the header anyway, we can as well
// outsource the inline functions to here.

#include "tree/tree_link.hpp"

namespace genesis {

/**
 * @brief True iff the node is a leaf/tip.
 */
template <class NDT, class BDT>
inline bool TreeNode<NDT, BDT>::IsLeaf() const
{
    return link_->IsLeaf();
}

/**
 * @brief True iff the node is an inner node.
 */
template <class NDT, class BDT>
inline bool TreeNode<NDT, BDT>::IsInner() const
{
    return link_->IsInner();
}

} // namespace genesis

// =============================================================================
//     Member Functions
// =============================================================================

#include "tree/tree_broker.hpp"
#include "tree/tree_link.hpp"

namespace genesis {

/**
 * @brief Rank of the node, i.e. how many immediate children it has.
 */
template <class NDT, class BDT>
int TreeNode<NDT, BDT>::Rank() const
{
    int rank = -1;
    TreeLink<NDT, BDT>* link = link_;

    do {
        ++rank;
        link = link->Next();
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
std::string TreeNode<NDT, BDT>::Dump() const
{
    return "Rank: " + std::to_string(Rank()) + " \t " + data.Dump();
}

} // namespace genesis
