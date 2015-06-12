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
 * @brief Returns a Tree where the branch lengths are the average of the trees in the set, given that
 * they all have the same topology.
 *
 * The method works only under the following conditions:
 *
 *     * All trees must have the same topology.
 *     * The TreeType must provide field `branch_length` for the edges.
 *
 * Otherwise, the method will return an empty tree. It does not check for node names, but the
 * returned tree will contain the names of the first tree in the set.
 */
template <class TreeType>
TreeType TreeSet<TreeType>::average_branch_length_tree () const
{
    if (trees_.size() == 0) {
        LOG_WARN << "TreeSet is empty.";
        return TreeType();
    }

    if (!all_identical_topology()) {
        LOG_WARN << "Trees in TreeSet do not have the same topology.";
        return TreeType();
    }

    // Prepare storage for average branch lengths.
    size_t num_edges = trees_.front().tree->edge_count();
    auto avgs = std::vector<double>(num_edges, 0.0);

    // We traverse all trees (again, because all_identical_topology() already did this). This is
    // probably a bit slower than the previous version of this method which worked with less
    // traversals, but way easier to understand and debug.
    for (auto& ct : trees_) {
        // Use an index for position in the preorder traversal. This makes sure that the
        // index actually always points to the correct edges, indepently of their order in
        // different trees in the set.
        size_t idx = 0;

        // Do a preorder traversal and collect branch lengths.
        for (
            auto it = ct.tree->begin_preorder();
            it != ct.tree->end_preorder();
            ++it
        ) {
            // The first iteration points to an edge which will be covered later again.
            // Skip it to prevent double coverage.
            if (it.is_first_iteration()) {
                continue;
            }

            avgs[idx] += it.edge()->branch_length;
            ++idx;
        }
    }

    // Calcuate avergage branch lengths.
    for (auto& bl : avgs) {
        bl /= trees_.size();
    }

    // We know that all trees have the same topology. So we take a copy of the first one (thus, also
    // copying its node names) and modify its branch lengths.
    TreeType tree = TreeType(*trees_.front().tree);
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        (*it)->branch_length = avgs[(*it)->index()];
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
    // If all pairs of two adjacent trees are equal, all of them are.
    // Thus, we do not need a complete pairwise comparision.
    for (size_t i = 1; i < trees_.size(); i++) {
        if (!TreeType::equal(*trees_[i-1].tree, *trees_[i].tree)) {
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
    // If all pairs of two adjacent trees have same the topology, all of them have.
    // Thus, we do not need a complete pairwise comparision.
    for (size_t i = 1; i < trees_.size(); i++) {
        if (!TreeType::identical_topology(*trees_[i-1].tree, *trees_[i].tree)) {
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
