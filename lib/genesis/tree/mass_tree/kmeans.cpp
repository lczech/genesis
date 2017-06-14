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

#include "genesis/tree/mass_tree/kmeans.hpp"

#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Settings
// =================================================================================================

size_t MassTreeKmeans::accumulate_centroid_masses() const
{
    return accumulate_centroid_masses_;
}

void MassTreeKmeans::accumulate_centroid_masses( size_t value )
{
    accumulate_centroid_masses_ = value;
}

// =================================================================================================
//     Mass Tree Kmeans
// =================================================================================================

void MassTreeKmeans::pre_loop_hook(
    std::vector<Point>  const& data,
    std::vector<size_t>&       assignments,
    std::vector<Point>&        centroids
) {
    (void) data;
    (void) assignments;

    if( accumulate_centroid_masses_ ) {
        for( auto& mt : centroids ) {
            mass_tree_center_masses_on_branches_averaged( mt );
        }
    }
}

bool MassTreeKmeans::data_validation( std::vector<Point> const& data ) const
{
    // Check if all Trees have the correct data types.
    for( auto const& tree : data ) {
        if( ! tree_data_is< MassTreeNodeData, MassTreeEdgeData >( tree ) ) {
            throw std::invalid_argument( "Trees for Kmeans do not have MassTree data types." );
        }
    }

    // Check if all Trees have the same topology. Important to caluclate EMD.
    // adapted from all_identical_topology()
    for (size_t i = 1; i < data.size(); i++) {
        if( ! identical_topology( data[i-1], data[i] )) {
            throw std::invalid_argument( "Trees for Kmeans do not have identical topologies." );
        }
    }

    return true;
}

void MassTreeKmeans::update_centroids(
    std::vector<Point>  const& data,
    std::vector<size_t> const& assignments,
    std::vector<Point>&        centroids
) {
    // Shorthand.
    auto const k = centroids.size();

    // Clear all centroid masses from previous iteration.
    #pragma omp parallel for
    for( size_t c = 0; c < k; ++c ) {
        mass_tree_clear_masses( centroids[ c ] );
    }

    // This function is only called from within the run() function, which already
    // checks this condition. So, simply assert it here, instead of throwing.
    assert( data.size() == assignments.size() );

    #ifdef GENESIS_OPENMP

        // Parallelize over centroids
        #pragma omp parallel for
        for( size_t c = 0; c < k; ++c ) {

            // Thread-local data.
            auto& centroid = centroids[ c ];
            size_t count = 0;

            // Work through the data and assigments and accumulate...
            for( size_t d = 0; d < data.size(); ++d ) {

                // ( Check correct assignments. )
                assert( assignments[ d ] < k );

                // ... but only the relevant parts.
                if( assignments[ d ] != c ) {
                    continue;
                }

                // Accumulate centroid.
                mass_tree_merge_trees_inplace( centroid, data[ d ] );
                ++count;
            }

            // Make sure that the sum of masses is okay. This is a bit wibbly wobbly because
            // of the double equality check, but we have to live with it.
            assert( utils::almost_equal_relative(
                count, mass_tree_sum_of_masses( centroid ), 0.00001
            ));

            mass_tree_normalize_masses( centroid );
            if( accumulate_centroid_masses_ == 1 ) {
                mass_tree_center_masses_on_branches_averaged( centroid );
            } else if( accumulate_centroid_masses_ > 1 ) {
                mass_tree_binify_masses( centroid, accumulate_centroid_masses_ );
            }
        }

    #else

        // In the serial case, we only want to traverse the data once.

        // Count how many mass trees are accumulated per centroid.
        auto counts = std::vector<size_t>( k, 0 );

        // Work through the data and assigments and accumulate.
        for( size_t d = 0; d < data.size(); ++d ) {

            // Check correct assignments.
            assert( assignments[ d ] < k );

            // Accumulate centroid.
            mass_tree_merge_trees_inplace( centroids[ assignments[ d ] ], data[ d ] );
            ++counts[ assignments[ d ] ];
        }

        // Normalize the centroids.
        for( size_t c = 0; c < k; ++c ) {

            // Make sure that the sum of masses is okay. This is a bit wibbly wobbly because
            // of the double equality check, but we have to live with it.
            assert( utils::almost_equal_relative(
                counts[ c ], mass_tree_sum_of_masses( centroids[ c ] ), 0.00001
            ));

            mass_tree_normalize_masses( centroids[ c ] );
            if( accumulate_centroid_masses_ == 1 ) {
                mass_tree_center_masses_on_branches_averaged( centroids[ c ] );
            } else if( accumulate_centroid_masses_ > 1 ) {
                mass_tree_binify_masses( centroids[ c ], accumulate_centroid_masses_ );
            }
        }

    #endif
}

double MassTreeKmeans::distance( Point const& lhs, Point const& rhs ) const
{
    return earth_movers_distance( lhs, rhs );
}

} // namespace tree
} // namespace genesis
