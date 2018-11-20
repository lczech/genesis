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
 * @ingroup utils
 */

#include "genesis/utils/math/distance.hpp"

#include "genesis/utils/containers/matrix/operators.hpp"

#include <cassert>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace utils {

// =============================================================================
//     Distance Matrices
// =============================================================================

Matrix<double> p_norm_distance_matrix( Matrix<double> const& data, double p )
{
    // Init result matrix.
    auto result = utils::Matrix<double>( data.rows(), data.rows(), 0.0 );

    // Parallel specialized code.
    #ifdef GENESIS_OPENMP

        // We only need to calculate the upper triangle. Get the number of indices needed
        // to describe this triangle.
        size_t const max_k = utils::triangular_size( data.rows() );

        // Calculate.
        #pragma omp parallel for
        for( size_t k = 0; k < max_k; ++k ) {

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
        }

    // If no threads are available at all, use serial version.
    #else

        for( size_t i = 0; i < data.rows() - 1; ++i ) {

            // The result is symmetric - we only calculate the upper triangle.
            for( size_t j = i + 1; j < data.cols(); ++j ) {

                auto const dist = p_norm_distance(
                    data.row(i).begin(), data.row(i).end(),
                    data.row(j).begin(), data.row(j).end(),
                    p
                 );
                result( i, j ) = dist;
                result( j, i ) = dist;
            }
        }

    #endif

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
