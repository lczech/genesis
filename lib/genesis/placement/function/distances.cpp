/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/function/distances.hpp"

#include "genesis/placement/pquery.hpp"
#include "genesis/placement/pquery/plain.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery to Pquery Distances
// =================================================================================================

double pquery_distance(
    PqueryPlain const&           pquery_a,
    PqueryPlain const&           pquery_b,
    utils::Matrix<double> const& node_distances,
    bool                         with_pendant_length
) {
    double sum = 0.0;

    #pragma omp parallel for
    for( size_t pppai = 0; pppai < pquery_a.placements.size(); ++pppai ) {
        auto const& place_a = pquery_a.placements[ pppai ];

        #pragma omp parallel for
        for( size_t pppbi = 0; pppbi < pquery_b.placements.size(); ++pppbi ) {
            auto const& place_b = pquery_b.placements[ pppbi ];
            double dist;

            if (place_a.edge_index == place_b.edge_index) {
                // same branch case
                dist = std::abs(place_a.proximal_length - place_b.proximal_length);

            } else {
                double pp, pd, dp;

                // proximal-proximal case
                pp = place_a.proximal_length
                   + node_distances(place_a.primary_node_index, place_b.primary_node_index)
                   + place_b.proximal_length;

                // proximal-distal case
                pd = place_a.proximal_length
                   + node_distances(place_a.primary_node_index, place_b.secondary_node_index)
                   + place_b.branch_length - place_b.proximal_length;

                // distal-proximal case
                dp = place_a.branch_length - place_a.proximal_length
                   + node_distances(place_a.secondary_node_index, place_b.primary_node_index)
                   + place_b.proximal_length;

                // find min of the three cases and
                dist = std::min({ pp, pd, dp });
            }

            //  If needed, use pendant length; normalize it to the weight ratios.
            if (with_pendant_length) {
                dist += place_a.pendant_length + place_b.pendant_length;
            }
            dist *= place_a.like_weight_ratio * place_b.like_weight_ratio;

            #pragma omp atomic
            sum += dist;
        }
    }

    return sum;
}

/**
 * @brief Local helper function to avoid code duplication.
 */
template<typename DistanceFunction>
double pquery_distance(
    Pquery const& pquery_a,
    Pquery const& pquery_b,
    DistanceFunction distance_function
) {
    double sum = 0.0;

    #pragma omp parallel for
    for( size_t pai = 0; pai < pquery_a.placement_size(); ++pai ) {
        auto const& place_a = pquery_a.placement_at( pai );

        #pragma omp parallel for
        for( size_t pab = 0; pab < pquery_b.placement_size(); ++pab ) {
            auto const& place_b = pquery_b.placement_at( pab );

            double dist = distance_function( place_a, place_b );
            dist *= place_a.like_weight_ratio * place_b.like_weight_ratio;

            #pragma omp atomic
            sum += dist;
        }
    }

    return sum;
}

double pquery_distance(
    Pquery const&                pquery_a,
    Pquery const&                pquery_b,
    utils::Matrix<double> const& node_distances,
    bool                         with_pendant_length
) {
    return pquery_distance(
        pquery_a,
        pquery_b,
        [&]( PqueryPlacement const& place_a, PqueryPlacement const& place_b ){
            double dist = placement_distance( place_a, place_b, node_distances );
            if( with_pendant_length ) {
                dist += place_a.pendant_length + place_b.pendant_length;
            }
            return dist;
        }
    );
}

double placement_distance(
    PqueryPlacement const& place_a,
    PqueryPlacement const& place_b,
    utils::Matrix<double> const& node_distances
) {
    double pp, pd, dp, dist;

    if( place_a.edge().index() == place_b.edge().index() ) {
        // same branch case
        dist = std::abs( place_a.proximal_length - place_b.proximal_length );

    } else {
        // proximal-proximal case
        pp = place_a.proximal_length
           + node_distances(
               place_a.edge().primary_node().index(),
               place_b.edge().primary_node().index()
           )
           + place_b.proximal_length;

        // proximal-distal case
        pd = place_a.proximal_length
           + node_distances(
               place_a.edge().primary_node().index(),
               place_b.edge().secondary_node().index()
           )
           + place_b.edge().data<tree::DefaultEdgeData>().branch_length
           - place_b.proximal_length;

        // distal-proximal case
        dp = place_a.edge().data<tree::DefaultEdgeData>().branch_length
           - place_a.proximal_length
           + node_distances(
               place_a.edge().secondary_node().index(),
               place_b.edge().primary_node().index()
           )
           + place_b.proximal_length;

        // find min of the three cases
        dist = std::min({ pp, pd, dp });
    }

    return dist;
}

double pquery_path_length_distance(
    Pquery const&                pquery_a,
    Pquery const&                pquery_b,
    utils::Matrix<size_t> const& node_path_lengths
) {
    return pquery_distance(
        pquery_a,
        pquery_b,
        [&]( PqueryPlacement const& place_a, PqueryPlacement const& place_b ){
            return placement_path_length_distance( place_a, place_b, node_path_lengths );
        }
    );
}

size_t placement_path_length_distance(
    PqueryPlacement const&       place_a,
    PqueryPlacement const&       place_b,
    utils::Matrix<size_t> const& node_path_lengths
) {
    // special case
    if( place_a.edge().index() == place_b.edge().index() ) {
        return 0;
    }

    // primary primary
    size_t const pp = node_path_lengths(
        place_a.edge().primary_node().index(),
        place_b.edge().primary_node().index()
    );

    // primary secondary
    size_t const ps = node_path_lengths(
        place_a.edge().primary_node().index(),
        place_b.edge().secondary_node().index()
    );

    // secondary primary
    size_t const sp = node_path_lengths(
        place_a.edge().secondary_node().index(),
        place_b.edge().primary_node().index()
    );

    return std::min({ pp, ps, sp }) + 1;
}

// =================================================================================================
//     Pquery to Tree Element Distances
// =================================================================================================

/**
 * @brief Local helper function to avoid code duplication.
 */
template<typename DistanceFunction>
double pquery_distance( Pquery const& pquery, DistanceFunction distance_function )
{
    double sum = 0.0;

    // Calculate the weighted distance of the pquery, using the specified function.
    #pragma omp parallel for
    for( size_t pai = 0; pai < pquery.placement_size(); ++pai ) {
        auto const& placement = pquery.placement_at( pai );

        auto dist = distance_function( placement );
        dist *= placement.like_weight_ratio;

        #pragma omp atomic
        sum += dist;
    }

    return sum;
}

double pquery_distance(
    Pquery const&                pquery,
    tree::TreeNode const&        node,
    utils::Matrix<double> const& node_distances
) {
    return pquery_distance(
        pquery,
        [&]( PqueryPlacement const& placement ){
            return placement_distance( placement, node, node_distances );
        }
    );
}

double placement_distance(
    PqueryPlacement const&       placement,
    tree::TreeNode const&        node,
    utils::Matrix<double> const& node_distances
) {
    // proximal
    double const pd = placement.proximal_length
        + node_distances( node.index(), placement.edge().primary_node().index() )
    ;

    // distal
    double const dd = placement.edge().data<tree::DefaultEdgeData>().branch_length
        - placement.proximal_length
        + node_distances( node.index(), placement.edge().secondary_node().index() )
    ;

    return std::min( pd, dd );
}

// double pquery_path_length_distance(
//     Pquery const&                pquery,
//     tree::TreeNode const&        node,
//     utils::Matrix<size_t> const& node_path_lengths
// ) {
//     return pquery_distance(
//         pquery,
//         [&]( PqueryPlacement const& placement ){
//             return placement_path_length_distance( placement, node, node_path_lengths );
//         }
//     );
// }
//
// size_t placement_path_length_distance(
//     PqueryPlacement const&       placement,
//     tree::TreeNode const&        node,
//     utils::Matrix<size_t> const& node_path_lengths
// ){
//     size_t const pd = node_path_lengths( placement.edge().primary_node().index(), node.index() );
//     size_t const dd = node_path_lengths( placement.edge().secondary_node().index(), node.index() );
//     return std::min( pd, dd );
// }

double pquery_path_length_distance(
    Pquery const&                pquery,
    tree::TreeEdge const&        edge,
    utils::Matrix<size_t> const& edge_path_lengths
){
    return pquery_distance(
        pquery,
        [&]( PqueryPlacement const& placement ){
            return placement_path_length_distance( placement, edge, edge_path_lengths );
        }
    );
}

size_t placement_path_length_distance(
    PqueryPlacement const&       placement,
    tree::TreeEdge const&        edge,
    utils::Matrix<size_t> const& edge_path_lengths
){
    return edge_path_lengths( placement.edge().index(), edge.index() );
}

} // namespace placement
} // namespace genesis
