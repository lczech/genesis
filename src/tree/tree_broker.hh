#ifndef GNS_TREE_TREESTACK_H_
#define GNS_TREE_TREESTACK_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <deque>
#include <string>

namespace genesis {

// =============================================================================
//     TreeBrokerNode
// =============================================================================

/** @brief POD struct that stores the information for one tree node.
 *
 * All its members are public, as it is intended to serve an an intermediate data
 * exchange format, so different callers might need to modify its content.
 * However, this means paying attention when working with the data, as it can
 * be changed from anywhere.
 *
 * See TreeBroker class for a description of this intermediate format.
 */
struct TreeBrokerNode
{
public:
    /** @brief Constructor, initializes the item values. */
    TreeBrokerNode() : branch_length(0.0), depth(0), rank(0), is_leaf(false) {};

    /**
     * @brief Name of the node.
     *
     * In case it is a leaf, this is usually the name of the taxon represented by the node.
     * Internal nodes are named "Internal Node" in case no name is specified in the Newick format,
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

    /**
     * @brief An arbitrary string that can be attached to a node, e.g. in Newick format via "{}".
     */
    std::string tag;

    /**
     * @brief An arbitrary string that can be attached to a node, e.g. in Newick format via "[]".
     */
    std::string comment;
};

// =============================================================================
//     TreeBroker
// =============================================================================

    // TODO parsing now does not assign ranks to nodes (how many children they have).
    // TODO this might become imporant in the future, eg to check if it is a binary tree.
    // TODO add AssignRanks() (see PLL newick.c)
    // TODO add Validate() (see PLL newick.c)
    // TODO write copy ctor and assign op

class TreeBroker
{
public:
    ~TreeBroker();

    inline void pop_back()
    {
        stack_.pop_back();
    }

    inline void push_back (TreeBrokerNode* node)
    {
        stack_.push_back(node);
    }

    void clear();

    // TODO validate checks if leaf nodes are really leaves, not more than one level at a time nested,
    // also give funcs for is bifurcating/is binary.
    bool Validate();
    void Dump();

    inline int NodeCount()
    {
        return stack_.size();
    }

    int LeafCount();

protected:
    std::deque<TreeBrokerNode*> stack_;
};

} // namespace genesis

#endif // include guard
