/**
 * @brief Implementation of TreeEdge functions.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated NDT and EDT, respectively.
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
 * @brief Returns the node of this edge that points towards the root.
 */
template <class NDT, class EDT>
inline TreeNode<NDT, EDT>* TreeEdge<NDT, EDT>::primary_node() const
{
    return link_p_->node();
}

/**
 * @brief Returns the node of this edge that points away from the root.
 */
template <class NDT, class EDT>
inline TreeNode<NDT, EDT>* TreeEdge<NDT, EDT>::secondary_node() const
{
    return link_s_->node();
}

} // namespace genesis

// =============================================================================
//     Member Functions
// =============================================================================

namespace genesis {

/**
 * @brief Returns a one-line dump summary of the data of this edge.
 *
 * TODO this method assumes that the tree node has a name. not good.
 */
template <class NDT, class EDT>
std::string TreeEdge<NDT, EDT>::dump() const
{
    return "Node P: " + primary_node()->data.name + ", Node S: " + secondary_node()->data.name + "\t" + data.dump();
}

} // namespace genesis
