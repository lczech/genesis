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
#include "tree/iterator/postorder.hpp"
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
        std::string name = pqry->names.size() > 0 ? pqry->names[0]->name : "";
        if( pqry->names.size() > 1 ) {
            name += " (+" + std::to_string( pqry->names.size() - 1 ) + ")";
        }

        for( auto const& p : pqry->placements ) {
            table << std::to_string(i);
            table << name;
            table << std::to_string( p->edge_num() );
            table << std::to_string( p->likelihood );
            table << std::to_string( p->like_weight_ratio );
            table << std::to_string( p->proximal_length );
            table << std::to_string( p->pendant_length );
        }

        ++i;
    }

    out << utils::simple_layout()(table);
    return out;
}

/**
 * @brief Return a simple view of the Tree of a Sample with information about the
 * @link Pqyery Pqueries @endlink on it.
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

// =================================================================================================
//     Verification
// =================================================================================================

bool has_correct_edge_nums( Sample const& smp )
{
    auto const& tree = smp.tree();
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

    // get map of placements per edge
    auto place_map = placements_per_edge( smp );

    // check edges
    std::unordered_map<int, PlacementTree::EdgeType*> edge_num_map;
    size_t edge_place_count = 0;
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

        // make sure the pointers and references are set correctly
        for( PqueryPlacement const* p : place_map[ edge->index() ]) {
            if (&p->edge() != edge) {
                LOG_INFO << "Inconsistent pointer from placement to edge at edge num '"
                         << edge->data.edge_num() << "'.";
                return false;
            }
            if (p->edge_num() != edge->data.edge_num()) {
                LOG_INFO << "Inconsistent edge_num between edge and placement: '"
                         << edge->data.edge_num() << " != " << p->edge_num() << "'.";
                return false;
            }
            ++edge_place_count;
        }
    }

    // check pqueries
    size_t pqry_place_count = 0;
    for( const auto& pqry : smp.pqueries() ) {
        // use this name for reporting invalid placements.
        std::string name;
        if (pqry->names.size() > 0) {
            name = "'" + pqry->names[0]->name + "'";
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
            // make sure the pointers and references are set correctly
            int found_placement_on_edge = 0;
            for( PqueryPlacement const* pe : place_map[ p->edge().index() ]) {
                if (pe == p.get()) {
                    ++found_placement_on_edge;
                }
            }
            if( place_map[ p->edge().index() ].size() > 0 && found_placement_on_edge == 0) {
                LOG_INFO << "Inconsistency between placement and edge: edge num '"
                         << p->edge().data.edge_num() << "' does not contain pointer to a placement "
                         << "that is referring to that edge at " << name << ".";
                return false;
            }
            if (found_placement_on_edge > 1) {
                LOG_INFO << "Edge num '" << p->edge().data.edge_num() << "' contains a pointer to one "
                         << "of its placements more than once at " << name << ".";
                return false;
            }
            if (p->edge_num() != p->edge().data.edge_num()) {
                LOG_INFO << "Inconsistent edge_num between edge and placement: '"
                         << p->edge().data.edge_num() << " != " << p->edge_num()
                         << "' at " << name << ".";
                return false;
            }
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
        for (const auto& n : pqry->names) {
            // make sure the pointers and references are set correctly
            if (n->pquery != pqry.get()) {
                LOG_INFO << "Inconsistent pointer from name '" << n->name << "' to pquery.";
                return false;
            }
        }
    }

    if (edge_place_count != pqry_place_count) {
        LOG_INFO << "Inconsistent number of placements on edges (" << edge_place_count
                 << ") and pqueries (" << pqry_place_count << ").";
        return false;
    }

    return true;
}

} // namespace placement
} // namespace genesis
