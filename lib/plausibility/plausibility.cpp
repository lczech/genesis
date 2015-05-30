/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "plausibility/plausibility.hpp"

#include "tree/bipartition_set.hpp"
#include "tree/newick_processor.hpp"
#include "tree/tree_set.hpp"
#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Plausibility
// =============================================================================

void Plausibility::spiderpig_function (
    const std::string& reference_tree_file,
    const std::string& small_tree_file
) {
    // read trees from files
    PlausibilityTree small_tree;
    NewickProcessor::FromFile(reference_tree_file, reference_tree);
    NewickProcessor::FromFile(small_tree_file,     small_tree);

    // create preorder ids for every node, and a reference map for leaves
    std::map<std::string, size_t> reference_map;
    size_t c = 0;
    for (
        auto it = reference_tree.BeginPreorder();
        it != reference_tree.EndPreorder();
        ++it
    ) {
        it.Node()->preorder_id = c;
        if (it.Node()->IsLeaf()) {
            reference_map[it.Node()->name] = c;
        }
        ++c;
    }

    // do euler traversal and collect ids on the fly
    std::vector<size_t> euler_ids;
    euler_ids.reserve(4 * reference_tree.NodeCount());
    std::string ausgabe;
    for (
        auto it = reference_tree.BeginEulertour();
        it != reference_tree.EndEulertour();
        ++it
    ) {
        euler_ids.push_back(it.Node()->preorder_id);

        ausgabe += std::to_string(it.Node()->preorder_id) + " ";
    }
    LOG_DBG << ausgabe;

    // TODO fertig machen!
}

} // namespace genesis
