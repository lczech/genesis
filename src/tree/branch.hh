#ifndef GNS_TREE_BRANCH_H_
#define GNS_TREE_BRANCH_H_

/**
 * @brief This class represents a branch inside of a tree.
 *
 * For more information, see Branch class.
 *
 * @file
 * @ingroup tree
 */

//~ #include "tree/node.hh"
// forward declare node
class Node;

namespace genesis {

class Branch
{
public:
    signed int tag;

protected:
    double length_;

    Node* p_;
    Node* q_;

};

} // namespace genesis

#endif // include guard
