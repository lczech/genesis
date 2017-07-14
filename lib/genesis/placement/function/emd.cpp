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

#include "genesis/placement/function/emd.hpp"

#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/mass_tree/emd.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/math/matrix.hpp"

#include <cassert>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Mass Tree Conversion
// =================================================================================================

/**
 * @brief Local helper function to copy masses from a Sample to a
 * @link tree::MassTree MassTree@endlink.
 *
 * Helper function that copies the masses from a Sample to a MassTree.
 * It furthermore returns the amount of work needed to move the masses from their pendant
 * position to the branch (this result is only used if `with_pendant_length` is `true` in the
 * calculation functions).
 *
 * Yep, this function does quite a lot of different things. But it's faster this way and it is
 * only a local function. Don't judge me.
 */
double add_sample_to_mass_tree(
    Sample const& smp, double const sign, double const scaler, tree::MassTree& target
) {
    double pendant_work = 0.0;

    for( auto const& pqry : smp.pqueries() ) {
        double multiplicity = total_multiplicity( pqry );

        for( auto const& place : pqry.placements() ) {
            auto& edge = target.edge_at( place.edge().index() );

            // Use the relative position of the mass on its original branch to put it to the
            // same position relative to its new branch.
            double position
                = place.proximal_length
                / place.edge().data<PlacementEdgeData>().branch_length
                * edge.data<tree::MassTreeEdgeData>().branch_length;

            // Add the mass at that position, normalized and using the sign.
            edge.data<tree::MassTreeEdgeData>().masses[ position ]
                += sign * place.like_weight_ratio * multiplicity / scaler;

            // Accumulate the work we need to do to move the masses from their pendant
            // positions to the branches.
            pendant_work += place.like_weight_ratio * multiplicity * place.pendant_length / scaler;
        }
    }

    return pendant_work;
}

std::pair< tree::MassTree, double > convert_to_mass_tree( Sample const& sample )
{
    auto mass_tree = tree::convert_default_tree_to_mass_tree( sample.tree() );
    double const total_mass = total_placement_mass_with_multiplicities( sample );
    double const pend_work = add_sample_to_mass_tree(
        sample, +1.0, total_mass, mass_tree
    );
    return { std::move( mass_tree ), pend_work };
}

std::pair<
    std::vector<tree::MassTree>,
    std::vector<double>
> convert_to_mass_trees( SampleSet const& sample_set )
{
    // Build an average branch length tree for all trees in the SampleSet.
    // This also serves as a check whether all trees in the set are compatible with each other,
    // as average_branch_length_tree() throws if the trees have different topologies.
    // Then, turn the resulting tree into a MassTree.
    tree::TreeSet avg_tree_set;
    for( auto const& smp : sample_set ) {
        avg_tree_set.add( "", smp.sample.tree() );
    }
    auto const mass_tree = tree::convert_default_tree_to_mass_tree(
        tree::average_branch_length_tree( avg_tree_set )
    );
    avg_tree_set.clear();
    // TODO if we introduce an avg tree calculation that accepts an iterator, we do not need
    // TODO to create a copied tree set of all trees here.

    // Prepare mass trees for all Samples, by copying the average mass tree.
    // This massively speeds up the calculations (at the cost of extra storage for all the trees).
    auto mass_trees = std::vector<tree::MassTree>( sample_set.size(), mass_tree );

    // Also, prepare a vector to store the pendant works.
    auto pend_works = std::vector<double>( sample_set.size(), 0.0 );

    // Add the placement mass of each Sample to its MassTree.
    #pragma omp parallel for schedule( dynamic )
    for( size_t i = 0; i < sample_set.size(); ++i ) {
        // Get the total sum of placement masses for the sample...
        double const total_mass = total_placement_mass_with_multiplicities( sample_set[i].sample );

        // ... and use it as scaler to add the mass to the mass tree for this sample.
        double const pend_work = add_sample_to_mass_tree(
            sample_set[i].sample, +1.0, total_mass, mass_trees[i]
        );

        // Also, store the pend work.
        pend_works[ i ] = pend_work;
    }

    return { std::move( mass_trees ), std::move( pend_works ) };
}

// =================================================================================================
//     Earth Movers Distance
// =================================================================================================

// -------------------------------------------------------------------------
//     EMD between two Samples
// -------------------------------------------------------------------------

double earth_movers_distance (
    Sample const& lhs,
    Sample const& rhs,
    double const  p,
    bool const    with_pendant_length
) {
    // Get a tree with the average branch lengths of both provided trees.
    // This function also throws in case the trees have different topologies.
    tree::TreeSet tset;
    tset.add( "lhs", lhs.tree() );
    tset.add( "rhs", rhs.tree() );
    auto avg_length_tree = tree::average_branch_length_tree( tset );

    // Create an EMD tree from the average branch length tree, then calc the EMD.
    auto mass_tree = tree::convert_default_tree_to_mass_tree( avg_length_tree );

    // Use the sum of masses as normalization factor for the masses.
    double totalmass_l = total_placement_mass_with_multiplicities( lhs );
    double totalmass_r = total_placement_mass_with_multiplicities( rhs );

    // Copy masses of both samples to the EMD tree, with different signs.
    double pendant_work_l = add_sample_to_mass_tree( lhs, +1.0, totalmass_l, mass_tree );
    double pendant_work_r = add_sample_to_mass_tree( rhs, -1.0, totalmass_r, mass_tree );

    // Calculate EMD.
    double work = tree::earth_movers_distance( mass_tree, p ).first;

    // If we also want the amount of work that was needed to move the placement masses from their
    // pendant position to the branch, we need to add those values.
    if( with_pendant_length ) {
        work += pendant_work_l + pendant_work_r;
    }

    return work;
}

// -------------------------------------------------------------------------
//     EMD matrix for a SampleSet
// -------------------------------------------------------------------------

utils::Matrix<double> earth_movers_distance(
    SampleSet const& sample_set,
    double const     p,
    bool const       with_pendant_length
) {
    // Get mass tress and the pendant work that was needed to create them.
    auto mass_trees = convert_to_mass_trees( sample_set );

    // Calculate the pairwise distance matrix.
    auto result = tree::earth_movers_distance( mass_trees.first, p );

    // If needed, add the pend work for each matrix position.
    if( with_pendant_length ) {
        assert( mass_trees.second.size() == sample_set.size() );
        for( size_t i = 0; i < sample_set.size(); ++i ) {
            for( size_t j = 0; j < sample_set.size(); ++j ) {
                result( i, j ) += mass_trees.second[ i ] + mass_trees.second[ j ];
            }
        }
    }

    return result;
}

} // namespace placement
} // namespace genesis
