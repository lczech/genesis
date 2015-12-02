/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"
#include "tree/iterators/postorder.hpp"
#include "tree/operators.hpp"

#include "utils/core/logging.hpp"

namespace genesis {

// =================================================================================================
//     Comparision and Equality
// =================================================================================================

bool compatible_trees (const PlacementMap& lhs, const PlacementMap& rhs)
{
    auto node_comparator = [] (
        const PlacementTree::NodeType& node_l,
        const PlacementTree::NodeType& node_r
    ) {
        return node_l.data.name == node_r.data.name &&
               node_l.index()   == node_r.index();
    };

    auto edge_comparator = [] (
        const PlacementTree::EdgeType& edge_l,
        const PlacementTree::EdgeType& edge_r
    ) {
        return edge_l.data.edge_num             == edge_r.data.edge_num             &&
               edge_l.primary_node()->index()   == edge_r.primary_node()->index()   &&
               edge_l.secondary_node()->index() == edge_r.secondary_node()->index();
    };

    return equal(lhs.tree(), rhs.tree(), node_comparator, edge_comparator);
}

// =================================================================================================
//     Verification
// =================================================================================================

bool has_correct_edge_nums( PlacementMap const& map )
{
    auto const& tree = map.tree();
    int current = 0;

    // Edge numbers need to be in ascending order via postorder traversal. Check this.
    for (auto it = tree.begin_postorder(); it != tree.end_postorder(); ++it) {
        // The last iteration is skipped, as the root does not have an edge.
        if (it.is_last_iteration()) {
            continue;
        }

        if (it.edge()->data.edge_num != current) {
            return false;
        }
        ++current;
    }

    return true;
}

} // namespace genesis
