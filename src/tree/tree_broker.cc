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

void TreeBroker::clear()
{
    for (TreeBrokerNode* node : stack_) {
        delete node;
    }
    stack_.clear();
}

void TreeBroker::Dump()
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
            + (node->is_leaf ? " (Leaf)\n" : "\n");
    }
    LOG_INFO << out;
}

/**
 * @brief Returns the number of leaf nodes in the tree.
 *
 * We need to count them anew every time, because the node objects are publicly
 * mutable, thus we can not use an internal counter.
 */
int TreeBroker::LeafCount()
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
