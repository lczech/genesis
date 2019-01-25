#ifndef GENESIS_TREE_MASS_TREE_BALANCES_H_
#define GENESIS_TREE_MASS_TREE_BALANCES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
#include "genesis/utils/containers/matrix.hpp"

#include <vector>
#include <unordered_set>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

using MassTree = Tree;

// =================================================================================================
//     Balance Settings
// =================================================================================================

/**
 * @brief Settings to calculate balances and the Phylogenetic ILR Transform.
 *
 * The class stores settings for different parts of the balances calculation:
 *
 *  * The taxon weights used in soft thresholding are described in Section
 *    "Soft thresholding through weighting taxa" of [1]. See there for details.
 *    We implement the weights `p` as described there, offering different ways to assess the central
 *    tendency of counts and to measure the norm of their relative abundances.
 *    These two terms (BalanceSettings::tendency and BalanceSettings::norm) are multiplied
 *    to get the weight for a taxon.
 *    By default, the geometric mean times the euclidean norm are used as weights.
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
};

// =================================================================================================
//     Balance Data
// =================================================================================================

/**
 * @brief Data needed to calculate balances.
 *
 * This data is needed for calculations such as mass_balance(), phylogenetic_ilr_transform(), and
 * phylogenetic_factorization(). Use
 * @link mass_balance_data( std::vector<MassTree> const&, BalanceSettings ) mass_balance_data( std::vector<MassTree> const&, ... )@endlink
 * to calculate this data, and see BalanceSettings for the settings to tune these calculations.
 */
struct BalanceData
{
    /**
     * @brief The Tree on which to calculate balances.
     *
     * As multiple input tress need to have the same topology, it suffices to store just one copy.
     * This copy is not a MassTree any more, in order to avoid accidents of trying to re-use this
     * tree for mass calculations. Instead, it is a simple CommonTree.
     */
    Tree tree;

    /**
     * @brief The relative per-edge masses per original input Tree.
     *
     * Each row corresponds to a Tree/Sample, each column to the edges of the trees,
     * stored in the order of their @link TreeEdge::index() indices@endlink.
     *
     * The masses are relative, that is, they were divided by their sum per row.
     * Furthermore, prior to this normalization, pseudeo-counts might be added to the masses
     * in order to compensate for zero values (which are not valid in balance calculations).
     * Also, if taxon weighing is used, the masses are divided by their weight.
     * See BalanceSettings for the possible options for pseudo-counts.
     * These are the values that are used for the balances computations.
     *
     * In order to get the absolute masses without taxon weighing, and without normalization,
     * see #raw_edge_masses.
     */
    utils::Matrix<double> edge_masses;

    /**
     * @brief The absolute raw per-edge masses per original input Tree.
     *
     * Each row corresponds to a Tree/Sample, each column to the edges of the trees,
     * stored in the order of their @link TreeEdge::index() indices@endlink.
     *
     * The masses are absolute, that is, they just add up the MassTreeEdgeData::masses per edge.
     * Furthermore, pseudeo-counts might be added to the masses in order to compensate
     * for zero values (which are not valid in balance calculations).
     * See BalanceSettings for the possible options for pseudo-counts.
     *
     * In order to get the final edge masses that are used in the balances computation,
     * see #edge_masses.
     */
    utils::Matrix<double> raw_edge_masses;

    /**
     * @brief The taxon/edge weights calculated from mulitple trees.
     *
     * These weights are used in soft thresholding as described in Section
     * "Soft thresholding through weighting taxa" of [1]. See there for details.
     * See BalanceSettings for the possible options to calculate these weights.
     * The weights are calculated per edge of the tree, which we call taxa here,
     * to avoid confusion of masses and weights.
     *
     * > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
     * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
     * > Elife, vol. 6, p. e21887, Feb. 2017.
     * > https://elifesciences.org/articles/21887
     */
    std::vector<double> taxon_weights;
};

// =================================================================================================
//     Balances and Weights Calculations
// =================================================================================================

/**
* @brief Calculate the data needed to calculate balances on MassTree%s.
*
* This function takes a set of MassTree%s and BalanceSettings and calculates the data needed
* for calculations such as mass_balance(), phylogenetic_ilr_transform(), and
* phylogenetic_factorization().
*
* The function assumes that the Tree%s are populated with masses along thir branches which represent
* metagenomic sequences being placed there. The masses must not be normalized - there has to be
* at least a mass of ~1.0 per Tree for this function to work. That is each sequence should contribute
* about 1.0 mass to the tree (leaving out some lower masses in case of phylogenetic placement data).
*/
BalanceData mass_balance_data(
    std::vector<MassTree> const& trees,
    BalanceSettings settings = {}
);

/**
 * @brief Calculate the data needed to calculate balances of a MassTree.
 *
 * This is the version for a single MassTree.
 * Here, the @link BalanceData::taxon_weights taxon weights@endlink are left empty, as they cannot
 * be estimated from just a single tree.
 * See @link mass_balance_data( std::vector<MassTree> const&, BalanceSettings ) mass_balance_data( std::vector<MassTree> const&, ... )@endlink
 * for the version for multiple MassTree%s, which also calculates proper taxon weights.
 */
BalanceData mass_balance_data(
    MassTree const& tree,
    BalanceSettings settings = {}
);

/**
 * @brief Calculate the balance of edge masses between two sets of edges.
 *
 * The function calculates all balances (for all trees) between the two sets of edges.
 *
 * @see phylogenetic_ilr_transform( MassTree const& ) for a function that calculates this
 * for the subtrees below all nodes in a rooted Tree.
 */
std::vector<double> mass_balance(
    BalanceData const& data,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices
);

/**
 * @brief Calculate the balance of edge masses between two sets of edges.
 *
 * The function calculates the balances of a specific tree between the two sets of edges.
 * The given @p tree_index corresponds to the index in the original `vector` of MassTree%s
 * used in @link mass_balance_data( std::vector<MassTree> const&, BalanceSettings ) mass_balance_data()@endlink.
 * This index is thus also the row index in the BalanceData::edge_masses matrix.
 *
 * @see phylogenetic_ilr_transform( MassTree const& ) for a function that calculates this
 * for the subtrees below all nodes in a rooted Tree.
 */
double mass_balance(
    BalanceData const& data,
    std::unordered_set<size_t> const& numerator_edge_indices,
    std::unordered_set<size_t> const& denominator_edge_indices,
    size_t tree_index
);

} // namespace tree
} // namespace genesis

#endif // include guard
