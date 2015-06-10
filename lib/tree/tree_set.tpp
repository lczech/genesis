/**
 * @brief Implementation of TreeSet class.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <algorithm>

#include "tree/tree_set.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Modifiers
// =============================================================================

/**
 * @brief
 */
template <class TreeType>
void TreeSet<TreeType>::add (const std::string& name, TreeType* tree)
{
    trees_.push_back( { name, std::unique_ptr<TreeType>(tree) } );
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief
 */
template <class TreeType>
TreeType* TreeSet<TreeType>::get_first(const std::string& name)
{
    auto ct = trees_.begin();
    while (ct != trees_.end()) {
        if (ct->name == name) {
            return ct->tree->get();
        }
        ++ct;
    }
    return nullptr;
}

// =============================================================================
//     Dump & Debug
// =============================================================================

/**
 * @brief
 */
template <class TreeType>
std::string TreeSet<TreeType>::dump(bool full)
{
    std::string res = "";
    for (auto ct : trees_) {
        res += ct->name + "\n";
        if (full) {
            res += ct->tree->dump() + "\n";
        }
    }
    return res;
}

} // namespace genesis
