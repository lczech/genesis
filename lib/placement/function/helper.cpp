/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/helper.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Return a mapping of edge_num integers to the corresponding TreeEdge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
std::unordered_map<int, PlacementTree::EdgeType*> edge_num_to_edge_map( PlacementTree const& tree )
{
    auto en_map = std::unordered_map<int, PlacementTree::EdgeType*>();
    for (
        PlacementTree::ConstIteratorEdges it = tree.begin_edges();
        it != tree.end_edges();
        ++it
    ) {
        const auto& edge = *it;
        assert(en_map.count(edge->data.edge_num()) == 0);
        en_map.emplace(edge->data.edge_num(), edge.get());
    }
    return en_map;
}

/**
 * @brief Return a mapping of edge_num integers to the corresponding TreeEdge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
std::unordered_map<int, PlacementTree::EdgeType*> edge_num_to_edge_map( Sample const & smp )
{
    return edge_num_to_edge_map( smp.tree() );
}

/**
 * @brief Return a mapping from PlacementTreeEdge indices to a vector of all PqueryPlacement%s that
 * are placed on that edge, for all edges of the Sample.
 */
std::unordered_map< size_t, std::vector< PqueryPlacement const* >> placements_per_edge(
    Sample const& smp
) {
    std::unordered_map< size_t, std::vector< PqueryPlacement const* >> result;

    for( auto const& pqry : smp.pqueries() ) {
        for( auto const& place : pqry->placements ) {
            result[ place->edge->index() ].push_back( place.get() );
        }
    }

    return result;
}

/**
 * @brief Return a vector of all PqueryPlacement%s that are placed on the given PlacementTreeEdge.
 */
std::vector<PqueryPlacement const*> placements_per_edge(
    Sample            const& smp,
    PlacementTreeEdge const& edge
) {
    std::vector<PqueryPlacement const*> result;

    for( auto const& pqry : smp.pqueries() ) {
        for( auto const& place : pqry->placements ) {
            if( place->edge == &edge ) {
                result.push_back( place.get() );
            }
        }
    }

    return result;
}

/**
 * @brief Return a plain representation of all pqueries of this map.
 *
 * This method produces a whole copy of all pqueries and their placements (though, not their names)
 * in a plain POD format. This format is meant for speeding up computations that need access to
 * the data a lot - which would require several pointer indirections in the normal representation
 * of the data.
 *
 * This comes of course at the cost of reduced flexibility, as all indices are fixed in the
 * plain data structre: changing a value here will not have any effect on the original data or
 * even on the values of the pqueries. Thus, most probably this will lead to corruption. Therefore,
 * this data structure is meant for reading only.
 */
std::vector<PqueryPlain> plain_queries( Sample const & smp )
{
    auto pqueries = std::vector<PqueryPlain>( smp.pqueries().size() );
    for (size_t i = 0; i < smp.pqueries().size(); ++i) {
        pqueries[i].index = i;

        const auto& opqry = smp.pquery_at(i);
        pqueries[i].placements = std::vector<PqueryPlacementPlain>(opqry.placements.size());

        for (size_t j = 0; j < opqry.placements.size(); ++j) {
            const auto& oplace = opqry.placements[j];
            auto& place = pqueries[i].placements[j];

            place.edge_index           = oplace->edge->index();
            place.primary_node_index   = oplace->edge->primary_node()->index();
            place.secondary_node_index = oplace->edge->secondary_node()->index();

            place.branch_length        = oplace->edge->data.branch_length;
            place.pendant_length       = oplace->pendant_length;
            place.proximal_length      = oplace->proximal_length;
            place.like_weight_ratio    = oplace->like_weight_ratio;
        }
    }
    return pqueries;
}

} // namespace placement
} // namespace genesis
