#ifndef GNS_TREE_TREE_H_
#define GNS_TREE_TREE_H_

/**
 * @brief Provides the basic interface for phylogenetic trees.
 *
 * For more information, see Tree class.
 *
 * @file
 * @ingroup tree
 */

 #include <string>

namespace genesis {

typedef struct TreeNode {
    /**
     * Name of the node. In case its a leaf, this is usually the name of the
     * taxon represented by the node.
     */
    std::string name;

    /** Pointer to parent node. */
    TreeNode*   parent;

    /** Branch length of the branch leading to the nodes parent. */
    double      branch_length;

    /** True if the node is a leaf/tip, false otherwise. */
    bool        leaf;

    /** Rank of the node, i.e. how many children it has. */
    int         rank;

    /** Depth of the node in the tree, i.e. its distance from root. */
    int         depth;
} TreeNode;


class Tree
{
    public:
        Tree();
        ~Tree();

        void ReadNewickFile   (std::string fn);
        void ReadNewickString (std::string tree);

        void WriteNewickFile (std::string fn);
        std::string WriteNewickString ();
};

} // namespace genesis

#endif // include guard
