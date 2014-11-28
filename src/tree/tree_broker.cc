/**
 * @brief Implementation of Tree Stack functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_broker.hh"

//~ #include <assert.h>
#include <string>

#include "utils/logging.hh"
//~ #include "utils/utils.hh"

namespace genesis {

/** @brief Destructor. Calls clear() to delete all nodes. */
TreeBroker::~TreeBroker()
{
    clear();
}

// -------------------------------------------------------------------------
//     Modifiers
// -------------------------------------------------------------------------

/** @brief Deletes all nodes from the broker. */
void TreeBroker::clear()
{
    for (TreeBrokerNode* node : stack_) {
        delete node;
    }
    stack_.clear();
}

// -------------------------------------------------------------------------
//     State Functions
// -------------------------------------------------------------------------

/** @brief Iterate over the tree and assign ranks (= number of immediate children) to all nodes.
 *
 * This function is for example needed to check whether it is a bifurcating/binary tree, or to check
 * how many leaves and inner nodes the tree has.
 */
void TreeBroker::AssignRanks()
{
    // we use a stack containing the parents of each subtree. whenever we enter a new subtree,
    // we push its parent to the stack and increase its rank count while encountering its immediate
    // children.
    std::deque<TreeBrokerNode*> parent_stack;

    // iterate over all nodes, starting at the root, and assign ranks to them
    for (TreeBroker::const_iterator n_itr = stack_.cbegin(); n_itr != stack_.cend(); ++n_itr) {
        // prepare the current node
        TreeBrokerNode* node = *n_itr;
        node->rank_ = 0;

        // check if the current node is in a different subtree than the current stack elements. this
        // is the case when its depths is smaller or equal to the stack elements. then, we have to
        // leave the subtree (possibly multiple levels, thus the loop) and remove those parents from
        // the stack
        while (!parent_stack.empty() && node->depth <= parent_stack.back()->depth) {
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
        parent_stack.push_back(node);
    }
}

/**
 * @brief Returns the number of leaf nodes in the tree. Before calling it, AssignRanks() has to be
 * called first.
 */
int TreeBroker::LeafCount() const
{
    int sum = 0;
    for (TreeBrokerNode* node : stack_) {
        if (node->rank_ == -1) {
            LOG_WARN << "TreeBroker::AssignRanks() was not called before.";
            return -1;
        }
        if (node->rank_ == 0) {
            ++sum;
        }
    }
    return sum;
}

/**
 * @brief Return a readable string representation of the elements of the TreeBroker.
 */
std::string TreeBroker::Dump() const
{
    std::string out;
    out += "Tree contains " + std::to_string(NodeCount()) + " nodes (thereof "
        + std::to_string(LeafCount()) + " leaves)" + (stack_.empty() ? "." : ":") + "\n";
    for (TreeBrokerNode* node : stack_) {
        for (int i = 0; i < node->depth; ++i) {
            out += "    ";
        }
        out += node->name
            + (node->branch_length != 0.0 ? ":" + std::to_string(node->branch_length) : "")
            + (!node->comment.empty() ? " [" + node->comment + "]" : "")
            + (!node->tag.empty() ? " {" + node->tag + "}" : "")
            + (node->rank_ > 0 ? " Rank(" + std::to_string(node->rank_) + ")" : "")
            + (node->is_leaf ? " (Leaf)\n" : "\n");
    }
    return out;
}

} // namespace genesis
