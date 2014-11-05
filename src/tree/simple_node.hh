#ifndef GNS_TREE_SIMPLENODE_H_
#define GNS_TREE_SIMPLENODE_H_

/**
 * @brief This class represents a node inside of a tree.
 *
 * For more information, see SimpleNode class.
 *
 * @file
 * @ingroup tree
 */

 #include <string>
 #include <vector>

 #include "node.hh"

namespace genesis {

class SimpleNode : public Node
{
    public:
        SimpleNode(std::string name) : name_(name) {};

    protected:
        /**
         * Name of the node. In case its a leaf, this is usually the name of the
         * taxon represented by the node.
         */
        std::string name_;

        /** Pointer to parent node. */
        Node*       parent;

        std::vector<Node*> children;

        /** Branch length of the branch leading to the nodes parent. */
        double      branch_length;

        /** True if the node is a leaf/tip, false otherwise. */
        bool        leaf;

        /** Rank of the node, i.e. how many children it has. */
        int         rank;

        /** Depth of the node in the tree, i.e. its distance from root. */
        int         depth;
};

} // namespace genesis

#endif // include guard
