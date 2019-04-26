/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @brief Implementation of CommonTree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/common_tree/distances.hpp"

#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/lca_lookup.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/containers/matrix/operators.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

utils::Matrix<double> node_branch_length_distance_matrix(
    Tree const& tree
) {
    utils::Matrix<double> result( tree.node_count(), tree.node_count(), 0.0 );

    // We need to keep track of the edges for which we run updates in the iterations below.
    // Init with root node, as it does not have a proximal edge, and hence is not going to be visited.
    std::vector<size_t> visited_indices;
    visited_indices.reserve( tree.node_count() );
    visited_indices.push_back( tree.root_node().index() );

    // Go through the tree, and use the preorder traversal to get inner distances towards the root
    // first, and later use them to calculate the outer ones, getting further and further away
    // from the root.
    for( auto it : preorder( tree )) {

        // We want to visit each edge once: it.edge() gives the edge going towards the root.
        // Hence, we skip the first iteration, which gives one of the edges of the root that will
        // be visited later again anyway, and we do not want to visit it twice.
        if( it.is_first_iteration() ) {
            continue;
        }

        // Get the node away from the root, its parent towards the root,
        // and the length of the current edge (the one between those two nodes).
        auto const node_id = it.node().index();
        auto const upper_id = it.node().primary_link().outer().node().index();
        auto const br_len = it.edge().data<CommonEdgeData>().branch_length;
        assert( node_id == it.edge().secondary_node().index() );
        assert( upper_id == it.edge().primary_node().index() );

        // Now set the length from the given node to all the ones that we have visited before
        // in the preorder. We already have their distances, and can use them to calculate the
        // distances for the given node. This is kind of like dynamic programming combined with
        // an upper triangle matrix calculation or inductive computation. Fancy fancy.
        for( auto const& cur_id : visited_indices ) {

            // We are visiting each node once. The current one, being part of the already visited
            // nodes, can hence not be the one of the outer preorder loop.
            assert( cur_id != node_id );

            // Get the distance between the node currently being updated and the parent node
            // of the node of the outer preorder loop.
            auto const upper_br_len = result( upper_id, cur_id );
            assert( upper_br_len == result( cur_id, upper_id ));

            // Set the branch length between the current node and the outer preorder node
            // as the sum of the parent and the current branch length.
            // That is, we use the distance of the parent node plus the distance from that parent
            // to the outer preorder node.
            assert( result( node_id, cur_id ) == 0.0 );
            assert( result( cur_id, node_id ) == 0.0 );
            result( node_id, cur_id ) = upper_br_len + br_len;
            result( cur_id, node_id ) = upper_br_len + br_len;
        }

        // Now add the preorder node to the already visited ones, so that it is updated in
        // the subsequent iterations.
        visited_indices.push_back( node_id );
    }

    return result;
}

std::vector<double> node_branch_length_distance_vector(
    Tree const& tree,
    TreeNode const* node
) {
    if (!node) {
        node = &tree.root_node();
    }

    // Store the distance from each node to the given node.
    auto vec = std::vector<double>(tree.node_count(), -1.0);
    vec[node->index()] = 0.0;

    // Calculate the distance vector via levelorder iteration.
    for( auto it : levelorder( *node ) ) {
        // Skip the starting node (it is already set to 0).
        if (it.is_first_iteration()) {
            continue;
        }

        // We do not have the distance of the current node, but the one of its "parent" (the one in
        // direction of the starting node)!
        assert(vec[it.node().index()] == -1.0);
        assert(vec[it.link().outer().node().index()] > -1.0);

        // The distance is the distance from the "parent" node (the next one in direction towards
        // the starting node) plus the branch length.
        vec[it.node().index()]
            = vec[it.link().outer().node().index()]
            + it.edge().data<CommonEdgeData>().branch_length;
    }

    return vec;
}

utils::Matrix<double> edge_branch_length_distance_matrix(
    Tree const& tree
) {
    // Result matrix that will be returned.
    utils::Matrix<double> mat (tree.edge_count(), tree.edge_count());

    // For calculating the distance between edges, we use the distances between nodes and for every
    // pair of edged find the nodes at the ends of the edges that are closest to each other. This
    // is then the shortest distance between the two edges.
    // There is probably a way to get this distance via some tree traversal, which would save us
    // some lookups and calculation of the min, but be more complex and error prone.
    // For now, this version should be fast enough.
    auto node_dist_mat = node_branch_length_distance_matrix(tree);

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto const& row_edge = tree.edge_at(i);

        for( auto const& col_edge : tree.edges() ) {

            // Set the diagonal element of the matrix. We don't need to compare nodes in this case,
            // and particularly want to skip the part where we add half the branch lengths to the
            // distance. After all, the distance between and element and itself should always be 0!
            if (row_edge.index() == col_edge.index()) {
                mat(row_edge.index(), row_edge.index()) = 0.0;
                continue;
            }

            // primary-primary case
            double pp = node_dist_mat(
                row_edge.primary_node().index(),
                col_edge.primary_node().index()
            );

            // primary-secondary case
            double ps = node_dist_mat(
                row_edge.primary_node().index(),
                col_edge.secondary_node().index()
            );

            // secondary-primary case
            double sp = node_dist_mat(
                row_edge.secondary_node().index(),
                col_edge.primary_node().index()
            );

            // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
            // (if this ever happens, the tree is broken).
            double dist = std::min(pp, std::min(ps, sp));
            assert(dist <= node_dist_mat(
                row_edge.secondary_node().index(),
                col_edge.secondary_node().index()
            ));

            // Store in matrix, with halves of the branch lengths.
            mat(row_edge.index(), col_edge.index())
                = (dist)
                + ( row_edge.data<CommonEdgeData>().branch_length / 2.0 )
                + ( col_edge.data<CommonEdgeData>().branch_length / 2.0 )
            ;
        }
    }

    return mat;
}

std::vector<double> edge_branch_length_distance_vector(
    Tree const& tree,
    TreeEdge const* edge
) {
    std::vector<double> vec (tree.edge_count());

    if( edge == nullptr ) {
        throw std::runtime_error( "Cannot use nullptr edge for distance calulcation." );
    }

    // Works similar to edge_branch_length_distance_matrix(). See there for a description of the implementation.

    // We just need two rows of the distance matrix - let's take the vectors instead for speed.
    auto p_node_dist = node_branch_length_distance_vector(tree, &edge->primary_node());
    auto s_node_dist = node_branch_length_distance_vector(tree, &edge->secondary_node());

    #pragma omp parallel for
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto const& col_edge = tree.edge_at(i);

        if (edge->index() == col_edge.index()) {
            vec[ edge->index() ] = 0.0;
            continue;
        }

        // primary-primary case
        double pp = p_node_dist[ col_edge.primary_node().index() ];

        // primary-secondary case
        double ps = p_node_dist[ col_edge.secondary_node().index() ];

        // secondary-primary case
        double sp = s_node_dist[ col_edge.primary_node().index() ];

        // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
        // (if this ever happens, the tree is broken).
        double dist = std::min(pp, std::min(ps, sp));
        assert(dist <= s_node_dist[ col_edge.secondary_node().index() ]);

        // Store in vector, with halves of the branch lengths.
        vec[ col_edge.index() ]
            = ( dist )
            + ( edge->data<CommonEdgeData>().branch_length / 2.0 )
            + ( col_edge.data<CommonEdgeData>().branch_length / 2.0 )
        ;
    }

    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

double deepest_distance(Tree const& tree)
{
    double max = 0.0;
    auto leaf_dist = closest_leaf_distance_vector(tree);

    for( auto const& e : tree.edges() ) {
        auto idx_p = e.primary_node().index();
        auto idx_s = e.secondary_node().index();

        double d = (leaf_dist[idx_p].second
                 + e.data<CommonEdgeData>().branch_length
                 + leaf_dist[ idx_s ].second) / 2.0;

        if (d > max) {
            max = d;
        }
    }
    return max;
}

/**
 * @brief Local helper function to calculate either closest_leaf_distance_vector() or
 * furthest_leaf_distance_vector().
 */
template< class Comparator >
std::vector<std::pair< TreeNode const*, double >> leaf_distance_vector(
    Tree const& tree,
    utils::Matrix<double> const& node_distances,
    Comparator  comp
) {
    // prepare a result vector with the size of number of nodes.
    std::vector<std::pair< TreeNode const*, double>> vec;
    vec.resize(tree.node_count(), {nullptr, 0.0});

    if( node_distances.rows() != tree.node_count() || node_distances.cols() != tree.node_count() ) {
        throw std::runtime_error( "Invalid node_branch_length_distance_matrix." );
    }

    // fill the vector for every node.
    // there is probably a faster way of doing this: preorder traversal with pruning. but for now,
    // this simple O(n^2) version works.
    for( auto const& node : tree.nodes() ) {

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert( vec[ node.index() ].first == nullptr );

        TreeNode const* res_node = nullptr;
        double res_dist = 0.0;

        // try out all other nodes, and find the closest leaf.
        for( auto const& other : tree.nodes() ) {

            if(! is_leaf( other )) {
                continue;
            }

            double dist = node_distances( node.index(), other.index() );
            if( res_node == nullptr || comp( dist, res_dist )) {
                res_node = &other;
                res_dist = dist;
            }
        }

        vec[ node.index() ].first  = res_node;
        vec[ node.index() ].second = res_dist;
    }

    return vec;
}

std::vector<std::pair< TreeNode const*, double >> closest_leaf_distance_vector(
    Tree const& tree
) {
    // we need the pairwise distances between all nodes, so we can do quick lookups.
    auto node_distances = node_branch_length_distance_matrix(tree);

    return leaf_distance_vector( tree, node_distances, std::less<double>() );
}

std::vector<std::pair< TreeNode const*, double>> closest_leaf_distance_vector(
    Tree const& tree,
    utils::Matrix<double> const& node_distances
) {
    return leaf_distance_vector( tree, node_distances, std::less<double>() );
}

std::vector<std::pair< TreeNode const*, double>> furthest_leaf_distance_vector(
    Tree const& tree
) {
    // we need the pairwise distances between all nodes, so we can do quick lookups.
    auto node_distances = node_branch_length_distance_matrix(tree);

    return leaf_distance_vector( tree, node_distances, std::greater<double>() );
}

std::vector<std::pair< TreeNode const*, double>> furthest_leaf_distance_vector(
    Tree const& tree,
    utils::Matrix<double> const& node_distances
) {
    return leaf_distance_vector( tree, node_distances, std::greater<double>() );
}

} // namespace tree
} // namespace genesis
