#ifndef GENESIS_TREE_FORMATS_NEWICK_BROKER_H_
#define GENESIS_TREE_FORMATS_NEWICK_BROKER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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

#include "genesis/tree/formats/newick/element.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     NewickBroker
// =================================================================================================

/**
 * @brief Stores a Newick tree in an intermediate format that can be further processed into a Tree.
 *
 * The NewickBroker is a transitional format between the Newick formatted tree and a Tree object.
 * It is neccessary to have this intermediate step, because the ordering of nodes in the Newick
 * format differs from our requirements. Newick trees starts with leaf nodes, while our internal
 * representation is build up from the root.
 *
 * The NewickBroker is organized as a stack, where the root of the tree is at the top/front. Then
 * follow the nodes in a preorder manner, where each node is of type NewickBrokerElement.
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
 *  *  The attribute NewickBrokerElement::is_leaf of the NewickBrokerElements can be set (for
 *     exmaple when parsing a Newick tree) and then be used to validate the integrity of the tree
 *     using validate(). However, the attribute is not further used -- see its description for more
 *     on this.
 * %
 */
class NewickBroker
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::deque<NewickBrokerElement>::iterator               iterator;
    typedef std::deque<NewickBrokerElement>::const_iterator         const_iterator;
    typedef std::deque<NewickBrokerElement>::reverse_iterator       reverse_iterator;
    typedef std::deque<NewickBrokerElement>::const_reverse_iterator const_reverse_iterator;

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

    /**
     * @brief Deletes all nodes from the broker.
     */
    void clear();

    void push_top (NewickBrokerElement const& node);
    void push_top (NewickBrokerElement&& node);

    void push_bottom (NewickBrokerElement const& node);
    void push_bottom (NewickBrokerElement&& node);

    void pop_top();
    void pop_bottom();

    // -------------------------------------------------------------------------
    //     State Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Iterate over the tree and assign ranks (= number of immediate children) to all nodes.
     *
     * This function is for example needed to check whether it is a bifurcating/binary tree, or to check
     * how many leaves and inner nodes the tree has. Thus, it is usually called after the broker is
     * filled with data.
     */
    void assign_ranks() const;

    /**
     * @brief Returns the number of leaf nodes in the tree. assign_ranks() has to be called first.
     */
    int leaf_count() const;

    int inner_count() const;

    /**
     * @brief Alias for size().
     */
    int node_count() const;

    /**
     * @brief Returns the highest rank of the nodes in the tree. assign_ranks() has to be called first.
     */
    int max_rank() const;

    bool is_bifurcating() const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Returns whether the stack is empty.
     */
    bool   empty() const;

    /**
     * @brief Returns the size of the stack, i.e. the number of nodes stored in the broker.
     */
    size_t size()  const;

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
    NewickBrokerElement      & operator [] (std::size_t index);
    NewickBrokerElement const& operator [] (std::size_t index) const;

    /**
     * @brief Provides index based array access to the nodes, doing a boundary check first.
     *
     * In out of bounds cases, an exception is thrown.
     */
    NewickBrokerElement      & at(std::size_t index);
    NewickBrokerElement const& at(std::size_t index) const;

    /**
     * @brief Returns a reference to the top node of the tree stack.
     *
     * Usually, the top element is the root of the tree (i.e., it has depth zero). Only when called
     * during the broker is being filled with nodes (for example, while parsing a Newick tree),
     * the top element is not the root.
     *
     * Calling this function on an empty() broker causes undefined behavior.
     */
    NewickBrokerElement      & top();
    NewickBrokerElement const& top() const;

    /**
     * @brief Returns a reference to the bottom node of the tree stack.
     *
     * Calling this function on an empty() broker causes undefined behavior.
     */
    NewickBrokerElement      & bottom();
    NewickBrokerElement const& bottom() const;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    /**
     * @brief Returns an iterator to the top of the stack.
     */
    iterator begin();

    /**
     * @brief Returns an iterator to the top of the stack for const objects.
     */
    const_iterator begin() const;

    /**
     * @brief Returns an iterator to the end of the token list.
     */
    iterator end();

    /**
     * @brief Returns an iterator to the end of the token list for const objects.
     */
    const_iterator end() const;

    /**
     * @brief Const version of begin().
     */
    const_iterator cbegin() const;

    /**
     * @brief Const version of end().
     */
    const_iterator cend() const;

    /**
     * @brief Returns a reverse iterator to the nodes on the stack.
     */
    reverse_iterator rbegin();

    /**
     * @brief Returns a reverse iterator to the nodes on the stack for const objects.
     */
    const_reverse_iterator rbegin() const;

    /**
     * @brief Reverse version of end().
     */
    reverse_iterator rend();

    /**
     * @brief Reverse version of end() for const objects.
     */
    const_reverse_iterator rend() const;

    /**
     * @brief Const version of rbegin().
     */
    const_reverse_iterator crbegin();

    /**
     * @brief Const version of rend().
     */
    const_reverse_iterator crend();

    // -------------------------------------------------------------------------
    //     Dump and Debug
    // -------------------------------------------------------------------------

    /**
     * @brief Returns true iff the tree is valid. assign_ranks() has to be called first.
     *
     * A valid tree in a NewickBroker has to fullfill those criteria:
     *
     *  *  The depth (nesting level) of the nodes cannot increase more than one level between nodes,
     *     as this would imply a non-existing node with a depth in between. However, it can arbitrarily
     *     decrease, as this simply means the end of a subtree.
     *  *  Furthermore, rank 1 is not valid, as this represents a node that is not furcating in any way.
     * %
     */
    bool validate() const;

    /**
     * @brief Return a readable string representation of the elements of the NewickBroker.
     */
    std::string dump() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::deque<NewickBrokerElement> stack_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
