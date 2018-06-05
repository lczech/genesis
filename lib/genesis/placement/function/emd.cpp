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

#include "genesis/placement/function/emd.hpp"

#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/masses.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/mass_tree/emd.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/containers/matrix.hpp"

#include <cassert>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

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
    auto const avg_length_tree = tree::average_branch_length_tree( tset );

    // Create an EMD tree from the average branch length tree, then calc the EMD.
    auto mass_tree = tree::convert_default_tree_to_mass_tree( avg_length_tree );

    // Use the sum of masses as normalization factor for the masses.
    double totalmass_l = total_placement_mass_with_multiplicities( lhs );
    double totalmass_r = total_placement_mass_with_multiplicities( rhs );

    // Copy masses of both samples to the EMD tree, with different signs.
    double const pendant_work_l = add_sample_to_mass_tree( lhs, +1.0, totalmass_l, mass_tree );
    double const pendant_work_r = add_sample_to_mass_tree( rhs, -1.0, totalmass_r, mass_tree );

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
    auto const mass_trees = convert_sample_set_to_mass_trees( sample_set );

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
