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

    void push_top (NewickBrokerElement const& node);
    void push_top (NewickBrokerElement&& node);

    void push_bottom (NewickBrokerElement const& node);
    void push_bottom (NewickBrokerElement&& node);

    void pop_top();
    void pop_bottom();

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    typedef std::deque<NewickBrokerElement>::iterator               iterator;
    typedef std::deque<NewickBrokerElement>::const_iterator         const_iterator;
    typedef std::deque<NewickBrokerElement>::reverse_iterator       reverse_iterator;
    typedef std::deque<NewickBrokerElement>::const_reverse_iterator const_reverse_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin();
    const_reverse_iterator crend();

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    bool   empty() const;
    size_t size()  const;

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    NewickBrokerElement      & operator [] (std::size_t index);
    NewickBrokerElement const& operator [] (std::size_t index) const;

    NewickBrokerElement      & at(std::size_t index);
    NewickBrokerElement const& at(std::size_t index) const;

    NewickBrokerElement      & top();
    NewickBrokerElement const& top() const;

    NewickBrokerElement      & bottom();
    NewickBrokerElement const& bottom() const;

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

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::deque<NewickBrokerElement> stack_;
};

} // namespace genesis

#endif // include guard
