#ifndef GENESIS_PLACEMENT_FUNCTION_EMD_H_
#define GENESIS_PLACEMENT_FUNCTION_EMD_H_

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

#include <utility>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace placement {

    class Sample;
    class SampleSet;

}

namespace utils {

    template<typename T>
    class Matrix;

}

namespace placement {

// =================================================================================================
//     Earth Movers Distance
// =================================================================================================

/**
 * @brief Calculate the earth mover's distance between two Sample%s.
 *
 * This function interprets the @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink
 * of the PqueryPlacement%s as masses distributed along the branches of a tree. It then calculates
 * the earth mover's distance between those masses for the distrubitons induced by the two given
 * Sample%s.
 *
 * In order to do so, first, a tree with the average branch lengths of the two PlacementTree%s is
 * calculated. This is because of numerical issues that might yield different branch lengths.
 * This necessiates that the trees have the same topology. If not, an std::runtime_error is thrown.
 * The masses are then distributed on this tree, using the same relative position on their branches
 * that they had in their original trees.
 *
 * The calculation furthermore takes the @link PqueryName::multiplicity multiplicities@endlink of
 * the @link Pquery Pqueries@endlink into account. That means, pqueries with higher (total)
 * multiplicity have a higher influence on the calculated distance.
 *
 * As the two Sample%s might have a different total number of @link Pquery Pqueries@endlink, the
 * masses of the Samples are first normalized to 1.0, using all the
 * @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink and
 * @link PqueryName::multiplicity multiplicities@endlink of the Pqueries.
 * As a consequence, the resulting distance will not reflect the total number of Pqueries, but only
 * their relative (normalized) distrubution on the tree.
 *
 * Furthermore, the parameter @p p is used to control the influence of mass and distance, with
 * `0.0 < p < inf`, and default `p == 1.0`, which is the neutral case.
 * A larger @p p increases the impact of distance traveled, while a smaller @p p emphasizes
 * differences of mass.
 *
 * See @link tree::earth_movers_distance( MassTree const& lhs, MassTree const& rhs, double )
 * earth_movers_distance( MassTree const&, MassTree const& )@endlink for more information on the
 * actual distance calculation and details on the parameter @p p.
 */
double earth_movers_distance (
    Sample const& lhs,
    Sample const& rhs,
    double const  p = 1.0,
    bool const    with_pendant_length = false
);

/**
 * @brief Calculate the pairwise Earth Movers Distance for all Sample%s in a SampleSet.
 *
 * The result is a pairwise distance @link utils::Matrix Matrix@endlink using the indices of the
 * Sample%s in the SampleSet.
 * See @link earth_movers_distance( const Sample&, const Sample&, double, bool )
 * earth_movers_distance( Sample const&, Sample const&, ... )@endlink for details on this distance
 * measure on Sample%s, and see
 * @link tree::earth_movers_distance( MassTree const& lhs, MassTree const& rhs, double )
 * earth_movers_distance( MassTree const&, MassTree const& )@endlink for more information on the
 * actual distance calculation, and the parameter @p p.
 */
utils::Matrix<double> earth_movers_distance(
    SampleSet const& sample_set,
    double const     p = 1.0,
    bool const       with_pendant_length = false
);

} // namespace placement
} // namespace genesis

#endif // include guard
