/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/placement/function/helper.hpp"

#include "genesis/placement/pquery/plain.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/utils/core/logging.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Helper Functions
// =================================================================================================

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

std::unordered_map<int, PlacementTreeEdge*> edge_num_to_edge_map( Sample const& smp )
{
    return edge_num_to_edge_map( smp.tree() );
}

std::vector< std::vector< PqueryPlacement const* >> placements_per_edge(
    Sample const& smp,
    bool only_max_lwr_placements
) {
    std::vector< std::vector< PqueryPlacement const* >> result;
    result.resize( smp.tree().edge_count() );

    for( auto const& pqry : smp.pqueries() ) {
        if( only_max_lwr_placements ) {

            // If we are only interested in the most probably placement, find it first.
            PqueryPlacement const* max_p = nullptr;
            double max_v = std::numeric_limits<double>::lowest();
            for( auto const& place : pqry.placements() ) {
                if( place.like_weight_ratio > max_v ) {
                    max_v = place.like_weight_ratio;
                    max_p = &place;
                }
            }
            // If there is one, add it to the list for its edge.
            if( max_p ) {
                result[ max_p->edge().index() ].push_back( max_p );
            }

        } else {
            // If we instead want all placement, simply add them.
            for( auto const& place : pqry.placements() ) {
                result[ place.edge().index() ].push_back( &place );
            }
        }
    }

    return result;
}

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

utils::Matrix<size_t> placement_count_per_edge( SampleSet const& sample_set )
{
    // Basics.
    auto const set_size = sample_set.size();
    if( set_size == 0 ) {
        return {};
    }

    // Init matrix.
    auto result = utils::Matrix<size_t>( set_size, sample_set[ 0 ].sample.tree().edge_count(), 0 );

    // Fill matrix.
    #pragma omp parallel for
    for( size_t i = 0; i < set_size; ++i ) {
        for( auto const& pqry : sample_set[ i ].sample.pqueries() ) {
            for( auto const& place : pqry.placements() ) {
                ++result( i, place.edge().index() );
            }
        }
    }

    return result;
}

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

utils::Matrix<double> placement_weight_per_edge( SampleSet const& sample_set )
{
    // Basics.
    auto const set_size = sample_set.size();
    if( set_size == 0 ) {
        return {};
    }

    // Init matrix.
    auto result = utils::Matrix<double>( set_size, sample_set[ 0 ].sample.tree().edge_count(), 0.0 );

    // Fill matrix.
    #pragma omp parallel for
    for( size_t i = 0; i < set_size; ++i ) {
        for( auto const& pqry : sample_set[ i ].sample.pqueries() ) {
            for( auto const& place : pqry.placements() ) {
                result( i, place.edge().index() ) += place.like_weight_ratio;
            }
        }
    }

    return result;
}

std::vector<PqueryPlain> plain_queries( Sample const & smp )
{
    auto pqueries = std::vector<PqueryPlain>( smp.size() );

    #pragma omp parallel for
    for (size_t i = 0; i < smp.size(); ++i) {
        pqueries[i].index = i;

        const auto& opqry = smp.at(i);
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

bool has_consecutive_edge_nums( PlacementTree const& tree )
{
    // List all edge nums.
    auto order = std::vector<int>();
    for( auto const& edge : tree.edges() ) {
        order.push_back( edge->data<PlacementEdgeData>().edge_num() );
    }

    // Sort them and use unique to see wether there are duplicates.
    std::sort( order.begin(), order.end() );
    if( std::unique( order.begin(), order.end() ) != order.end() ) {
        return false;
    }

    // Now check whether they are consecutive and start at 0.
    if( order.empty() ) {
        return true;
    } else {
        assert( order.size() >= 1 );
        return order.front() == 0 && order.back() == static_cast<int>( order.size() - 1 );
    }
}

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

        // Check the sum of LWRs, with some small tolerance.
        if (check_values && ratio_sum > 1.000000001) {
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
