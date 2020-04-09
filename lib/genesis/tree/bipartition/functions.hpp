#ifndef GENESIS_TREE_BIPARTITION_FUNCTIONS_H_
#define GENESIS_TREE_BIPARTITION_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/bipartition/bipartition.hpp"
#include "genesis/utils/math/bitvector.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Bipartition Helper Functions
// =================================================================================================

/**
 * @brief
 */
std::vector<Bipartition> bipartition_set( Tree const& tree );

std::vector<size_t> node_to_leaf_map( Tree const& tree );

/**
 * @brief Return a Bitvector that has as many entries as the @p tree has leaf nodes,
 * and is `true` where the given @p leaf_nodes are.
 */
utils::Bitvector leaf_node_bitvector( Tree const& tree, std::vector<TreeNode const*> leaf_nodes );

std::vector<size_t> get_subtree_edges( TreeLink const& subtree );

/**
 * @brief Find the smallest subtree (measured in number of nodes) that contains all given nodes.
 *
 * A subtree is defined by one of the two parts of a tree that are splitted by one edge. Thus,
 * conceptually, this function tries all subtrees by leaving out each edge once.
 * It then returns the smalles subtree that contains all of the given nodes.
 *
 * The subtree might contain additional nodes that are not in the given set.
 * If no fitting subtree exists, the function returns an empty Bipartition.
 */
Bipartition find_smallest_subtree(
    Tree const& tree,
    std::vector<Bipartition> const& bipartitions,
    std::vector<TreeNode const*> const& nodes
);

// =================================================================================================
//     Monophyletic Subtree Functions
// =================================================================================================

/**
 * @brief Find clades of the tree that are monophyletic with respect to the given list of nodes,
 * that is, clades that only contain nodes from that list. Return all edge indices of those clades.
 *
 * The function takes a set of leaf nodes, and selects all branches of the tree that belong to
 * monophyletic clades containing only leaf nodes from the given set:
 *
 * ![Three different sets of nodes and the resulting clades.](tree/monophyletic_subtrees.png)
 *
 * In the example, the function was called three times with three different sets of nodes.
 * In each case, all branches are selected/colorized that are monophyletic with respect to the
 * nodes.
 * In other words, the function conceptually iterates all edges of the tree.
 * If one side of the split induced by an edge only contains leaf nodes from the given set,
 * the whole clade is monophyletic with respect to that set, and hence added to the resulting
 * list of edges.
 *
 * If @p include_splitting_edge is `true` (default), the edges that separate each clade from the
 * rest of the tree are also included. This is particularly important for edges leading to a
 * leaf/tip of the tree: If set to `false`, those edges are not included, meaning that
 * the respective node does not contribute to the result at all.
 *
 * In order to solve/refine this, that is to *not* include the splitting edge of larger clades,
 * but still include an edge that leads to a single leaf node (if this node is not part of any
 * larger clade), the additional parameter @p include_leaf_edges can be used.
 * It also defaults to `true`, meaning that those edges are included by default.
 */
std::vector<size_t> find_monophyletic_subtree_edges(
    Tree const& tree,
    std::vector<Bipartition> const& bipartitions,
    std::vector<TreeNode const*> const& nodes,
    bool include_splitting_edges = true,
    bool include_leaf_edges = true
);

std::vector<size_t> find_monophyletic_subtree_edges(
    Tree const& tree,
    std::vector<TreeNode const*> const& nodes,
    bool include_splitting_edges = true,
    bool include_leaf_edges = true
);

std::vector<size_t> find_monophyletic_subtree_edges(
    Tree const& tree,
    std::vector< std::string > const& node_names,
    bool include_splitting_edges = true,
    bool include_leaf_edges = true
);

// =================================================================================================
//     Whole Clade Functions
// =================================================================================================

std::vector<size_t> get_clade_edges(
    Tree const& tree,
    std::vector< tree::TreeNode const* > const& nodes
);

std::vector<size_t> get_clade_edges(
    Tree const& tree,
    std::vector< std::string > const& node_names
);

} // namespace tree
} // namespace genesis

#endif // include guard
