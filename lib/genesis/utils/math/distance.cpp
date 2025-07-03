/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/distance.hpp"

#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/threading/thread_pool.hpp"
#include "genesis/utils/threading/thread_functions.hpp"

#include <cassert>

namespace genesis {
namespace utils {

// =============================================================================
//     Distance Matrices
// =============================================================================

Matrix<double> p_norm_distance_matrix( Matrix<double> const& data, double p )
{
    // Init result matrix.
    auto result = utils::Matrix<double>( data.rows(), data.rows(), 0.0 );

    // We only need to calculate the upper triangle. Get the number of indices needed
    // to describe this triangle.
    size_t const max_k = utils::triangular_size( data.rows() );

    // Calculate.
    utils::parallel_for( 0, max_k, [&]( size_t k )
    {
        // For the given linear index, get the actual position in the Matrix.
        auto const ij = utils::triangular_indices( k, data.rows() );
        auto const i = ij.first;
        auto const j = ij.second;

        // Calculate EMD and fill symmetric Matrix.
        auto const dist = p_norm_distance(
            data.row(i).begin(), data.row(i).end(),
            data.row(j).begin(), data.row(j).end(),
            p
        );

        assert( result( i, j ) == 0.0 );
        assert( result( j, i ) == 0.0 );
        result( i, j ) = dist;
        result( j, i ) = dist;
    });

    // If no threads are available at all, use serial version.
    // The result is symmetric - we only calculate the upper triangle.
    // for( size_t i = 0; i < data.rows() - 1; ++i ) {
    //     for( size_t j = i + 1; j < data.cols(); ++j ) {
    //         auto const dist = p_norm_distance(
    //             data.row(i).begin(), data.row(i).end(),
    //             data.row(j).begin(), data.row(j).end(),
    //             p
    //             );
    //         result( i, j ) = dist;
    //         result( j, i ) = dist;
    //     }
    // }

    return result;
}

Matrix<double> manhattan_distance_matrix( Matrix<double> const& data )
{
    return p_norm_distance_matrix( data, 1.0 );
}

Matrix<double> euclidean_distance_matrix( Matrix<double> const& data )
{
    return p_norm_distance_matrix( data, 2.0 );
}

Matrix<double> maximum_distance_matrix( Matrix<double> const& data )
{
    return p_norm_distance_matrix( data, std::numeric_limits<double>::infinity() );
}

} // namespace utils
} // namespace genesis
