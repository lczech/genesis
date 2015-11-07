/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"
#include "tree/operators.hpp"

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

} // namespace genesis
