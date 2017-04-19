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

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

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

    enum class InitializationStrategy
    {
        kRandomAssignments,
        kRandomCentroids,
        kKmeansPlusPlus,
        kNone
    };

    /**
     * @brief Helper POD that stores the variances and number of data points of each centroid,
     * as well as the distances from all data points to their assigned centroids.
     */
    struct ClusteringInfo
    {
        std::vector<double> variances;
        std::vector<size_t> counts;
        std::vector<double> distances;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Kmeans() = default;
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
        // Run basic checks. This throws if necessary.
        argument_checks_( data, k );

        // Init assigments and centroids.
        initialize( data, k );

        // Call the hook.
        pre_loop_hook( data, assignments_, centroids_ );

        // By now, the result vectors should be filled correctly.
        // This replaces asserts. It is slightly more expensive, but this class offers so much
        // expansion points and custom behaviour, that we better check thoroughly.
        runtime_checks_( data, k );

        size_t iteration = 0;
        bool changed_assigment;

        do {
            // Start a new iteration.
            LOG_INFO << "Iteration " << iteration;
            changed_assigment = lloyd_step( data, assignments_, centroids_ );

            // Check again.
            runtime_checks_( data, k );

            // Check if there are empty centroids, and if so, treat them.
            auto const empty_centroids = get_empty_centroids_();
            if( ! empty_centroids.empty() ) {
                LOG_INFO << "Empty centroid occurred: " << empty_centroids.size();
                changed_assigment |= treat_empty_centroids(
                    data, assignments_, centroids_, empty_centroids
                );
            }

            ++iteration;
        } while( changed_assigment && iteration < max_iterations_ );

        // Call the hook.
        post_loop_hook( data, assignments_, centroids_ );

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
        return centroids_;
    }

    Kmeans& centroids( std::vector<Point> const& value )
    {
        centroids_ = value;
        return *this;
    }

    std::vector<size_t> cluster_sizes() const
    {
        if( assignments_.empty() || centroids_.empty() ) {
            throw std::runtime_error( "No assignments or centroids set yet." );
        }
        auto result = std::vector<size_t>( centroids_.size(), 0 );
        for( auto ass : assignments_ ) {
            assert( ass <= result.size() );
            ++result[ ass ];
        }
        return result;
    }

    void clear()
    {
        assignments_.clear();
        centroids_.clear();
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    size_t max_iterations() const
    {
        return max_iterations_;
    }

    Kmeans& max_iterations( size_t value )
    {
        if( value == 0 ) {
            throw std::runtime_error( "Cannot use 0 as max_iterations for Kmeans." );
        }
        max_iterations_ = value;
        return *this;
    }

    InitializationStrategy initialization_strategy() const
    {
        return init_strategy_;
    }

    Kmeans& initialization_strategy( InitializationStrategy value )
    {
        init_strategy_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------------------

protected:

    virtual void initialize( std::vector<Point> const& data, size_t const k )
    {
        // Select init stragegies.
        switch( init_strategy_ ) {
            case InitializationStrategy::kRandomAssignments: {
                init_with_random_assignments_( data, k );
                break;
            }
            case InitializationStrategy::kRandomCentroids: {
                init_with_random_centroids_( data, k );
                break;
            }
            case InitializationStrategy::kKmeansPlusPlus: {
                init_with_kmeans_plus_plus_( data, k );
                break;
            }
            default: {}
        }

        // If the strategy did not yield useful values, we still need to init somehow,
        // so do this now. This also applies if kNone was selected for init, but no centroids were set.
        if( assignments_.size() == 0 && centroids_.size() == 0 ) {
            // Nothing given: Sample random centroids from the data.
            init_with_random_centroids_( data, k );

        } else if( assignments_.size() == 0 && centroids_.size() > 0 ) {
            // Centroids given, but no assigments: Nothing to do for now.
            // We will calculate the proper assigments in the main loop.

        } else if( assignments_.size() > 0 && centroids_.size() == 0 ) {
            // Assignments given, but not centroids: Caculate the latter.
            update_centroids( data, assignments_, centroids_ );

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

    virtual bool data_validation( std::vector<Point> const& data ) const
    {
        (void) data;
        return true;
    }

    virtual void pre_loop_hook(
        std::vector<Point>  const& data,
        std::vector<size_t>&       assignments,
        std::vector<Point>&        centroids
    ) {
        (void) data;
        (void) assignments;
        (void) centroids;
    }

    virtual bool lloyd_step(
        std::vector<Point>  const& data,
        std::vector<size_t>&       assignments,
        std::vector<Point>&        centroids
    ) {
        // Calculate new assignments and check whether they changed.
        auto changed_assigment = assign_to_centroids( data, centroids, assignments );

        // Recalculate the centroids.
        update_centroids( data, assignments, centroids );

        return changed_assigment;
    }

    virtual bool assign_to_centroids(
        std::vector<Point> const& data,
        std::vector<Point> const& centroids,
        std::vector<size_t>&      assignments
    ) {
        // Store whether anything changed.
        bool changed_assigment = false;

        // Assign each Point to its nearest centroid.
        #pragma omp parallel for
        for( size_t i = 0; i < data.size(); ++i ) {
            auto const new_idx = find_nearest_cluster( centroids, data[i] ).first;

            if( new_idx != assignments[i] ) {
                // Update the assignment. No need for locking, as each thread works on its own i.
                assignments[i] = new_idx;

                // If we have a new assigment for this datum, we need to do another loop iteration.
                // Do this atomically, as all threads use this variable.
                #pragma omp atomic write
                changed_assigment = true;
            }
        }

        return changed_assigment;
    }

    virtual std::pair<size_t, double> find_nearest_cluster(
        std::vector<Point> const& centroids,
        Point const&              datum
    ) const {
        size_t min_i = std::numeric_limits<size_t>::max();
        double min_d = std::numeric_limits<double>::max();

        assert( centroids.size() > 0 );
        for( size_t i = 0; i < centroids.size(); ++i ) {
            auto const dist = distance( datum, centroids[i] );
            if( dist < min_d ) {
                min_i = i;
                min_d = dist;
            }
        }

        return { min_i, min_d };
    }

    virtual ClusteringInfo cluster_info(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        std::vector<Point>  const& centroids
    ) const {
        auto const k = centroids.size();

        auto result = ClusteringInfo();
        result.variances = std::vector<double>( k, 0.0 );
        result.counts    = std::vector<size_t>( k, 0 );
        result.distances = std::vector<double>( data.size(), 0.0 );

        // Work through the data and assigments and accumulate.
        #pragma omp parallel for
        for( size_t i = 0; i < data.size(); ++i ) {

            // Shorthands.
            auto const a = assignments[ i ];
            assert( a < k );
            auto const& centroid = centroids[ a ];

            // Get dist from datum to centroid.
            auto const dist = distance( centroid, data[ i ] );
            result.distances[ i ] = dist;

            // Update centroid accumulators.
            #pragma omp atomic update
            result.variances[ a ] += dist * dist;
            #pragma omp atomic update
            ++result.counts[ a ];
        }

        // Build the mean dist to get the variance for each centroid.
        for( size_t i = 0; i < k; ++i ) {
            if( result.counts[ i ] > 0 ) {
                result.variances[ i ] /= result.counts[ i ];
            }
        }

        return result;
    }

    virtual bool treat_empty_centroids(
        std::vector<Point>  const&        data,
        std::vector<size_t>&              assignments,
        std::vector<Point>&               centroids,
        std::unordered_set<size_t> const& empty_centroids
    ) {
        // If there are not empty centroids, we have nothing to do, and did not change anything,
        // so return false.
        if( empty_centroids.empty() ) {
            return false;
        }

        // Process all empty centroid indices.
        for( auto const& ec_idx : empty_centroids ) {
            // Get variances and counts of clusters and distances from data to them.
            auto clus_info = cluster_info( data, assignments, centroids );
            assert( clus_info.variances.size() == centroids.size() );
            assert( clus_info.distances.size() == data.size() );
            assert( data.size() == assignments.size() );

            // Get index of centroid with max variance.
            auto const max_var_idx = static_cast<size_t>( std::distance(
                clus_info.variances.begin(),
                std::max_element( clus_info.variances.begin(), clus_info.variances.end() )
            ));

            // If the max variance is 0, we cannot do anything. All points are the same.
            if( clus_info.variances[ max_var_idx ] == 0.0 ) {
                return false;
            }

            // The empty centroid cannot be the same as the one we want to take a point from,
            // because empty clusters have a variance of 0. If this assertion fails, we probably
            // ran an analysis with k >> data.size() or so.
            assert( ec_idx != max_var_idx );

            // The current empty cluster should actually be empty.
            assert( clus_info.counts[ ec_idx ] == 0 );
            assert( clus_info.variances[ ec_idx ] == 0.0 );

            // Find the point in the max var cluster that is furthest away from the centroid.
            size_t furth_idx  = std::numeric_limits<size_t>::max();
            double furth_dist = std::numeric_limits<double>::lowest();
            for( size_t i = 0; i < data.size(); ++i ) {
                if( assignments[i] != max_var_idx ) {
                    continue;
                }
                if( clus_info.distances[ i ] > furth_dist ) {
                    furth_idx  = i;
                    furth_dist = clus_info.distances[ i ];
                }
            }

            // The point needs to be part of the max var cluster, otherwise something went wrong.
            assert( assignments[ furth_idx ] == max_var_idx );

            // Add the point to the empty cluster.
            assignments[ furth_idx ] = ec_idx;

            // The following is some test code that could be used to avoid calculating the cluster
            // info for each empty cluster. However, as we probably almost never run into this
            // function anyway, we better keep it simple and calculate the info fresh every time.

            // // Adjust variance and count of the max var cluster.
            // auto const dsqrt = clus_info.distances[ furth_idx ] * clus_info.distances[ furth_idx ];
            // clus_info.variances[ max_var_idx ] *= clus_info.counts[ max_var_idx ];
            // clus_info.variances[ max_var_idx ] -= dsqrt;
            // --clus_info.counts[ max_var_idx ];
            // clus_info.variances[ max_var_idx ] /= clus_info.counts[ max_var_idx ];
            //
            // // Adjust count of the (now not any longer) empty cluster.
            // ++clus_info.counts[ ec_idx ];

            // Finally, we need to update the centroids in order to reflect the changes.
            update_centroids( data, assignments, centroids );
        }

        // Now we return true, because we changed some assignments.
        return true;
    }

    virtual double distance(
        Point const& lhs,
        Point const& rhs
    ) const = 0;

    virtual void update_centroids(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        std::vector<Point>&        centroids
    ) = 0;

    virtual void post_loop_hook(
        std::vector<Point>  const& data,
        std::vector<size_t>&       assignments,
        std::vector<Point>&        centroids
    ) {
        (void) data;
        (void) assignments;
        (void) centroids;
    }

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    void argument_checks_(
        std::vector<Point> const& data,
         size_t const             k
    ) const {
        // Basic checks.
        if( k > data.size() ) {
            LOG_WARN << "Cannot run Kmeans with more clusters (k == " << k << ") than data points ("
                     << data.size() << ")";
        }
        if( k == 0 ) {
            throw std::runtime_error(
                "Cannot run Kmeans with zero clusters (k == 0)."
            );
        }

        // Validate the data. The function might also throw on its own, in order
        // to provide a more helpful message about what is actually invalid about the data.
        if( ! data_validation( data ) ) {
            throw std::runtime_error( "Invalid data." );
        }
    }

    void runtime_checks_(
        std::vector<Point> const& data,
         size_t const             k
    ) const {
        if( assignments_.size() != data.size() ) {
            throw std::runtime_error(
                "Assignments has size " + std::to_string( assignments_.size() ) +
                " but data has size " + std::to_string( data.size() ) + "."
            );
        }
        for( auto const& assign : assignments_ ) {
            if( assign >= k ) {
                throw std::runtime_error(
                    "Invalid assignment " + std::to_string( assign ) +
                    " >= k = " + std::to_string( k ) + "."
                );
            }
        }
        if( centroids_.size() != k ) {
            throw std::runtime_error(
                "Centroids has size " + std::to_string( centroids_.size() ) +
                " but k is " + std::to_string( k ) + "."
            );
        }
    }

    void init_with_random_assignments_(
        std::vector<Point> const& data,
        size_t const              k
    ) {
        // Prepare a vector of the desired size.
        assignments_ = std::vector<size_t>( data.size(), 0 );

        // Prepare a random distribution in range [0,k).
        auto& engine = Options::get().random_engine();
        std::uniform_int_distribution<size_t> distribution( 0, k - 1 );

        // Assign random cluster indices for each data point.
        for( size_t i = 0; i < data.size(); ++i ) {
            assignments_[ i ] = distribution( engine );
        }
    }

    void init_with_random_centroids_(
        std::vector<Point> const& data,
        size_t const              k
    ) {
        // Prepare centroids vector. Empty, because we don't want to assume any default
        // constructor for the Points.
        centroids_ = std::vector<Point>();

        // Select k unique numbers out of the interval [ 0, data.size() ),
        // and copy those data points to the centroids.
        auto idxs = select_without_replacement( k, data.size() );
        for( auto const idx : idxs ) {
            centroids_.push_back( data[idx] );
        }

        assert( centroids_.size() == k );
    }

    void init_with_kmeans_plus_plus_(
        std::vector<Point> const& data,
        size_t const              k
    ) {
        // Prepare centroids vector. Empty, because we don't want to assume any default
        // constructor for the Points.
        centroids_ = std::vector<Point>();

        // Shorthand.
        auto& engine = Options::get().random_engine();

        // Use a random point as first centroid.
        std::uniform_int_distribution<size_t> first_dist( 0, data.size() - 1 );
        centroids_.push_back( data[ first_dist(engine) ]);

        // Prepare a vector of probabilities to select each data point as a centroid.
        auto data_probs = std::vector<double>( data.size(), 0.0 );

        // Add more centroids.
        for( size_t i = 1; i < k; ++i ) {

            // For each data point...
            #pragma omp parallel for
            for( size_t di = 0; di < data.size(); ++di ) {

                // ...find the closest centroid (of the ones that are produced so far), ...
                double const min_d = find_nearest_cluster( centroids_, data[ di ] ).second;

                // ...and use its square as probability to select this point.
                // (No need for OpenMP locking here, as di is unique to each thread).
                data_probs[ di ] = min_d * min_d;
            }

            // Now select a new centroid from the data, according to the given probabilities.
            std::discrete_distribution<size_t> distribution(
                data_probs.begin(), data_probs.end()
            );
            auto idx = distribution( engine );
            assert( idx < data.size() );
            centroids_.push_back( data[ idx ] );
        }

        assert( centroids_.size() == k );
    }

    std::unordered_set<size_t> get_empty_centroids_() {
        auto const k = centroids_.size();

        // Fill a list with all numbers up to k...
        auto empties = std::unordered_set<size_t>();
        for( size_t i = 0; i < k; ++i ) {
            empties.insert( i );
        }

        // ... then remove all assigned ones again.
        for( size_t i = 0; i < assignments_.size(); ++i ) {
            assert( assignments_[i] < k );
            empties.erase( assignments_[i] );

            // Prematurely exit if there is nothing else to remove.
            // We don't want to go to all assignments if not necessary.
            if( empties.empty() ) {
                return empties;
            }
        }

        // If we are here, there are empty centroids, otherwise we'd have prematurely exited.
        assert( ! empties.empty() );
        return empties;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<size_t> assignments_;
    std::vector<Point>  centroids_;

    size_t max_iterations_ = 100;
    InitializationStrategy init_strategy_ = InitializationStrategy::kKmeansPlusPlus;

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

    virtual bool data_validation( std::vector<Point> const& data ) const override;

    virtual void update_centroids(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        std::vector<Point>&        centroids
    ) override;

    virtual double distance( Point const& lhs, Point const& rhs ) const override;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    size_t dimensions_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
