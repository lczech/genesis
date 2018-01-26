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

#include "genesis/utils/math/bitvector.hpp"
#include "genesis/tree/bipartition/bipartition.hpp"
#include "genesis/tree/bipartition/bipartition_set.hpp"
#include "genesis/tree/default/functions.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Bipartition Functions
// =================================================================================================

std::unordered_set<size_t> get_clade_edges( Tree const& tree, std::vector< tree::TreeNode const* > const& nodes )
{
    // Find the edges that are part of the subtree of this clade.
    // This part is a bit messy and might be cleaned up in the future.
    auto bipartitions = tree::BipartitionSet( tree );
    auto smallest     = bipartitions.find_smallest_subtree( nodes );
    auto subedges     = bipartitions.get_subtree_edges( smallest->link() );
    return subedges;
}

std::unordered_set<size_t> get_clade_edges( Tree const& tree, std::vector< std::string > const& node_names )
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
