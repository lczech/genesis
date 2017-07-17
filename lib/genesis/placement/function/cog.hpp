#ifndef GENESIS_PLACEMENT_FUNCTION_COG_H_
#define GENESIS_PLACEMENT_FUNCTION_COG_H_

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
 * @ingroup placement
 */

#include "genesis/placement/placement_tree.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sample;

// =================================================================================================
//     Center of Gravity
// =================================================================================================

/**
 * @brief Calculate the Center of Gravity of the placements on a tree.
 *
 * The center of gravity is the point on the tree where all masses of the placements on the one
 * side of it times their distance from the point are equal to this sum on the other side of the
 * point. In the following example, the hat `^` marks this point on a line with two placements:
 * One has mass 1 and distance 3 from the central point, and one as mass 3 and distance 1,
 * so that the product of their mass and distance to the point is the same:
 *
 *                   3
 *                   |
 *     1             |
 *     |_____________|
 *               ^
 *
 * It is thus like calculating masses and torques on a lever in order to find their physical
 * center of mass/gravity.
 *
 * This calculation is done for the whole tree, with the masses calculated from the
 * `like_weight_ratio` and distances in terms of the `branch_length` of the edges and the
 * `proximal_length` and (if specificed in the method parameter) the `pendant_length` of the
 * placements.
 */
std::pair<PlacementTreeEdge const*, double> center_of_gravity (
    Sample const& smp,
    bool const    with_pendant_length = false
);

/**
 * @brief Calcualte the variance of the PqueryPlacement%s of a Sample around its Center of Gravity.
 *
 * The caluclation of the variance is as follows:
 *
 * \f$ Var(X) = E[ (x - \mu)^2 ] = \frac{\sum (x - \mu)^2 \cdot \omega} {\sum \omega} \f$, where
 * the weights \f$ \omega \f$ are the `like_weight_ratio`s of the placements.
 *
 * See center_of_gravity() for more.
 */
double center_of_gravity_variance (
    Sample const& smp,
    bool const    with_pendant_length = false
);

/**
 * @brief Calculate the distance between the two Centers of Gravity of two Sample%s.
 *
 * The distance is measured in branch length units; for the Center of Gravity, see
 * center_of_gravity().
 */
double center_of_gravity_distance (
    Sample const& smp_a,
    Sample const& smp_b,
    bool const    with_pendant_length = false
);

} // namespace placement
} // namespace genesis

#endif // include guard
