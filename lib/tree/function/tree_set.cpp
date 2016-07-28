/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "tree/function/tree_set.hpp"

#include "tree/tree.hpp"
#include "tree/tree_set.hpp"

#include "tree/default/tree.hpp"
#include "tree/function/operators.hpp"
#include "tree/iterator/preorder.hpp"

#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Set Functions
// =================================================================================================

/**
 * @brief Get the first Tree in a TreeSet that is stored with a given name,
 * or `nullptr` if not found.
 */
Tree* find_tree ( TreeSet& tset, std::string const& name)
{
    for (auto& tree : tset) {
        if( tree.name == name ) {
            return &tree.tree;
        }
    }
    return nullptr;
}

/**
 * @brief Get the first Tree in a TreeSet that is stored with a given name,
 * or `nullptr` if not found.
 */
Tree const* find_tree ( TreeSet const& tset, std::string const& name)
{
    for (auto& tree : tset) {
        if( tree.name == name ) {
            return &tree.tree;
        }
    }
    return nullptr;
}

/**
 * @brief Return a Tree where the branch lengths are the average of the Tree%s in the TreeSet,
 * given that they all have the same topology.
 *
 * The function works only under the following conditions:
 *
 *     * All trees must have the same topology.
 *     * The TreeType must provide a data member `branch_length` for the edges.
 *
 * Otherwise, the function throws an `std::runtime_error`. It does not check for node names,
 * but the returned tree will contain the names of the first tree in the set.
 *
 * TODO this function assumes that the tree edge has a branch_length. move it to default tree.
 */
Tree average_branch_length_tree( TreeSet const& tset )
{
    using TreeType = Tree;

    if( tset.size() == 0 ) {
        return TreeType();
    }

    if( ! all_identical_topology( tset )) {
        throw std::runtime_error( "Trees in TreeSet do not have the same topology." );
    }

    // Prepare storage for average branch lengths.
    size_t num_edges = tset.at(0).tree.edge_count();
    auto avgs = std::vector<double>(num_edges, 0.0);

    // We traverse all trees (again, because all_identical_topology() already did this). This is
    // probably a bit slower than the previous version of this method which worked with less
    // traversals, but way easier to understand and debug.
    for( auto& ct : tset ) {
        // Use an index for position in the preorder traversal. This makes sure that the
        // index actually always points to the correct edges, indepently of their order in
        // different trees in the set.
        size_t idx = 0;

        // Do a preorder traversal and collect branch lengths.
        for( auto it : preorder(ct.tree) ) {
            // The first iteration points to an edge which will be covered later again.
            // Skip it to prevent double coverage.
            if (it.is_first_iteration()) {
                continue;
            }

            avgs[idx] += it.edge().data<DefaultEdgeData>().branch_length;
            ++idx;
        }
    }

    // We know that all trees have the same topology. So we take a copy of the first one
    // (thus, also copying its node names) and modify its branch lengths.
    TreeType tree = TreeType( tset.at(0).tree );

    // Do the same kind of traversal as before in order to keep the indexing order (preorder) and
    // set the branch lengths.
    size_t idx = 0;
    for( auto it : preorder(tree) ) {
        // The first iteration points to an edge which will be covered later again.
        // Skip it to prevent double coverage.
        if (it.is_first_iteration()) {
            continue;
        }

        it.edge().data<DefaultEdgeData>().branch_length = avgs[idx] / tset.size();
        ++idx;
    }

    return tree;
}

// =================================================================================================
//     Comparators
// =================================================================================================

/**
 * @brief Compare whether all Tree%s in a TreeSet are equal using a given comparator functional.
 *
 * See Tree::equal() for more information.
 */
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

/**
 * @brief Compare whether all Tree%s in a TreeSet are equal using their default comparision
 * operators for nodes and edges.
 */
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

/**
 * @brief Return true iff all Tree%s in a TreeSet have an identical topology.
 */
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
