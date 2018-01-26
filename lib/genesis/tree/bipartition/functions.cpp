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
 * @brief Implementation of BipartitionSet class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/bipartition/functions.hpp"

#include "genesis/tree/bipartition/bipartition.hpp"
#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Bipartition Functions
// =================================================================================================

std::vector<size_t> node_to_leaf_map( Tree const& tree )
{
    // Make an lookup of consecutive leaf nodes.
    std::vector<size_t> node_to_leaf_map;
    node_to_leaf_map.resize( tree.node_count() );

    size_t leaf_idx = 0;
    for( auto const& node_it : tree.nodes() ) {
        if( node_it->is_leaf() ) {
            node_to_leaf_map[ node_it->index() ] = leaf_idx;
            ++leaf_idx;
        } else {
            node_to_leaf_map[ node_it->index() ] = std::numeric_limits<std::size_t>::max();
        }
    }

    return node_to_leaf_map;
}

std::vector<Bipartition> bipartition_set( Tree const& tree )
{
    // Result
    const size_t num_leaves = leaf_node_count( tree );
    auto bipartitions = std::vector<Bipartition>( tree.node_count() );

    auto const node_to_leafs = node_to_leaf_map( tree );

    // Fill bitvectors
    for( auto it : postorder(tree) ) {
        if (it.is_last_iteration()) {
            continue;
        }

        auto bp = Bipartition( it.link(), utils::Bitvector( num_leaves ));

        // If the iterator is at a leaf, just set one bit in the bitvector.
        if( it.node().is_leaf() ) {
            auto const leaf_idx = node_to_leafs[ it.node().index() ];
            assert( leaf_idx > std::numeric_limits<std::size_t>::max() );
            bp.bitvector().set(leaf_idx);

        // For inner iterator positions, consider the whole subtree below it.
        } else {
            auto l = &it.link().next();
            while( l != &it.link() ) {
                assert( ! bipartitions[ l->edge().index() ].empty() );
                bp.bitvector() |= bipartitions[ l->edge().index() ].leaf_nodes();
                l = &l->next();
            }
        }

        // Store at the current edge
        assert( bipartitions[ it.edge().index() ].empty() );
        bipartitions[ it.edge().index() ] = bp;
    }

    return bipartitions;
}

std::vector<size_t> get_subtree_edges( TreeLink const& subtree )
{
    std::vector<size_t> ret;

    // We don't want to use the standard iterator wrapper function here, as we are going
    // to end the iteration after the end of the subtree, instead of iterating the whole tree.
    // So we need to use the iterator class directly.
    using Preorder = IteratorPreorder< TreeLink const, TreeNode const, TreeEdge const >;

    for(
        auto it = Preorder( subtree.next() );
        it != Preorder() && &it.link() != &subtree.outer();
        ++it
    ) {
        if (it.is_first_iteration()) {
            continue;
        }
        ret.push_back( it.edge().index() );
    }

    return ret;
}

Bipartition find_smallest_subtree( Tree const& tree, std::vector<Bipartition> const& bips, std::vector<TreeNode const*> nodes )
{
    auto const node_to_leafs = node_to_leaf_map( tree );
    utils::Bitvector comp( leaf_node_count( tree ));

    // make bitvector containing all wanted nodes.
    for( auto n : nodes ) {
        auto const leaf_idx = node_to_leafs[n->index()];
        if( leaf_idx == std::numeric_limits<std::size_t>::max() ) {
            throw std::runtime_error(
                "Node at index " + utils::to_string( n->index() ) + " is not a leaf."
            );
        }
        comp.set(leaf_idx);
    }

    Bipartition best_bp;
    size_t      min_count = 0;

    // loop over all bipartitions and compare their bitvectors to the given one, to find one that
    // is a superset. try both ways (normal and inverted) for each bipartition.
    for( Bipartition const& bp : bips ) {
        if( bp.empty() ) {
            continue;
        }

        if( utils::is_subset( comp, bp.leaf_nodes() )) {
            if( min_count == 0 || bp.leaf_nodes().count() < min_count ) {
                best_bp   = bp;
                min_count = bp.leaf_nodes().count();
            }
        }
        if( utils::is_subset( comp, ~(bp.leaf_nodes())) ) {
            if (min_count == 0 || (~bp.leaf_nodes()).count() < min_count)  {
                best_bp   = bp;
                best_bp.invert();
                min_count = bp.leaf_nodes().count();
            }
        }
    }

    return best_bp;
}

std::vector<size_t> get_clade_edges( Tree const& tree, std::vector< tree::TreeNode const* > const& nodes )
{
    // Find the edges that are part of the subtree of this clade.
    // This part is a bit messy and might be cleaned up in the future.
    auto bipartitions = bipartition_set( tree );
    auto smallest     = find_smallest_subtree( tree, bipartitions, nodes );
    auto subedges     = get_subtree_edges( smallest.link() );
    return subedges;
}

std::vector<size_t> get_clade_edges( Tree const& tree, std::vector< std::string > const& node_names )
{
    // Find the nodes that belong to the taxa of this clade.
    std::vector< tree::TreeNode const* > node_list;
    for( auto const& taxon : node_names ) {
        auto node = find_node( tree, taxon );
        if( node == nullptr ) {
            throw std::runtime_error( "Cannot find node " + taxon + " in tree." );
        }
        node_list.push_back( node );
    }
    return get_clade_edges( tree, node_list );
}

} // namespace tree
} // namespace genesis
