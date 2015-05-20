/**
 * @brief Implementation of TreeSet class.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_set.hpp"
#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Tree Set Methods
// =============================================================================

/**
 * @brief
 */
template <class NDT, class EDT>
void TreeSet<NDT, EDT>::AddTree(const std::string& name, TreeType& tree)
{
    if (map_.count(name) > 0) {
        LOG_WARN << "TreeSet already contains a tree with name '" << name << "'.";
        return;
    }
    map_[name] = tree;
}

} // namespace genesis
