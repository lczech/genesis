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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/function/tree.hpp"

#include "genesis/placement/function/helper.hpp"
#include "genesis/tree/default/operators.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/manipulation.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/tree.hpp"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Helper Functions
// =================================================================================================

tree::Tree labelled_tree(
    Sample const&      sample,
    bool const         fully_resolve,
    std::string const& name_prefix
) {
    // Get a copy of the original tree that contains only default data.
    return labelled_tree(
        sample,
        tree::convert_to_default_tree( sample.tree() ),
        fully_resolve,
        name_prefix
    );
}

tree::Tree labelled_tree(
    Sample const&      sample,
    tree::Tree const&  tree,
    bool               fully_resolve,
    std::string const& name_prefix
) {
    // -----------------------------------------------------
    //     Preparation
    // -----------------------------------------------------

    // Get a copy of the original tree that we can add edges to.
    auto result = tree;

    // Check whether the tree is compatible with the sample tree.
    // This is a bit wasteful for cases when this function is called directly from its sample
    // version above (then, we can be sure that it is compatible), but necessary otherwise.
    if( ! tree::identical_topology( result, sample.tree() )) {
        throw std::runtime_error(
            "Tree provided for labelled_tree() is not compatible "
            "with the Tree of the provided Sample."
        );
    }

    // Get a list of the edge pointers so that we can iterate and add to the tree at the same time.
    std::vector< tree::TreeEdge* > edge_list;
    edge_list.reserve( result.edge_count() );
    for( auto const& edge : result.edges() ) {
        edge_list.push_back( edge.get() );
    }
    assert( edge_list.size() == sample.tree().edge_count() );

    // Helper struct that stores a placement along with its containing pquery.
    struct PlacementPair
    {
        PlacementPair( Pquery const* pq, PqueryPlacement const* pp )
            : pquery(    pq )
            , placement( pp )
        {}

        Pquery const*          pquery;
        PqueryPlacement const* placement;
    };

    // For each edge, get a list of placements - but only the most probable ones per Pquery.
    // This is similar to placements_per_edge(), but we also need a pointer to the pquery, in
    // order to get its name(s).
    auto placement_pairs_per_edge = []( Sample const& sample ){
        std::vector< std::vector< PlacementPair >> place_map;
        place_map.resize( sample.tree().edge_count() );
        for( auto const& pqry : sample.pqueries() ) {

            // Find the most probable placement.
            auto max_it = std::max_element(
                pqry.placements().begin(),
                pqry.placements().end(),
                []( PqueryPlacement const& lhs, PqueryPlacement const& rhs ){
                    return lhs.like_weight_ratio < rhs.like_weight_ratio;
                }
            );

            // If there is one (i.e., the pquery has at least one placement),
            // add it to the list for its edge.
            if( max_it != pqry.placements().end() ) {
                place_map[ max_it->edge().index() ].emplace_back( &pqry, &*max_it );
            }

            // Alternative handcrafted version...
            // PqueryPlacement const* max_p = nullptr;
            // double max_v = std::numeric_limits<double>::lowest();
            // for( auto const& place : pqry.placements() ) {
            //     if( place.like_weight_ratio > max_v ) {
            //         max_v = place.like_weight_ratio;
            //         max_p = &place;
            //     }
            // }
            // if( max_p ) {
            //     place_map[ max_p->edge().index() ].emplace_back( &pqry, max_p );
            // }
        }
        return place_map;
    };
    auto place_map = placement_pairs_per_edge( sample );
    assert( place_map.size() == edge_list.size() );

    // Sort the placements per edge, so that we can attach them to the tree in order.
    // For a fully resolved tree, we want to sort by proximal length, as this is the order in which
    // the placements will form new nodes on the tree.
    // For a multifurcating tree, we sort by pendant length, so that the short ones come first.
    auto sort_placements = fully_resolve
        ? []( PlacementPair const& lhs, PlacementPair const& rhs ){
              return lhs.placement->proximal_length < rhs.placement->proximal_length;
          }
        : []( PlacementPair const& lhs, PlacementPair const& rhs ){
              return lhs.placement->pendant_length < rhs.placement->pendant_length;
          }
    ;
    for( auto& edge_l : place_map ) {
        std::sort( edge_l.begin(), edge_l.end(), sort_placements );
    }

    // -----------------------------------------------------
    //     Single Placement on the Edge
    // -----------------------------------------------------

    // Helper function that adds one placement, if it is the only one on its edge.
    auto add_lonely_placement = [ &name_prefix ](
        tree::Tree&          tree,
        tree::TreeEdge&      edge,
        PlacementPair const& placement_pair
    ) {
        // Add the new edges to the tree and get all necessary edges.
        auto& pendant_edge  = tree::add_new_node( tree, edge );
        auto& proximal_edge = pendant_edge.primary_link().next().edge();
        auto& distal_edge   = pendant_edge.primary_link().next().next().edge();

        // The primary node is new, so it should be bifurcating and a leaf.
        assert( pendant_edge.primary_node().rank() == 2 );
        assert( pendant_edge.secondary_node().is_leaf() );

        // Some shorthands to the edge data.
        auto& pend_data = pendant_edge.data<tree::DefaultEdgeData>();
        auto& prox_data = proximal_edge.data<tree::DefaultEdgeData>();
        auto& dist_data  = distal_edge.data<tree::DefaultEdgeData>();

        // The pendant and distal edges are new, so they should have default branch lengths.
        assert( pend_data.branch_length == 0.0 );
        assert( dist_data.branch_length == 0.0 );

        // Set all three branch lengths. Make sure they are not negative.
        double const distal_len = prox_data.branch_length - placement_pair.placement->proximal_length;
        pend_data.branch_length = placement_pair.placement->pendant_length;
        prox_data.branch_length = placement_pair.placement->proximal_length;
        dist_data.branch_length = std::max( 0.0, distal_len );

        // Set the leaf node name, if there is one. Also, assert that ther is at most one -
        // otherwise, this is not a lonely placement any more!
        assert( placement_pair.pquery->name_size() <= 1 );
        if( placement_pair.pquery->name_size() == 1 ) {
            auto& pendant_node_data = pendant_edge.secondary_node().data<tree::DefaultNodeData>();
            pendant_node_data.name = name_prefix + placement_pair.pquery->name_at(0).name;
        }
    };

    // -----------------------------------------------------
    //     Multiple Placements, Fully Resolved
    // -----------------------------------------------------

    // Helper function that adds all placements for one edge, fully resolved.
    auto process_edge_fully_resolved = [ &name_prefix ] (
        tree::Tree&                       tree,
        tree::TreeEdge&                   edge,
        std::vector<PlacementPair> const& placement_pairs
    ) {
        // In each step, we add a new node, along the original edge, splitting it more and more.
        // The branch lengths of those fragments are the differences between the proximal lengths
        // of the placements.
        // To handle this, store the proximal length that we already used along the branch.
        double used_length = 0.0;

        // Also, we need the original branch length, because it will be lost in the process,
        // but later needed.
        auto const orig_length = edge.data<tree::DefaultEdgeData>().branch_length;

        // Store an edge pointer at which we insert the next placement edge.
        auto insertion_edge = &edge;

        // Process all placements.
        for( auto const& placement_pair : placement_pairs ) {
            // Each name gets its own branch.
            for( auto const& pquery_name : placement_pair.pquery->names() ) {

                // Create the new edges.
                auto& pendant_edge  = tree::add_new_node( tree, *insertion_edge );
                auto& proximal_edge = pendant_edge.primary_link().next().edge();
                auto& distal_edge   = pendant_edge.primary_link().next().next().edge();

                // The primary node is new, so it should be bifurcating and a leaf.
                assert( pendant_edge.primary_node().rank() == 2 );
                assert( pendant_edge.secondary_node().is_leaf() );

                // Some shorthands to the edge data. We dont need dist data; its branch length
                // will be set in a later step. Either it becomes the prox edge in the next
                // iteration, or it is the final edge after the loop.
                auto& pend_data = pendant_edge.data<tree::DefaultEdgeData>();
                auto& prox_data = proximal_edge.data<tree::DefaultEdgeData>();

                // The pendant and distal edges are new, so they should have default branch lengths.
                assert( pend_data.branch_length == 0.0 );
                assert( distal_edge.data<tree::DefaultEdgeData>().branch_length == 0.0 );

                // We sorted the placements by prox len. Thus, this should hold.
                assert( placement_pair.placement->proximal_length >= used_length );

                // Set branch properties.
                pend_data.branch_length = placement_pair.placement->pendant_length;
                prox_data.branch_length = placement_pair.placement->proximal_length - used_length;

                // Set the leaf name.
                auto& node_data = pendant_edge.secondary_node().data<tree::DefaultNodeData>();
                node_data.name = name_prefix + pquery_name.name;

                // Set new values for the keeping-track variables, for the next iteration.
                used_length    = placement_pair.placement->proximal_length;
                insertion_edge = &distal_edge;
            }
        }

        // Now, the insertion edge is the last remaining edge part towards the original
        // end of the edge. Its branch length is not yet set. Assert this, then set it to what
        // is left of it.
        // If the remaining length is however smaller than the original one, that means we have
        // placements on this edge with a proximal length greater than the original edge length.
        // This is a bit weird in terms of placement, but might happen due to BLO in the placement.
        // In this case, we want to avoid negative length, so simply set it to zero.
        // That means that in total the edge now grew due to its placements. So the tree is not
        // optimized any more - but for the purposes of this function, this should be okay.
        auto& end_edge_data = insertion_edge->data<tree::DefaultEdgeData>();
        assert( end_edge_data.branch_length == 0.0 );
        if( used_length < orig_length ) {
            end_edge_data.branch_length = orig_length - used_length;
        }
    };

    // -----------------------------------------------------
    //     Multiple Placements, Multifurcating
    // -----------------------------------------------------

    // Helper function that adds all placements for one edge, multifurcating,
    // if there is more than one placement for the edge.
    auto process_edge_multifurcating = [ &name_prefix ] (
        tree::Tree&                       tree,
        tree::TreeEdge&                   edge,
        std::vector<PlacementPair> const& placement_pairs
    ) {
        // Add a new leaf node to the tree, attached to the middle of the given edge (this spltis
        // the edge in half and adds one more node there). This will be the base to which we then
        // attach all further nodes, multifurcating.
        auto& base_edge = tree::add_new_node( tree, edge );
        auto& pri_edge  = base_edge.primary_link().next().edge();
        auto& sec_edge  = base_edge.primary_link().next().next().edge();

        // The base node is new, so it should be bifurcating and a leaf
        assert( base_edge.primary_node().rank() == 2 );
        assert( base_edge.secondary_node().is_leaf() );

        // Some shorthands to the edge data.
        auto& base_data = base_edge.data<tree::DefaultEdgeData>();
        auto& pri_data  = pri_edge.data<tree::DefaultEdgeData>();
        auto& sec_data  = sec_edge.data<tree::DefaultEdgeData>();

        // The pendant and distal edges are new, so they should have default branch lengths.
        assert( base_data.branch_length == 0.0 );
        assert( sec_data.branch_length  == 0.0 );

        // Calculate the average attachment position (proximal length) of all placements.
        auto const avg_prox_len = std::accumulate(
            placement_pairs.begin(),
            placement_pairs.end(),
            0.0,
            []( double in, PlacementPair const& cur ){
                return in + cur.placement->proximal_length;
            }
        ) / static_cast<double>( placement_pairs.size() );

        // Set the branch lengths of the two splitted edges (the ones that were one edge before)
        // so that they each take their share of the original branch length, divided using
        // the average proximal length of the placements. This way, the multifurcation that contains
        // the placements will sit at the sweet spot.
        // Set the second one first, as it uses the former length of the first one (i.e., the
        // original branch length).
        // Also, make sure that each new length is non-negative. This might happen if
        // the placements are somehow weirdly placed.
        sec_data.branch_length = std::max( 0.0, pri_data.branch_length - avg_prox_len );
        pri_data.branch_length = std::max( 0.0, avg_prox_len );

        // This function should only be called with at least one placement. Assert this.
        // Then, we can safely get the placement with the smallest pendant length.
        assert( placement_pairs.size() > 0 );
        auto const min_pen_len = std::max( 0.0, std::min_element(
            placement_pairs.begin(),
            placement_pairs.end(),
            []( PlacementPair const& lhs, PlacementPair const& rhs ){
                return lhs.placement->pendant_length < rhs.placement->pendant_length;
            }
        )->placement->pendant_length );

        // TODO we sorted them first, so taking the first placement should also work:
        // (if so, we can drop the above search)
        assert( min_pen_len == placement_pairs[0].placement->pendant_length );

        // Use the min bl for the multifurcation base edge.
        base_data.branch_length = min_pen_len;

        // Now add all placements as edges to the base node (secondary node of the base edge).
        for( auto const& placement_pair : placement_pairs ) {
            // Each name gets its own branch.
            for( auto const& pquery_name : placement_pair.pquery->names() ) {

                // Make a new leaf node for this placement.
                auto& p_edge = tree::add_new_node( tree, base_edge.secondary_node() );

                // Set the pendant branch length. It is subtracted by
                // the min pen length, as this is already incorporated into the base's bl.
                auto& p_data = p_edge.data<tree::DefaultEdgeData>();
                assert( placement_pair.placement->pendant_length >= min_pen_len );
                p_data.branch_length = placement_pair.placement->pendant_length - min_pen_len;

                // Set the leaf node name.
                auto& p_node_data = p_edge.secondary_node().data<tree::DefaultNodeData>();
                p_node_data.name = name_prefix + pquery_name.name;
            }
        }
    };

    // -----------------------------------------------------
    //     Main Loop over Edges
    // -----------------------------------------------------

    // Process each edge.
    for( size_t edge_i = 0; edge_i < edge_list.size(); ++edge_i ) {
        // Some safety. There needs to be a valid pointer to an edge.
        assert( edge_list[ edge_i ] );

        // Nothing to do if there are no placements for this edge.
        if( place_map[ edge_i ].size() == 0 ) {
            continue;
        }

        // If there is only one placement with at most one name,
        // both algorithms behave the same, so shortcut this.
        if( place_map[ edge_i ].size() == 1 && place_map[ edge_i ][0].pquery->name_size() <= 1 ) {
            assert( place_map[ edge_i ][0].pquery && place_map[ edge_i ][0].placement );
            add_lonely_placement( result, *edge_list[ edge_i ], place_map[ edge_i ][0] );
            continue;
        }

        // Select which algorithm to use for more than one placement at the edge.
        if( fully_resolve ) {
            process_edge_fully_resolved(
                result, *edge_list[ edge_i ], place_map[ edge_i ]
            );
        } else {
            process_edge_multifurcating(
                result, *edge_list[ edge_i ], place_map[ edge_i ]
            );
        }
    }

    return result;
}

} // namespace placement
} // namespace genesis
