#ifndef GENESIS_TREE_MASS_TREE_FUNCTIONS_H_
#define GENESIS_TREE_MASS_TREE_FUNCTIONS_H_

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

// =================================================================================================
//     Earth Movers Distance
// =================================================================================================

namespace tree {

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
 * See @link placement::earth_movers_distance( const Sample& lhs, const Sample& rhs, bool with_pendant_length )
 * earth_movers_distance( Sample const&, ... ) @endlink for an exemplary
 * usage of this function, which applies the earth mover's distance to the placement weights
 * (@link placement::PqueryPlacement::like_weight_ratio like_weight_ratio@endlink) of a
 * PlacementTree.
 */
double earth_movers_distance( MassTree const& lhs, MassTree const& rhs );

/**
 * @brief Calculate the pairwise earth mover's distance for all @link MassTree MassTrees@endlink.
 *
 * The result is a pairwise distance @link utils::Matrix Matrix@endlink using the indices of the
 * given `vector`. See earth_movers_distance( MassTree const&, MassTree const& ) for details
 * on the calculation.
 */
utils::Matrix<double> earth_movers_distance( std::vector<MassTree> const& trees );

/**
 * @brief Calculate the earth mover's distance of masses on a given Tree.
 *
 * This function is mainly used as a speed-up for calculating
 * @link earth_movers_distance( MassTree const& lhs, MassTree const& rhs )
 * earth_movers_distance( MassTree const& , MassTree const& )@endlink.
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
std::pair<double, double> earth_movers_distance( MassTree const& tree );

// =================================================================================================
//     Manipulate Masses
// =================================================================================================

/**
 * @brief Merge all masses of two @link MassTree MassTrees@endlink into one and return it.
 */
MassTree mass_tree_merge_trees( MassTree const& lhs, MassTree const& rhs );

/**
 * @brief Merge all masses of two @link MassTree MassTrees@endlink by adding them to the first
 * MassTree.
 */
void mass_tree_merge_trees_inplace( MassTree& lhs, MassTree const& rhs );

/**
 * @brief Clear all masses of a ::MassTree, while keeping its topology.
 */
void mass_tree_clear_masses( MassTree& tree );

/**
 * @brief Reverse the sign of each mass point on a ::MassTree.
 */
void mass_tree_reverse_signs( MassTree& tree );

/**
 * @brief Scale all masses of a ::MassTree with the multiplicative factor @p factor.
 */
void mass_tree_scale_masses( MassTree& tree, double factor );

/**
 * @brief Scale all masses of a ::MassTree so that they sum up to `1.0`.
 */
void mass_tree_normalize_masses( MassTree& tree );

/**
 * @brief Set all branch lengths of a ::MassTree to `1.0`, while keeping the relative position of all
 * masses on the branches.
 */
void mass_tree_transform_to_unit_branch_lengths( MassTree& tree );

/**
 * @brief Accumulate all masses of a ::MassTree on the centers of their edges.
 *
 * This function can be used to minimize the data load of a ::MassTree. It is equal to
 * mass_tree_binify_masses() when using `number_of_bins == 1`.
 *
 * Return the work (mass times distance) that was needed to move the masses to the centers.
 */
double mass_tree_center_masses_on_branches( MassTree& tree );

/**
 * @brief Accumulate all masses of a ::MassTree at the average mass position per edge.
 *
 * This function is similar to mass_tree_center_masses_on_branches(), but instead of
 * accumulating the masses at the branch center, they are accumulated at their average position
 * on the branch.
 *
 * Return the work (mass times distance) that was needed to move the masses to the centers.
 */
double mass_tree_center_masses_on_branches_averaged( MassTree& tree );

/**
 * @brief Accumulate all masses of a ::MassTree into bins on each branch.
 *
 * Each branch is divided into intervals of equal size, where @p number_of_bins is the number of
 * those intervals. The mid points of these intervals are then used as bins, to which the masses
 * on the branch are moved. Each mass point is moved to its closest bin, so that all mass is
 * accumulated at the bins.
 *
 * This function is useful to reduce the data load of big ::MassTree%s, without affecting the
 * accuracy of downstream analyses too much. Using the interval mid points as bins means that masses
 * are moved as little as possible.
 *
 * Example: Given `number_of_bins == 6`, for a branch of length `3.6`, the bins look like this:
 *
 *     Intervals   0.0   0.6   1.2   1.8   2.4   3.0   3.6
 *                 |     |     |     |     |     |     |
 *                    ^     ^     ^     ^     ^     ^
 *     Bins           0.3   0.9   1.5   2.1   2.7   3.3
 *
 * The function returns the work (mass times distance) that was needed to move the masses to the
 * bins.
 */
double mass_tree_binify_masses( MassTree& tree, size_t number_of_bins );

// =================================================================================================
//     Others
// =================================================================================================

/**
 * @brief Return a `std::vector` that contains the total @link MassTreeEdgeData::masses Mass@endlink
 * for each @link ::MassTreeEdge edge@endlink of the given @link ::MassTree MassTree@endlink.
 *
 * The vector is indexed using the @link MassTreeEdge::index() index@endlink of the edges.
 */
std::vector<double> mass_tree_mass_per_edge( MassTree const& tree );

/**
 * @brief Return the total sum of all masses on the ::MassTree.
 *
 * In order for the earth_movers_distance() algorithm to work properly (and give meaningful
 * results), the total mass on the @link MassTree MassTrees@endlink should ideally be the same.
 * This function can be used to check this.
 *
 * Because of numerical issues however, be aware that the result might be slighly off. This
 * is okay, as it usually is in the last digits of the double.
 */
double mass_tree_sum_of_masses( MassTree const& tree );

/**
 * @brief Validate the data on a ::MassTree.
 *
 * This function returns true iff the data on the Tree is valid:
 *
 *  *  The node and edge data types have to be @link MassTreeNodeData MassTreeNodeData@endlink
 *     and @link MassTreeEdgeData MassTreeEdgeData@endlink, respectively.
 *  *  The positions of the masses are in `[ 0.0, branch_length ]` on their respective branches.
 *  *  If the optional arugment @p valid_total_mass_difference is not negative, the sum of all
 *     masses is also checked. It has to be close to 0.0, using the argument as the absolute allowed
 *     difference. This is useful to check whether the masses for calculating the one-argument
 *     version of the earth_movers_distance( MassTree const& ) are correct.
 *
 * The function stops at the first encountered invalid condition and outputs a description message
 * of the invalid value to LOG_INFO.
 *
 * @param tree                        ::MassTree to be validated.
 * @param valid_total_mass_difference If set to a non-negative value, it is used as the absolute
 *                                    allowed difference from zero for the total sum of all masses
 *                                    on the tree.
 */
bool mass_tree_validate( MassTree const& tree, double valid_total_mass_difference = -1.0);

} // namespace tree
} // namespace genesis

#endif // include guard
