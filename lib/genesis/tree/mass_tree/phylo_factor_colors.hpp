#ifndef GENESIS_TREE_MASS_TREE_PHYLO_FACTOR_COLORS_H_
#define GENESIS_TREE_MASS_TREE_PHYLO_FACTOR_COLORS_H_

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

#include "genesis/tree/mass_tree/phylo_factor.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/list_qualitative.hpp"

#include <functional>
#include <limits>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Subtree;
class Tree;
using MassTree = Tree;

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Get a list of all edges that have factored out by phylogenetic_factorization().
 *
 * By default, all edges that are factors are returned, that is, the list of
 * all PhyloFactor::edge_index in the input @p factors (in other words, the indices of the "winning"
 * edges). If @p max_factor is set to a value > 0, only this many factors (the first ones) are
 * returned. 0 is also allowed, in which case an empty vector is returned.
 */
std::vector<size_t> phylo_factor_edge_indices(
    std::vector<PhyloFactor> const& factors,
    size_t max_factor = std::numeric_limits<std::size_t>::max()
);

// =================================================================================================
//     Single Factor Coloring
// =================================================================================================

/**
 * @brief Store a set of colors for making visualizations of individual phylo factors.
 *
 * Used by phylo_factor_single_factor_colors(), see there for details.
 *
 * @see phylogenetic_factorization()
 */
struct PhyloFactorSingleColors
{
    /**
     * @brief Color for the edge of that phylo factor.
     *
     * Default is black.
     */
    utils::Color factor_edge = utils::Color( 0.0, 0.0, 0.0 );

    /**
     * @brief Color for the edges towards the root that have been used in this phylo factor.
     *
     * Default is purple.
     */
    utils::Color primary_edges = utils::Color( 0.529411765, 0.439215686, 0.670588235 );

    /**
     * @brief Color for the edges away from the root that have been used in this phylo factor.
     *
     * Default is green.
     */
    utils::Color secondary_edges = utils::Color( 0.352941176, 0.682352941, 0.380392157 );

    /**
     * @brief Color for the edges that have been factored out before (earlier factors in the greedy
     * search).
     *
     * Default is black.
     */
    utils::Color previous_factors = utils::Color( 0.0, 0.0, 0.0 );

    /**
     * @brief Color for the edges that have not been used in this phylo factor.
     *
     * Defaut is a light gray.
     */
    utils::Color neutral_edges = utils::Color( 0.8, 0.8, 0.8 );
};

/**
 * @brief Return a color for each edge indicating its role in a single phylogenetic factor.
 *
 * The function takes one of the factors resulting from a phylogenetic_factorization(),
 * and colorizes the edges of the tree for user output, indicating for each edge whether:
 *
 *  * it is the "winning" edge of the factor;
 *  * it is part of the edges that have been used for the balance computation,
 *    which is further separated into the two parts split by the winning edge,
 *    that is, the primary and the secondary parts;
 *  * it is an edge that was a winning edge in a previous factor (with smaller @p factor_index);
 *  * or a "neutral" edge that has not been considered for the balance of the given factor.
 *
 * The resulting colors can be used for visualizing a tree.
 */
std::vector<utils::Color> phylo_factor_single_factor_colors(
    Tree const& tree,
    std::vector<PhyloFactor> const& factors,
    size_t factor_index,
    PhyloFactorSingleColors colors = {}
);

// =================================================================================================
//     Factor Clade Coloring
// =================================================================================================

/**
 * @brief Store a set of colors for making visualizations of the clades of all phylo factors.
 *
 * Used by phylo_factor_clade_colors(), see there for details.
 *
 * @see phylogenetic_factorization()
 */
struct PhyloFactorCladeColors
{
    /**
     * @brief Color for the "winning" edges of phylo factors.
     *
     * Default is black.
     */
    utils::Color factor_edges = utils::Color( 0.0, 0.0, 0.0 );

    /**
     * @brief Color for the edges towards the root of the first phylo factor.
     *
     * Default is a light gray. This color is used for the primary part (towards the root)
     * of the first factor. As phylo_factor_clade_colors() only colorizes the secondar (away from
     * the root) splits of each factor, this color can be considered the color for the
     * "remaining" part of the tree.
     */
    utils::Color base_edges = utils::Color( 0.8, 0.8, 0.8 );

    /**
     * @brief Colors for the sets of edges away from the root that have been split by the
     * phylo factors.
     *
     * That is, each factor gets a color of this list.
     */
    std::vector<utils::Color> clade_colors = utils::color_list_set1();
};

/**
 * @brief Return a color for each edge, indicating which factor (phylogenetic unit, clade)
 * it belongs to.
 *
 * Phylo Factorization splits the tree into multiple parts, where each factor splits away a
 * monophyletic clade. This function colors the edges of each clade in a different color,
 * for visualiuzation purposes.
 *
 * By default, @p num_factors is 0, meaning that the clades of all factors are used (up to the
 * number of `max_iterations` that the factorization was run with). By setting @p num_factors
 * to a smaller number, only these first clades are visualized.
 *
 * The visualization is done by setting the color for the _secondary_ edges of the factor (away from
 * the root). This is done so that nested clades (factors found within a previously split clade)
 * are not overwritten. Thus, the first factor leaves its primary part uncolorized,
 * for which the @link PhyloFactorCladeColors::base_edges base_color@endlink is used.
 */
std::vector<utils::Color> phylo_factor_clade_colors(
    Tree const& tree,
    std::vector<PhyloFactor> const& factors,
    size_t num_factors = 0,
    PhyloFactorCladeColors colors = {}
);

} // namespace tree
} // namespace genesis

#endif // include guard
