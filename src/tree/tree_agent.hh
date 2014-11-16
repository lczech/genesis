#ifndef GNS_TREE_TREESTACK_H_
#define GNS_TREE_TREESTACK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <stack>

namespace genesis {

// =============================================================================
//     TreeAgentNode
// =============================================================================

/** @brief POD struct that stores the information for one tree node.
 *
 * All its members are public, as it is intended to serve an an intermediate data
 * exchange format, so different callers might need to modify its content.
 *
 * See TreeStack class for a description of this intermediate format.
 */
struct TreeAgentNode
{
public:
    /** @brief Constructor, initializes the item values. */
    TreeAgentNode() : branch_length(0.0), depth(0), rank(0), is_leaf(false) {};

    /**
     * @brief Name of the node.
     *
     * In case it is a leaf, this is usually the name of the taxon represented by the node.
     * Internal nodes are names "Internal Node" in case no name is specified in the Newick format,
     * same applies to the (possibly virtual) root, which is named "Root Node" by default.
     */
    std::string name;

    /** @brief Branch length associated with the node, i.e. the branch leading to its parent. */
    double      branch_length;

    /** @brief Depth of the node in the tree, i.e. its distance from the root. */
    int         depth;

    /** @brief Rank of the node, i.e. how many children it has. */
    int         rank;

    /** @brief True if the node is a leaf/tip, false otherwise. */
    bool        is_leaf;

    /** @brief An arbitrary string that can be attached to a node in Newick format via "{}". */
    std::string tag;

    /** @brief An arbitrary string that can be attached to a node in Newick format via "[]". */
    std::string comment;
};

// =============================================================================
//     TreeAgent
// =============================================================================

class TreeAgent
{
public:
    TreeAgent();
    ~TreeAgent();

    void clear();
    std::string Dump();

    void push_back (TreeStackNode* node);

    inline int NodeCount() {return stack_.size();};
    int LeafCount();

protected:
    std::stack<TreeStackNode*> stack_;
};

} // namespace genesis

#endif // include guard
