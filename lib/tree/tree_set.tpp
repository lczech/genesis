/**
 * @brief Implementation of TreeSet class.
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
 * @brief Adds a Tree to the TreeSet.
 */
template <class TreeType>
void TreeSet<TreeType>::add (const std::string& name, TreeType* tree)
{
    trees_.push_back( { name, std::shared_ptr<TreeType>(tree) } );
}

/**
 * @brief Clears the TreeSet and destroys all contained Trees.
 */
template <class TreeType>
void TreeSet<TreeType>::clear ()
{
    trees_.clear();
}

// =============================================================================
//     Comparators
// =============================================================================

/**
 * @brief Compares whether all Trees in the set are equal using their default comparision operators
 * for nodes and edges.
 */
template <class TreeType>
bool TreeSet<TreeType>::all_equal()
{
    for (size_t i = 1; i < trees_.size(); i++) {
        if (!TreeType::equal(trees_[i-1], trees_[i])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Returns true iff all Trees in the set have an identical topology.
 */
template <class TreeType>
bool TreeSet<TreeType>::all_identical_topology()
{
    for (size_t i = 1; i < trees_.size(); i++) {
        if (!TreeType::identical_topology(trees_[i-1], trees_[i])) {
            return false;
        }
    }
    return true;
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief Get the first Tree in the set that is stored with a given name.
 */
template <class TreeType>
TreeType* TreeSet<TreeType>::get_first(const std::string& name)
{
    auto ct = trees_.begin();
    while (ct != trees_.end()) {
        if (ct->name == name) {
            return ct->tree.get();
        }
        ++ct;
    }
    return nullptr;
}

// =============================================================================
//     Dump & Debug
// =============================================================================

/**
 * @brief Dump a list of all names of the Trees in the set. If provided with the optional parameter
 * `full`, also dump all Trees.
 */
template <class TreeType>
std::string TreeSet<TreeType>::dump(bool full)
{
    std::string res = "";
    for (auto& ct : trees_) {
        res += ct.name + "\n";
        if (full) {
            res += ct.tree->dump() + "\n";
        }
    }
    return res;
}

} // namespace genesis
