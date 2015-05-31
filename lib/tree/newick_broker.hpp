#ifndef GENESIS_TREE_NEWICK_BROKER_H_
#define GENESIS_TREE_NEWICK_BROKER_H_

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
//     NewickBrokerElement
// =============================================================================

// forward declaration
class NewickBroker;

/** @brief POD struct that stores the information for one tree node.
 *
 * Most of its members are public, as it is intended to serve an an intermediate data
 * exchange format, so different callers might need to modify its content.
 * However, this means paying attention when working with the data, as it can
 * be changed from anywhere.
 *
 * See NewickBroker class for a description of this intermediate format.
 */
struct NewickBrokerElement
{
    friend NewickBroker;

public:
    /**
     * @brief Constructor, initializes the item values.
     */
    NewickBrokerElement() : branch_length(0.0), depth(0), is_leaf(false), rank_(-1) {};

    /**
     * @brief Returns the rank (number of immediate children) of this node.
     *
     * NewickBroker::assign_ranks() has to be called before using this function. Otherwise,
     * it will return -1.
     */
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

    /**
     * @brief Branch length associated with the node, i.e. the edge leading to its parent.
     */
    double      branch_length;

    /**
     * @brief Depth of the node in the tree, i.e. its distance from the root.
     */
    int         depth;

    /**
     * @brief True if the node is a leaf/tip, false otherwise.
     *
     * This value can be set for example while parsing a Newick tree, and is used by
     * NewickBroker::validate() as a check for correctness. However, it is (so far) not used further,
     * because it is not reliable (it can be changed arbitrarily without checking whether this is
     * conform with the tree topology).
     *
     * Therefore, use rank() to check whether a node is a leaf (in order to use rank, first
     * NewickBroker::assign_ranks() has to be called).
     */
    bool        is_leaf;

    /**
     * @brief Arbitrary strings that can be attached to a node, e.g. in Newick format via "{}".
     */
    std::deque<std::string> tags;

    /**
     * @brief Arbitrary strings that can be attached to a node, e.g. in Newick format via "[]".
     */
    std::deque<std::string> comments;

protected:
    /**
     * @brief Rank of the node, i.e. how many children it has.
     */
    int         rank_;
};

// =============================================================================
//     NewickBroker
// =============================================================================

    // TODO write copy ctor and assign op

    // TODO introduce other parsers and generators, e.g. http://en.wikipedia.org/wiki/DOT_%28graph_description_language%29

    // TODO update doc blocks to new use of Newick Broker instead of old Tree Broker!

/**
 * @brief Stores a tree in an intermediate format that can be used as transfer between different
 * representations.
 *
 * The NewickBroker offers a simple structure to represent a tree. It is used to transfer the
 * information contained in a tree (topology and data of the nodes and edges) between different
 * representations of the tree. Those can for example be data structures or string formats as
 * the Newick file format.
 *
 * It is organized as a stack, where the root of the tree is at the top/front. Then follow the nodes
 * in a postorder manner, where each node is of type NewickBrokerElement.
 *
 * The topology of the tree is represented via a depth attribute of each node: Two subsequent nodes
 * are siblings (belong to the same parent node), if they have the same depth. If the second node
 * has a depth that is one higher than the first ones, it is it's child (depth thus cannot increase
 * more than one between two nodes). Lastly, if the depth of the second node is smaller than the
 * first ones, it belongs to a different subtree.
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
 * Here, the rank represents the number of immediate children of this node. Leaf nodes have no
 * children and thus rank zero.
 *
 * Every function modifiying the content of the broker is required to leave it in a valid state,
 * meaning:
 *
 *  *  The top node has to be the root of the tree, which is equivalent to having depth zero. This
 *     is also true for trees rooted on a leaf.
 *  *  The nesting of the nodes has to be correct, so the depth cannot increase more than one per
 *     node when going down the tree.
 *  *  The attribute NewickBrokerElement::is_leaf of the NewickBrokerElements can be set (for exmaple when
 *     parsing a Newick tree) and then be used to validate the integrity of the tree using
 *     validate(). However, the attribute is not further used -- see its description for more on
 *     this.
 * %
 */
class NewickBroker
{
public:
    ~NewickBroker();

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void clear();

    inline void push_top (NewickBrokerElement* node)
    {
        stack_.push_front(node);
    }

    inline void push_bottom (NewickBrokerElement* node)
    {
        stack_.push_back(node);
    }

    inline void pop_top()
    {
        delete stack_.front();
        stack_.pop_front();
    }

    inline void pop_bottom()
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
     *     NewickBroker b;
     *     for (NewickBroker::iterator n = b.begin(); n != b.end(); ++n) {
     *         std::cout << n->name << std::endl;
     *     }
     * %
     */
    typedef std::deque<NewickBrokerElement*>::iterator               iterator;

    /** @brief Const version of the iterator. */
    typedef std::deque<NewickBrokerElement*>::const_iterator         const_iterator;

    /** @brief Reverse version of the iterator. */
    typedef std::deque<NewickBrokerElement*>::reverse_iterator       reverse_iterator;

    /** @brief Const reverse version of the iterator. */
    typedef std::deque<NewickBrokerElement*>::const_reverse_iterator const_reverse_iterator;

    /**
     * @brief Returns an iterator to the top of the stack.
     *
     * This is used for the iterator and also allows to use range based
     * looping over the nodes:
     *
     *     NewickBroker b;
     *     for (NewickBrokerElement& n : b) {
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
     * @brief Provides index based array access to the nodes.
     *
     * This also allows to iterate over them using:
     *
     *     NewickBroker tb;
     *     for (size_t i = 0; i < tb.size(); ++i) {
     *        NewickBrokerElement* tn = tb[i];
     *        std::cout << tn.name << std::endl;
     *     }
     *
     * Caveat: this operator does no boundary check. If you need this check,
     * use at() instead.
     */
    inline NewickBrokerElement* operator [] (const std::size_t index) const
    {
        return stack_[index];
    }

    /**
     * @brief Provides index based array access to the nodes, doing a boundary check first.
     *
     * In out of bounds cases, a `nullptr` is returned.
     */
    inline NewickBrokerElement* at(const std::size_t index) const
    {
        if (index < stack_.size()) {
            return stack_[index];
        } else {
            return nullptr;
        }
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
    inline NewickBrokerElement* top()
    {
        return stack_.front();
    }

    /**
     * @brief Returns a reference to the bottom node of the tree stack.
     *
     * Calling this function on an empty() broker causes undefined behavior.
     */
    inline NewickBrokerElement* bottom()
    {
        return stack_.back();
    }

    // -------------------------------------------------------------------------
    //     State Functions
    // -------------------------------------------------------------------------

    void assign_ranks();

    int leaf_count() const;

    inline int inner_count() const
    {
        return stack_.size() - leaf_count();
    }

    // same as size()
    inline int node_count() const
    {
        return stack_.size();
    }

    int max_rank() const;

    inline bool is_bifurcating() const
    {
        return max_rank() == 2;
    }

    bool validate() const;

    // -------------------------------------------------------------------------
    //     dump and Debug
    // -------------------------------------------------------------------------

    std::string dump() const;

protected:
    std::deque<NewickBrokerElement*> stack_;
};

} // namespace genesis

#endif // include guard
