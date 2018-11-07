#ifndef GENESIS_TREE_MASS_TREE_PHYLO_ILR_H_
#define GENESIS_TREE_MASS_TREE_PHYLO_ILR_H_

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

#include "genesis/tree/mass_tree/balances.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
using MassTree = Tree;

// =================================================================================================
//     Phylogenetic ILR Tranform
// =================================================================================================

/**
 * @brief Calculate the Phylogenetic Isometric Log Ratio transformation.
 *
 * The balances are calculated per node of the tree, similar to [1]. We however extend this original
 * idea by being able to place masses on inner branches as well, instead of just the tips (OTUs).
 * The tree has to be bifurcating and rooted. The calculated balances are stored using the node
 * indices. Their sign (order of the subtrees) is according to the TreeLink order of each TreeNode:
 * The numerator is the first link, the denominator is the second link - unless @p reverse_signs
 * is set to `true`, in which case this is flipped.
 * Use sign_matrix() to get the ordering (sign) used for the subtrees.
 *
 * The function expects @p data coming from mass_balance_data(), which can be calculated for a
 * single tree, or for a set of trees. In the latter case, per-taxon (that is, per-edge) weights
 * can also be calculated, see BalanceSettings for details.
 *
 * > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 */
utils::Matrix<double> phylogenetic_ilr_transform(
    BalanceData const& data,
    bool reverse_signs = false
);

/**
 * @brief Calculate edge balances using the Isometric Log Ratio transformation.
 *
 * This is a hybrid method between the phylogenetic_ilr_transform() and
 * @link epca_imbalance_matrix() edge imbalances@endlink: We calcualte the _balance_ between the
 * masses on the two sides of the split induced by each edge. This is similar to edge imbalances,
 * in that it splits the tree at each edge, but instead of calculating the imbalance, we
 * use the ILR transform to calculate balances instead.
 */
utils::Matrix<double> edge_balances(
    BalanceData const& data,
    bool reverse_signs = false
);

} // namespace tree
} // namespace genesis

#endif // include guard
