/**
 * @brief Implementation of TreeSet class.
 *
 * @file
 * @ingroup tree
 */

#include <algorithm>
#include <assert.h>

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
void TreeSet<TreeType>::add (const std::string& name, std::shared_ptr<TreeType> tree)
{
    trees_.push_back( { name, tree } );
}

/**
 * @brief Clears the TreeSet and destroys all contained Trees.
 */
template <class TreeType>
void TreeSet<TreeType>::clear ()
{
    trees_.clear();
}

/**
 * @brief Returns a Tree with the same topology of all trees of this set, but averaged branch
 * lenghts.
 *
 * The method works only under the following conditions:
 *
 *     * The TreeType must provide fields `name` for the nodes and `branch_length` for the edges.
 *     * All trees must have the same topology and names.
 *
 * Otherwise, the method will return an empty tree.
 */
template <class TreeType>
TreeType TreeSet<TreeType>::average_branch_length_tree () const
{
    if (trees_.size() == 0) {
        LOG_WARN << "TreeSet is empty.";
        return TreeType();
    }

    auto comparator = [] (
        typename TreeType::ConstIteratorPreorder& it_l,
        typename TreeType::ConstIteratorPreorder& it_r
    ) {
        return it_l.node()->name == it_r.node()->name;
    };

    // Prepare storage for average branch lengths.
    size_t num_edges = trees_.front().edge_count();
    auto avgs = std::vector<double>(num_edges, 0.0);

    // Check whether all trees have same topology and names. Furthermore, start adding the branch
    // lengths of all trees to `avgs`.
    for (size_t i = 0; i < trees_.size(); i++) {
        for (auto it = trees_[i].begin_edges(); it != trees_[i].end_edges(); ++it) {
            assert((*it)->index < avgs.size());
            avgs[(*it)->index] += (*it)->branch_length;
        }

        // Check whether two adjacent trees have same topology and names. As all are supposed to be
        // the same, we do not need a pairwise comparison.
        if (i > 0 && !TreeType::equal(trees_[i-1], trees_[i], comparator)) {
            LOG_WARN << "Trees in set are not equal for creating an average branch length tree.";
            return TreeType();
        }
    }

    // Calcuate avergage branch lengths.
    for (auto& bl : avgs) {
        bl /= trees_.size();
    }

    // Now we know that all trees are the same (for the purposes of this method). So we take a copy
    // and modify its branch lengths.
    TreeType tree = TreeType(trees_.front());
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        (*it)->branch_length = avgs[(*it)->index];
    }

    return tree;
}

// =============================================================================
//     Comparators
// =============================================================================

/**
 * @brief Compares whether all Trees in the set are equal using their default comparision operators
 * for nodes and edges.
 */
template <class TreeType>
bool TreeSet<TreeType>::all_equal() const
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
bool TreeSet<TreeType>::all_identical_topology() const
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
TreeType* TreeSet<TreeType>::get_first(const std::string& name) const
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
std::string TreeSet<TreeType>::dump(bool full) const
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
