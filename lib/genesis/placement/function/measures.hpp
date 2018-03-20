#ifndef GENESIS_PLACEMENT_FUNCTION_MEASURES_H_
#define GENESIS_PLACEMENT_FUNCTION_MEASURES_H_

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

namespace placement {

// =================================================================================================
//     Expected Distance between Placement Locations
// =================================================================================================

/**
 * @brief Calculate the EDPL uncertainty values for a Pquery.
 *
 * This is the function that does the actual computation. It is used by the other `edpl`
 * functions, which first calculate the @p node_distances matrix before calling this function.
 * It is useful to separate these steps in order to avoid duplicate work when calculating the
 * edpl for many @link Pquery Pqueries@endlink at a time.
 *
 * @p node_distances has to be the result of node_branch_length_distance_matrix().
 */
double edpl( Pquery const& pquery, utils::Matrix<double> const& node_distances );

/**
 * @brief Calculate the @link edpl( Sample const&, Pquery const& ) edpl()@endlink
 * for all @link Pquery Pqueries@endlink in the Sample.
 *
 * @p node_distances has to be the result of node_branch_length_distance_matrix().
 */
std::vector<double> edpl( Sample const& sample, utils::Matrix<double> const& node_distances );

/**
 * @brief Calculate the EDPL uncertainty values for a Pquery.
 *
 * See http://matsen.github.io/pplacer/generated_rst/guppy_edpl.html for more information.
 *
 * This function expects a Pquery and the Sample it belongs to. This is necessary in order to
 * get the Tree of the Sample and calculate distances between its Nodes.
 */
double edpl( Sample const& sample, Pquery const& pquery );

/**
 * @brief Calculate the @link edpl( Sample const&, Pquery const& ) edpl()@endlink
 * for all @link Pquery Pqueries@endlink in the Sample.
 *
 * See http://matsen.github.io/pplacer/generated_rst/guppy_edpl.html for more information.
 */
std::vector<double> edpl( Sample const& sample );

// =================================================================================================
//     Pairwise Distance
// =================================================================================================

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

// =================================================================================================
//     Variance
// =================================================================================================

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
