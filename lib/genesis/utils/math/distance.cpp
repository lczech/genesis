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

#include <genesis/utils/math/distance.hpp>

#include <genesis/utils/container/matrix/operator.hpp>
#include <genesis/utils/threading/thread_pool.hpp>
#include <genesis/utils/threading/thread_function.hpp>

#include <cassert>

namespace genesis {
namespace utils {
namespace math {

// =============================================================================
//     Distance Matrices
// =============================================================================

genesis::utils::containers::Matrix<double> p_norm_distance_matrix(
    genesis::utils::containers::Matrix<double> const& data,
    double p
) {
    // Init result matrix.
    auto result = genesis::utils::containers::Matrix<double>( data.rows(), data.rows(), 0.0 );

    // We only need to calculate the upper triangle. Get the number of indices needed
    // to describe this triangle.
    size_t const max_k = genesis::utils::containers::triangular_size( data.rows() );

    // Calculate.
    genesis::utils::threading::parallel_for( 0, max_k, [&]( size_t k )
    {
        // For the given linear index, get the actual position in the Matrix.
        auto const ij = genesis::utils::containers::triangular_indices( k, data.rows() );
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

genesis::utils::containers::Matrix<double> manhattan_distance_matrix(
    genesis::utils::containers::Matrix<double> const& data
) {
    return p_norm_distance_matrix( data, 1.0 );
}

genesis::utils::containers::Matrix<double> euclidean_distance_matrix(
    genesis::utils::containers::Matrix<double> const& data
) {
    return p_norm_distance_matrix( data, 2.0 );
}

genesis::utils::containers::Matrix<double> maximum_distance_matrix(
    genesis::utils::containers::Matrix<double> const& data
) {
    return p_norm_distance_matrix( data, std::numeric_limits<double>::infinity() );
}

} // namespace math
} // namespace utils
} // namespace genesis
