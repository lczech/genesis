#ifndef GENESIS_UTILS_MATH_KMEANS_H_
#define GENESIS_UTILS_MATH_KMEANS_H_

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
 * @ingroup utils
 */

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/math/random.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Generic K-Means Class
// =================================================================================================

/**
 * @brief
 */
template< typename Point >
class Kmeans
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    using value_type = Point;

    using data_validation_function = std::function< bool(
        std::vector<Point> const& data
    ) >;

    using calculate_centroids_function = std::function< std::vector<Point>(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        size_t                     k
    ) >;

    using distance_metric_function = std::function< double(
        Point const& lhs,
        Point const& rhs
    ) >;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Kmeans()  = default;
    virtual ~Kmeans() = default;

    Kmeans( Kmeans const& ) = default;
    Kmeans( Kmeans&& )      = default;

    Kmeans& operator= ( Kmeans const& ) = default;
    Kmeans& operator= ( Kmeans&& )      = default;

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    size_t run( std::vector<Point> const& data, size_t const k )
    {
        // Run basic checks first. This throws if necessary.
        runtime_checks_( data, k );

        // Init assigments and centroids.
        initialize_( data, k );

        // By now, the result vectors should be filled correctly.
        assert( assignments_.size() == data.size() );
        assert( centroids_.size() == k );

        size_t iteration = 0;
        bool changed_assigment;

        do {
            // Start a new iteration.
            LOG_INFO << "Iteration " << iteration;

            // Calculate new assignments and check whether they changed.
            changed_assigment = update_assignments_( data );

            // Recalculate the centroids.
            centroids_ = calculate_centroids( data, assignments_, k );

            ++iteration;
        } while( changed_assigment && iteration < max_iterations_ );

        return iteration;
    }

    // -------------------------------------------------------------------------
    //     Data Access
    // -------------------------------------------------------------------------

    std::vector<size_t> const& assignments() const
    {
        return assignments_;
    }

    Kmeans& assignments( std::vector<size_t> const& value )
    {
        assignments_ = value;
        return *this;
    }

    std::vector<Point> const& centroids() const
    {
        return assignments_;
    }

    Kmeans& centroids( std::vector<Point> const& value )
    {
        centroids_ = value;
        return *this;
    }

    size_t max_iterations() const
    {
        return max_iterations_;
    }

    Kmeans& max_iterations( size_t value )
    {
        max_iterations_ = value;
        return *this;
    }

    void clear()
    {
        assignments_.clear();
        centroids_.clear();
    }

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    void runtime_checks_( std::vector<Point> const& data, size_t const k ) const
    {
        // Check if necessary functions are set.
        if( ! calculate_centroids ) {
            throw std::runtime_error( "No calculate_centroids() function set." );
        }
        if( ! distance_metric ) {
            throw std::runtime_error( "No distance_metric() function set." );
        }

        // Basic checks.
        if( k > data.size() ) {
            LOG_WARN << "Running Kmeans with more clusters (k == " << k << ") than data points ("
                     << data.size() << ")";
        }
        if( k == 0 ) {
            throw std::runtime_error(
                "Cannot run Kmeans with zero clusters (k == 0)."
            );
        }
        if( assignments_.size() > 0 && assignments_.size() != data.size() ) {
            throw std::runtime_error(
                "Initial assignments has size " + std::to_string( assignments_.size() ) +
                " but data has size " + std::to_string( data.size() ) + "."
            );
        }
        if( centroids_.size() > 0 && centroids_.size() != k ) {
            throw std::runtime_error(
                "Initial centroids has size " + std::to_string( centroids_.size() ) +
                " but k is " + std::to_string( k ) + "."
            );
        }

        // If set, validate the data. The function might also throw on its own, in order
        // to provide a more helpful message about what is actually invalid about the data.
        if( data_validation && ! data_validation( data ) ) {
            throw std::runtime_error( "Invalid data." );
        }
    }

    void initialize_( std::vector<Point> const& data, size_t const k )
    {
        // Different init stragegies.
        if( assignments_.size() == 0 && centroids_.size() == 0 ) {
            // Nothing given: Sample random centroids from the data.
            random_initial_centroids_( data, k );

        } else if( assignments_.size() == 0 && centroids_.size() > 0 ) {
            // Centroids given, but no assigments: Nothing to do for now.
            // We will calculate the proper assigments in the main loop.

        } else if( assignments_.size() > 0 && centroids_.size() == 0 ) {
            // Assignments given, but not centroids: Caculate the latter.
            centroids_ = calculate_centroids( data, assignments_, k );

        } else {
            // Both given: Nothing to do.
            assert( assignments_.size() > 0 && centroids_.size() > 0 );
        }

        // If we do not have an assigment vector yet, make one. It will be assigned proper values
        // once we enter the main loop.
        if( assignments_.size() == 0 ) {
            assignments_ = std::vector<size_t>( data.size(), 0 );
        }
    }

    void random_initial_assignments_( size_t const data_size, size_t const k )
    {
        // Prepare a vector of the desired size.
        assignments_ = std::vector<size_t>( data_size, 0 );

        // Prepare a random distribution in range [0,k).
        auto& engine = Options::get().random_engine();
        std::uniform_int_distribution<size_t> distribution( 0, k - 1 );

        // Assign random cluster indices for each data point.
        for( size_t i = 0; i < data_size; ++i ) {
            assignments_[ i ] = distribution( engine );
        }
    }

    void random_initial_centroids_( std::vector<Point> const& data, size_t const k )
    {
        auto idxs = select_without_replacement( k, data.size() );
        centroids_ = std::vector<Point>();
        for( auto const& idx : idxs ) {
            centroids_.push_back( data[idx] );
        }
        assert( centroids_.size() == k );
    }

    bool update_assignments_( std::vector<Point> const& data )
    {
        // Store whether anything changed.
        bool changed_assigment = false;

        // Assign each Point to its nearest centroid.
        for( size_t i = 0; i < data.size(); ++i ) {
            auto new_idx = nearest_cluster_index_( data[i] );

            // If we have a new assigment for this datum, we need to do another loop iteration.
            if( new_idx != assignments_[i] ) {
                assignments_[i]   = new_idx;
                changed_assigment = true;
            }
        }

        return changed_assigment;
    }

    size_t nearest_cluster_index_( Point const& datum ) const
    {
        double min_d = std::numeric_limits<double>::max();
        size_t min_i;

        for( size_t i = 0; i < centroids_.size(); ++i ) {
            auto dist = distance_metric( datum, centroids_[i] );
            if( dist < min_d ) {
                min_d = dist;
                min_i = i;
            }
        }

        return min_i;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<size_t> assignments_;
    std::vector<Point>  centroids_;

    size_t max_iterations_ = 100;

public:

    data_validation_function     data_validation;

    calculate_centroids_function calculate_centroids;

    distance_metric_function     distance_metric;

};

// =================================================================================================
//     Euclidean K-Means Specialization
// =================================================================================================

class EuclideanKmeans
    : public Kmeans< std::vector<double> >
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    using Point = std::vector<double>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    EuclideanKmeans( size_t dimensions );

    virtual ~EuclideanKmeans() = default;

    EuclideanKmeans( EuclideanKmeans const& ) = default;
    EuclideanKmeans( EuclideanKmeans&& )      = default;

    EuclideanKmeans& operator= ( EuclideanKmeans const& ) = default;
    EuclideanKmeans& operator= ( EuclideanKmeans&& )      = default;

    // -------------------------------------------------------------------------
    //     Default K-Means Functions
    // -------------------------------------------------------------------------

private:

    bool data_dimension_validation_( std::vector<Point> const& data ) const;

    std::vector<Point> calculate_mean_centroids_(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        size_t                     k
    ) const;

    double euclidean_distance_( Point const& lhs, Point const& rhs ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    size_t dimensions_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
