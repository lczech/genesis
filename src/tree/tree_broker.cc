/**
 * @brief Implementation of Tree Stack functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_broker.hh"

#include <algorithm>
#include <string>

#include "utils/logging.hh"

namespace genesis {

/** @brief Destructor. Calls clear() to delete all nodes. */
TreeBroker::~TreeBroker()
{
    clear();
}

// -----------------------------------------------------------------------------
//     Modifiers
// -----------------------------------------------------------------------------

/** @brief Deletes all nodes from the broker. */
void TreeBroker::clear()
{
    for (TreeBrokerNode* node : stack_) {
        delete node;
    }
    stack_.clear();
}

// -----------------------------------------------------------------------------
//     State Functions
// -----------------------------------------------------------------------------

/**
 * @brief Iterate over the tree and assign ranks (= number of immediate children) to all nodes.
 *
 * This function is for example needed to check whether it is a bifurcating/binary tree, or to check
 * how many leaves and inner nodes the tree has. Thus, it is usually called after the broker is
 * filled with data.
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
 * @brief Returns the number of leaf nodes in the tree. AssignRanks() has to be called first.
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
 * @brief Returns the highest rank of the nodes in the tree. AssignRanks() has to be called first.
 */
int TreeBroker::MaxRank()
{
    int max = -1;
    for (TreeBrokerNode* node : stack_) {
        if (node->rank_ == -1) {
            LOG_WARN << "TreeBroker::AssignRanks() was not called before.";
            return -1;
        }
        if (node->rank_ == 1) {
            LOG_WARN << "Node with rank 1 found. This is a node without furcation.";
        }
        max = std::max(node->rank_, max);
    }
    return max;
}

// TODO see PLL newick.c for more stuff than can be validated in a tree!
/**
 * @brief Returns true iff the tree is valid. AssignRanks() has to be called first.
 *
 * A valid tree in a TreeBroker has to fullfill those criteria:
 *
 *  *  It's rank has to match the property is_leaf: Leaves have rank 0; a node with a higher rank
 *     cannot be a leaf.
 *  *  Furthermore, rank 1 is not valid, as this represents a node that is not furcating in any way.
 *  *  The depth (nesting level) of the nodes cannot increase more than one level between nodes,
 *     as this would imply a non-existing node with a depth in between. However, it can arbitrarily
 *     decrease, as this simply means the end of a subtree.
 * %
 */
bool TreeBroker::Validate()
{
    int cur_depth = 0;
    for (TreeBrokerNode* node : stack_) {
        if (node->rank_ == -1) {
            LOG_WARN << "TreeBroker::AssignRanks() was not called before.";
            return false;
        }
        if (node->rank_ == 0 && !(node->is_leaf)) {
            LOG_WARN << "Leaf node found with rank == 0, but is_leaf == false.";
            return false;
        }
        if (node->rank_ == 1) {
            LOG_WARN << "Node with rank 1 found. This is a node without furcation.";
            return false;
        }
        if (node->rank_ > 1 && node->is_leaf) {
            LOG_WARN << "Inner node found with rank > 1, but is_leaf == true.";
            return false;
        }
        if (node->depth > cur_depth + 1) {
            LOG_WARN << "Node found that increases depth more than 1 compared to parent.";
            return false;
        }
        cur_depth = node->depth;
    }
    return true;
}

// -----------------------------------------------------------------------------
//     Dump and Debug
// -----------------------------------------------------------------------------

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
