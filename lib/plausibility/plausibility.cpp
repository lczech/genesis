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
    NewickProcessor::from_file(reference_tree_file, reference_tree);
    NewickProcessor::from_file(small_tree_file,     small_tree);

    // create preorder ids for every node, and a reference map for leaves
    std::map<std::string, size_t> reference_map;
    size_t c = 0;
    for (
        auto it = reference_tree.begin_preorder();
        it != reference_tree.end_preorder();
        ++it
    ) {
        it.node()->preorder_id = c;
        if (it.node()->is_leaf()) {
            reference_map[it.node()->name] = c;
        }
        ++c;
    }

    // do euler traversal and collect ids on the fly
    std::vector<size_t> euler_ids;
    euler_ids.reserve(4 * reference_tree.node_count());
    std::string ausgabe;
    for (
        auto it = reference_tree.begin_eulertour();
        it != reference_tree.end_eulertour();
        ++it
    ) {
        euler_ids.push_back(it.node()->preorder_id);

        ausgabe += std::to_string(it.node()->preorder_id) + " ";
    }
    LOG_DBG << ausgabe;

    // TODO fertig machen!
}

} // namespace genesis
