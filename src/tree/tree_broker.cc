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

TreeBroker::~TreeBroker()
{
    clear();
}

// -------------------------------------------------------------------------
//     Modifiers
// -------------------------------------------------------------------------

void TreeBroker::clear()
{
    for (TreeBrokerNode* node : stack_) {
        delete node;
    }
    stack_.clear();
}

// -------------------------------------------------------------------------
//     Member Functions
// -------------------------------------------------------------------------

/** @brief Iterate over the tree and assign ranks (= number of immediate children) to all nodes.
 *
 * This function is useful to find out how many immediate children each node has, thus it is needed
 * for example to check whether it is a bifurcating/binary tree.
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

        // now the top element of the stack points to the parent of the current nodem, so we can
        // increase its rank counter, because the current node is evidence that the parent has one
        // more child
        if (!parent_stack.empty()) {
            ++(parent_stack.back()->rank_);
        }

        // from now on, the current node is the beginning of the subtree for the now following
        // nodes, so push it to the stack
        parent_stack.push_back(node);
    }

    // version without stack but instead a nested loop. probably slower.
    /*
    // iterate over all nodes, starting at the root, and assign ranks to them
    for (TreeBroker::const_iterator n_itr = stack_.cbegin(); n_itr != stack_.cend(); ++n_itr) {
        TreeBrokerNode* node = *n_itr;
        int num_children = 0;

        // go down the stack, starting at the successor of the current node, and stop either when
        // reaching the end of the stack or when back at the same depth as the current node.
        // this means, we look at the whole subtree that starts at the current node.
        // while doing so, increase the child counter whenever we see a node that is exactly one
        // level deeper than the current node, because those are its immediate children.
        for (
            TreeBroker::const_iterator c_itr = n_itr + 1;
            c_itr != stack_.end() && (*c_itr)->depth > node->depth;
            ++c_itr
        ) {
            if ((*c_itr)->depth == node->depth + 1) {
                ++num_children;
            }
        }
        node->rank = num_children;
    }
    */
}

/**
 * @brief Returns the number of leaf nodes in the tree.
 *
 * We need to count them anew every time, because the node objects are publicly
 * mutable, thus we can not use an internal counter.
 */
int TreeBroker::LeafCount() const
{
    int sum = 0;
    for (TreeBrokerNode* node : stack_) {
        if (node->is_leaf) {
            ++sum;
        }
    }
    return sum;
}

/** @brief Return a readable string representation of the elements of the TreeBroker. */
std::string TreeBroker::Dump()
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
