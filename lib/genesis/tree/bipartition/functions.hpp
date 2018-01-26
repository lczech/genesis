#ifndef GENESIS_TREE_BIPARTITION_FUNCTIONS_H_
#define GENESIS_TREE_BIPARTITION_FUNCTIONS_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/bipartition/bipartition.hpp"
#include "genesis/utils/math/bitvector.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Bipartition Functions
// =================================================================================================

std::vector<Bipartition> bipartition_set( Tree const& tree );

std::vector<size_t> node_to_leaf_map( Tree const& tree );

/**
 * @brief Return a Bitvector that has as many entries as the @p tree has leaf nodes,
 * and is `true` where the given @p leaf_nodes are.
 */
utils::Bitvector leaf_node_bitvector( Tree const& tree, std::vector<TreeNode const*> leaf_nodes );

std::vector<size_t> get_subtree_edges( TreeLink const& subtree );

/**
 * @brief Find clades of the tree that are monophyletic with respect to the given list of nodes,
 * that is, clades that only contain nodes from that list. Retun all edge indices of those clades.
 */
std::vector<size_t> find_monophyletic_subtree_edges(
    Tree const& tree,
    std::vector<Bipartition> const& bipartitions,
    std::vector<TreeNode const*> nodes
);

/**
 * @brief Find the smallest subtree (measured in number of nodes) that contains all given nodes.
 *
 * A subtree is defined by one of the two parts of a tree that are splitted by one edge. Thus, this
 * function tries all subtrees by leaving out each edge once.
 *
 * If no fitting subtree exists, the function returns an empty Bipartition.
 */
Bipartition find_smallest_subtree(
    Tree const& tree,
    std::vector<Bipartition> const& bipartitions,
    std::vector<TreeNode const*> nodes
);

std::vector<size_t> get_clade_edges( Tree const& tree, std::vector< tree::TreeNode const* > const& nodes );

std::vector<size_t> get_clade_edges( Tree const& tree, std::vector< std::string > const& node_names );

} // namespace tree
} // namespace genesis

#endif // include guard
