#ifndef GENESIS_PLACEMENT_FUNCTION_MASSES_H_
#define GENESIS_PLACEMENT_FUNCTION_MASSES_H_

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
 * @ingroup placement
 */

#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Mulitplicities
// =================================================================================================

/**
 * @brief Return the sum of all @link PqueryName::multiplicity multiplicities @endlink of the
 * Pquery.
 */
double total_multiplicity( Pquery const& pqry );

/**
 * @brief Return the sum of all @link PqueryName::multiplicity multiplicities @endlink of all the
 * @link Pquery Pqueries@endlink of the Sample.
 */
double total_multiplicity( Sample const& sample );

// =================================================================================================
//     Masses
// =================================================================================================

/**
 * @brief Return a vector that contains the sum of the masses of the PqueryPlacement%s per
 * @link ::PlacementTreeEdge edge@endlink of the @link ::PlacementTree tree@endlink of the Sample,
 * using the @link PqueryName::multiplicity multiplicities @endlink as factors.
 */
std::vector<double> placement_mass_per_edges_with_multiplicities( Sample const& sample );

/**
 * @brief Return a Matrix that contains the placement masses per edge, using the
 * @link PqueryName::multiplicity multiplicities @endlink as factors.
 *
 * See placement_mass_per_edges_with_multiplicities( Sample const& ) for details.
 * The Matrix is simply a row-wise concatenation of this vector function.
 */
utils::Matrix<double> placement_mass_per_edges_with_multiplicities( SampleSet const& sample_set );

/**
 * @brief Get the mass of all PqueryPlacement%s of the Sample, using the
 * @link PqueryName::multiplicity multiplicities @endlink as factors.
 *
 * This function returns the summed mass of all PqueryPlacement%s in all
 * @link Pquery Pqueries @endlink of the
 * given Sample, where mass is measured by `like_weight_ratio`, and the mass of each Pquery is
 * multiplied by the sum of the @link PqueryName::multiplicity multiplicities @endlink of this
 * Pquery.
 *
 * This method returns the same value as total_placement_mass_without_multiplicities()
 * in case that the `multiplicity` is left at its default value of 1.0 for all Pqueries.
 */
double total_placement_mass_with_multiplicities( Sample const& smp );

// =================================================================================================
//     Masses without Multiplicities
// =================================================================================================

/**
 * @brief Return a vector that contains the sum of the masses of the PqueryPlacement%s per
 * @link ::PlacementTreeEdge edge@endlink of the @link ::PlacementTree tree@endlink of the Sample.
 *
 * The mass is measured in @link PqueryPlacement::like_weight_ratio `like_weight_ratio`@endlink.
 * The vector is indexed using the @link PlacementTreeEdge::index() index@endlink of the edges.
 *
 * Note that this does not take multiplicities into account; that is, it simply gives the sum
 * of all `like_weight_ratio`s. See placement_mass_per_edges_with_multiplicities() for
 * a version of this function that also takes
 * the @link PqueryName::multiplicity multiplicities @endlink of the Pqueries into account.
 */
std::vector<double> placement_mass_per_edge_without_multiplicities( Sample const& sample );

/**
 * @brief Return a Matrix that contains the placement masses per edge.
 *
 * See placement_mass_per_edge_without_multiplicities( Sample const& ) for details.
 * The Matrix is simply a row-wise concatenation of this vector function.
 */
utils::Matrix<double> placement_mass_per_edge_without_multiplicities( SampleSet const& sample_set );

/**
 * @brief Get the summed mass of all PqueryPlacement%s in all @link Pquery Pqueries @endlink of the
 * given Sample, where mass is measured by the
 * @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink of the PqueryPlacement%s.
 *
 * Be aware that this function only gives the pure sum of the `like_weight_ratio`s. See
 * total_placement_mass_with_multiplicities() for a version of this function, which also takes
 * the @link PqueryName::multiplicity multiplicities @endlink of the Pqueries into account.
 */
double total_placement_mass_without_multiplicities(  Sample const& smp );

} // namespace placement
} // namespace genesis

#endif // include guard
