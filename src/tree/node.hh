#ifndef GNS_TREE_NODE_H_
#define GNS_TREE_NODE_H_

/**
 * @brief This class represents a node inside of a tree.
 *
 * For more information, see Node class.
 *
 * @file
 * @ingroup tree
 */

 #include <string>
 #include <vector>

namespace genesis {

class Node
{
    public:
        Node();
        Node(std::string name);

        std::string GetName() {return name_;};

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
};

} // namespace genesis

#endif // include guard
