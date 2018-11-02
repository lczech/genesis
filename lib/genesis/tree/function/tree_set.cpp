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
 * @ingroup tree
 */

#include "genesis/tree/function/tree_set.hpp"

#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree_set.hpp"

#include "genesis/tree/function/operators.hpp"

#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Set Functions
// =================================================================================================

Tree* find_tree( TreeSet& tree_set, std::string const& name )
{
    for( size_t i = 0; i < tree_set.size(); ++i ) {
        if( tree_set.name_at(i) == name ) {
            return &tree_set[i];
        }
    }
    return nullptr;
}

Tree const* find_tree( TreeSet const& tree_set, std::string const& name )
{
    for( size_t i = 0; i < tree_set.size(); ++i ) {
        if( tree_set.name_at(i) == name ) {
            return &tree_set[i];
        }
    }
    return nullptr;
}

// =================================================================================================
//     Comparators
// =================================================================================================

bool equal(
    TreeSet const& tree_set,
    std::function<bool( TreeNode const&, TreeNode const& )> node_comparator,
    std::function<bool( TreeEdge const&, TreeEdge const& )> edge_comparator
) {
    return equal( tree_set.trees(), node_comparator, edge_comparator );
}

// bool all_equal( TreeSet const& tree_set )
// {
//     // If all pairs of two adjacent trees are equal, all of them are.
//     // Thus, we do not need a complete pairwise comparision.
//     for (size_t i = 1; i < tree_set.size(); i++) {
//         if( ! equal( tree_set[i-1].tree, tree_set[i].tree )) {
//             return false;
//         }
//     }
//     return true;
// }

bool identical_topology( TreeSet const& tree_set )
{
    return identical_topology( tree_set.trees() );
}

} // namespace tree
} // namespace genesis
