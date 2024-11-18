#ifndef GENESIS_UTILS_MATH_HAC_H_
#define GENESIS_UTILS_MATH_HAC_H_

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

#include "genesis/utils/core/options.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/threading/thread_functions.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     Hierarchical Agglomerative Clustering
// ================================================================================================

/**
 * @brief Generic class for computing a Hierarchical Agglomerative Clustering (HAC).
 *
 * The class provides the basic clustering mechanism, i.e., iteratively finding the closest two
 * clusters, and merge them. By design, in order to allow for greater flexibility, it however
 * does not directly include typical aspects such as the linkage function for obtaining a distance
 * between clusters. Instead, we here assume that our observations of type `T` are merged in each
 * step into a new instance of `T` representing the two original clusters, and that the distance
 * function operates on this type `T` directly.
 *
 * Thus, a typical HAC can be implemented by using `T = std::vector<U>` of the desired underlying
 * type `U` of observations, setting the merge function to move elements from the two original
 * clusters (or copy, if they are still needed) to the new one, and setting the distance function
 * to use the desired linkage function on these vectors. See hac_with_linkage() and
 * hac_with_distance_matrix() for helper functions to set up a clustering for these use cases.
 *
 * The design hence also allows variants of HAC where we can naturally merge observations into
 * clusters, such as bitvectors, where the distance can be the Jaccard similarity between them,
 * and the merge operation computes their union. On that data type, this might be more natural,
 * as we are only ever operating on single bitvectors, and no linkage is needed in the distance
 * computation.
 *
 * To set up and run the clustering, at least the distance_function and merge_function have to
 * be provided, as well as the set_data() function to be used to set the intial data per cluster.
 *
 * An important consideration is whether the distance calculation is meant to be ultrametric,
 * such as when using this class to implement UPGMA. For the resulting dendrogram, by default,
 * we compute branch lengths as the distance between parent and child nodes, by simply applying
 * the distance function between them. However, to achieve an ultrametric dendrogram, we then also
 * need to subtract the branch lengths that are already implicitly captures in the sub-clusters.
 * See ultrametric_branch_lenghts() for details.
 *
 * Note that we always build a bifurcating histogram. That is, if there are multiple pairs of
 * clusters with the same distance between them, we pick only the first one to merge.
 */
template <typename T>
class HierarchicalAgglomerativeClustering
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    struct Cluster
    {
        /**
         * @brief The data that this cluster represents.
         *
         * In the beginning of the algorithm, those are simply the observations we want to cluster.
         * Those are then successively merged to form bigger clusters.
         */
        T data;

        /**
         * @brief Is this cluster active, i.e., is it not yet part of a larger cluster, or otherwise
         * deactivated by the keep_active_function? Only active clusters are considered for merging.
         */
        bool active;

        /**
         * @brief Distances from this cluster to all clusters with a lower index
         * in the clusters() vector.
         *
         * We do not store the distances in a global distance matix, but in a vector for each cluster
         * instead, as this makes it trivial to keep track of the data when merging clusters.
         * No need to keep track of which row belongs to which cluster etc.
         */
        std::vector<double> distances;

        /**
         * @brief If this cluster is the result of merging two other clusters,
         * this stores the index of the Merger object describing the merger.
         */
        size_t merger_index = std::numeric_limits<size_t>::max();
    };

    struct Merger
    {
        /**
         * @brief Distance between the two clusters being merged.
         */
        double distance;

        /**
         * @brief Index of the parent cluster (the new cluster that this merger created).
         */
        size_t cluster_index_p;

        /**
         * @brief Index of the first child cluster in the new (parent) cluster.
         */
        size_t cluster_index_a;

        /**
         * @brief Index of the second child cluster in the new (parent) cluster.
         */
        size_t cluster_index_b;

        /**
        * @brief Branch length of the first child cluster to the new (parent) cluster.
        *
        * This can either be computed by applying distance_function to the child and parent cluster,
        * or, if the distance function and resultind dendrogram are meant to be ultrametric,
        * be computed as the difference in distances from the parent and child cluster
        * to all their shared leaves.
        */
        double branch_length_a;

        /**
        * @brief Branch length of the second child cluster to the new (parent) cluster.
        */
        double branch_length_b;
    };

    // -------------------------------------------------------------------------
    //     Functionals
    // -------------------------------------------------------------------------

    /**
     * @brief Distance function between two clusters/observations.
     *
     * This is the pairwise distance function between two clusters, which needs to be set by the
     * user. When using `T = std::vector<U>` as a means of clustering with linkage, this distance
     * function needs to perform the linkage function and its internal distance function.
     */
    std::function<double( T const& data_a, T const& data_b )> distance_function;

    /**
     * @brief Function to merge the observations of two clusters into a new cluster.
     *
     * This is the operation that is performed for the two clusters with the smallest distance
     * in every iteration of the algorithm, and needs to be set by the user. The two inputs
     * are provided via non-const reference here, such that their contents can be moved or deleted
     * as needed, as they will not be accessed by later iterations of the clusterin anyway.
     *
     * Thus, for instance when using `T = std::vector<U>` to have clusters represet the set of
     * all observations in a cluster, all elements from the two clusters can be moved to the
     * new cluster here, in order to save memory.
     */
    std::function<T( T const& data_a, T const& data_b )> merge_function;

    /**
     * @brief Decide if a cluster shall be deactivated, i.e., not considered it for further merging.
     *
     * If this function is set, it is evaluated initially for all observations, as well as after
     * a new cluster has been formed by merging two clusters during an iteration. If it theh
     * returns `false`, the cluster is deactivated and does not participate in further merging
     * any more. This is will result in a forest of dendrogram with disconnected sub-trees,
     * and can be useful for instance to only allow clusters of a given maximum size or some
     * other data-dependent criterion.
     */
    std::function<bool( T const& data )> keep_active_function;

    /**
     * @brief Clean up the cluster data if a cluster is being deactivated.
     *
     * If set, this function is called on both clusters that are being merged in a step. It is meant
     * to provide a way to clean up data and save memory for clusters that are not being considered
     * for merging in subsequent iterations any more.
     */
    std::function<void( T& data )> deactivate_function;

    /**
     * @brief Write cluster data.
     *
     * If set, this function is called for each cluster (including the original observations),
     * with the internal index used in cluster(), so that the cluster data can be written or stored.
     */
    std::function<void( Cluster const& cluster, size_t index )> write_cluster_function;

    /**
     * @brief Function to report progress to the user the beginning of a new step.
     */
    std::function<void( size_t iteration, size_t total_iterations )> report_step_begin_function;

    /**
     * @brief Function to report to the user when a clustering step is completed.
     *
     * This allows to report the result of the iteration to the user, for instance by printing
     * properties of the cluster and merger created in this iteration.
     */
    std::function<void(
        size_t iteration, size_t total_iterations, Cluster const& cluster, Merger const& merger
    )> report_step_end_function;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    HierarchicalAgglomerativeClustering() = default;

    HierarchicalAgglomerativeClustering(
        std::shared_ptr<utils::ThreadPool> thread_pool
    ) {
        thread_pool_ = thread_pool ? thread_pool : Options::get().global_thread_pool();
    }

    ~HierarchicalAgglomerativeClustering() = default;

    HierarchicalAgglomerativeClustering(
        HierarchicalAgglomerativeClustering const&
    ) = default;
    HierarchicalAgglomerativeClustering(
        HierarchicalAgglomerativeClustering&&
    ) = default;

    HierarchicalAgglomerativeClustering& operator= (
        HierarchicalAgglomerativeClustering const&
    ) = default;
    HierarchicalAgglomerativeClustering& operator= (
        HierarchicalAgglomerativeClustering&&
    ) = default;

    // -------------------------------------------------------------------------
    //     Public Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Set the data to perform the clustering on.
     */
    void set_data( std::vector<T>&& observations )
    {
        data_ = std::move( observations );
    }

    /**
     * @brief Set the data to perform the clustering on.
     */
    void set_data( std::vector<T> const& observations )
    {
        data_ = observations;
    }

    /**
     * @brief Set whether to compute the branch lenghts of the dendogram to be ultrametric.
     *
     * By default, we compute branch lengths of the resulting dendrogram simply by applying the
     * distance function between the clusters involved in a merger, and store these in the Merger
     * branch lengths. That is, if clusters A and B are merged into a new cluster P in an iteration
     * of the algorithm, we compute the two resulting branches from P to A and B by applying the
     * distance function to both pairs of clusters.
     *
     * However, for instance when computing UPGMA, the resulting branch lengths are then overestimated,
     * as the distances between P and A/B are the overall distances, and do not account for the
     * distances within A and B respectively towards their original observations. See
     * https://en.wikipedia.org/wiki/UPGMA for an example. Hence, when activating this option here,
     * the branch lengths within the original clusters A/B are subtracted from the ones towards P,
     * such that the resulting dendrogram is ultrametric. For instance, the distance from P to A is
     * then computed by applying the distance function as before, but then subtracting the distance
     * from A to its leaves from that.
     *
     * This requires that the distance function is monotonic: When merging two clusters, the
     * resulting distance between them can never be smaller than the distances within these clusters
     * (from their parent to their leaves - all of them are equal in this case, as we assume
     * ultrametricity to begin with). If this is not the case, we would get a negative distance
     * from P to A or B, in which case the computation here will throw an exception.
     *
     * @see hac_dendrogram() to compute the dendrogram.
     */
    void ultrametric_branch_lenghts( bool value )
    {
        ultrametric_branch_lenghts_ = value;
    }

    /**
     * @brief Perfom Hierarchical Agglomerative Clustering
     */
    void run()
    {
        // We need data!
        if( data_.empty() ) {
            throw std::invalid_argument(
                "No data provided for hierarchical agglomerative clustering"
            );
        }

        // Maximum number of iterations we are going to run the clustering for,
        // corresponding to a full hierarchical clustering of all observations.
        // If more clusters are deactivated (via the keep_active_function),
        // the actual number of iterations might be lowered during the run.
        assert( data_.size() > 0 );
        size_t total_iterations = data_.size() - 1;

        // Init the result object.
        init_();

        // Do a full clustering for all iterations.
        size_t iteration = 1;
        while( iteration <= total_iterations ) {
            if( report_step_begin_function ) {
                report_step_begin_function( iteration, total_iterations );
            }

            // Get and merge the next two clusters.
            auto const min_pair = min_distance_pair_();
            assert( min_pair.first < min_pair.second );
            merge_clusters_( min_pair.first, min_pair.second );

            // The merging created a new cluster at the end, which might have been deactivated
            // due to the keep_active_function. If so, our total iterations need to be decreased,
            // as we will not be merging that cluster any more.
            assert( clusters_.size() > 0 );
            if( ! clusters_.back().active ) {
                --total_iterations;
            }

            // End of the iteration reporting and moving on.
            if( report_step_end_function ) {
                assert( clusters_.size() > 0 && mergers_.size() > 0 );
                report_step_end_function(
                    iteration, total_iterations, clusters_.back(), mergers_.back()
                );
            }
            ++iteration;
        }

        // At the end, we only have one big cluster, or none, if all have been deactivated
        // already due to the keep_active_function.
        assert( 1 >= [&](){
            size_t count_active = 0;
            for( auto const& cluster : clusters_ ) {
                if( cluster.active ) {
                    ++count_active;
                }
            }
            return count_active;
        }() );
    }

    Cluster const& cluster( size_t i ) const
    {
        return clusters_.at( i );
    }

    std::vector<Cluster> const& clusters() const
    {
        return clusters_;
    }

    Merger const& merger( size_t i ) const
    {
        return mergers_.at( i );
    }

    std::vector<Merger> const& mergers() const
    {
        return mergers_;
    }

    // -------------------------------------------------------------------------
    //     Private Functions
    // -------------------------------------------------------------------------

private:

    void init_()
    {
        // Basic checks
        if( ! distance_function || ! merge_function ) {
            throw std::runtime_error( "Need to set distance and merge functions to run HAC" );
        }

        // Clear. Both its clusters and mergeres are empty.
        clusters_.clear();
        mergers_.clear();

        // Move all observations as single data points to the cluster list,
        // and compute the distances for the lower trinangle of entries.
        clusters_.resize( data_.size() );
        for( size_t i = 0; i < data_.size(); ++i ) {
            init_cluster_( i, std::move( data_[i] ));
        }
        data_.clear();
    }

    void init_cluster_( size_t index, T&& data )
    {
        assert( distance_function );
        assert( index < clusters_.size() );

        // Move the data into the cluster
        clusters_[index].data = std::move( data );

        // Fill the "lower triangle" of distances to all active clusters, i.e., all distances
        // to elements with lower indices than the current one. We don't store this in a global
        // distance matix, but in a vector for each cluster instead, as this makes it trivial to
        // keep track of the data when merging clusters. No need to keep track of which row belongs
        // to which cluster etc.
        // The cluster need i many distance entries, i.e., cluster 0 needs 0 entries,
        // cluster 1 needs 1 entry (to compare it to cluster 0), and so forth.
        // Then, calculate the distances, in parallel.
        clusters_[index].distances.resize( index, 0.0 );
        if( thread_pool_ ) {
            parallel_for(
                static_cast<size_t>( 0 ), index,
                [&]( size_t k ){
                    compute_distance_( index, k );
                },
                thread_pool_
            );
        } else {
            for( size_t k = 0; k < index; ++k ) {
                compute_distance_( index, k );
            }
        }

        // Set its activation status, and write the cluster out for the user.
        clusters_[index].active = true;
        if( keep_active_function && ! keep_active_function( clusters_[index].data )) {
            if( deactivate_function ) {
                deactivate_function( clusters_[index].data );
            }
            clusters_[index].active = false;
        }
        if( write_cluster_function ) {
            write_cluster_function( clusters_[index], index );
        }
    }

    void compute_distance_( size_t i, size_t k )
    {
        assert( k < i );
        assert( i < clusters_.size() );
        if( ! clusters_[k].active ) {
            return;
        }
        auto const dist = distance_function( clusters_[i].data, clusters_[k].data );
        if( !std::isfinite( dist ) || dist < 0.0 ) {
            throw std::runtime_error(
                "Invalid distance computation in hierarchical agglomerative clustering "
                "with distance that is negative or not finite."
            );
        }
        assert( std::isfinite( dist ) && dist >= 0.0 );
        assert( k < clusters_[i].distances.size() );
        clusters_[i].distances[k] = dist;
    }

    double distance_to_leaves_( size_t cluster_index )
    {
        assert( cluster_index < clusters_.size() );
        auto const& cluster = clusters_[cluster_index];

        // Leaves have a distance of 0 to leaves. Duh.
        if( cluster.merger_index == std::numeric_limits<size_t>::max() ) {
            return 0.0;
        }

        // In all other cases, this cluster is the result of merging two other clusters.
        // So, we get their distances recursively, and compre, to be sure that we are doing
        // the right thing.
        assert( cluster.merger_index < mergers_.size() );
        auto const& merger = mergers_[cluster.merger_index];
        auto const dist_a = merger.branch_length_a + distance_to_leaves_( merger.cluster_index_a );

        // This function is only called when we want ultrametric dendrograms,
        // in which case the distance to all leaves is the same, so we only need to recurse on
        // one of them (dist_a above), and do not need to repeat this for the other.
        // This brings down the cost for this function from exponential to linear.
        // Still, for debugging, we want to assert that all distances recursively are the same
        // (down to floating point precision) - which is expensive for large datasets!
        // Expensive assertion!
        assert( almost_equal_relative(
            dist_a, merger.branch_length_b + distance_to_leaves_( merger.cluster_index_b )
        ));
        // auto const dist_b = merger.branch_length_b + distance_to_leaves_( merger.cluster_index_b );
        // if( ! almost_equal_relative( dist_a, dist_b )) {
        //     throw std::runtime_error(
        //         "Inconsistent ultrametric branch lengths in sub-clusters with unequal lengths " +
        //         std::to_string( dist_a ) + " != " + std::to_string( dist_b )
        //     );
        // }
        return dist_a;
    }

    std::pair<size_t, size_t> min_distance_pair_() const
    {
        // Init.
        size_t min_i = 0;
        size_t min_j = 0;
        double min_d = std::numeric_limits<double>::max();

        // Find the pair of indices of clusters with the smallest distance
        for( size_t i = 0; i < clusters_.size(); ++i ) {
            if( ! clusters_[i].active ) {
                continue;
            }

            // We only need to check the "lower triangle"
            assert( clusters_[i].distances.size() == i );
            for( size_t j = 0; j < i; ++j ) {
                if( ! clusters_[j].active ) {
                    continue;
                }
                assert( j < clusters_[i].distances.size() );
                assert(
                    std::isfinite( clusters_[i].distances[j] ) &&
                    clusters_[i].distances[j] >= 0.0
                );
                if( clusters_[i].distances[j] < min_d ) {
                    min_i = i;
                    min_j = j;
                    min_d = clusters_[i].distances[j];
                }
            }
        }

        // This could be an assertion, as this should not really be able happen,
        // but it is a more sustantial check, so we include it is an exception here.
        if( !std::isfinite( min_d )) {
            assert( min_i == 0 && min_j == 0 );
            throw std::runtime_error( "Clustering with no more active clusters." );
        }

        // We return reverse order, so that i < j. This is just more intuitive to work with.
        assert( min_i > min_j );
        return { min_j, min_i };
    }

    void merge_clusters_( size_t i, size_t j )
    {
        assert( merge_function );
        assert( i < j );
        assert( i < clusters_.size() && j < clusters_.size() );
        assert( i < clusters_[j].distances.size() );

        // Make new cluster by merging the data from the two children.
        // This also calculates distances to still active clusters, including the two clusters
        // that we are about to merge. Those distances are then stored in the Merger object.
        clusters_.emplace_back();
        auto const parent_index = clusters_.size() - 1;
        init_cluster_( parent_index, merge_function( clusters_[i].data, clusters_[j].data ));
        auto& new_cluster = clusters_.back();
        new_cluster.merger_index = mergers_.size();

        // Make a new cluster merger.
        assert( i < clusters_[j].distances.size() );
        auto const inter_dist = clusters_[j].distances[i];
        if( ultrametric_branch_lenghts_ ) {
            // When we want ultrametricity, the distance between the two child clusters is meant
            // to represent the total path length between them (going across the new parent cluster).
            // Hence, each branch from the new parent to the two children gets half of that.
            // Then, we also need to adjust for the parts of the branch lengths that the two
            // child clusters already represent towards their children, and subtract those.
            auto const br_len_i = ( inter_dist / 2.0 ) - distance_to_leaves_( i );
            auto const br_len_j = ( inter_dist / 2.0 ) - distance_to_leaves_( j );
            if(
                !std::isfinite( br_len_i ) || br_len_i < 0.0 ||
                !std::isfinite( br_len_j ) || br_len_j < 0.0
            ) {
                throw std::runtime_error(
                    "Cannot compute ultrametric dendrogram for hierarchical agglomerative clustering "
                    "with negative branch lengths. This likely means that the distance function "
                    "is not ultrametric in the first place."
                );
            }
            mergers_.push_back({
                inter_dist, parent_index, i, j, br_len_i, br_len_j
            });
        } else {
            // For non-ultrametric trees, we use the distances computed between the new parent
            // and its two child clusters as the branch lengths of the merger.
            assert( i < new_cluster.distances.size() );
            assert( j < new_cluster.distances.size() );
            mergers_.push_back({
                inter_dist, parent_index, i, j, new_cluster.distances[i], new_cluster.distances[j]
            });
        }

        // Deactive the two clusters that are now merged.
        clusters_[i].active = false;
        clusters_[j].active = false;
        if( deactivate_function ) {
            deactivate_function( clusters_[i].data );
            deactivate_function( clusters_[j].data );
        }

        // We don't need the distances any more. Save memory.
        // clusters_[i].distances.clear();
        // clusters_[j].distances.clear();

        // We can also free the old cluster data, to save memory.
        // They haven been written before, and are not needed any more.
        // Deactivated for now, as we leave that to the merge function.
        // It might be that a user wants to keep the data around, so we do not delete it for them.
        // clusters_[i].data = T{};
        // clusters_[j].data = T{};
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool ultrametric_branch_lenghts_ = false;

    // We store the data intially, but then move all its elements to the clusters
    // once we start running, and append to the clusters and mergers
    // for every clustering step that we take.
    std::vector<T>       data_;
    std::vector<Cluster> clusters_;
    std::vector<Merger>  mergers_;

    // Keep the internally used thread pool
    std::shared_ptr<utils::ThreadPool> thread_pool_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
