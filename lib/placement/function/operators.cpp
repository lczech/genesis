/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/operators.hpp"

#include "placement/function/helper.hpp"
#include "placement/function/functions.hpp"
#include "placement/sample.hpp"
#include "tree/operators.hpp"
#include "tree/printer/compact.hpp"
#include "utils/text/table.hpp"

#include <sstream>
#include <unordered_map>

namespace genesis {
namespace placement {

// =================================================================================================
//     Comparision and Equality
// =================================================================================================

bool compatible_trees (const Sample& lhs, const Sample& rhs)
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
        return edge_l.data.edge_num()           == edge_r.data.edge_num()           &&
               edge_l.primary_node()->index()   == edge_r.primary_node()->index()   &&
               edge_l.secondary_node()->index() == edge_r.secondary_node()->index();
    };

    return tree::equal<PlacementTree, PlacementTree>(
        lhs.tree(), rhs.tree(), node_comparator, edge_comparator
    );
}

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Print a table of all Pqueries with their Placements and Names to the stream.
 */
std::ostream& operator << (std::ostream& out, Sample const& smp)
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
    for( auto const& pqry : smp.pqueries() ) {
        std::string name = pqry->name_size() > 0 ? pqry->name_at(0).name : "";
        if( pqry->name_size() > 1 ) {
            name += " (+" + std::to_string( pqry->name_size() - 1 ) + ")";
        }

        for( auto pit = pqry->begin_placements(); pit != pqry->end_placements(); ++pit ) {
            auto const& p = *pit;

            table << std::to_string(i);
            table << name;
            table << std::to_string( p.edge_num() );
            table << std::to_string( p.likelihood );
            table << std::to_string( p.like_weight_ratio );
            table << std::to_string( p.proximal_length );
            table << std::to_string( p.pendant_length );
        }

        ++i;
    }

    out << utils::simple_layout()(table);
    return out;
}

/**
 * @brief Return a simple view of the Tree of a Sample with information about the
 * @link Pquery Pqueries @endlink on it.
 */
std::string print_tree( Sample const& smp )
{
    auto place_map = placements_per_edge( smp );

    auto print_line = [ &place_map ] (typename PlacementTree::ConstIteratorPreorder& it)
    {
        return it.node()->data.name + " [" + std::to_string(it.edge()->data.edge_num()) + "]" ": "
            + std::to_string( place_map[ it.edge()->index() ].size() ) + " placements";
    };
    return tree::PrinterCompact().print< PlacementTree >( smp.tree(), print_line );
}

} // namespace placement
} // namespace genesis
