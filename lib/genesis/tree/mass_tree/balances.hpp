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
using MassTree = Tree;

// =================================================================================================
//     Balance Settings
// =================================================================================================

/**
 * @brief Settings to calculate balances and the Phylogenetic ILR Transform.
 *
 * The class stores settings for different parts of the balances calcualtion:
 *
 *  * The taxon weights used in soft thresholding are described in Section
 *    "Soft thresholding through weighting taxa" of [1]. See there for details.
 *    We implement the weights `p` as described there, offering different ways to assess the central
 *    tendency of counts and to measure the norm of their relative abundances.
 *    These two terms (BalanceSettings::tendency and BalanceSettings::norm) are multiplied
 *    to get the weight for a taxon.
 *  * To avoid zero counts in the taxon masses, two ways of adding pseudo-counts are offered:
 *    Either a constant is added to all taxon masses, or just to the ones that are zero
 *    (BalanceSettings::pseudo_count_summand_all and BalanceSettings::pseudo_count_summand_zeros).
 *
 * > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 *
 * @see phylogenetic_ilr_transform().
 */
struct BalanceSettings
{
    enum class WeightTendency
    {
        /**
         * @brief Use no tendency, that is, use `1.0` for this term.
         */
        kNone,

        /**
         * @brief Use the median of the taxon masses.
         * @see median()
         */
        kMedian,

        /**
         * @brief Use the arithmetic mean of the taxon masses.
         * @see arithmetic_mean()
         */
        kArithmeticMean,

        /**
         * @brief Use the geometric mean of the taxon masses.
         * @see geometric_mean()
         */
        kGeometricMean
    };

    enum class WeightNorm
    {
        /**
         * @brief Use no norm, that is, use `1.0` for this term.
         */
        kNone,

        /**
         * @brief Use the Manhattan norm of the relative abundances of the taxon.
         * @see manhattan_norm()
         */
        kManhattan,

        /**
         * @brief Use the Euclidean norm of the relative abundances of the taxon.
         * @see euclidean_norm()
         */
        kEuclidean,

        /**
         * @brief Use the Maximum norm of the relative abundances of the taxon.
         * @see maximum_norm()
         */
        kMaximum,

        /**
         * @brief Use the Aitchison norm of the relative abundances of the taxon.
         * @see aitchison_norm()
         */
        kAitchison
    };

    /**
     * @brief Set the term for asssing the central tendency of taxon masses for calculating
     * the taxon weights.
     */
    WeightTendency tendency = WeightTendency::kGeometricMean;

    /**
    * @brief Set the term for the norm of relative abundances for calculating the taxon weights.
    */
    WeightNorm     norm     = WeightNorm::kEuclidean;

    /**
     * @brief Set the constant that is added to all taxon masses to avoid zero counts.
     */
    double pseudo_count_summand_all = 1.0;

    /**
    * @brief Set the constant that is added to taxon masses that are zero, to avoid zero counts.
    */
    double pseudo_count_summand_zeros = 0.0;

    /**
     * @brief If set to `true`, the numerator and the denominator of the ILR transform are flipped.
     */
    bool reverse_signs = false;
};

// =================================================================================================
//     Balances and Weights
// =================================================================================================

/**
* @brief Calcualte per-taxon weights for balances and Phylogenetic ILR Tranform.
*
* Per-taxon weights are calcuated per edge of the MassTree%s, as each edge represents a taxon
* (including inner edges) in our calculations. See BalanceSettings for the options
* to calculate these weights. See phylogenetic_ilr_transform() for details on the calculation.
*/
std::vector<double> mass_balance_edge_weights(
    std::vector<MassTree> const& trees,
    BalanceSettings settings = {}
);

/**
 * @brief Calcualte the balance of edge masses between two sets of edges.
 *
 * @see phylogenetic_ilr_transform( MassTree const& ) for a function that calculates this
 * for the subtrees below all nodes in a rooted Tree.
 */
double mass_balance(
    std::vector<double> const& edge_masses,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices,
    std::vector<double> const& edge_weights = {}
);

} // namespace tree
} // namespace genesis

#endif // include guard
