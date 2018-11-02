/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech, Pierre Barbera and HITS gGmbH

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

#include "genesis/placement/function/manipulation.hpp"

#include "genesis/tree/function/manipulation.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/function/functions.hpp"

#include "genesis/placement/sample.hpp"
#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/pquery/placement.hpp"
#include "genesis/placement/function/helper.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace placement {

void make_rooted( Sample& sample, PlacementTreeEdge& target_edge )
{
    auto& tree = sample.tree();

    if( tree::is_rooted( tree )) {
        throw std::invalid_argument{ "Cannot root a PlacementTree that is already rooted." };
    }
    if( not tree::belongs_to( tree, target_edge )) {
        throw std::invalid_argument{ "The given edge does not belong to the tree." };
    }

    // Get old root
    auto& old_root = tree.root_node();

    // Make rooted: add root to edge. Make sure that the new node points
    auto& new_node = make_rooted( tree, target_edge );
    auto& new_edge = target_edge.primary_link().next().edge();
    assert( &new_node.primary_link() == &target_edge.primary_link() );
    assert( degree( new_node ) == 2 );
    (void) new_node;

    // This creates a new TreeNode, with the original `edge` being one of the two edges adjacent to the new node.
    // the original edge will be the one that is more _towards_ the original root of the tree,
    // see add_new_node( Tree&, TreeEdge&, ...)
    // Thus, as we generally want this new root to be more toward the interior of the tree,
    // we scale the branch length accordingly.

    // Rescale adjacent branch lengths of newly created root node to 0% and 100%
    auto const original_branch_length = target_edge.data<PlacementEdgeData>().branch_length;
    target_edge.data<PlacementEdgeData>().branch_length = 0.0;
    assert( new_edge.has_data() && new_edge.data<PlacementEdgeData>().branch_length == 0.0 );
    new_edge.data<PlacementEdgeData>().branch_length = original_branch_length;

    // Next, we need to identify the edges that had their direction to root changed,
    // as this is information used in the placements (`distal_length` or `proximal_length`).
    // Once we know which they are, we can iterate over all placements and adjust those numbers
    // for all placements associated with those edges.

    // Iterate over the path between old and new root.
    // path_to_root() also returns the root_link of the tree, but we can already get the
    // relevant edge from the link before. So out with it
    auto path = tree::path_to_root( old_root );
    assert( ! path.empty() && is_root( *path.back() ));
    path.pop_back();

    // Get the set of edges towards the old root.
    std::unordered_set<PlacementTreeEdge*> edges_to_adjust;
    for( auto link_ptr : path ) {
        edges_to_adjust.insert( const_cast<PlacementTreeEdge*>( &link_ptr->edge() ) );
    }

    // Look for relevant placements, adjust the distal length
    for( auto& pq : sample ) {
        for( auto& p : pq.placements() ) {
            auto place_edge_ptr = &( p.edge() );

            // If the placement points to the edge on which we rooted, change to new edge
            if ( place_edge_ptr == &target_edge ) {
                p.reset_edge( new_edge );

                // The target edge is on the path. Check this, and change the current pointer
                // to the new edge, which is not on the path. Otherwise we'd wrongly
                // flip the proximal length later...
                assert( edges_to_adjust.count( place_edge_ptr ) > 0 );
                place_edge_ptr = &( p.edge() );
                assert( edges_to_adjust.count( place_edge_ptr ) == 0 );
            }

            // The current edge can never be the target edge, because we excluded this case above.
            assert( place_edge_ptr != &target_edge );

            // If this placement belongs to one of the relevant edges, adjust the proximal_length.
            if( edges_to_adjust.count( place_edge_ptr ) > 0 ) {
                auto full_length = place_edge_ptr->data<PlacementEdgeData>().branch_length;
                p.proximal_length = full_length - p.proximal_length;
            }
        }
    }

    // Recalculate the edge nums. As the placements use pointer to their edges to get the edge nums,
    // no change is needed for the placements themselves.
    reset_edge_nums( tree );
}

} // namespace placement
} // namespace genesis
