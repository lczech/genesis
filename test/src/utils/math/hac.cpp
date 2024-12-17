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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/printer/compact.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/hac.hpp"
#include "genesis/utils/math/hac/functions.hpp"
#include "genesis/utils/math/hac/linkage.hpp"
#include "genesis/utils/text/string.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>

using namespace genesis;
using namespace tree;
using namespace utils;

template<typename T>
void print_hac_(
    HierarchicalAgglomerativeClustering<T> const& clust,
    std::vector<std::string> const& labels
) {
    // Turn the dendrogram into our tree visualization
    auto const dend = hac_dendrogram( clust, labels );
    LOG_MSG << dend;
    auto const lines = split( hac_dendrogram( clust, labels ), '\n' );
    for( auto const& line : lines ) {
        auto const tree = CommonTreeNewickReader().read( from_string( line ));
        LOG_MSG << PrinterCompact().print( tree );
    }
}

TEST( HierarchicalAgglomerativeClustering, DistanceMatrix )
{
    // Set up a distance matrix, exmple from https://en.wikipedia.org/wiki/UPGMA
    auto const labels = std::vector<std::string>{ "a", "b", "c", "d", "e" };
    auto const distances = Matrix<double>(
        5, 5, {
            0,  17, 21, 31, 23,
            17,  0, 30, 34, 21,
            21, 30,  0, 28, 39,
            31, 34, 28,  0, 43,
            23, 21, 39, 43,  0
        }
    );

    // Debugging output, if needed.
    // clust.report_step_begin_function = []( size_t iteration, size_t total_iterations )
    // {
    //     LOG_MSG << "At " << iteration << " of " << total_iterations;
    // };
    // clust.report_step_end_function = [](
    //     size_t iteration, size_t total_iterations,
    //     decltype(clust)::Cluster const& cluster, decltype(clust)::Merger const& merger
    // ) {
    //     LOG_MSG << "Done " << iteration << " of " << total_iterations << " with cluster indices "
    //             << join( cluster.data ) << " of cluster indices "
    //             << merger.cluster_index_a << " dist(" << merger.branch_length_a << ")" << " and "
    //             << merger.cluster_index_b << " dist(" << merger.branch_length_b << ")" << " and cluster dist "
    //             << merger.distance
    //     ;
    // };

    // Single linkage, https://en.wikipedia.org/wiki/Single-linkage_clustering
    {
        auto clust = hac_with_distance_matrix(
            distances,
            HierarchicalAgglomerativeClusteringLinkage::kSingleLinkage
        );
        clust.ultrametric_branch_lenghts( true );
        clust.run();
        // print_hac_( clust, labels );
        EXPECT_EQ(
            "(d:14,(e:10.5,(c:10.5,(a:8.5,b:8.5)5:2)6:0)7:3.5)8;",
            hac_dendrogram( clust, labels )
        );
    }

    // Complete linkage, https://en.wikipedia.org/wiki/Complete-linkage_clustering
    {
        auto clust = hac_with_distance_matrix(
            distances,
            HierarchicalAgglomerativeClusteringLinkage::kCompleteLinkage
        );
        clust.ultrametric_branch_lenghts( true );
        clust.run();
        // print_hac_( clust, labels );
        EXPECT_EQ(
            "((e:11.5,(a:8.5,b:8.5)5:3)6:10,(c:14,d:14)7:7.5)8;",
            hac_dendrogram( clust, labels )
        );
    }

    // Unweighted Average linkage, https://en.wikipedia.org/wiki/UPGMA
    {
        auto clust = hac_with_distance_matrix(
            distances,
            HierarchicalAgglomerativeClusteringLinkage::kUnweightedAverageLinkage
        );
        clust.ultrametric_branch_lenghts( true );
        clust.run();
        // print_hac_( clust, labels );
        EXPECT_EQ(
            "((e:11,(a:8.5,b:8.5)5:2.5)6:5.5,(c:14,d:14)7:2.5)8;",
            hac_dendrogram( clust, labels )
        );
    }

    // LOG_MSG << hac_dendrogram( clust, labels, false );
}

TEST( HierarchicalAgglomerativeClustering, EuclideanDistance )
{
    // Set up an example using euclidean distance,
    // inspired by https://en.wikipedia.org/wiki/Hierarchical_clustering

    // Define some points in the plane.
    using Point = std::pair<int, int>;
    auto const labels = std::vector<std::string>{ "a", "b", "c", "d", "e", "f" };
    auto const points = std::vector<Point>{
        {2, 5}, {1, 2}, {1, 1}, {3, 2}, {3, 1}, {4, 1}
    };

    // Euclidean distance
    auto euc_distance = []( Point const& a, Point const& b )
    {
        return std::sqrt( squared( a.first - b.first ) + squared( a.second - b.second ));
    };

    // Expected newick string, using numbers for merged clusters.
    // We do not use branch lengths here, as the example has rather quirky numbers,
    // which might hence lead to mismatches due to differing precision of the output printing.
    std::string const exp_newick = "(a,((b,c)6,(f,(d,e)7)8)9)10;";

    // Single Linkage
    {
        auto clust = hac_with_linkage<Point>(
            std::vector<Point>( points ), euc_distance,
            HierarchicalAgglomerativeClusteringLinkage::kSingleLinkage
        );
        clust.ultrametric_branch_lenghts( true );
        clust.run();
        // print_hac_( clust, labels );
        EXPECT_EQ( exp_newick, hac_dendrogram( clust, labels, false ));
    }

    // Complete Linkage
    {
        auto clust = hac_with_linkage<Point>(
            std::vector<Point>( points ), euc_distance,
            HierarchicalAgglomerativeClusteringLinkage::kCompleteLinkage
        );
        clust.ultrametric_branch_lenghts( true );
        clust.run();
        // print_hac_( clust, labels );
        EXPECT_EQ( exp_newick, hac_dendrogram( clust, labels, false ));
    }

    // Unweighted Average Linkage
    {
        auto clust = hac_with_linkage<Point>(
            std::vector<Point>( points ), euc_distance,
            HierarchicalAgglomerativeClusteringLinkage::kUnweightedAverageLinkage
        );
        clust.ultrametric_branch_lenghts( true );
        clust.run();
        // print_hac_( clust, labels );
        EXPECT_EQ( exp_newick, hac_dendrogram( clust, labels, false ));
    }
}

TEST( HierarchicalAgglomerativeClustering, Bitvectors )
{
    std::srand(std::time(nullptr));

    // LOG_TIME << "init";
    // Make some (more or less) random bitvectors.
    // We create them such that we have groups of 5 that are similar to each other.
    // auto const num_elems = 10;
    // auto const bv_len = 8000000000;
    auto const num_elems = 25;
    auto const bv_len = 1000;
    auto bvs = std::vector<Bitvector>( num_elems );
    std::vector<std::string> labels;
    for( size_t i = 0; i < num_elems; ++i ) {
        bvs[i] = Bitvector( bv_len );
        if( i % 5 == 0 ) {
            for( size_t p = 0; p < 1000; ++p ) {
                bvs[i].flip( std::rand() % bv_len );
            }
        } else {
            bvs[i] = bvs[i - (i % 5)];
            bvs[i].flip( std::rand() % bv_len );
        }
        labels.push_back( std::to_string( i ));
    }

    // Make a clustering where each element is a bitvector, such that a new cluster is formed
    // as the union of the two bitvectors being merged. This differes from the above (more typical)
    // usage cases, where the clusters simply are collections of elements. Here, instead, each
    // cluster is only ever represented by a single element (one bitvector), that contains the
    // "merged" bits of its children.
    // LOG_TIME << "clust";
    // auto clustering = HierarchicalAgglomerativeClustering<Bitvector>(
    //     std::make_shared<ThreadPool>( 12 )
    // );
    auto clustering = HierarchicalAgglomerativeClustering<Bitvector>();
    clustering.set_data( std::move( bvs ));
    clustering.distance_function = &jaccard_distance;
    clustering.merge_function = []( Bitvector const& a, Bitvector const& b )
    {
        return a | b;
    };
    clustering.deactivate_function = []( Bitvector& data )
    {
        // Free the memory, as we do not need the data any more here.
        data = Bitvector();
    };
    // clustering.report_step_begin_function = []( size_t iteration, size_t total_iterations )
    // {
    //     LOG_TIME << "At " << iteration << " of " << total_iterations;
    // };

    // Run the clustering
    // LOG_TIME << "run";
    clustering.run();
    // print_hac_( clustering, labels );
    // LOG_TIME << "done";

    // With random data, it's hard to test anything - mostly, this test case is meant to ensure
    // that we can compile with more complex distance functions. For a more in-depth test, see below.
    EXPECT_EQ( 2 * num_elems - 1, clustering.clusters().size() );
    EXPECT_EQ(     num_elems - 1, clustering.mergers().size() );
}

TEST( HierarchicalAgglomerativeClustering, BitvectorsLimited )
{
    std::srand(std::time(nullptr));

    // Make some (more or less) random bitvectors.
    // We create them such that we have groups of 5 that are similar to each other.
    // Here, we furthermore keep track of how many elements there are in each cluster.
    using T = std::pair<Bitvector, size_t>;
    auto const num_elems = 25;
    auto const bv_len = 1000;
    auto bvs = std::vector<T>( num_elems );
    std::vector<std::string> labels;
    for( size_t i = 0; i < num_elems; ++i ) {
        if( i % 5 == 0 ) {
            // Every firth bv is completely randomized.
            bvs[i].first = Bitvector( bv_len );
            for( size_t p = 0; p < bv_len; ++p ) {
                bvs[i].first.flip( std::rand() % bv_len );
            }
        } else {
            // All other (4 out of 5) have a single bit difference
            // to the preceeding one.
            EXPECT_EQ( bv_len, bvs[i - (i % 5)].first.size() );
            bvs[i].first = bvs[i - (i % 5)].first;
            bvs[i].first.flip( std::rand() % bv_len );
        }
        bvs[i].second = 1;

        // Give the five different clusters names from A0..A4 .. E0..E4
        labels.push_back( std::string( 1, 'A' + i/5 ) + std::to_string( i%5 ));
    }

    // See above fore the general setup of the clustering. Here, we impose the additional constraint
    // that clusters can have at most 5 elements, and deactivate a cluster for further consideration
    // once it reaches that number of elements.
    auto clustering = HierarchicalAgglomerativeClustering<T>();
    clustering.set_data( std::move( bvs ));
    clustering.distance_function = []( T const& a, T const& b )
    {
        return jaccard_distance( a.first, b.first );
    };
    clustering.merge_function = []( T const& a, T const& b )
    {
        return T{ ( a.first | b.first ), ( a.second + b.second ) };
    };
    clustering.deactivate_function = []( T& data )
    {
        data.first = Bitvector();
    };
    clustering.keep_active_function = []( T const& data )
    {
        return data.second < 5;
    };
    // clustering.report_step_begin_function = []( size_t iteration, size_t total_iterations )
    // {
    //     LOG_MSG << "At " << iteration << " of " << total_iterations;
    // };

    // Run the clustering
    clustering.run();
    // print_hac_( clustering, labels );

    // We created 5 clusters, and so the dendrogram should contain 5 lines in newick format.
    EXPECT_EQ( 5, split( hac_dendrogram( clustering, labels ), '\n' ).size() );

    // Similar to above, we just test the number of clusters and mergers created.
    // That is the normal number (49 and 24), but reduced by 4 each, as instead of 1,
    // we here created 5 clusters (whose difference is 4).
    EXPECT_EQ( 45, clustering.clusters().size() );
    EXPECT_EQ( 20, clustering.mergers().size() );

    // We do test though that each cluster only contains the label names that we expect.
    // That is, one cluster only as A0..A4 labels, one only has B0..B4 labels, etc.
    auto const trees = split( hac_dendrogram( clustering, labels ), '\n' );
    for( auto const& tree : trees ) {
        size_t total_cnt = 0;
        for( int c = 'A'; c <= 'E'; ++c ) {
            auto const char_cnt = count_substring_occurrences( tree, std::string( 1, c ));

            // The tree can only contain 5 occurrences of a given character, or none.
            EXPECT_TRUE( 0 == char_cnt || 5 == char_cnt );
            total_cnt += char_cnt;
        }

        // In total, each tree has exactly 5 characters from the label names.
        EXPECT_EQ( 5, total_cnt );
    }
}
