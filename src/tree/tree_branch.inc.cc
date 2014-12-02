/**
 * @brief Implementation of TreeBranch functions.
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

#include "tree/tree_link.hh"

namespace genesis {

template <class NDT, class BDT>
inline TreeNode<NDT, BDT>* TreeBranch<NDT, BDT>::PrimaryNode()
{
    return link_p_->node();
}

template <class NDT, class BDT>
inline TreeNode<NDT, BDT>* TreeBranch<NDT, BDT>::SecondaryNode()
{
    return link_s_->node();
}

} // namespace genesis

// =============================================================================
//     Member Functions
// =============================================================================

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
