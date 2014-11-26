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

std::string TreeBroker::Dump()
{
    std::string out;
    out += "Tree contains " + std::to_string(NodeCount()) + " nodes (thereof "
        + std::to_string(LeafCount()) + " leaves):" + "\n";
    for (TreeBrokerNode* node : stack_) {
        for (int i = 0; i < node->depth; ++i) {
            out += "    ";
        }
        out += node->name
            + (node->branch_length != 0.0 ? ":" + std::to_string(node->branch_length) : "")
            + (!node->comment.empty() ? " [" + node->comment + "]" : "")
            + (!node->tag.empty() ? " {" + node->tag + "}" : "")
            + (node->rank > 0 ? " Rank(" + std::to_string(node->rank) + ")" : "")
            + (node->is_leaf ? " (Leaf)\n" : "\n");
    }
    return out;
}

void TreeBroker::AssignRanks()
{
    // iterate over all nodes, starting at the root, and assign ranks to them
    for (TreeBroker::const_iterator n_itr = stack_.cbegin(); n_itr != stack_.cend(); ++n_itr) {
        TreeBrokerNode* node = *n_itr;
        int num_children = 0;

        // go down the stack, starting at the current node, and end either when reaching the end
        // of the stack or when back at the same depth as the current node.
        // this means, we look at the subtree that starts at the current node. while doing so,
        // increase the child counter whenever we see a node that is exactly one level deeper than
        // the current node, because those are its immediate children.
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

} // namespace genesis
