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
    // TODO add Validate() (see PLL newick.c), that also checks if the leaves are really leaves
    // TODO write copy ctor and assign op
    // TODO write IsBifurcatingTree() function (maybe use Rank() for this)
    // TODO write iterators
    // TODO rename pop and push?!

/**
 *
 * Every function modifiying the content of the broker is required to leave it in a valid state,
 * meaning:
 *
 *  *  The top node has to be the root of the tree, which is equivalent to having depth zero. This
 *     is also true for trees rooted on a leaf.
 *  *  The nesting of the nodes has to be correct, so the depth cannot increase more than one per
 *     node when going down the tree.
 */
class TreeBroker
{
public:
    ~TreeBroker();

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void clear();

    inline void PushTop (TreeBrokerNode* node)
    {
        stack_.push_front(node);
    }

    inline void PushBottom (TreeBrokerNode* node)
    {
        stack_.push_back(node);
    }

    inline void PopTop()
    {
        delete stack_.front();
        stack_.pop_front();
    }

    inline void PopBottom()
    {
        delete stack_.back();
        stack_.pop_back();
    }

    // -------------------------------------------------------------------------
    //     Accessors and Iterators
    // -------------------------------------------------------------------------

    /**
     * @brief Iterator type to access the stack elements.
     *
     * This iterator allows to use a loop like this:
     *
     *     TreeBroker b;
     *     for (TreeBroker::iterator n = b.begin(); n != b.end(); ++n) {
     *         std::cout << n->name << std::endl;
     *     }
     * %
     */
    typedef std::deque<TreeBrokerNode*>::iterator       iterator;

    /** @brief Const version of the iterator. */
    typedef std::deque<TreeBrokerNode*>::const_iterator const_iterator;

    /** @brief Reverse version of the iterator. */
    typedef std::deque<TreeBrokerNode*>::reverse_iterator reverse_iterator;

    /** @brief Const reverse version of the iterator. */
    typedef std::deque<TreeBrokerNode*>::const_reverse_iterator const_reverse_iterator;

    /**
     * @brief Returns an iterator to the top of the stack.
     *
     * This is used for the iterator and also allows to use range based
     * looping over the nodes:
     *
     *     TreeBroker b;
     *     for (TreeBrokerNode& n : b) {
     *         std::cout << n.name << std::endl;
     *     }
     * %
     */
    inline iterator begin()
    {
        return stack_.begin();
    }

    /** @brief Returns an iterator to the end of the token list. */
    inline iterator end()
    {
        return stack_.end();
    }

    /** @brief Const version of begin(). */
    inline const_iterator cbegin() const
    {
        return stack_.cbegin();
    }

    /** @brief Const version of end(). */
    inline const_iterator cend() const
    {
        return stack_.cend();
    }

    /**
     * @brief Returns a reverse iterator to the nodes on the stack.
     *
     * This is useful for iterating starting at the leaves and ending at the root.
     */
    inline reverse_iterator rbegin()
    {
        return stack_.rbegin();
    }

    /** @brief Reverse version of end(). */
    inline reverse_iterator rend()
    {
        return stack_.rend();
    }

    /** @brief Const version of rbegin(). */
    inline const_reverse_iterator crbegin()
    {
        return stack_.crbegin();
    }

    /** @brief Const version of rend(). */
    inline const_reverse_iterator crend()
    {
        return stack_.crend();
    }

    /** @brief Returns whether the stack is empty. */
    inline bool empty() const
    {
        return stack_.empty();
    }

    /** @brief Returns the size of the stack, i.e. the number of nodes stored in the broker. */
    inline size_t size() const
    {
        return stack_.size();
    }

    /**
     * @brief Returns a reference to the top node of the tree stack.
     *
     * Usually, the top element is the root of the tree (i.e., it has depth zero). Only when called
     * during the broker is being filled with nodes (for example, while parsing a Newick tree),
     * the top element is not the root.
     *
     * Calling this function on an empty() broker causes undefined behavior.
     */
    inline TreeBrokerNode* Top()
    {
        return stack_.front();
    }

    /**
     * @brief Returns a reference to the bottom node of the tree stack.
     *
     * Calling this function on an empty() broker causes undefined behavior.
     */
    inline TreeBrokerNode* Bottom()
    {
        return stack_.back();
    }

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    // TODO validate checks if leaf nodes are really leaves, not more than one level at a time nested,
    // also give funcs for is bifurcating/is binary.
    bool Validate();
    std::string Dump();

    inline int NodeCount() const
    {
        return stack_.size();
    }

    int LeafCount() const;

protected:
    std::deque<TreeBrokerNode*> stack_;
};

} // namespace genesis

#endif // include guard
