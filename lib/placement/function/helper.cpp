/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/function/helper.hpp"

#include "placement/pquery/plain.hpp"
#include "tree/function/operators.hpp"
#include "tree/iterator/postorder.hpp"

#include "utils/core/logging.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Return a mapping of edge_num integers to the corresponding PlacementTreeEdge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( PlacementTree const& tree )
{
    auto en_map = std::unordered_map<int, PlacementTreeEdge*>();
    for (
        PlacementTree::ConstIteratorEdges it = tree.begin_edges();
        it != tree.end_edges();
        ++it
    ) {
        auto const& edge      = *it;
        auto const& edge_data = edge->data<PlacementEdgeData>();
        assert( en_map.count( edge_data.edge_num() ) == 0);
        en_map.emplace( edge_data.edge_num(), edge.get());
    }
    return en_map;
}

/**
 * @brief Return a mapping of edge_num integers to the corresponding PlacementTreeEdge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( Sample const & smp )
{
    return edge_num_to_edge_map( smp.tree() );
}

/**
 * @brief Return a mapping from PlacementTreeEdge indices to a vector of all PqueryPlacement%s that
 * are placed on that edge, for all edges of the Sample.
 *
 * This map is invalidated after calling Pquery::add_placement().
 */
std::unordered_map< size_t, std::vector< PqueryPlacement const* >> placements_per_edge(
    Sample const& smp
) {
    std::unordered_map< size_t, std::vector< PqueryPlacement const* >> result;

    for( auto const& pqry : smp.pqueries() ) {
        for( auto const& place : pqry.placements() ) {
            result[ place.edge().index() ].push_back( &place );
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
 *
 * This map is invalidated after calling Pquery::add_placement().
 */
std::vector<PqueryPlacement const*> placements_per_edge(
    Sample            const& smp,
    PlacementTreeEdge const& edge
) {
    std::vector<PqueryPlacement const*> result;

    for( auto const& pqry : smp.pqueries() ) {
        for( auto const& place : pqry.placements() ) {
            if( &place.edge() == &edge ) {
                result.push_back( &place );
            }
        }
    }

    return result;
}

/**
 * @brief Return a vector that contains the number of PqueryPlacement%s per
 * @link ::PlacementTreeEdge edge@endlink of the @link ::PlacementTree tree@endlink of the Sample.
 *
 * The vector is indexed using the @link PlacementTreeEdge::index() index@endlink of the edges.
 */
std::vector<size_t> placement_count_per_edge( Sample const& sample )
{
    auto result = std::vector<size_t>( sample.tree().edge_count(), 0 );

    for( auto const& pqry : sample.pqueries() ) {
        for( auto const& place : pqry.placements() ) {
            ++result[ place.edge().index() ];
        }
    }

    return result;
}

/**
 * @brief Return a vector that contains the sum of the weights of the PqueryPlacement%s per
 * @link ::PlacementTreeEdge edge@endlink of the @link ::PlacementTree tree@endlink of the Sample.
 *
 * The weight is measured in @link PqueryPlacement::like_weight_ratio `like_weight_ratio`@endlink.
 * The vector is indexed using the @link PlacementTreeEdge::index() index@endlink of the edges.
 */
std::vector<double> placement_weight_per_edge( Sample const& sample )
{
    auto result = std::vector<double>( sample.tree().edge_count(), 0.0 );

    for( auto const& pqry : sample.pqueries() ) {
        for( auto const& place : pqry.placements() ) {
            result[ place.edge().index() ] += place.like_weight_ratio;
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
    auto pqueries = std::vector<PqueryPlain>( smp.pquery_size() );
    for (size_t i = 0; i < smp.pquery_size(); ++i) {
        pqueries[i].index = i;

        const auto& opqry = smp.pquery_at(i);
        pqueries[i].placements = std::vector<PqueryPlacementPlain>(opqry.placement_size());

        for (size_t j = 0; j < opqry.placement_size(); ++j) {
            auto const& oplace = opqry.placement_at(j);
            auto& place = pqueries[i].placements[j];

            place.edge_index           = oplace.edge().index();
            place.primary_node_index   = oplace.edge().primary_node().index();
            place.secondary_node_index = oplace.edge().secondary_node().index();

            auto const& oplace_data    = oplace.edge().data<PlacementEdgeData>();
            place.branch_length        = oplace_data.branch_length;
            place.pendant_length       = oplace.pendant_length;
            place.proximal_length      = oplace.proximal_length;
            place.like_weight_ratio    = oplace.like_weight_ratio;
        }
    }
    return pqueries;
}

// =================================================================================================
//     Verification
// =================================================================================================

/**
 * @brief Reset all edge nums of a ::PlacementTree.
 *
 * The `edge_num` property of the PlacementTreeEdge%s is defined by the `jplace` standard.
 * The values have to be assigned increasingly with a postorder traversal of the tree.
 * This function resets them so that this is established.
 *
 * See has_correct_edge_nums() to check whether the edge nums are already correct. This should
 * be the case for any valid `jplace` file.
 */
void reset_edge_nums( PlacementTree& tree )
{
    // Edge numbers need to be in ascending order via postorder traversal.
    int current = 0;
    for( auto it : postorder(tree) ) {
        // The last iteration is skipped, as the root does not have an edge.
        if (it.is_last_iteration()) {
            continue;
        }

        it.edge().data<PlacementEdgeData>().reset_edge_num( current );
        ++current;
    }
}

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
    for( auto it : postorder(tree) ) {
        // The last iteration is skipped, as the root does not have an edge.
        if (it.is_last_iteration()) {
            continue;
        }

        if( it.edge().data<PlacementEdgeData>().edge_num() != current) {
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
    if( ! tree::validate_topology( smp.tree() ) ) {
        LOG_INFO << "Invalid placement tree topology.";
        return false;
    }
    if( ! tree::tree_data_is< PlacementNodeData, PlacementEdgeData >( smp.tree() )) {
        LOG_INFO << "Tree does not only contain Placement Node and Edge data types.";
        return false;
    }

    // check edges
    std::unordered_map<int, PlacementTreeEdge*> edge_num_map;
    for (
        auto it_e = smp.tree().begin_edges();
        it_e != smp.tree().end_edges();
        ++it_e
    ) {
        // make sure every edge num is used once only
        PlacementTreeEdge& edge = *(*it_e).get();
        if( edge_num_map.count( edge.data<PlacementEdgeData>().edge_num() ) > 0 ) {
            LOG_INFO << "More than one edge has edge_num '"
                     << edge.data<PlacementEdgeData>().edge_num() << "'.";
            return false;
        }
        edge_num_map.emplace(
            edge.data<PlacementEdgeData>().edge_num(), (*it_e).get()
        );
    }
    if( ! has_correct_edge_nums( smp.tree() )) {
        LOG_INFO << "Tree does not have correct edge nums.";
        return false;
    }

    // check pqueries
    size_t pqry_place_count = 0;
    for( auto const& pqry : smp.pqueries() ) {
        // use this name for reporting invalid placements.
        std::string name;
        if (pqry.name_size() > 0) {
            name = "'" + pqry.name_at(0).name + "'";
        } else {
            name = "(unnamed pquery)";
        }

        // check placements
        if (check_values && pqry.placement_size() == 0) {
            LOG_INFO << "Pquery without any placements at '" << name << "'.";
            if (break_on_values) {
                return false;
            }
        }
        double ratio_sum = 0.0;
        for( auto pit = pqry.begin_placements(); pit != pqry.end_placements(); ++pit ) {
            auto const& p = *pit;
            auto const& edge_data = p.edge().data<PlacementEdgeData>();

            // Check if the placement has a valid pointer to its edge. This is a bit hard to do,
            // as we use a raw pointer, so there is no easy way of telling whether it is valid
            // or dangling. Instead, we simply check if the object behind it has the correct
            // properties.
            if(
                p.edge().index() >= smp.tree().edge_count()         ||
                edge_num_map.count( edge_data.edge_num() ) == 0     ||
                edge_data.edge_num() != p.edge_num()
            ) {
                LOG_INFO << "Invlaid edge pointer or edge num.";
                return false;
            }

            // now we know that all references between placements and edges are correct, so this
            // assertion breaks only if we forgot to check some sort of weird inconsistency.
            assert( edge_num_map.count( p.edge_num() ) > 0 );
            ++pqry_place_count;

            // check numerical values
            if (!check_values) {
                continue;
            }
            if (p.like_weight_ratio < 0.0 || p.like_weight_ratio > 1.0) {
                LOG_INFO << "Invalid placement with like_weight_ratio '" << p.like_weight_ratio
                        << "' not in [0.0, 1.0] at " << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p.pendant_length < 0.0 || p.proximal_length < 0.0) {
                LOG_INFO << "Invalid placement with pendant_length '" << p.pendant_length
                         << "' or proximal_length '" << p.proximal_length << "' < 0.0 at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p.proximal_length > edge_data.branch_length) {
                LOG_INFO << "Invalid placement with proximal_length '" << p.proximal_length
                         << "' > branch_length '"
                         << edge_data.branch_length << "' at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            ratio_sum += p.like_weight_ratio;
        }
        if (check_values && ratio_sum > 1.0) {
            LOG_INFO << "Invalid pquery with sum of like_weight_ratio '" << ratio_sum
                     << "' > 1.0 at " << name << ".";
            if (break_on_values) {
                return false;
            }
        }

        // check names
        if (check_values && pqry.name_size() == 0) {
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
