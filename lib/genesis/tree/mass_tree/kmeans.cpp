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

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Mass Tree Kmeans
// =================================================================================================

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
    for( auto& centroid : centroids ) {
        mass_tree_clear_masses( centroid );
    }

    // This function is only called from within the run() function, which already
    // checks this condition. So, simply assert it here, instead of throwing.
    assert( data.size() == assignments.size() );

    // Work through the data and assigments and accumulate.
    for( size_t i = 0; i < data.size(); ++i ) {

        // Shorthands.
        auto const& datum    = data[ i ];
        auto&       centroid = centroids[ assignments[i] ];

        // Accumulate centroid.
        mass_tree_merge_trees_inplace( centroid, datum );
    }
}

double MassTreeKmeans::distance( Point const& lhs, Point const& rhs ) const
{
    auto copy = lhs;
    mass_tree_reverse_signs( copy );
    mass_tree_merge_trees_inplace( copy, rhs );
    return earth_movers_distance( copy ).first;
}

} // namespace tree
} // namespace genesis
