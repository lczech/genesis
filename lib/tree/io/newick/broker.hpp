#ifndef GENESIS_TREE_IO_NEWICK_BROKER_H_
#define GENESIS_TREE_IO_NEWICK_BROKER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <deque>
#include <stdexcept>
#include <string>
#include <vector>

#include "tree/io/newick/element.hpp"

namespace genesis {

// =================================================================================================
//     NewickBroker
// =================================================================================================

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

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    NewickBroker() = default;
    ~NewickBroker() = default;

    NewickBroker(NewickBroker const&) = default;
    NewickBroker(NewickBroker&&)      = default;

    NewickBroker& operator= (NewickBroker const&) = default;
    NewickBroker& operator= (NewickBroker&&)      = default;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void clear();

    void push_top (NewickBrokerElement const& node)
    {
        stack_.push_front(node);
    }

    void push_top (NewickBrokerElement&& node)
    {
        stack_.push_front(std::move(node));
    }

    void push_bottom (NewickBrokerElement const& node)
    {
        stack_.push_back(node);
    }

    void push_bottom (NewickBrokerElement&& node)
    {
        stack_.push_back(std::move(node));
    }

    void pop_top()
    {
        stack_.pop_front();
    }

    void pop_bottom()
    {
        stack_.pop_back();
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    /**
     * @brief Iterator type to access the stack elements.
     */
    typedef std::deque<NewickBrokerElement>::iterator               iterator;

    /**
     * @brief Const version of the iterator.
     */
    typedef std::deque<NewickBrokerElement>::const_iterator         const_iterator;

    /**
     * @brief Reverse version of the iterator.
     */
    typedef std::deque<NewickBrokerElement>::reverse_iterator       reverse_iterator;

    /**
     * @brief Const reverse version of the iterator.
     */
    typedef std::deque<NewickBrokerElement>::const_reverse_iterator const_reverse_iterator;

    /**
     * @brief Returns an iterator to the top of the stack.
     */
    iterator begin()
    {
        return stack_.begin();
    }

    const_iterator begin() const
    {
        return stack_.begin();
    }

    /**
     * @brief Returns an iterator to the end of the token list.
     */
    iterator end()
    {
        return stack_.end();
    }

    const_iterator end() const
    {
        return stack_.end();
    }

    /**
     * @brief Const version of begin().
     */
    const_iterator cbegin() const
    {
        return stack_.cbegin();
    }

    /**
     * @brief Const version of end().
     */
    const_iterator cend() const
    {
        return stack_.cend();
    }

    /**
     * @brief Returns a reverse iterator to the nodes on the stack.
     */
    reverse_iterator rbegin()
    {
        return stack_.rbegin();
    }

    const_reverse_iterator rbegin() const
    {
        return stack_.rbegin();
    }

    /**
     * @brief Reverse version of end().
     */
    reverse_iterator rend()
    {
        return stack_.rend();
    }

    const_reverse_iterator rend() const
    {
        return stack_.rend();
    }

    /**
     * @brief Const version of rbegin().
     */
    const_reverse_iterator crbegin()
    {
        return stack_.crbegin();
    }

    /**
     * @brief Const version of rend().
     */
    const_reverse_iterator crend()
    {
        return stack_.crend();
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Returns whether the stack is empty.
     */
    bool empty() const
    {
        return stack_.empty();
    }

    /**
     * @brief Returns the size of the stack, i.e. the number of nodes stored in the broker.
     */
    size_t size() const
    {
        return stack_.size();
    }

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

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
    NewickBrokerElement& operator [] (std::size_t index)
    {
        return stack_[index];
    }

    NewickBrokerElement const& operator [] (std::size_t index) const
    {
        return stack_[index];
    }

    /**
     * @brief Provides index based array access to the nodes, doing a boundary check first.
     *
     * In out of bounds cases, a `nullptr` is returned.
     */
    NewickBrokerElement& at(std::size_t index)
    {
        if (index >= stack_.size()) {
            throw std::out_of_range("__FUNCTION__: out_of_range");
        }
        return stack_[index];
    }

    NewickBrokerElement const& at(std::size_t index) const
    {
        if (index >= stack_.size()) {
            throw std::out_of_range("__FUNCTION__: out_of_range");
        }
        return stack_[index];
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
    NewickBrokerElement& top()
    {
        return stack_.front();
    }

    NewickBrokerElement const& top() const
    {
        return stack_.front();
    }

    /**
     * @brief Returns a reference to the bottom node of the tree stack.
     *
     * Calling this function on an empty() broker causes undefined behavior.
     */
    NewickBrokerElement& bottom()
    {
        return stack_.back();
    }

    NewickBrokerElement const& bottom() const
    {
        return stack_.back();
    }

    // -------------------------------------------------------------------------
    //     State Functions
    // -------------------------------------------------------------------------

    void assign_ranks() const;

    int leaf_count() const;

    int inner_count() const;

    int node_count() const;

    int max_rank() const;

    bool is_bifurcating() const;

    // -------------------------------------------------------------------------
    //     Dump and Debug
    // -------------------------------------------------------------------------

    bool validate() const;

    std::string dump() const;

private:

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    std::deque<NewickBrokerElement> stack_;
};

} // namespace genesis

#endif // include guard
