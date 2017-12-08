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
 * @ingroup tree
 */

#include "genesis/tree/function/lca_lookup.hpp"

#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree/node.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/iterator/eulertour.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace genesis {
namespace tree {

// =================================================================================================
//     Construction and Rule of Five
// =================================================================================================

LcaLookup::LcaLookup( Tree const& tree )
    : tree_( tree )
{
    init_( tree );
}

// =================================================================================================
//     Loopup
// =================================================================================================

size_t LcaLookup::operator()( size_t node_index_a, size_t node_index_b, size_t root_index ) const
{
    return lookup_( node_index_a, node_index_b, root_index );
}

TreeNode const& LcaLookup::operator()( TreeNode const& node_a, TreeNode const& node_b, TreeNode const& root_node ) const
{
    auto const idx = lookup_( node_a.index(), node_b.index(), root_node.index() );
    return tree_.node_at( idx );
}

size_t LcaLookup::operator()( size_t node_index_a, size_t node_index_b ) const
{
    return lookup_( node_index_a, node_index_b, root_idx_ );
}

TreeNode const& LcaLookup::operator()( TreeNode const& node_a, TreeNode const& node_b ) const
{
    auto const idx = lookup_( node_a.index(), node_b.index(), root_idx_ );
    return tree_.node_at( idx );
}

// =================================================================================================
//     Internal Helper Functions
// =================================================================================================

void LcaLookup::init_( Tree const& tree )
{
    // Get distances from each node to the root, in number of edges.
    auto const dists_to_root = node_path_length_vector( tree );

    // Store root, so that tree can be re-rooted outside of this class.
    root_idx_ = tree.root_node().index();

    // Init vectors.
    std::vector<int> eulertour_levels;
    eulertour_first_occurrence_.resize( tree.node_count() );
	std::fill(
        eulertour_first_occurrence_.begin(),
        eulertour_first_occurrence_.end(),
        std::numeric_limits<size_t>::max()
    );

    // Fill data structures.
	for( auto it : eulertour( tree )) {
        auto const node_idx = it.node().index();

		eulertour_order_.push_back( node_idx );
		eulertour_levels.push_back( dists_to_root[ node_idx ]);
		if( eulertour_first_occurrence_[ node_idx ] == std::numeric_limits<size_t>::max() ) {
			eulertour_first_occurrence_[ node_idx ] = eulertour_order_.size() - 1;
		}
	}
	eulertour_rmq_ = utils::RangeMinimumQuery( eulertour_levels );
}

size_t LcaLookup::eulertour_query_( size_t i, size_t j ) const
{
    if( i <= j ) {
        return eulertour_rmq_.query( i, j );
    } else {
        return eulertour_rmq_.query( j, i );
    }
}

size_t LcaLookup::lookup_( size_t node_index_a, size_t node_index_b, size_t root_index ) const
{
    size_t u_euler_idx = eulertour_first_occurrence_[ node_index_a ];
	size_t v_euler_idx = eulertour_first_occurrence_[ node_index_b ];
	size_t r_euler_idx = eulertour_first_occurrence_[ root_index   ];

	if( root_index == root_idx_ ) {
		return eulertour_order_[ eulertour_query_( u_euler_idx, v_euler_idx )];

	} else {

        // Use the "odd man out",
        // see http://stackoverflow.com/questions/25371865/find-multiple-lcas-in-unrooted-tree
		size_t candidate_a = eulertour_order_[ eulertour_query_( u_euler_idx, v_euler_idx )];
		size_t candidate_b = eulertour_order_[ eulertour_query_( u_euler_idx, r_euler_idx )];
		size_t candidate_c = eulertour_order_[ eulertour_query_( v_euler_idx, r_euler_idx )];

		if( candidate_a == candidate_b ) {
			return candidate_c;
		} else if( candidate_a == candidate_c ) {
			return candidate_b;
		} else {
            assert( candidate_b == candidate_c );
			return candidate_a;
		}
	}
}

} // namespace tree
} // namespace genesis
