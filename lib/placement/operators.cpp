/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/operators.hpp"

#include "placement/placement_map.hpp"
#include "tree/iterators/postorder.hpp"
#include "tree/operators.hpp"
#include "utils/text/table.hpp"

#include <sstream>

namespace genesis {
namespace placement {

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

    return equal<PlacementTree, PlacementTree>(
        lhs.tree(), rhs.tree(), node_comparator, edge_comparator
    );
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

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Print a table of all Pqueries with their Placements and Names to the stream.
 */
std::ostream& operator << (std::ostream& out, PlacementMap const& map)
{
    auto table = utils::Table();
    auto const kRight = utils::Table::Column::Justification::kRight;

    table.add_column("#").justify(kRight);
    table.add_column("name");
    table.add_column("edge_num").justify(kRight);
    table.add_column("likelihood").justify(kRight);
    table.add_column("like_weight_ratio").justify(kRight);
    table.add_column("proximal_length").justify(kRight);
    table.add_column("pendant_length").justify(kRight);

    size_t i = 0;
    for( auto const& pqry : map.pqueries() ) {
        std::string name = pqry->names.size() > 0 ? pqry->names[0]->name : "";
        if( pqry->names.size() > 1 ) {
            name += " (+" + std::to_string( pqry->names.size() - 1 ) + ")";
        }

        for( auto const& p : pqry->placements ) {
            table << std::to_string(i);
            table << name;
            table << std::to_string(p->edge_num);
            table << std::to_string(p->likelihood);
            table << std::to_string(p->like_weight_ratio);
            table << std::to_string(p->proximal_length);
            table << std::to_string(p->pendant_length);
        }

        ++i;
    }

    out << utils::simple_layout()(table);
    return out;
}

} // namespace placement
} // namespace genesis
