/**
 * @brief Implementation of Tree Stack functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_agent.hh"

//~ #include <assert.h>
//~ #include <string>

//~ #include "utils/logging.hh"
//~ #include "utils/utils.hh"

void TreeAgent::push_back (TreeStackNode* node)
{

}

/**
 * @brief Returns the number of leaf nodes in the tree.
 *
 * We need to count them anew every time, because the node objects are publicly
 * mutable, thus we can not use an internal counter.
 */
int TreeAgent::LeafCount()
{
    int sum = 0;
    for (TreeStackNode* node : stack_) {
        if (node->is_leaf) {
            ++sum;
        }
    }
    return sum;
}

void TreeAgent::clear()
{
    for (TreeAgentNode* item : items_) {
        delete item;
    }
    items_.clear();
    nodes_ = leaves_ = 0;
}
