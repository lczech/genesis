#ifndef GENESIS_TREE_FUNCTION_FUNCTIONS_H_
#define GENESIS_TREE_FUNCTION_FUNCTIONS_H_

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

#include <cstddef> // size_t
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Node Count Properties
// =================================================================================================

// TODO add other interesting member functions: http://en.wikipedia.org/wiki/Tree_%28data_structure%29

int max_rank( Tree const& tree );

bool is_bifurcating( Tree const& tree );

size_t leaf_node_count(  Tree const& tree );
size_t inner_node_count( Tree const& tree );

// =================================================================================================
//     Subtrees
// =================================================================================================

size_t subtree_size( Tree const& tree, TreeLink const& link );

std::vector<size_t> subtree_sizes( Tree const& tree, TreeNode const& node );
std::vector<size_t> subtree_sizes( Tree const& tree );

size_t subtree_max_path_height( Tree const& tree, TreeLink const& link );

// std::vector<size_t> subtree_max_path_heights( Tree const& tree, TreeNode const& node );
// std::vector<size_t> subtree_max_path_heights( Tree const& tree );

// =================================================================================================
//     Misc
// =================================================================================================

std::vector< TreeLink const* > path_to_root( TreeNode const& node );

TreeNode const& lowest_common_ancestor( TreeNode const& node_a, TreeNode const& node_b );
TreeNode&       lowest_common_ancestor( TreeNode& node_a,       TreeNode& node_b );

} // namespace tree
} // namespace genesis

#endif // include guard
