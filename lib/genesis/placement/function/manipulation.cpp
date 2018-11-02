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

#include "genesis/utils/core/algorithm.hpp"

#include <cassert>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace genesis {
namespace placement {

void root(Sample& sample, PlacementTreeEdge& edge)
{
    auto& tree = sample.tree();

    if (tree::is_rooted( tree )) {
        throw std::invalid_argument{"This function is only valid for rooting unrooted trees."};
    }

    if (not tree::belongs_to( tree, edge )) {
        throw std::invalid_argument{"The given edge was not found in the tree."};
    }

    // get node_id of old root
    auto& old_root = tree.root_node();
    // remember old branch length
    auto original_branch_length = edge.data<PlacementEdgeData>().branch_length;

    // add root to edge
    add_root_node( tree, edge );
    auto& new_edge = edge.primary_link().next().edge();

    /* this creates a new TreeNode, with the original `edge` being one of the two edges adjacent to the new node.
     * the original edge will be the one that is more _towards_ the original root of the tree
     * (see http://doc.genesis-lib.org/add_new_node_edge.png)
     * Thus, as we generally want this new root to be more toward the interior of the tree,
     * we scale the branch length accordingly.
    */
    // rescale adjacent branch lengths of newly created root node to 0% and 100%
    edge.data<PlacementEdgeData>().branch_length = 0.0;
    new_edge.data<PlacementEdgeData>().branch_length = original_branch_length;

    /* Next, we need to identify the edges that had their direction to root changed
     * as this is information used in the placements (`distal_length` or `proximal_length`).
     * Once we know which they are, we can iterate over all placements and adjust those numbers
     * for all placements associated with those edges.
     */
    // iterate over the path between old and new root
    auto path = tree::path_to_root( old_root );
    std::vector<PlacementTreeEdge*> edges_to_adjust;
    // `path_to_root` also returns the root_link of the tree, but we can already get the
    // relevant edge from the link before. So out with it
    path.pop_back();
    // get the set of edges
    for ( auto link_ptr : path ) {
        edges_to_adjust.push_back( const_cast<PlacementTreeEdge*>( &link_ptr->edge() ) );
    }

    // look for relevant placements, adjust the distal length
    for ( auto& pq : sample ) {
        for ( auto& p : pq.placements() ) {
            auto place_edge_ptr = &( p.edge() );

            // if the placement points to the edge on which we rooted, change to new edge
            if ( place_edge_ptr == &edge ) {
                p.reset_edge( new_edge );
                place_edge_ptr = &( p.edge() );
            }

            // if this placement belongs to one of the relevant edges...
            if ( utils::contains( edges_to_adjust, place_edge_ptr ) ) {
                // adjust the `proximal_length`
                auto full_length = place_edge_ptr->data<PlacementEdgeData>().branch_length;
                p.proximal_length = full_length - p.proximal_length;
            }

        }
    }

    // recalculate the edge ids (this changes the pointers in the placements)
    reset_edge_nums( tree );
}

} // namespace placement
} // namespace genesis
