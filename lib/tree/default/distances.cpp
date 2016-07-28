/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Implementation of Default Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include "tree/default/distances.hpp"

#include "tree/tree.hpp"
#include "tree/iterator/levelorder.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Distance Related Propoerties
// =================================================================================================

/**
 * @brief Get the length of the tree, i.e., the sum of all branch lengths.
 */
double length(Tree const& tree)
{
    double len = 0.0;
    for( auto const& edge : tree.edges() ) {
        len += edge->data<DefaultEdgeData>().branch_length;
    }
    return len;
}

/**
 * @brief Get the height of the tree, i.e., the longest distance from the root to a leaf,
 * measured using the branch_length.
 */
double height(Tree const& tree)
{
    auto dists = node_branch_length_distance_vector(tree);
    return *std::max_element(dists.begin(), dists.end());
}

/**
 * @brief Get the diameter of the tree, i.e., the longest distance between any two nodes,
 * measured using the branch_length.
 */
double diameter( Tree const& tree )
{
    auto dist_mat = node_branch_length_distance_matrix( tree );
    return *std::max_element( dist_mat.begin(), dist_mat.end() );
}

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

/**
 * @brief Return a distance matrix containing pairwise distances between all Nodes, using the
 * branch_length of the Edges as distance measurement.
 *
 * The elements of the matrix are indexed using node().index().
 */
utils::Matrix<double> node_branch_length_distance_matrix(
    Tree const& tree
) {
    utils::Matrix<double> mat (tree.node_count(), tree.node_count(), -1.0);

    // fill every row of the matrix
    for (auto row_it = tree.begin_nodes(); row_it != tree.end_nodes(); ++row_it) {
        const auto row_node = row_it->get();

        // set the diagonal element of the matrix.
        mat(row_node->index(), row_node->index()) = 0.0;

        // the columns are filled using a levelorder traversal. this makes sure that for every node
        // we know how to calculate the distance to the current row node.
        // unfortunately, this prevents us from simply calculating the upper triangle of the matrix
        // and copying it (distance is symmetric), because we do not really know which nodes are in
        // which half during a levelorder traversal...
        for( auto it : levelorder( row_node->link() )) {
            // skip the diagonal of the matrix.
            if (it.is_first_iteration()) {
                continue;
            }

            // make sure we don't have touched the current position, but have calculated
            // the needed dependency already.
            assert(mat(row_node->index(), it.node().index()) == -1.0);
            assert(mat(row_node->index(), it.link().outer().node().index()) > -1.0);

            // the distance to the current row node is: the length of the current branch plus
            // the distance from the other end of that branch to the row node.
            mat(row_node->index(), it.node().index())
                = it.edge().data<DefaultEdgeData>().branch_length
                + mat(row_node->index(), it.link().outer().node().index());
        }
    }

    return mat;
}

/**
 * @brief Return a vector containing the distance of all nodes with respect to the given start node,
 * where distance is measured in the sum of branch lengths between the nodes.
 *
 * The vector is indexed using the node().index() for every node. Its elements give the distance of
 * each node with respect to the given start node. The distance is the sum of branch lengths of the
 * edges visited on the path between the two nodes.
 *
 * If no Node pointer is provided, the root is taken as node.
 */
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
            + it.edge().data<DefaultEdgeData>().branch_length;
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

    for (auto row_it = tree.begin_edges(); row_it != tree.end_edges(); ++row_it) {
        auto const& row_edge = *row_it->get();

        for (auto col_it = tree.begin_edges(); col_it != tree.end_edges(); ++col_it) {
            auto const& col_edge = *col_it->get();

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
                + ( row_edge.data<DefaultEdgeData>().branch_length / 2 )
                + ( col_edge.data<DefaultEdgeData>().branch_length / 2 )
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

    for (auto col_it = tree.begin_edges(); col_it != tree.end_edges(); ++col_it) {
        auto const& col_edge = *col_it->get();

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
            + ( edge->data<DefaultEdgeData>().branch_length / 2 )
            + ( col_edge.data<DefaultEdgeData>().branch_length / 2 )
        ;
    }

    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Return the longest distance from any point in the tree (on the edges) to any leaf.
 */
double deepest_distance(Tree const& tree)
{
    double max = 0.0;
    auto leaf_dist = closest_leaf_distance_vector(tree);

    for( auto const& e : tree.edges() ) {
        int idx_p = e->primary_node().index();
        int idx_s = e->secondary_node().index();

        double d = (leaf_dist[idx_p].second
                 + e->data<DefaultEdgeData>().branch_length
                 + leaf_dist[ idx_s ].second) / 2;

        if (d > max) {
            max = d;
        }
    }
    return max;
}

/**
 * @brief Return a vector containing the closest leaf node for each node, using the branch_length
 * as distance measure.
 *
 * The vector is indexed using the node().index() for every node. Its value contains an std::pair,
 * where the first element is a NodeType* to the closest leaf node of the node at the index,
 * measured using the branch_length; the second element of the pair is the distance value itself.
 * Thus, leaf nodes will have a pointer to themselves and a distance value of 0.
 */
std::vector<std::pair< TreeNode const*, double >> closest_leaf_distance_vector(
    Tree const& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector<std::pair< TreeNode const*, double>> vec;
    vec.resize(tree.node_count(), {nullptr, 0.0});

    // we need the pairwise distances between all nodes, so we can do quick lookups.
    auto node_distances = node_branch_length_distance_matrix(tree);

    // fill the vector for every node.
    // there is probably a faster way of doing this: preorder traversal with pruning. but for now,
    // this simple O(n^2) version works.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        TreeNode const* min_node = nullptr;
        double min_dist = 0.0;

        // try out all other nodes, and find the closest leaf.
        for (auto other_it = tree.begin_nodes(); other_it != tree.end_nodes(); ++other_it) {
            auto other = other_it->get();

            if (!other->is_leaf()) {
                continue;
            }

            double dist = node_distances(node->index(), other->index());
            if (min_node == nullptr || dist < min_dist) {
                min_node = other;
                min_dist = dist;
            }
        }

        vec[node->index()].first  = min_node;
        vec[node->index()].second = min_dist;
    }

    return vec;
}

} // namespace tree
} // namespace genesis
