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
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/core/algorithm.hpp"
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

std::vector<Bipartition> bipartition_set( Tree const& tree )
{
    // Result
    const size_t num_leaves = leaf_node_count( tree );
    auto bipartitions = std::vector<Bipartition>( tree.edge_count() );

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
            assert( leaf_idx != std::numeric_limits<std::size_t>::max() );
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

    // Make sure all bips are filled.
    assert(
        [ &bipartitions ](){
            for( auto const& bip : bipartitions ) {
                if( bip.empty() ) {
                    return false;
                }
            }
            return true;
        }()
    );

    return bipartitions;
}

std::vector<size_t> node_to_leaf_map( Tree const& tree )
{
    // Currently, we fix this so that leaves are always ordered.
    // This is a bit slower, but better when working with multiple trees.
    // In the future, this whole interface could be refined...
    bool const sort_leaves = true;

    // Make an lookup of consecutive leaf nodes.
    std::vector<size_t> nodes_to_leafs;
    nodes_to_leafs.resize( tree.node_count() );

    // Find node string order if needed.
    std::vector<size_t> name_order;
    if( sort_leaves ) {

        // Make a sorted list of leave node names.
        std::vector<std::string> node_names;
        for( auto const& node_it : tree.nodes() ) {
            if( node_it->is_leaf() ) {
                node_names.push_back( node_it->data<DefaultNodeData>().name );
            }
        }

        // Get an order mapping list, that gives us the n-th index according to order.
        // That is, it maps order to indices.
        auto const ordered = utils::sort_indices( node_names.begin(), node_names.end() );

        // Check for duplicate names.
        for( size_t i = 1; i < ordered.size(); ++i ) {
            if( node_names[ ordered[ i ]] == node_names[ ordered[ i - 1 ]] ) {
                throw std::runtime_error(
                    "Cannot build bipartitions for Tree that has duplicate node names."
                );
            }
        }

        // Reverse the mapping, so that we get a lookup from index to order.
        name_order.resize( ordered.size() );
        for( size_t i = 0; i < ordered.size(); ++i ) {
            name_order[ ordered[i] ] = i;
        }
    }

    // Assign indices to each node.
    size_t leaf_idx = 0;
    for( auto const& node_it : tree.nodes() ) {
        if( node_it->is_leaf() ) {
            if( sort_leaves ) {

                // Use the index to order map to get the ordered leaf index.
                assert( leaf_idx < name_order.size() );
                nodes_to_leafs[ node_it->index() ] = name_order[ leaf_idx ];
            } else {

                // Unsorted case: just count upwards.
                nodes_to_leafs[ node_it->index() ] = leaf_idx;
            }
            ++leaf_idx;
        } else {
            nodes_to_leafs[ node_it->index() ] = std::numeric_limits<std::size_t>::max();
        }
    }

    return nodes_to_leafs;
}

utils::Bitvector leaf_node_bitvector( Tree const& tree, std::vector<TreeNode const*> leaf_nodes )
{
    auto const node_to_leafs = node_to_leaf_map( tree );
    utils::Bitvector result( leaf_node_count( tree ));
    for( auto n : leaf_nodes ) {
        auto const leaf_idx = node_to_leafs[ n->index() ];
        if( leaf_idx == std::numeric_limits<std::size_t>::max() ) {
            throw std::runtime_error(
                "Node at index " + utils::to_string( n->index() ) + " is not a leaf."
            );
        }
        result.set( leaf_idx );
    }
    return result;
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

std::vector<size_t> find_monophyletic_subtree_edges(
    Tree const& tree,
    std::vector<Bipartition> const& bips,
    std::vector<TreeNode const*> nodes
) {
    // Result. We use a bitvec of the edges that we want, to save space.
    auto result_edges = utils::Bitvector( tree.edge_count() );

    // Helper funciton to set result edges of a bipartition.
    auto set_result_edges = [ &result_edges ]( Bipartition const& bip ){

        // Add all subtree edges via custom traversal
        using Preorder = IteratorPreorder< TreeLink const, TreeNode const, TreeEdge const >;
        for(
            auto it = Preorder( bip.link().next() );
            it != Preorder() && &it.link() != &bip.link().outer();
            ++it
        ) {
            if (it.is_first_iteration()) {
                continue;
            }
            result_edges.set( it.edge().index() );
        }

        // Also add the edge of the split itself. This is necessary for leaves,
        // but also we want to consider inner branches to be part of the clade.
        result_edges.set( bip.link().edge().index() );
    };

    // Get the bitvec that represets the leaf nodes we are looking for.
    auto const leaves = leaf_node_bitvector( tree, nodes );

    // For each bip, check if one of its splits contains only nodes we are looking for.
    // If so, add all edges of that split to the result.
    for( auto const& bip : bips ) {
        if( bip.empty() ) {
            continue;
        }

        // If all tips of the bip are in our node list, we found a monophyletic clade.
        if( ( bip.leaf_nodes() & leaves ) == bip.leaf_nodes() ) {
            set_result_edges( bip );
        }

        // Same for inverted case.
        auto inverted = bip;
        inverted.invert();
        if( ( inverted.leaf_nodes() & leaves ) == inverted.leaf_nodes() ) {
            set_result_edges( inverted );
        }
    }

    // Turn bitvec into edges by adding all indices where the bitvec is set.
    std::vector<size_t> edges;
    for( size_t i = 0; i < result_edges.size(); ++i ) {
        if( result_edges.get( i ) ) {
            edges.push_back( i );
        }
    }
    return edges;
}

Bipartition find_smallest_subtree(
    Tree const& tree,
    std::vector<Bipartition> const& bipartitions,
    std::vector<TreeNode const*> nodes
) {
    // Get the bitvector to compare against
    auto const comp = leaf_node_bitvector( tree, nodes );

    // Store best results.
    Bipartition best_bip;
    size_t      min_count = 0;

    // Loop over all bipartitions and compare their bitvectors to the given one, to find one that
    // is a superset. Try both ways (normal and inverted) for each bipartition.
    for( Bipartition const& bip : bipartitions ) {
        if( bip.empty() ) {
            continue;
        }

        auto const inverted = ~(bip.leaf_nodes());
        if( utils::is_subset( comp, bip.leaf_nodes() )) {
            if( min_count == 0 || bip.leaf_nodes().count() < min_count ) {
                best_bip   = bip;
                min_count = bip.leaf_nodes().count();
            }
        }
        if( utils::is_subset( comp, inverted ) ) {
            if (min_count == 0 || inverted.count() < min_count)  {
                best_bip   = bip;
                best_bip.invert();
                min_count = bip.leaf_nodes().count();
            }
        }
    }

    return best_bip;
}

std::vector<size_t> get_clade_edges( Tree const& tree, std::vector< tree::TreeNode const* > const& nodes )
{
    // Find the edges that are part of the subtree of this clade.
    // This part is a bit messy and might be cleaned up in the future.
    auto const bipartitions = bipartition_set( tree );
    auto const smallest     = find_smallest_subtree( tree, bipartitions, nodes );
    auto const subedges     = get_subtree_edges( smallest.link() );
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
