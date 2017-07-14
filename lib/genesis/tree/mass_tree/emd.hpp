#ifndef GENESIS_TREE_MASS_TREE_EMD_H_
#define GENESIS_TREE_MASS_TREE_EMD_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

    template<typename T>
    class Matrix;

}

namespace tree {

    class Tree;
    class TreeNode;
    class TreeEdge;

    using MassTree = Tree;

}

namespace tree {

// =================================================================================================
//     Earth Movers Distance
// =================================================================================================

/**
 * @brief Calculate the earth mover's distance of two distributions of masses on a given Tree.
 *
 * The earth mover's distance is typically a distance measure between two distributions.
 * See [Earth mover's distance](https://en.wikipedia.org/wiki/Earth_mover's_distance) for an
 * introduction.
 *
 * In our case, we use distibutions of masses along the branches of a tree. Each branch can have
 * multiple masses at different positions within [0.0, branch_length].
 *
 * The distance is calculated as the amount of work needed to move the masses of one distribution
 * so that they end up in the positions of the masses of the other distribution.
 * Work is here defined as mass times dislocation. Thus, the work is higher if either more mass has
 * to be moved, or if mass has to be moved further.
 *
 * Here, the parameter @p p is used to control the influence of mass and distance, with
 * `0.0 < p < inf`, and default `p == 1.0`, which is the neutral case.
 * A larger @p p increases the impact of distance traveled, while a smaller @p p emphasizes
 * differences of mass. For details, see the references cited below.
 *
 * The resulting distance is independed of the rooting of the tree and commutative with respect
 * to the two mass distributions.
 *
 * The earth mover's distance is only meaningful if both mass distributions contain the same amount
 * of total mass.
 * See @link mass_tree_sum_of_masses( MassTree const& tree ) mass_tree_sum_of_masses() @endlink
 * to check this. Also, in order to give comparable results over different tree topologies, the
 * mass can be normalized using mass_tree_normalize_masses(). Then, the result of the earth mover's
 * distance is always in the range `[ 0.0, 1.0 ]`.
 *
 * See @link placement::earth_movers_distance( const Sample&, const Sample&, double, bool )
 * earth_movers_distance( Sample const&, ... ) @endlink for an exemplary
 * usage of this function, which applies the earth mover's distance to the placement weights
 * (@link placement::PqueryPlacement::like_weight_ratio like_weight_ratio@endlink) of a
 * PlacementTree.
 *
 * References:
 *
 * > [1] Guppy Documentation: http://matsen.github.io/pplacer/generated_rst/guppy_kr.html#guppy-kr
 *
 * > [2] F. A. Matsen and S. N. Evans, <b>“Edge principal components and squash clustering: using the
 * > special structure of phylogenetic placement data for sample comparison.”</b>, *PLoS One, 2011*.
 * > [DOI: 10.1371/journal.pone.0056859](http://dx.doi.org/10.1371/journal.pone.0056859)
 *
 * > [3] S. N. Evans and F. A. Matsen, <b>“The phylogenetic Kantorovich-Rubinstein metric for
 * > environmental sequence samples.”</b>, *Statistical Methodology, 2012*.
 * > [DOI: 10.1111/j.1467-9868.2011.01018.x](http://dx.doi.org/10.1111/j.1467-9868.2011.01018.x)
 */
double earth_movers_distance( MassTree const& lhs, MassTree const& rhs, double p = 1.0 );

/**
 * @brief Calculate the pairwise earth mover's distance for all @link MassTree MassTrees@endlink.
 *
 * The result is a pairwise distance @link utils::Matrix Matrix@endlink using the indices of the
 * given `vector`. See earth_movers_distance( MassTree const&, MassTree const&, double ) for details
 * on the calculation.
 */
utils::Matrix<double> earth_movers_distance( std::vector<MassTree> const& trees, double p = 1.0 );

/**
 * @brief Calculate the earth mover's distance of masses on a given Tree.
 *
 * This function is mainly used as a speed-up for calculating
 * @link earth_movers_distance( MassTree const& lhs, MassTree const& rhs, double )
 * earth_movers_distance( MassTree const&, MassTree const& )@endlink. See there for more details.
 *
 * It uses the following convention for the two distributions: The masses of one distribution are
 * stored using a positive sign, the masses of the other distribution use a negative sign.
 * This way, only one Tree needs to be stored, and the algorithm is significantly simplyfied.
 *
 * Thus, as the earth mover's distance is only meaningful if both distributions have the same sum,
 * and we use opposite signs to store the masses, the sum of all masses on the tree should ideally
 * be zero (apart from numerical derivations).
 * See @link mass_tree_sum_of_masses( MassTree const& tree ) mass_tree_sum_of_masses() @endlink and
 * @link mass_tree_validate( MassTree const& tree, double valid_total_mass_difference )
 * mass_tree_validate() @endlink for functions to verify this.
 *
 * The function returns two doubles: The first one is the actual distance, the second one gives
 * the remaining mass at the root node. This should also be close to `0.0`, as there, all masses
 * from the subtrees should ideally cancel each other out. Use this value to check whether this
 * actually worked out. Too big numbers indicate that something is wrong with the sums of the signed
 * masses.
 */
std::pair<double, double> earth_movers_distance( MassTree const& tree, double p = 1.0 );

} // namespace tree
} // namespace genesis

#endif // include guard
