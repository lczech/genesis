/**
 * @brief Implementation of TreeNode functions.
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
namespace tree {

/**
 * @brief True iff the node is a leaf/tip.
 */
template <class NDT, class EDT>
inline bool TreeNode<NDT, EDT>::is_leaf() const
{
    return link_->is_leaf();
}

/**
 * @brief True iff the node is an inner node.
 */
template <class NDT, class EDT>
inline bool TreeNode<NDT, EDT>::is_inner() const
{
    return link_->is_inner();
}

} // namespace tree
} // namespace genesis

// =============================================================================
//     Member Functions
// =============================================================================

#include "tree/tree_link.hpp"

namespace genesis {
namespace tree {

/**
 * @brief Rank of the node, i.e. how many immediate children it has.
 */
template <class NDT, class EDT>
size_t TreeNode<NDT, EDT>::rank() const
{
    int rank = -1;
    TreeLink<NDT, EDT>* link = link_;

    do {
        ++rank;
        link = link->next();
    } while (link != link_);

    // We add at least 1 to the initial value of the rank, so this is valid.
    return static_cast<size_t>(rank);
}

/**
 * @brief Returns a one-line dump summary of the data of this node.
 *
 * TODO this method assumes that the tree node has a name. not good.
 */
template <class NDT, class EDT>
std::string TreeNode<NDT, EDT>::dump() const
{
    return "Rank: " + std::to_string(rank()) + " \t " + "name: " + this->data.name;
}

} // namespace tree
} // namespace genesis
