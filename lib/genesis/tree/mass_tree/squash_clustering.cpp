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

#include "genesis/tree/mass_tree/squash_clustering.hpp"

#include "genesis/tree/mass_tree/emd.hpp"
#include "genesis/tree/mass_tree/functions.hpp"

#include "genesis/utils/core/logging.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Squash Clustering
// =================================================================================================

SquashClustering squash_clustering_init( std::vector<MassTree>&& trees, double const p  )
{
    // Create empty result object. Both its clusters and mergeres are empty.
    SquashClustering sc;

    // Move all trees as single data points to the cluster list, and make them active.
    sc.clusters.resize( trees.size() );
    for( size_t i = 0; i < trees.size(); ++i ) {
        sc.clusters[i].tree   = std::move( trees[i] );
        sc.clusters[i].count  = 1;
        sc.clusters[i].active = true;
    }

    // Fill the "lower triangle" of distances, i.e., all distances to elements with lower indices
    // than the current one. We don't store this in a global distance matix, but in a vector
    // for each cluster instead, as this makes it trivial to keep track of the data when merging
    // clusters. No need to keep track of which row belongs to which cluster etc.
    // We do this in a second loop, so that all trees haven been moved and OpenMP can access them.
    for( size_t i = 0; i < sc.clusters.size(); ++i ) {

        // The cluster need i many distance entries, i.e., cluster 0 needs 0 entries,
        // cluster 1 needs 1 entry (to compare it to cluster 0), and so forth.
        sc.clusters[i].distances.resize( i );

        // Calculate the distances.
        #pragma omp parallel for
        for( size_t k = 0; k < i; ++k ) {
            auto const dist = earth_movers_distance( sc.clusters[i].tree, sc.clusters[k].tree, p );
            sc.clusters[i].distances[k] = dist;
        }
    }

    return sc;
}

std::pair<size_t, size_t> squash_clustering_min_entry( SquashClustering const& sc )
{
    // Init.
    size_t min_i = 0;
    size_t min_j = 0;
    double min_d = std::numeric_limits<double>::max();

    // Find min cell.
    #pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < sc.clusters.size(); ++i ) {
        if( ! sc.clusters[i].active ) {
            continue;
        }

        // We only need to check the "lower triangle".
        assert( sc.clusters[i].distances.size() == i );
        for( size_t j = 0; j < i; ++j ) {
            if( ! sc.clusters[j].active ) {
                continue;
            }

            // Comparison.
            #pragma omp flush( min_d )
            if( sc.clusters[i].distances[j] < min_d ) {
                #pragma omp critical( GENESIS_TREE_MASS_TREE_SQUASH_CLUSTERING_MIN_ENTRY_UPDATE )
                {
                    if( sc.clusters[i].distances[j] < min_d ) {
                        min_i = i;
                        min_j = j;
                        min_d = sc.clusters[i].distances[j];
                    }
                }
            }
        }
    }

    // We return reverse order, so that i < j. This is just more intuitive to work with.
    assert( min_i > min_j );
    return { min_j, min_i };
}

void squash_clustering_merge_clusters( SquashClustering& sc, size_t i, size_t j, double const p  )
{
    assert( i < j );
    assert( i < sc.clusters.size() && j < sc.clusters.size() );
    assert( i < sc.clusters[j].distances.size() );

    // Make new cluster.
    sc.clusters.emplace_back();
    auto& new_cluster = sc.clusters.back();

    // Make a new cluster tree as the weighted average of both given trees.
    auto const weight_i = static_cast<double>( sc.clusters[i].count );
    auto const weight_j = static_cast<double>( sc.clusters[j].count );
    new_cluster.tree = mass_tree_merge_trees(
        sc.clusters[i].tree, sc.clusters[j].tree, weight_i, weight_j
    );
    mass_tree_normalize_masses( new_cluster.tree );

    // Old way: Scale masses, merge them, scale back.
    // Scale both trees according to their weight (= count).
    // mass_tree_scale_masses( sc.clusters[i].tree, static_cast<double>( sc.clusters[i].count ));
    // mass_tree_scale_masses( sc.clusters[j].tree, static_cast<double>( sc.clusters[j].count ));
    // new_cluster.tree = mass_tree_merge_trees( sc.clusters[i].tree, sc.clusters[j].tree );
    //
    // mass_tree_normalize_masses( new_cluster.tree );
    // mass_tree_scale_masses( sc.clusters[i].tree, 1.0 / static_cast<double>( sc.clusters[i].count ));
    // mass_tree_scale_masses( sc.clusters[j].tree, 1.0 / static_cast<double>( sc.clusters[j].count ));

    // Set other properties of the new cluster.
    new_cluster.count  = sc.clusters[i].count + sc.clusters[j].count;
    new_cluster.active = true;
    new_cluster.distances.resize( sc.clusters.size() - 1, 0.0 );

    // Calculate distances to still active clusters, which also includes the two clusters that
    // we are about to merge. We will deactivate them after the loop. This way, we also compute
    // their distances in parallel, maximizing OpenMP throughput!
    #pragma omp parallel for schedule(dynamic)
    for( size_t k = 0; k < sc.clusters.size() - 1; ++k ) {
        if( ! sc.clusters[k].active ) {
            continue;
        }

        auto const dist = earth_movers_distance( new_cluster.tree, sc.clusters[k].tree, p );
        new_cluster.distances[k] = dist;
    }

    // Get the distance between the two clusters that we want to merge,
    // and make a new cluster merger.
    sc.mergers.push_back({ i, new_cluster.distances[i], j, new_cluster.distances[j] });

    // Test. How much does the avg dist differ from proper emd dist?
    // We need to access the distance in reverse j i order, because of the lower triangle.
    // auto const inner_dist = sc.clusters[j].distances[i];
    // auto const avg_dist_i = inner_dist / static_cast<double>( sc.clusters[i].count );
    // auto const avg_dist_j = inner_dist / static_cast<double>( sc.clusters[j].count );

    // Deactive. Those two clusters are now merged.
    sc.clusters[i].active = false;
    sc.clusters[j].active = false;

    // We don't need the distances any more. Save mem.
    sc.clusters[i].distances.clear();
    sc.clusters[j].distances.clear();

    // We can also destroy those trees. For now. Maybe later, we want to write them first.
    sc.clusters[i].tree.clear();
    sc.clusters[j].tree.clear();
}

SquashClustering squash_clustering( std::vector<MassTree>&& trees, double const p  )
{
    LOG_INFO << "Squash Clustering: Initialize";
    SquashClustering sc = squash_clustering_init( std::move( trees ), p );

    // Do a full clustering, until only one is left.
    for( size_t i = 0; i < trees.size() - 1; ++i ) {
        LOG_INFO << "Squash Clustering: Step " << (i+1) << " of " << trees.size() - 1;

        auto const min_pair = squash_clustering_min_entry( sc );
        assert( min_pair.first < min_pair.second );

        squash_clustering_merge_clusters( sc, min_pair.first, min_pair.second, p );
    }

    // At the end, we only have one big cluster node.
    assert( 1 == [&](){
        size_t count_active = 0;
        for( auto const& c : sc.clusters ) {
            if( c.active ) {
                ++count_active;
            }
        }
        return count_active;
    }() );

    return sc;
}

std::string squash_cluster_tree( SquashClustering const& sc, std::vector<std::string> const& labels )
{
    auto list = labels;
    for( size_t i = 0; i < sc.mergers.size(); ++i ) {
        auto const& cm = sc.mergers[i];

        auto node_a = list[ cm.index_a ] + ":" + std::to_string( cm.distance_a );
        auto node_b = list[ cm.index_b ] + ":" + std::to_string( cm.distance_b );

        list[ cm.index_a ].clear();
        list[ cm.index_b ].clear();

        list.push_back( "(" + node_a + "," + node_b + ")" + std::to_string( i + labels.size() ) );
    }

    return list.back() + ";";
}

} // namespace tree
} // namespace genesis
