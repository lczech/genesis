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

// forward declaration
class TreeBroker;

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
    friend TreeBroker;

public:
    /** @brief Constructor, initializes the item values. */
    TreeBrokerNode() : branch_length(0.0), depth(0), is_leaf(false), rank_(0) {};

    inline int rank()
    {
        return rank_;
    }

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

protected:
    /** @brief Rank of the node, i.e. how many children it has. */
    int         rank_;
};

// =============================================================================
//     TreeBroker
// =============================================================================

    // TODO parsing now does not assign ranks to nodes (how many children they have).
    // TODO this might become imporant in the future, eg to check if it is a binary tree.
    // TODO add AssignRanks() (see PLL newick.c)

    // TODO introduce IsDirty() that returns true iff the stack was modified (pop/push/clear/...)
    // TODO so that the internal state of the tree and its nodes is changed, e.g. AssignRanks()
    // TODO has to be called again (and all other state-assigning functions)

    // TODO add Validate() (see PLL newick.c), that also checks if the leaves are really leaves
    // TODO write copy ctor and assign op
    // TODO write IsBifurcatingTree() function (maybe use Rank() for this)

/**
 * @brief Stores a tree in an intermediate format that can be used as transfer between different
 * representations.
 *
 * The TreeBroker offers a simple structure to represent a tree. It is used to transfer the
 * information contained in a tree (topology and data of the nodes) between different
 * representations of the tree. Those can for example be data structures or string formats as
 * the Newick.
 *
 * It is organized as a stack, where the root of the tree is at the top/front.
 *
 * Postorder... TODO
 *
 *
 * For example, the following tree in Newick format:
 *
 *     ((A,((B,C,D)E,F)G)H,((I,J,K)L,M,N)O,P,Q)R;
 *
 * would be stored as the following stack:
 *
 *     R Rank(4)
 *         Q (Leaf)
 *         P (Leaf)
 *         O Rank(3)
 *             N (Leaf)
 *             M (Leaf)
 *             L Rank(3)
 *                 K (Leaf)
 *                 J (Leaf)
 *                 I (Leaf)
 *         H Rank(2)
 *             G Rank(2)
 *                 F (Leaf)
 *                 E Rank(3)
 *                     D (Leaf)
 *                     C (Leaf)
 *                     B (Leaf)
 *             A (Leaf)
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
        //~ delete stack_.front();
        stack_.pop_front();
    }

    inline void PopBottom()
    {
        //~ delete stack_.back();
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
    typedef std::deque<TreeBrokerNode*>::iterator               iterator;

    /** @brief Const version of the iterator. */
    typedef std::deque<TreeBrokerNode*>::const_iterator         const_iterator;

    /** @brief Reverse version of the iterator. */
    typedef std::deque<TreeBrokerNode*>::reverse_iterator       reverse_iterator;

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

    void AssignRanks();

    int LeafCount() const;

    inline int InnerCount() const
    {
        return stack_.size() - LeafCount();
    }

    inline int NodeCount() const
    {
        return stack_.size();
    }

    std::string Dump();

protected:
    std::deque<TreeBrokerNode*> stack_;
};

} // namespace genesis

#endif // include guard
