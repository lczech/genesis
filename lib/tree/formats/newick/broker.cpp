/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Implementation of Tree Stack functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/formats/newick/broker.hpp"

#include <algorithm>
#include <string>

#include "utils/core/logging.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Deletes all nodes from the broker.
 */
void NewickBroker::clear()
{
    stack_.clear();
}

void NewickBroker::push_top (NewickBrokerElement const& node)
{
    stack_.push_front(node);
}

void NewickBroker::push_top (NewickBrokerElement&& node)
{
    stack_.push_front(std::move(node));
}

void NewickBroker::push_bottom (NewickBrokerElement const& node)
{
    stack_.push_back(node);
}

void NewickBroker::push_bottom (NewickBrokerElement&& node)
{
    stack_.push_back(std::move(node));
}

void NewickBroker::pop_top()
{
    stack_.pop_front();
}

void NewickBroker::pop_bottom()
{
    stack_.pop_back();
}

// =================================================================================================
//     State Functions
// =================================================================================================

/**
 * @brief Iterate over the tree and assign ranks (= number of immediate children) to all nodes.
 *
 * This function is for example needed to check whether it is a bifurcating/binary tree, or to check
 * how many leaves and inner nodes the tree has. Thus, it is usually called after the broker is
 * filled with data.
 */
void NewickBroker::assign_ranks() const
{
    // we use a stack containing the parents of each subtree. whenever we enter a new subtree,
    // we push its parent to the stack and increase its rank count while encountering its immediate
    // children.
    std::deque<NewickBrokerElement const*> parent_stack;

    // iterate over all nodes, starting at the root, and assign ranks to them
    for (auto n_itr = stack_.cbegin(); n_itr != stack_.cend(); ++n_itr) {
        // prepare the current node
        n_itr->rank_ = 0;

        // check if the current node is in a different subtree than the current stack elements. this
        // is the case when its depths is smaller or equal to the stack elements. then, we have to
        // leave the subtree (possibly multiple levels, thus the loop) and remove those parents from
        // the stack
        while (!parent_stack.empty() && n_itr->depth <= parent_stack.back()->depth) {
            parent_stack.pop_back();
        }

        // now the top element of the stack points to the parent of the current node, so we can
        // increase its rank counter, because the current node is evidence that the parent has one
        // more child
        if (!parent_stack.empty()) {
            ++(parent_stack.back()->rank_);
        }

        // from now on, the current node is the beginning of the subtree for the now following
        // nodes, so push it to the stack
        parent_stack.push_back(&*n_itr);
    }
}

/**
 * @brief Returns the number of leaf nodes in the tree. assign_ranks() has to be called first.
 */
int NewickBroker::leaf_count() const
{
    int sum = 0;
    for (auto const& node : stack_) {
        if (node.rank_ == -1) {
            throw std::logic_error("NewickBroker::assign_ranks() was not called before.");
        }
        if (node.rank_ == 0) {
            ++sum;
        }
    }
    return sum;
}

int NewickBroker::inner_count() const
{
    return node_count() - leaf_count();
}

/**
 * @brief Alias for size().
 */
int NewickBroker::node_count() const
{
    return stack_.size();
}

/**
 * @brief Returns the highest rank of the nodes in the tree. assign_ranks() has to be called first.
 */
int NewickBroker::max_rank() const
{
    int max = -1;
    for (auto const& node : stack_) {
        if (node.rank_ == -1) {
            throw std::logic_error("NewickBroker::assign_ranks() was not called before.");
        }
        // if (node.rank_ == 1) {
        //     LOG_WARN << "Node with rank 1 found.";
        // }
        max = std::max(node.rank_, max);
    }
    return max;
}

bool NewickBroker::is_bifurcating() const
{
    return max_rank() == 2;
}

// =================================================================================================
//     Iterators
// =================================================================================================

/**
 * @brief Returns an iterator to the top of the stack.
 */
NewickBroker::iterator NewickBroker::begin()
{
    return stack_.begin();
}

/**
 * @brief Returns an iterator to the top of the stack for const objects.
 */
NewickBroker::const_iterator NewickBroker::begin() const
{
    return stack_.begin();
}

/**
 * @brief Returns an iterator to the end of the token list.
 */
NewickBroker::iterator NewickBroker::end()
{
    return stack_.end();
}

/**
 * @brief Returns an iterator to the end of the token list for const objects.
 */
NewickBroker::const_iterator NewickBroker::end() const
{
    return stack_.end();
}

/**
 * @brief Const version of begin().
 */
NewickBroker::const_iterator NewickBroker::cbegin() const
{
    return stack_.cbegin();
}

/**
 * @brief Const version of end().
 */
NewickBroker::const_iterator NewickBroker::cend() const
{
    return stack_.cend();
}

/**
 * @brief Returns a reverse iterator to the nodes on the stack.
 */
NewickBroker::reverse_iterator NewickBroker::rbegin()
{
    return stack_.rbegin();
}

/**
 * @brief Returns a reverse iterator to the nodes on the stack for const objects.
 */
NewickBroker::const_reverse_iterator NewickBroker::rbegin() const
{
    return stack_.rbegin();
}

/**
 * @brief Reverse version of end().
 */
NewickBroker::reverse_iterator NewickBroker::rend()
{
    return stack_.rend();
}

/**
 * @brief Reverse version of end() for const objects.
 */
NewickBroker::const_reverse_iterator NewickBroker::rend() const
{
    return stack_.rend();
}

/**
 * @brief Const version of rbegin().
 */
NewickBroker::const_reverse_iterator NewickBroker::crbegin()
{
    return stack_.crbegin();
}

/**
 * @brief Const version of rend().
 */
NewickBroker::const_reverse_iterator NewickBroker::crend()
{
    return stack_.crend();
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Returns whether the stack is empty.
 */
bool NewickBroker::empty() const
{
    return stack_.empty();
}

/**
 * @brief Returns the size of the stack, i.e. the number of nodes stored in the broker.
 */
size_t NewickBroker::size() const
{
    return stack_.size();
}

// =================================================================================================
//     Element Access
// =================================================================================================

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
NewickBrokerElement& NewickBroker::operator [] (std::size_t index)
{
    return stack_[index];
}

NewickBrokerElement const& NewickBroker::operator [] (std::size_t index) const
{
    return stack_[index];
}

/**
 * @brief Provides index based array access to the nodes, doing a boundary check first.
 *
 * In out of bounds cases, a `nullptr` is returned.
 */
NewickBrokerElement& NewickBroker::at(std::size_t index)
{
    if (index >= stack_.size()) {
        throw std::out_of_range("__FUNCTION__: out_of_range");
    }
    return stack_[index];
}

NewickBrokerElement const& NewickBroker::at(std::size_t index) const
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
NewickBrokerElement& NewickBroker::top()
{
    return stack_.front();
}

NewickBrokerElement const& NewickBroker::top() const
{
    return stack_.front();
}

/**
 * @brief Returns a reference to the bottom node of the tree stack.
 *
 * Calling this function on an empty() broker causes undefined behavior.
 */
NewickBrokerElement& NewickBroker::bottom()
{
    return stack_.back();
}

NewickBrokerElement const& NewickBroker::bottom() const
{
    return stack_.back();
}

// =================================================================================================
//     Dump and Debug
// =================================================================================================

// TODO see PLL newick.c for more stuff than can be validated in a tree!
/**
 * @brief Returns true iff the tree is valid. assign_ranks() has to be called first.
 *
 * A valid tree in a NewickBroker has to fullfill those criteria:
 *
 *  *  It's rank has to match the property is_leaf: Leaves have rank 0; a node with a higher rank
 *     cannot be a leaf.
 *  *  Furthermore, rank 1 is not valid, as this represents a node that is not furcating in any way.
 *  *  The depth (nesting level) of the nodes cannot increase more than one level between nodes,
 *     as this would imply a non-existing node with a depth in between. However, it can arbitrarily
 *     decrease, as this simply means the end of a subtree.
 * %
 */
bool NewickBroker::validate() const
{
    int cur_depth = 0;
    for (auto const& node : stack_) {
        if (node.rank_ == -1) {
            LOG_INFO << "NewickBroker::assign_ranks() was not called before.";
            return false;
        }
        if (node.rank_ == 0 && !(node.is_leaf)) {
            LOG_INFO << "Leaf node found with rank == 0, but is_leaf == false.";
            return false;
        }
        if (node.rank_ == 1) {
            LOG_INFO << "Node with rank 1 found.";
            return false;
        }
        if (node.rank_ > 1 && node.is_leaf) {
            LOG_INFO << "Inner node found with rank > 1, but is_leaf == true.";
            return false;
        }
        if (node.depth > cur_depth + 1) {
            LOG_INFO << "Node found that increases depth more than 1 compared to parent.";
            return false;
        }
        cur_depth = node.depth;
    }
    return true;
}

/**
 * @brief Return a readable string representation of the elements of the NewickBroker.
 */
std::string NewickBroker::dump() const
{
    std::string out;
    out += "Tree contains " + std::to_string(node_count()) + " nodes (thereof "
        + std::to_string(leaf_count()) + " leaves)" + (stack_.empty() ? "." : ":") + "\n";
    for (auto const& node : stack_) {
        // indent
        for (int i = 0; i < node.depth; ++i) {
            out += "    ";
        }

        // basic information
        out += node.name;

        // values
        for (std::string const& v : node.values) {
            out += " :" + v;
        }

        // comments
        for (std::string const& c : node.comments) {
            out += " [" + c + "]";
        }

        // tags
        for (std::string const& t : node.tags) {
            out += " {" + t + "}";
        }

        // additional information
        out += (node.rank_ > 0 ? " Rank(" + std::to_string(node.rank_) + ")" : "")
            +  (node.is_leaf ? " (Leaf)\n" : "\n");
    }
    return out;
}

} // namespace tree
} // namespace genesis
