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
#include <unordered_set>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;
class Subtree;

using MassTree = Tree;

// =================================================================================================
//     Phylogenetic ILR Tranform
// =================================================================================================

/**
 * @brief Calcualte the balance of edge masses between two sets of edges.
 *
 * @see phylogenetic_ilr_transform( MassTree const& ) for a function that calcualtes this
 * for the subtrees below all nodes in a rooted Tree.
 */
double mass_balance(
    std::vector<double> const& edge_masses,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices
);

/**
 * @brief Calcualte the Phylogenetic Isometric Log Ratio transformation of a MassTree.
 *
 * The balances are calculated per node of the tree, similar to [1]. We however extend this original
 * idea by being able to place masses on inner branches as well, instead of just the tips (OTUs).
 * The tree has to be bifurcating and rooted. The calcualted balances are stored using the node
 * indices. Their sign (order of the subtrees) is according to the TreeLink order of each TreeNode:
 * The numerator is the first link, the denominator is the second link.
 * Use sign_matrix() to get the ordering (sign) used for the subtrees.
 *
 * The function assumes that the Tree is populated with masses along its branches which represent
 * metagenomic sequences being placed there. The masses must not be normalized - there has to be
 * at least a mass of ~1.0 for this function to work. That is each sequence should contribute
 * about 1.0 mass to the tree (leaving out some lower masses in case of phylogenetic placement data).
 *
 * > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 */
std::vector<double> phylogenetic_ilr_transform( MassTree const& tree );

/**
 * @brief Calcualte the Phylogenetic Isometric Log Ratio transformation of a set of MassTree%s.
 *
 * Because we return a @link utils::Matrix Matrix@endlink of the results, all Tree%s have to have
 * identical topology.
 *
 * @see phylogenetic_ilr_transform( MassTree const& ) for details.
 */
utils::Matrix<double> phylogenetic_ilr_transform( std::vector<MassTree> const& trees );

} // namespace tree
} // namespace genesis

#endif // include guard
