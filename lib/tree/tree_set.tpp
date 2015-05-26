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
template <class NDT, class EDT>
void TreeSet<NDT, EDT>::Add (const std::string& name, TreeType* tree)
{
    trees_.push_back(std::make_pair(name, std::unique_ptr<TreeType>(tree)));
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief
 */
template <class NDT, class EDT>
typename TreeSet<NDT, EDT>::TreeType* TreeSet<NDT, EDT>::GetFirst(const std::string& name)
{
    auto ct = trees_.begin();
    while (ct != trees_.end()) {
        if (ct->first == name) {
            return ct->second->get();
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
template <class NDT, class EDT>
std::string TreeSet<NDT, EDT>::Dump(bool full)
{
    std::string res = "";
    auto ct = trees_.begin();
    while (ct != trees_.end()) {
        res += ct->first + "\n";
        if (full) {
            res += ct->second->Dump() + "\n";
        }
    }
    return res;
}

} // namespace genesis
