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

// forward declarations
class Tree;
class TreeLink;

class TreeBranch
{
    friend Tree;

public:
    typedef double BranchLength;

    TreeBranch() : length_(0.0), link_p_(nullptr), link_q_(nullptr) {}

protected:
    BranchLength length_;

    TreeLink* link_p_;
    TreeLink* link_q_;

};

} // namespace genesis

#endif // include guard
