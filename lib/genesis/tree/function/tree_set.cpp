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

#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/iterator/preorder.hpp"

#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Set Functions
// =================================================================================================

Tree* find_tree ( TreeSet& tset, std::string const& name)
{
    for (auto& tree : tset) {
        if( tree.name == name ) {
            return &tree.tree;
        }
    }
    return nullptr;
}

Tree const* find_tree ( TreeSet const& tset, std::string const& name)
{
    for (auto& tree : tset) {
        if( tree.name == name ) {
            return &tree.tree;
        }
    }
    return nullptr;
}

// =================================================================================================
//     Comparators
// =================================================================================================

bool all_equal(
    TreeSet const& tset,
    std::function<bool( TreeNode const&, TreeNode const& )> node_comparator,
    std::function<bool( TreeEdge const&, TreeEdge const& )> edge_comparator
) {
    // If all pairs of two adjacent trees are equal, all of them are.
    // Thus, we do not need a complete pairwise comparision.
    // TODO the namespace thing is weird, but currently neccesary because of an ambiguous call...
    for (size_t i = 1; i < tset.size(); i++) {
        if( ! tree::equal( tset[i-1].tree, tset[i].tree, node_comparator, edge_comparator )) {
            return false;
        }
    }
    return true;
}

// bool all_equal( TreeSet const& tset )
// {
//     // If all pairs of two adjacent trees are equal, all of them are.
//     // Thus, we do not need a complete pairwise comparision.
//     for (size_t i = 1; i < tset.size(); i++) {
//         if( ! equal( tset[i-1].tree, tset[i].tree )) {
//             return false;
//         }
//     }
//     return true;
// }

bool all_identical_topology( TreeSet const& tset )
{
    // If all pairs of two adjacent trees have same the topology, all of them have.
    // Thus, we do not need a complete pairwise comparision.
    for (size_t i = 1; i < tset.size(); i++) {
        if( ! identical_topology( tset[i-1].tree, tset[i].tree )) {
            return false;
        }
    }
    return true;
}

} // namespace tree
} // namespace genesis
