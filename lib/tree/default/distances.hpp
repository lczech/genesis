#ifndef GENESIS_TREE_DEFAULT_DISTANCES_H_
#define GENESIS_TREE_DEFAULT_DISTANCES_H_

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
 * @brief Header of Default Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include <vector>

#include "tree/default/tree.hpp"
#include "utils/math/matrix.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Distance Related Propoerties
// =================================================================================================

double length(   Tree const& tree );
double height(   Tree const& tree );
double diameter( Tree const& tree );

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

utils::Matrix<double> node_branch_length_distance_matrix(
    Tree const& tree
);

std::vector<double> node_branch_length_distance_vector(
    Tree const& tree,
    TreeNode const* node = nullptr
);

utils::Matrix<double> edge_branch_length_distance_matrix(
    Tree const& tree
);

std::vector<double> edge_branch_length_distance_vector(
    Tree const& tree,
    TreeEdge const* edge = nullptr
);

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

double deepest_distance( Tree const& tree );

std::vector<std::pair< TreeNode const*, double>> closest_leaf_distance_vector(
    Tree const& tree
);

} // namespace tree
} // namespace genesis

#endif // include guard
