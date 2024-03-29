/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/function/measures.hpp"

#include "genesis/placement/function/distances.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/masses.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/pquery/plain.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/common_tree/distances.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/iterator/node_links.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"

#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Expected Distance between Placement Locations
// =================================================================================================

double edpl( Pquery const& pquery, utils::Matrix<double> const& node_distances )
{
    double result = 0.0;

    #pragma omp parallel for
    for( size_t i = 0; i < pquery.placement_size(); ++i ) {

        #pragma omp parallel for
        for( size_t j = i + 1; j < pquery.placement_size(); ++j ) {

            auto const& place_i = pquery.placement_at(i);
            auto const& place_j = pquery.placement_at(j);

            auto const dist = placement_distance( place_i, place_j, node_distances );

            #pragma omp atomic
            result += place_i.like_weight_ratio * place_j.like_weight_ratio * dist;
        }
    }
    return 2 * result;
}

std::vector<double> edpl( Sample const& sample, utils::Matrix<double> const& node_distances )
{
    // Prepare result (facilitate copy elision).
    auto result = std::vector<double>( sample.size(), 0 );

    // Fill result vector.
    #pragma omp parallel for
    for( size_t qi = 0; qi < sample.size(); ++qi ) {
        auto const& pquery = sample.at( qi );
        result[qi] = edpl( pquery, node_distances );
    }
    return result;
}

double edpl( Sample const& sample, Pquery const& pquery )
{
    auto const node_distances = node_branch_length_distance_matrix( sample.tree() );
    return edpl( pquery, node_distances );
}

std::vector<double> edpl( Sample const& sample )
{
    auto const node_distances = node_branch_length_distance_matrix( sample.tree() );
    return edpl( sample, node_distances );
}

// =================================================================================================
//     Pairwise Distance
// =================================================================================================

double pairwise_distance(
    const Sample& smp_a,
    const Sample& smp_b,
    bool          with_pendant_length
) {
    if (!compatible_trees(smp_a, smp_b)) {
        throw std::invalid_argument("pairwise_distance: Incompatible trees.");
    }

    // Init.
    double sum = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. this gives a tremendous speedup!
    std::vector<PqueryPlain> const pqueries_a = plain_queries( smp_a );
    std::vector<PqueryPlain> const pqueries_b = plain_queries( smp_b );

    // Calculate a matrix containing the pairwise distance between all nodes. This way, we
    // do not need to search a path between placements every time. We use the tree of the first smp
    // here, ignoring branch lengths on tree b.
    // FIXME this might be made better by using average or so in the future.
    auto node_distances = node_branch_length_distance_matrix(smp_a.tree());

    for (const PqueryPlain& pqry_a : pqueries_a) {
        for (const PqueryPlain& pqry_b : pqueries_b) {
            auto dist = pquery_distance( pqry_a, pqry_b, node_distances, with_pendant_length );
            dist *= pqry_a.multiplicity * pqry_b.multiplicity;
            sum += dist;
        }
    }

    // Return normalized value.
    return sum / total_placement_mass_with_multiplicities( smp_a ) / total_placement_mass_with_multiplicities( smp_b );
}

// =================================================================================================
//     Variance
// =================================================================================================

/**
 * @brief Internal function that calculates the sum of distances contributed by one pquery for
 * the variance. See variance() for more information.
 *
 * This function is intended to be called by variance() -- it is not a stand-alone function.
 */
static double variance_partial_ (
    PqueryPlain const&              pqry_a,
    std::vector<PqueryPlain> const& pqrys_b,
    utils::Matrix<double> const&    node_distances,
    bool                            with_pendant_length
) {
    double partial = 0.0;

    for (const PqueryPlain& pqry_b : pqrys_b) {
        // Check whether it is same pquery (a=b, nothing to do, as their distance is zero),
        // or a pair of pqueries that was already calculated (a>b, skip it also).
        if (pqry_a.index >= pqry_b.index) {
            continue;
        }
        double dist = pquery_distance(pqry_a, pqry_b, node_distances, with_pendant_length);
        dist *= pqry_a.multiplicity * pqry_b.multiplicity;
        partial += dist * dist;
    }

    return partial;
}

double variance(
    const Sample& smp,
    bool          with_pendant_length
) {
    // Init.
    double variance = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. This gives a tremendous speedup!
    std::vector<PqueryPlain> vd_pqueries = plain_queries( smp );

    // Also, calculate a matrix containing the pairwise distance between all nodes. this way, we
    // do not need to search a path between placements every time.
    auto node_distances = node_branch_length_distance_matrix(smp.tree());

    // Do a pairwise calculation on all placements.
    // int progress = 0;
    #pragma omp parallel for
    for( size_t i = 0; i < vd_pqueries.size(); ++i ) {
        // LOG_PROG(++progress, vd_pqueries.size()) << "of Variance() finished.";

        PqueryPlain const& pqry_a = vd_pqueries[i];
        variance += variance_partial_(pqry_a, vd_pqueries, node_distances, with_pendant_length);
    }

    // Calculate normalizing factor. Should be the same value as given by placement_mass(),
    // however this calculation is probably faster, as we already have the plain values at hand.
    double mass = 0.0;
    for( auto const& pqry : vd_pqueries ) {
        for( auto const& place : pqry.placements ) {
            mass += place.like_weight_ratio * pqry.multiplicity;
        }
    }

    // Return the normalized value.
    return ((variance / mass) / mass);
}

} // namespace placement
} // namespace genesis
