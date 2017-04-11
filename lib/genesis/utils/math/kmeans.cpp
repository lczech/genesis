/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/math/kmeans.hpp"

#include <algorithm>
#include <cmath>

namespace genesis {
namespace utils {

// ================================================================================================
//     Euclidian K-Means Specialization
// ================================================================================================

// -------------------------------------------------------------------------
//     Constructors and Rule of Five
// -------------------------------------------------------------------------

EuclideanKmeans::EuclideanKmeans( size_t dimensions )
    : dimensions_( dimensions )
{}

// -------------------------------------------------------------------------
//     Default K-Means Functions
// -------------------------------------------------------------------------

bool EuclideanKmeans::data_validation( std::vector<Point> const& data ) const
{
    for( size_t i = 0; i < data.size(); ++i ) {
        auto const& datum = data[i];
        if( datum.size() != dimensions_ ) {
            throw std::runtime_error(
                "Datum at " + std::to_string( i ) + " has invalid dimension " +
                std::to_string( datum.size() ) + " instead of " + std::to_string( dimensions_ ) + "."
            );
        }
    }
    return true;
}

void EuclideanKmeans::update_centroids(
    std::vector<Point>  const& data,
    std::vector<size_t> const& assignments,
    std::vector<Point>&        centroids
) {
    // This function is only called from within the run() function, which already
    // checks this condition. So, simply assert it here, instead of throwing.
    assert( data.size() == assignments.size() );

    auto const k = centroids.size();

    #ifdef GENESIS_OPENMP

        // Parallelize over centroids
        #pragma omp parallel for
        for( size_t i = 0; i < k; ++i ) {

            // Thread-local Point
            auto centroid = Point( dimensions_, 0.0 );
            size_t count = 0;

            // Work through the data...
            for( size_t j = 0; j < data.size(); ++j ) {

                // ... but only the relevant parts.
                if( assignments[j] != i ) {
                    continue;
                }

                // Accumulate centroid.
                auto const& datum = data[ j ];
                for( size_t d = 0; d < dimensions_; ++d ) {
                    centroid[ d ] += datum[ d ];
                }
                ++count;
            }

            // Build the mean.
            if( count > 0 ) {
                for( size_t d = 0; d < dimensions_; ++d ) {
                    centroid[ d ] /= count;
                }
            }

            // Set the centroid. No need for locking, as we only access our i.
            centroids[ i ] = std::move( centroid );
        }

    #else

        // In the serial case, we only want to traverse the data once.

        // Init the result as well as counts for calculating the mean.
        centroids = std::vector<Point>( k, Point( dimensions_, 0.0 ) );
        auto counts = std::vector<size_t>( k, 0 );

        // Work through the data and assigments and accumulate.
        for( size_t i = 0; i < data.size(); ++i ) {

            // Shorthands.
            auto const& datum    = data[ i ];
            auto&       centroid = centroids[ assignments[i] ];

            // Accumulate centroid.
            for( size_t d = 0; d < dimensions_; ++d ) {
                centroid[ d ] += datum[ d ];
            }

            ++counts[ assignments[i] ];
        }

        // Build the mean.
        for( size_t i = 0; i < k; ++i ) {
            if( counts[ i ] > 0 ) {
                auto& centroid = centroids[ i ];
                for( size_t d = 0; d < dimensions_; ++d ) {
                    centroid[ d ] /= counts[ i ];
                }
            }
        }

    #endif
}

double EuclideanKmeans::distance( Point const& lhs, Point const& rhs ) const
{
    double sum = 0.0;
    for( size_t d = 0; d < dimensions_; ++d ) {
        auto const diff = lhs[d] - rhs[d];
        sum += diff * diff;
    }
    return std::sqrt( sum );
}

} // namespace utils
} // namespace genesis
