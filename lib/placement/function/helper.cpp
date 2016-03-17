/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/helper.hpp"

#include "tree/iterator/postorder.hpp"
#include "tree/operators.hpp"

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
            result[ place->edge().index() ].push_back( place.get() );
        }
    }

    return result;
}

/**
 * @brief Return a vector of all PqueryPlacement%s that are placed on the given PlacementTreeEdge.
 *
 * This functions iterates over all placements and collects those that are placed on the given
 * edge. In case that this is needed for multiple edges, it will be faster to use
 * placements_per_edge( Sample ) instead.
 */
std::vector<PqueryPlacement const*> placements_per_edge(
    Sample            const& smp,
    PlacementTreeEdge const& edge
) {
    std::vector<PqueryPlacement const*> result;

    for( auto const& pqry : smp.pqueries() ) {
        for( auto const& place : pqry->placements ) {
            if( &place->edge() == &edge ) {
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

            place.edge_index           = oplace->edge().index();
            place.primary_node_index   = oplace->edge().primary_node()->index();
            place.secondary_node_index = oplace->edge().secondary_node()->index();

            place.branch_length        = oplace->edge().data.branch_length;
            place.pendant_length       = oplace->pendant_length;
            place.proximal_length      = oplace->proximal_length;
            place.like_weight_ratio    = oplace->like_weight_ratio;
        }
    }
    return pqueries;
}

// =================================================================================================
//     Verification
// =================================================================================================

/**
 * @brief Verify that the tree has correctly set edge nums.
 *
 * The `edge_num` property of the PlacementTreeEdge%s is defined by the `jplace` standard.
 * The values have to be assigned increasingly with a postorder traversal of the tree.
 * This function checks whether this is the case.
 */
bool has_correct_edge_nums( PlacementTree const& tree )
{
    int current = 0;

    // Edge numbers need to be in ascending order via postorder traversal. Check this.
    for (auto it = tree.begin_postorder(); it != tree.end_postorder(); ++it) {
        // The last iteration is skipped, as the root does not have an edge.
        if (it.is_last_iteration()) {
            continue;
        }

        if (it.edge()->data.edge_num() != current) {
            return false;
        }
        ++current;
    }

    return true;
}

/**
 * @brief Validate the integrity of the pointers, references and data in a Sample object.
 *
 * Returns true iff everything is set up correctly. In case of inconsistencies, the function stops
 * and returns false on the first encountered error.
 *
 * If `check_values` is set to true, also a check on the validity of numerical values is done, for
 * example that the proximal_length is smaller than the corresponding branch_length.
 * If additionally `break_on_values` is set, validate() will stop on the first encountered invalid
 * value. Otherwise it will report all invalid values to the log stream.
 */
bool validate( Sample const& smp, bool check_values, bool break_on_values )
{
    // check tree
    if( ! tree::validate( smp.tree() ) ) {
        LOG_INFO << "Invalid placement tree.";
        return false;
    }

    // check edges
    std::unordered_map<int, PlacementTree::EdgeType*> edge_num_map;
    for (
        auto it_e = smp.tree().begin_edges();
        it_e != smp.tree().end_edges();
        ++it_e
    ) {
        // make sure every edge num is used once only
        PlacementTree::EdgeType* edge = (*it_e).get();
        if (edge_num_map.count(edge->data.edge_num()) > 0) {
            LOG_INFO << "More than one edge has edge_num '" << edge->data.edge_num() << "'.";
            return false;
        }
        edge_num_map.emplace(edge->data.edge_num(), edge);
    }
    if( ! has_correct_edge_nums( smp.tree() )) {
        LOG_INFO << "Tree does not have correct edge nums.";
        return false;
    }

    // check pqueries
    size_t pqry_place_count = 0;
    for( const auto& pqry : smp.pqueries() ) {
        // use this name for reporting invalid placements.
        std::string name;
        if (pqry->names.size() > 0) {
            name = "'" + pqry->names[0].name + "'";
        } else {
            name = "(unnamed pquery)";
        }

        // check placements
        if (check_values && pqry->placements.size() == 0) {
            LOG_INFO << "Pquery without any placements at '" << name << "'.";
            if (break_on_values) {
                return false;
            }
        }
        double ratio_sum = 0.0;
        for (const auto& p : pqry->placements) {
            // now we know that all references between placements and edges are correct, so this
            // assertion breaks only if we forgot to check some sort of weird inconsistency.
            assert( edge_num_map.count( p->edge_num() ) > 0 );
            ++pqry_place_count;

            // check numerical values
            if (!check_values) {
                continue;
            }
            if (p->like_weight_ratio < 0.0 || p->like_weight_ratio > 1.0) {
                LOG_INFO << "Invalid placement with like_weight_ratio '" << p->like_weight_ratio
                        << "' not in [0.0, 1.0] at " << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p->pendant_length < 0.0 || p->proximal_length < 0.0) {
                LOG_INFO << "Invalid placement with pendant_length '" << p->pendant_length
                         << "' or proximal_length '" << p->proximal_length << "' < 0.0 at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p->proximal_length > p->edge().data.branch_length) {
                LOG_INFO << "Invalid placement with proximal_length '" << p->proximal_length
                         << "' > branch_length '" << p->edge().data.branch_length << "' at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            ratio_sum += p->like_weight_ratio;
        }
        if (check_values && ratio_sum > 1.0) {
            LOG_INFO << "Invalid pquery with sum of like_weight_ratio '" << ratio_sum
                     << "' > 1.0 at " << name << ".";
            if (break_on_values) {
                return false;
            }
        }

        // check names
        if (check_values && pqry->names.size() == 0) {
            LOG_INFO << "Pquery without any names at '" << name << "'.";
            if (break_on_values) {
                return false;
            }
        }
    }

    return true;
}

} // namespace placement
} // namespace genesis
