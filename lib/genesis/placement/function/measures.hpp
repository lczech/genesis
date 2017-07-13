#ifndef GENESIS_PLACEMENT_FUNCTION_MEASURES_H_
#define GENESIS_PLACEMENT_FUNCTION_MEASURES_H_

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
 * @brief Header for Placement Measures functions.
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/placement_tree.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class TreeNode;
    class TreeEdge;
    class Tree;

    using MassTree = Tree;

}

namespace placement {

    class Sample;
    class SampleSet;
    class PlacementTreeEdgeData;
    class PlacementTreeNodeData;
    class Pquery;
    class PqueryPlacement;
    struct PqueryPlain;

}

namespace utils {

    template<typename T>
    class Matrix;

    class Histogram;

}

// =================================================================================================
//     Placement Measures
// =================================================================================================

namespace placement {

/**
 * @brief Calculates the normalized distance between two plain pqueries. It is mainly a helper
 * method for distance calculations (e.g., pairwise distance, variance).
 *
 * For each placement in the two pqueries, a distance is calculated, and their normalized sum is
 * returned. Normalization is done using the mass of placements in both pqueries.
 *
 * The distance between two placements is calculated as the shortest path between them. This
 * includes the their position on the branches, and - if specified - the pendant_length of both.
 * There are three cases that might occur:
 *
 *   1. **Both placements are on the same branch.**
 *      In this case, their distance is caluclated as their difference in proximal_lengths (plus
 *      if specified the sum of their pendant_lengths).
 *
 *   2. **The path between the placements includes the root.**
 *      The distance of a placement from its neighbouring nodes is usually given in form of the
 *      proximal_length, which is the distance of the placement to the node (at the end of its
 *      branch) that lies in direction of the root. Thus, there is an implicit notion of a root,
 *      that we need to consider. If the path between two placements contains the root, we can
 *      directly calculate their distance as the distance between the two promixal nodes plus
 *      proximal_lengths (and possibly pendant_lengths) of both placements. We call this the
 *      promixal-promixal case.
 *
 *   3. **The root is not part of the path between the placements.**
 *      This case means that one of the two placements lies on the path between the other placement
 *      and the root -- thus, the path between the placements does not contain the root.
 *      The distance between the placements cannot be calculated using the proximal_lengths
 *      directly, but we need to get the distal_length (away from the root) of the inner placement
 *      first. This is simply the difference between branch_length and proximal_length of that
 *      placement. Of course, this case comes in two flavours, because both placements can be the
 *      inner or outer one. They are called proximal-distal case and distal-proximal case,
 *      respectively.
 *
 * The first case is easy to detect by comparing the edge nums. However, distinguishing between the
 * latter two cases is expensive, as it involves finding the path to the root for both placements.
 * To speed this up, we instead use a distance matrix that is calculated in the beginning of any
 * algorithm using this method and contains the pairwise distances between all nodes of the tree.
 * Using this, we do not need to find paths between placements, but simply go to the nodes at the
 * end of the branches of the placements and do a lookup for those nodes.
 *
 * With this technique, we can calculate the distances between the placements for all
 * three cases (promixal-promixal, proximal-distal and distal-proximal) cheaply. The wanted distance
 * is then simply the minimum of those three distances. This is correct, because the two wrong cases
 * will always produce an overestimation of the distance.
 *
 * This distance is normalized using the `like_weight_ratio` of both placements, before
 * summing it up to calculate the total distance between the pqueries.
 */
double pquery_distance (
    const PqueryPlain&     pqry_a,
    const PqueryPlain&     pqry_b,
    const utils::Matrix<double>&  node_distances,
    bool                   with_pendant_length = false
);

double placement_distance(
    PqueryPlacement const& place_a,
    PqueryPlacement const& place_b,
    utils::Matrix<double> const& node_distances
);

// -------------------------------------------------------------------------------------------------
//     Mass Tree Conversion
// -------------------------------------------------------------------------------------------------

std::pair< tree::MassTree, double > convert_to_mass_tree( Sample const& sample );

std::pair< std::vector<tree::MassTree>, std::vector<double> >
convert_to_mass_trees( SampleSet const& sample_set );

// -------------------------------------------------------------------------------------------------
//     Expected Distance between Placement Locations
// -------------------------------------------------------------------------------------------------

/**
 * @brief Calculate the EDPL uncertainty values for a Pquery.
 *
 * See http://matsen.github.io/pplacer/generated_rst/guppy_edpl.html for more information.
 *
 * This function expects a Pquery and the Sample it belongs to. This is necessary in order to
 * get the Tree of the Sample and calculate distances between its Nodes.
 */
double expected_distance_between_placement_locations( Sample const& sample, Pquery const& pquery );

/**
 * @brief Shortcut alias for @link
 * expected_distance_between_placement_locations( Sample const&, Pquery const& )
 * expected_distance_between_placement_locations()@endlink.
 */
double edpl(                                          Sample const& sample, Pquery const& pquery );

/**
 * @brief Calculate the @link
 * expected_distance_between_placement_locations( Sample const&, Pquery const& )
 * expected_distance_between_placement_locations()@endlink for all @link Pquery Pqueries@endlink
 * in the Sample.
 */
std::vector<double> expected_distance_between_placement_locations( Sample const& sample );

/**
 * @brief Shortcut alias for @link expected_distance_between_placement_locations( Sample const& )
 * expected_distance_between_placement_locations()@endlink.
 */
std::vector<double> edpl(                                          Sample const& sample );

// -------------------------------------------------------------------------------------------------
//     Pairwise Distance
// -------------------------------------------------------------------------------------------------

/**
 * @brief Calculate the normalized pairwise distance between all placements of the two Samples.
 *
 * This method calculates the distance between two Sample%s as the normalized sum of the distances
 * between all pairs of @link Pquery Pqueries @endlink in the Sample. It is similar to the
 * variance() calculation, which calculates this sum for the squared distances between all Pqueries
 * of one Sample.
 *
 * @param  smp_a               First Sample to which the distances shall be calculated to.
 * @param  smp_b               Second Sample to which the distances shall be calculated to.
 * @param  with_pendant_length Whether or not to include all pendant lengths in the calculation.
 *
 * @return                     Distance value.
 */
double pairwise_distance (
    const Sample& smp_a,
    const Sample& smp_b,
    bool          with_pendant_length = false
);

// double closest_pair_distance (
//     const Sample& smp_a,
//     const Sample& smp_b,
//     bool                with_pendant_length = false
// );

// -------------------------------------------------------------------------------------------------
//     Node-Based Distances
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
//     Variance
// -------------------------------------------------------------------------------------------------

/**
 * @brief Calculate the variance of the placements on a tree.
 *
 * The variance is a measure of how far a set of items is spread out in its space
 * (http://en.wikipedia.org/wiki/variance). In many cases, it can be measured using the mean of the
 * items. However, when considering placements on a tree, this does not truly measure how far they
 * are from each other. Thus, this algorithm applies a different method of calculating the variance
 * in terms of squared deviations of all items from each other:
 * \f$ Var(X) = \frac{1}{n^2} \sum_{i=1}^n \sum_{j=1}^n \frac{1}{2}(x_i - x_j)^2 \f$,
 * where \f$ (x_i - x_j) \f$ denotes the distance between two placements.
 *
 * According to the formula above, each pair of placements is evaluated twice, and subsequently
 * their distance need to be halfed when being added to the sum of distanaces. Instead of that,
 * we calculate the distance for each pair only once, thus are able skip half the calculations, and
 * of course skip the division by two.
 *
 * Furthermore, the normalizing factor \f$ \frac{1}{n^2} \f$ of the variance usually contains the
 * number of elements being processed. However, as the placements are weighted by their
 * `like_weight_ratio`, we instead calculate `n` as the sum of the `like_weight_ratio` of all
 * placements. In case that for each pquery the ratios of all its placements sum up to 1.0, this
 * number will be equal to the number of pqueries (and thus be equal to the usual case of using the
 * number of elements). However, as this is not required (placements with small ratio can be
 * dropped, so that their sum per pquery is less than 1.0), we cannout simply use the count.
 */
double variance (
    const Sample& smp,
    bool          with_pendant_length = false
);

} // namespace placement
} // namespace genesis

#endif // include guard
