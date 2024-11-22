#ifndef GENESIS_UTILS_MATH_HAC_LINKAGE_H_
#define GENESIS_UTILS_MATH_HAC_LINKAGE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/math/hac.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     HAC with Linkage
// ================================================================================================

// -------------------------------------------------------------------------
//     hac_merge_by_copy
// -------------------------------------------------------------------------

/**
 * @brief Merge function for hierarchical agglomerative clustering that combines two vectors.
 *
 * This function can be used for the HierarchicalAgglomerativeClustering::merge_function in order
 * to combine two clusters that are being merged in one iteration of the algorithm into their new
 * parent cluster by copying their elements into the parent. This assumes that the
 * clustering object is of type `HierarchicalAgglomerativeClustering<std::vector<T>>`, meaning
 * that each cluster is represented by a vector of observations. Initially, each cluster will then
 * contain a single observation, and merged clusters will contain all elements of their children.
 */
template<typename T>
std::function<std::vector<T>( std::vector<T> const&, std::vector<T> const& )>
hac_merge_by_copy()
{
    return []( std::vector<T> const& cluster_a, std::vector<T> const& cluster_b )
    {
        // Copy all elements from both clusters into the new one.
        std::vector<T> new_cluster;
        new_cluster.reserve( cluster_a.size() + cluster_b.size() );
        std::copy( cluster_a.begin(), cluster_a.end(), std::back_inserter( new_cluster ));
        std::copy( cluster_b.begin(), cluster_b.end(), std::back_inserter( new_cluster ));

        // An alternative would be to directly move the data, as that's faster.
        // However, that would not allow us to compute the distance to the old clusters any more,
        // which might be good to have. Not sure. Could be optimized in the future.
        // If so, the merge_function needs to take cluster_a and cluster_b as non-const refs though.
        // std::move( cluster_a.begin(), cluster_a.end(), std::back_inserter( new_cluster ));
        // std::move( cluster_b.begin(), cluster_b.end(), std::back_inserter( new_cluster ));
        // cluster_a.clear();
        // cluster_b.clear();
        return new_cluster;
    };
}

// -------------------------------------------------------------------------
//     hac_distance_single_linkage
// -------------------------------------------------------------------------

/**
 * @brief Distance function for hierarchical agglomerative clustering that computes single linkage.
 *
 * The function can be used for the HierarchicalAgglomerativeClustering::distance_function in order
 * to compute single linkage clustering. It takes as input the pairwise distance function between
 * two observation. The returned linkage distance function then computes the minimum of
 * this pairwise distance for all pairs of elements in two clusters. This assumes that the
 * clustering object is of type `HierarchicalAgglomerativeClustering<std::vector<T>>`.
 *
 * In most cases, HierarchicalAgglomerativeClustering::ultrametric_branch_lenghts() is likely needed
 * to be set to `true` in order to achieve an ultrametric dendrogram with the desired properties.
 */
template<typename T>
std::function<double( std::vector<T> const&, std::vector<T> const& )>
hac_distance_single_linkage( std::function<double( T const&, T const& )> distance_function )
{
    return [distance_function]( std::vector<T> const& cluster_a, std::vector<T> const& cluster_b )
    {
        double min_dist = std::numeric_limits<double>::max();
        size_t count = 0;
        for( auto const& elem_a : cluster_a ) {
            for( auto const& elem_b : cluster_b ) {
                auto const dist = distance_function( elem_a, elem_b );
                if( !std::isfinite( dist ) || dist < 0.0 ) {
                    throw std::runtime_error(
                        "Invalid distance computation in single linkage hierarchical agglomerative "
                        " clustering with distance that is negative or not finite."
                    );
                }
                min_dist = std::min( min_dist, dist );
                ++count;
            }
        }
        if( count == 0 ) {
            throw std::runtime_error(
                "Cannot compute single linkage distance with empty clusters"
            );
        }
        assert( std::isfinite( min_dist ) && min_dist >= 0.0 );
        return min_dist;
    };
}

// -------------------------------------------------------------------------
//     hac_distance_complete_linkage
// -------------------------------------------------------------------------

/**
 * @brief Distance function for hierarchical agglomerative clustering that computes complete linkage.
 *
 * The function can be used for the HierarchicalAgglomerativeClustering::distance_function in order
 * to compute complete linkage clustering. It takes as input the pairwise distance function between
 * two observation. The returned linkage distance function then computes the maximum of
 * this pairwise distance for all pairs of elements in two clusters. This assumes that the
 * clustering object is of type `HierarchicalAgglomerativeClustering<std::vector<T>>`.
 *
 * In most cases, HierarchicalAgglomerativeClustering::ultrametric_branch_lenghts() is likely needed
 * to be set to `true` in order to achieve an ultrametric dendrogram with the desired properties.
 */
template<typename T>
std::function<double( std::vector<T> const&, std::vector<T> const& )>
hac_distance_complete_linkage( std::function<double( T const&, T const& )> distance_function )
{
    return [distance_function]( std::vector<T> const& cluster_a, std::vector<T> const& cluster_b )
    {
        double max_dist = 0.0;
        size_t count = 0;
        for( auto const& elem_a : cluster_a ) {
            for( auto const& elem_b : cluster_b ) {
                auto const dist = distance_function( elem_a, elem_b );
                if( !std::isfinite( dist ) || dist < 0.0 ) {
                    throw std::runtime_error(
                        "Invalid distance computation in complete linkage hierarchical agglomerative "
                        " clustering with distance that is negative or not finite."
                    );
                }
                max_dist = std::max( max_dist, dist );
                ++count;
            }
        }
        if( count == 0 ) {
            throw std::runtime_error(
                "Cannot compute complete linkage distance with empty clusters"
            );
        }
        assert( std::isfinite( max_dist ) && max_dist >= 0.0 );
        return max_dist;
    };
}

// -------------------------------------------------------------------------
//     hac_distance_unweighted_average_linkage
// -------------------------------------------------------------------------

/**
 * @brief Distance function for hierarchical agglomerative clustering that computes unweighted
 * average linkage.
 *
 * The function can be used for the HierarchicalAgglomerativeClustering::distance_function in order
 * to compute single linkage clustering. It takes as input the pairwise distance function between
 * two observation. The returned linkage distance function then computes the unweighted average of
 * this pairwise distance for all pairs of elements in two clusters. This assumes that the
 * clustering object is of type `HierarchicalAgglomerativeClustering<std::vector<T>>`.
 *
 * In most cases, HierarchicalAgglomerativeClustering::ultrametric_branch_lenghts() is likely needed
 * to be set to `true` in order to achieve an ultrametric dendrogram with the desired properties.
 * In this case, the linkage function represents UPGMA clustering.
 */
template<typename T>
std::function<double( std::vector<T> const&, std::vector<T> const& )>
hac_distance_unweighted_average_linkage( std::function<double( T const&, T const& )> distance_function )
{
    return [distance_function]( std::vector<T> const& cluster_a, std::vector<T> const& cluster_b )
    {
        double sum_dist = 0.0;
        size_t count = 0;
        for( auto const& elem_a : cluster_a ) {
            for( auto const& elem_b : cluster_b ) {
                auto const dist = distance_function( elem_a, elem_b );
                if( !std::isfinite( dist ) || dist < 0.0 ) {
                    throw std::runtime_error(
                        "Invalid distance computation in average linkage hierarchical agglomerative "
                        " clustering with distance that is negative or not finite."
                    );
                }
                sum_dist += dist;
                ++count;
            }
        }
        if( count == 0 ) {
            throw std::runtime_error(
                "Cannot compute average linkage distance with empty clusters"
            );
        }
        assert( std::isfinite( sum_dist ) && sum_dist >= 0.0 );
        return sum_dist / static_cast<double>( count );
    };
}

// ================================================================================================
//     HAC with Distance Matrix
// ================================================================================================

enum class HierarchicalAgglomerativeClusteringLinkage
{
    kNone,
    kSingleLinkage,
    kCompleteLinkage,
    kUnweightedAverageLinkage
};

// -------------------------------------------------------------------------
//     hac_set_linkage
// -------------------------------------------------------------------------

/**
 * @brief Set the linkage function for hierarchical agglomerative clustering.
 *
 * This helper function is meant to be used in standard cases where we want to apply some typical
 * linkage function to compute distances between clusters, given a @p distance_function that
 * operates on exactly two observations. The clustering is assumed to be conducted over a vector
 * of observations, which in each step are merged from two child clusters into the new parent
 * (merged) cluster.
 *
 * The linkage function typically performs some all-to-all comparison of all elements in one cluster
 * against all elements in the other cluster, see for instance hac_distance_single_linkage(),
 * hac_distance_complete_linkage(), and hac_distance_unweighted_average_linkage().
 *
 * The @p linkage can also be set to `kNone`, in which case the function is a no-op.
 * This is provided so that functions such as hac_with_distance_matrix() can also be used with
 * other linkage functions that are not provided here, by simply setting the
 * HierarchicalAgglomerativeClustering::distance_function to a custom function afterwards.
 */
template<typename T>
inline void hac_set_linkage(
    HierarchicalAgglomerativeClustering<std::vector<T>>& clustering,
    std::function<double( T const&, T const& )> distance_function,
    HierarchicalAgglomerativeClusteringLinkage linkage
) {
    switch( linkage ) {
        case HierarchicalAgglomerativeClusteringLinkage::kNone: {
            break;
        }
        case HierarchicalAgglomerativeClusteringLinkage::kSingleLinkage: {
            clustering.distance_function = hac_distance_single_linkage<T>(
                distance_function
            );
            break;
        }
        case HierarchicalAgglomerativeClusteringLinkage::kCompleteLinkage: {
            clustering.distance_function = hac_distance_complete_linkage<T>(
                distance_function
            );
            break;
        }
        case HierarchicalAgglomerativeClusteringLinkage::kUnweightedAverageLinkage: {
            clustering.distance_function = hac_distance_unweighted_average_linkage<T>(
                distance_function
            );
            break;
        }
        default: {
            throw std::invalid_argument( "Invalid HierarchicalAgglomerativeClusteringLinkage" );
        }
    }
}

// -------------------------------------------------------------------------
//     hac_with_linkage
// -------------------------------------------------------------------------

/**
 * @brief Instanciate an instance to compute hierarchical agglomerative clustering,
 * using some typical linkage function on a set ob @p observations.
 *
 * The function creates an instance of HAC where observations that are clustered in a step are
 * moved to the new cluster, by keeping all oberservations as vectors. Hence, the given
 * @p observations are initially moved to a `std::vector` of vectors, each containing exaclty
 * one element of the given vector. Then, in each step of the clustering, we use hac_merge_by_copy()
 * to move the observations of the two clusters being merged into their new parent cluster.
 * The given distance function and linkage are then applied to these clusters.
 */
template<typename T>
inline HierarchicalAgglomerativeClustering<std::vector<T>> hac_with_linkage(
    std::vector<T>&& observations,
    std::function<double( T const&, T const& )> distance_function,
    HierarchicalAgglomerativeClusteringLinkage linkage
) {
    // We move all observations into a vector of vectors, such that each clustering step
    // can merge those clusters together by simply copying/moving the oberservations.
    using U = std::vector<T>;

    // We need to move all observations into intividual vectors.
    auto data = std::vector<U>( observations.size() );
    for( size_t i = 0; i < observations.size(); ++i ) {
        data[i].push_back( std::move( observations[i] ));
    }
    observations.clear();

    // Set up the clustering instance.
    HierarchicalAgglomerativeClustering<U> clustering;
    clustering.set_data( std::move( data ));
    clustering.merge_function = hac_merge_by_copy<T>();
    clustering.deactivate_function = []( U& data ){ data.clear(); };
    hac_set_linkage( clustering, distance_function, linkage );
    return clustering;
}

// -------------------------------------------------------------------------
//     hac_with_index_vectors
// -------------------------------------------------------------------------

/**
 * @brief Instanciate an instance to compute hierarchical agglomerative clustering,
 * given a distance function that performes some form of index-based lookup.
 *
 * This is a helper function where the @p distance_function takes two indices and returns
 * the distance between the observations at those two indices. It is however agnostic with respect
 * to what those indices index into - this is up to the @p distance_function.
 * For instance, hac_with_distance_matrix() uses this to look up pairwise distances from a given
 * distance matrix between observations.
 *
 * The returned instance will compute a clustering using the given linkage.
 * Each cluster step then consists of the vector of indices into the matrix that were clustered
 * in that step (which hence is the template type of the returned HAC instance).
 */
inline HierarchicalAgglomerativeClustering<std::vector<size_t>> hac_with_index_vectors(
    size_t number_of_observations,
    std::function<double( size_t, size_t )> distance_function,
    HierarchicalAgglomerativeClusteringLinkage linkage
) {
    // We prepare a clustering where each cluster consists of indices into the distance matrix.
    // We use these to compute the distance function, and when merging clusters, these indicies
    // are simply compbined to form the new cluster.
    // using U = std::vector<size_t>;

    // Initialize the cluster data as a vector for each row/col of the matrix,
    // which itself contains a single element, namely, its own index.
    // During the clustering, these vectors will then be merged, meaning that each cluster
    // in the end contains the set of indices belonging to that cluster.
    auto cluster_indices = std::vector<size_t>( number_of_observations );
    std::iota( cluster_indices.begin(), cluster_indices.end(), 0 );

    // auto cluster_indices = std::vector<U>( number_of_observations, U( 1, 0 ));
    // for( size_t i = 0; i < number_of_observations; ++i ) {
    //     assert( cluster_indices[i].size() == 1 );
    //     cluster_indices[i][0] = i;
    // }

    // Set up the clustering instance.
    return hac_with_linkage<size_t>( std::move( cluster_indices ), distance_function, linkage );
}

// -------------------------------------------------------------------------
//     hac_with_distance_matrix
// -------------------------------------------------------------------------

/**
 * @brief Instanciate an instance to compute hierarchical agglomerative clustering,
 * given a pairwise distance matrix between observations.
 *
 * The returned instance will compute a clustering using the given distances and the given linkage.
 * Each cluster step then consists of the vector of indices into the matrix that were clustered
 * in that step (which hence is the template type of the returned HAC instance).
 */
inline HierarchicalAgglomerativeClustering<std::vector<size_t>> hac_with_distance_matrix(
    Matrix<double> const& distance_matrix,
    HierarchicalAgglomerativeClusteringLinkage linkage
) {
    // Need square symmetrical matrix with finite entries and zero diagonal.
    if( ! is_square( distance_matrix ) || ! is_symmetric( distance_matrix )) {
        throw std::invalid_argument(
            "Cannot run hierarchical agglomerative clustering with non-symmetrical distance matrix"
        );
    }
    auto const all_finite = std::all_of(
        distance_matrix.begin(), distance_matrix.end(),
        [](double x) {
            return std::isfinite(x);
        }
    );
    if( ! all_finite ) {
        throw std::invalid_argument(
            "Cannot run hierarchical agglomerative clustering with non-finite distance matrix"
        );
    }
    for( size_t i = 0; i < distance_matrix.rows(); ++i ) {
        if( distance_matrix( i, i ) != 0.0 ) {
            throw std::invalid_argument(
                "Cannot run hierarchical agglomerative clustering with distance matrix "
                "that has non-zero diagonal entries"
            );
        }
    }

    // Our distance function is a simple lookup in the distance matrix, for two entries
    // given by their indices from the above vectors.
    auto dist_fct = [&distance_matrix]( size_t index_a, size_t index_b )
    {
        assert( index_a < distance_matrix.rows() );
        assert( index_b < distance_matrix.rows() );
        return distance_matrix( index_a, index_b );
    };
    return hac_with_index_vectors( distance_matrix.rows(), dist_fct, linkage );
}

} // namespace utils
} // namespace genesis

#endif // include guard
