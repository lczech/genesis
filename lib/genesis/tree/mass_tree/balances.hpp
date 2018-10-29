#ifndef GENESIS_TREE_MASS_TREE_BALANCES_H_
#define GENESIS_TREE_MASS_TREE_BALANCES_H_

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

#include "genesis/utils/containers/matrix.hpp"

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

using MassTree = Tree;

// =================================================================================================
//     Phylogenetic ILR Tranform
// =================================================================================================

/**
 * @brief Compute the sign matrix or Sequential Binary Partition (SBP) of a Tree.
 *
 * The Tree has to be rooted and strictly bifurcating. Then, we can compute a matrix that tells for
 * each node a relative ordering of the other nodes. Say we have the tree:
 *
 *            /----------- T3
 *      /-----| N2
 *     |      \----------- T2
 *     | N1
 *     |
 *      \----------- T1
 *
 * This yields the (compressed) sign matrix:
 *
 * Taxa | T1 | T2 | T3
 * :--- |---:|---:|---:
 * N1   | +1 | -1 | -1
 * N2   |  0 | +1 | -1
 *
 * That is, all nodes in the subtree of the first child of a node get assigned a `+1`,
 * and all nodes in the subtree of the second child get a `-1`. The remaining nodes (the rest of
 * the tree, including the node itself) get assigned a `0`.
 *
 * This was introduced as Sequential Binary Partition (SBP) in [1], and called Sign Matrix in [2].
 * See there for more details.
 *
 * By default, the matrix dimensions are `n*n` with `n` beging the number of nodes in the tree.
 * That is, all node indices are used. This is easierst to work with in the context of other
 * functions that used node indices. However, the matrix contains a lot of zeros.
 *
 * If however @p compressed is `true`, the matrix is not indexed by the node indices.
 * Instead, the rows only contain inner nodes, and the columns only contain leaf nodes,
 * as shown in the table above. That means that the sign of inner nodes is not available in the matrix.
 * Use inner_node_indices() to get the node indices that correspond to the rows of the matrix,
 * and use leaf_node_indices() for the node indices that correspond to the columns.
 *
 * > [1] V. Pawlowsky-Glahn and J. J. Egozcue,
 * > "Exploring Compositional Data with the CoDa-Dendrogram,"
 * > Austrian J. Stat., vol. 40, no. 1&2, pp. 103–113, 2011.
 * > https://ajs.or.at/index.php/ajs/article/view/vol40,%20no1&2%20-%2011
 *
 * > [2] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 */
utils::Matrix<signed char> sign_matrix( Tree const& tree, bool compressed = false );

} // namespace tree
} // namespace genesis

#endif // include guard
