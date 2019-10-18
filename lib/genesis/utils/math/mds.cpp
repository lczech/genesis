/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

/*
    The implementation is based on the [SimpleMatrix library](https://sites.google.com/site/simpmatrix/)
    by [Quan Wang](https://github.com/wq2012), with his explicit permission to use this code here.
    The copyright (C) of the implementation is held by Quan Wang, 2013.
    We adapted the implementation to our genesis::utils::Matrix class and changed the error reporting
    mechanism to exceptions. For further details, see the Acknowledgements section of the documentation.
*/

#include "genesis/utils/math/mds.hpp"

#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/distance.hpp"
#include "genesis/utils/math/matrix.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     MDS Algorithms
// ================================================================================================

constexpr double MDS_EPSILON = 0.0000001;

static Matrix<double> multi_dimensional_scaling_ucf(
    Matrix<double> const& distances,
    Matrix<double> const& initial_values,
    size_t                dimensions,
    size_t                iterations
) {
    // This function is local, and we already checked the conditions below.
    // Thus, just assert them here again.
    assert( is_square( distances ));
    assert( dimensions >= 1 );
    assert( iterations >= 1 );
    assert( initial_values.rows() == distances.rows() );
    assert( initial_values.cols() == dimensions );

    // Algorithm constants
    double const learning_rate = 0.05;
    double const r_metric = 2.0;
    size_t const n = distances.rows();

    // Resutl matrix
    auto result = initial_values;
    assert( result.rows() == n );
    assert( result.cols() == dimensions );

    // Temporary matrices
    auto dh = Matrix<double>( n, n, 0.0 );
    auto pmat = Matrix<double>( n, dimensions, 0.0 );
    auto dhdum = std::vector<double>( n, 0.0 );
    auto dhmat = Matrix<double>( n - 1, dimensions, 0.0 );

    // Make a matrix with column-wise permutations of consecutive numbers.
    auto perms = Matrix<size_t>( n, iterations );
    for( size_t j = 0; j < perms.cols(); ++j ) {
        std::iota( perms.col(j).begin(), perms.col(j).end(), 0 );
        std::shuffle( perms.col(j).begin(), perms.col(j).end(), Options::get().random_engine() );
    }

    // Run the iterations.
    for( size_t it = 0; it < iterations; ++it ) {

        // Work on each vector in a randomly permuted order
        for( size_t rp = 0; rp < n; ++rp ) {
            auto const m = perms( rp, it );
            assert( m < n );

            for( size_t i = 0; i < n; ++i ) {
                for( size_t j = 0; j < dimensions; ++j ) {
                    pmat( i, j ) = result( m, j ) - result( i, j );
                }
            }
            for( size_t i = 0; i < n; ++i ) {
                double temp = 0.0;
                for( size_t j = 0; j < dimensions; ++j ) {
                    temp += std::pow( std::abs( pmat( i, j )), r_metric );
                }
                dhdum[ i ] = std::pow( temp, 1.0 / r_metric );
            }
            for( size_t i = 0; i < n; ++i ) {
                if( i == m ) {
                    continue;
                }
                dh( m, i ) = dhdum[ i ];
                dh( i, m ) = dhdum[ i ];
            }
            for( size_t i = 0; i < n - 1; ++i ) {
                size_t const ii = ( i < m ) ? i : i + 1;
                double const temp
                    = learning_rate
                    * ( dhdum[ ii ] - distances( ii, m ) )
                    * std::pow( dhdum[ ii ], 1.0 - r_metric )
                ;
                for( size_t j = 0; j < dimensions; ++j ) {
                    dhmat( i, j ) = temp;
                }
            }
            for( size_t i = 0; i < n - 1; ++i ) {
                size_t const ii = ( i < m ) ? i : i + 1;
                for( size_t j = 0; j < dimensions; ++j ) {
                    double temp = result( ii, j );
                    temp += dhmat( i, j )
                        * std::pow( std::abs( pmat( ii, j )), r_metric - 1.0 )
                        * signum<double>( pmat( ii, j ))
                    ;
                    result( ii, j ) = temp;
                }
            }
        }
    }

    return result;
}

static Matrix<double> multi_dimensional_scaling_smacof(
    Matrix<double> const& distances,
    Matrix<double> const& initial_values,
    size_t                dimensions,
    size_t                iterations
) {
    // This function is local, and we already checked the conditions below.
    // Thus, just assert them here again.
    assert( is_square( distances ));
    assert( dimensions >= 1 );
    assert( iterations >= 1 );
    assert( initial_values.rows() == distances.rows() );
    assert( initial_values.cols() == dimensions );
    (void) dimensions;

    // Prepare result and a copy of it for the updating process.
    auto result = initial_values;
    auto previous = initial_values;

    // Get initial interpoint distances of the initial values.
    auto idists = euclidean_distance_matrix( result );
    assert( idists.rows() == distances.rows() );
    assert( idists.cols() == distances.cols() );

    // Init empty stress matrix.
    auto stress = Matrix<double>( distances.rows(), distances.cols() );

    // Run the iterations.
    for( size_t it = 0; it < iterations; ++it ) {

        // Calcualte stress.
        for( size_t i = 0; i < distances.rows(); ++i ) {
            for( size_t j = 0; j < distances.cols(); ++j ) {
                if( i == j || std::abs( idists(i,j)) < MDS_EPSILON ) {
                    stress( i, j ) = 0.0;
                } else {
                    stress( i, j ) = - distances( i, j ) / idists( i, j );
                }
            }
        }

        // Calculate stress diagonal.
        for( size_t j = 0; j < distances.cols(); ++j ) {
            double temp = 0.0;
            for( size_t i = 0; i < distances.rows(); ++i ) {
                temp += stress( i, j );
            }
            stress( j, j ) = - temp;
        }

        // Update result.
        assert( stress.rows() == result.rows() );
        assert( previous.rows() == stress.cols() );
        assert( previous.cols() == result.cols() );
        for( size_t i = 0; i < result.rows(); ++i ) {
            for( size_t j = 0; j < result.cols(); ++j ) {
                double temp = 0.0;
                for( size_t k = 0; k < stress.cols(); ++k ) {
                    temp += stress( i, k ) * previous( k, j );
                }
                result( i, j ) = temp / static_cast<double>( distances.rows() );
            }
        }

        // Update intermediate data.
        idists = euclidean_distance_matrix( result );
        previous = result;
    }

    return result;
}

// ================================================================================================
//     MDS API Functions
// ================================================================================================

Matrix<double> multi_dimensional_scaling(
    Matrix<double> const& distances,
    size_t                dimensions,
    size_t                iterations,
    MdsAlgorithm          algorithm
) {
    // We skip all error checks here, because they will be done in the other function anyway.

    // Make a random init matrix in the range -0.5 to 0.5, and get the mean of the values
    // as if they were in the range 0.0 to 1.0. We need this for proper normalization.
    auto initial = Matrix<double>( distances.rows(), dimensions );
    auto& engine = Options::get().random_engine();
    auto distrib = std::uniform_real_distribution<double>( 0.0, 1.0 );
    double mean = 0.0;
    for( auto& e : initial ) {
        auto const r = distrib( engine );
        mean += r;
        e = r - 0.5;
    }
    mean /= initial.size();
    assert( 0.0 <= mean && mean <= 1.0 );

    // Normalize using the mean.
    for( auto& e : initial ) {
        e *= 0.1 * mean / ( 1.0 / 3.0 * std::sqrt( static_cast<double>( dimensions )));
    }

    // Run the algo.
    return multi_dimensional_scaling( distances, initial, dimensions, iterations, algorithm );
}

Matrix<double> multi_dimensional_scaling(
    Matrix<double> const& distances,
    Matrix<double> const& initial_values,
    size_t                dimensions,
    size_t                iterations,
    MdsAlgorithm          algorithm
) {
    if( ! is_square( distances )) {
        throw std::invalid_argument( "MDS input distance matrix is not square." );
    }
    if( dimensions < 1 ) {
        throw std::invalid_argument( "MDS dimensions has to be >= 1." );
    }
    if( iterations < 1 ) {
        throw std::invalid_argument( "MDS number of iterations has to be >= 1." );
    }
    if( initial_values.rows() != distances.rows() || initial_values.cols() != dimensions ) {
        throw std::invalid_argument( "MDS initial values matrix has invalid dimensions." );
    }
    if( distances.empty() ) {
        return {};
    }

    switch( algorithm ) {
        case MdsAlgorithm::kUcf: {
            return multi_dimensional_scaling_ucf(
                distances, initial_values, dimensions, iterations
            );
        }
        case MdsAlgorithm::kSmacof: {
            return multi_dimensional_scaling_smacof(
                distances, initial_values, dimensions, iterations
            );
        }
        default: {
            assert( false );
        }
    }
    return {};
}

} // namespace utils
} // namespace genesis
