#ifndef GENESIS_TREE_FUNCTION_DISTANCES_H_
#define GENESIS_TREE_FUNCTION_DISTANCES_H_

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
 * @brief Header of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "genesis/utils/math/matrix.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Foward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Distance Measures
// =================================================================================================

utils::Matrix<size_t> node_path_length_matrix( Tree const& tree );

std::vector<size_t> node_path_length_vector(
    Tree     const& tree,
    TreeNode const& node
);

std::vector<size_t> node_path_length_vector(
    Tree     const& tree
);

utils::Matrix<size_t> edge_path_length_matrix(
    Tree const& tree
);

std::vector<size_t> edge_path_length_vector(
    Tree     const& tree,
    TreeEdge const& edge
);

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

std::vector< std::pair< TreeNode const*, size_t >>  closest_leaf_depth_vector(
    Tree const& tree
);

} // namespace tree
} // namespace genesis

#endif // include guard
