#ifndef GNS_TREE_TREENODE_H_
#define GNS_TREE_TREENODE_H_

/**
 * @brief This class represents a node inside of a tree.
 *
 * For more information, see Node class.
 *
 * @file
 * @ingroup tree
 */

#include <string>

//~ #include "tree/tree_link.hh"

namespace genesis {

// forward declarations
class Tree;
class TreeLink;

class TreeNode
{
    friend Tree;

public:
    TreeNode() {}
    TreeNode(const std::string& name);

    std::string name() const
    {
        return name_;
    }

    /** Rank of the node, i.e. how many children it has. */
    int Rank();

    /** True if the node is a leaf/tip, false otherwise. */
    bool IsLeaf();

    /** True if the node is the root, false otherwise. */
    bool IsRoot();

    /** Depth of the node in the tree, i.e. its distance from root. */
    int Depth();

protected:
    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name_;

    TreeLink* link_;
};

} // namespace genesis

#endif // include guard
