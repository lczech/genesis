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

#include "utils/core/logging.hpp"

#include <algorithm>
#include <assert.h>

namespace genesis {
namespace tree {

// =================================================================================================
//     Distance Related Propoerties
// =================================================================================================

template <class Tree>
double length(const Tree& tree)
{
    // TODO if we introduce some naming convention for with/withou branch length, the without
    // branch lengths version of this function will basically return edges.size()... is that needed?
    // TODO use an algorithm here
    double len = 0.0;
    for (const auto edge_it = tree.begin_edges(); edge_it != tree.end_edges(); ++edge_it) {
        len += edge_it->data.branch_length;
    }
    return len;
}

template <class Tree>
double height(const Tree& tree)
{
    auto dists = node_distance_vector(tree);
    return *std::max_element(dists.begin(), dists.end());
}

template <class Tree>
double depth(const Tree& tree)
{
    // TODO implement!
    LOG_WARN << "Not yet implemented.";
    (void) tree;
    return 0.0;
}

template <class Tree>
double deepest_distance(const Tree& tree)
{
    // TODO this should better be called diameter, return the double of this value. also, it can
    // be calculated in a different way: take the max of the node distance matrix. this should yield
    // the same value. this is also a good way of verifiying the result for testing!
    // TODO diameter method already exists as a method called heigth, see above!
    // TODO also, find a naming convention for weighted (with branch lengths) and unweighted diamter and
    // all other functions dealing with tree distances!

    double max = 0.0;

    auto leaf_dist = closest_leaf_distance_vector(tree);

    for (const auto& e = tree.begin_edges(); e != tree.end_edges(); ++e) {
        int idx_p = e->primary_node().index();
        int idx_s = e->secondary_node().index();
        double d = (leaf_dist[idx_p].second + e->data.branch_length + leaf_dist[idx_s].second) / 2;

        if (d > max) {
            max = d;
        }
    }
    return max;
}

// =================================================================================================
//     Branch Distance Measures
// =================================================================================================

template <class Tree>
utils::Matrix<double>      node_distance_matrix (
    const Tree& tree
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
                = it.edge().data.branch_length
                + mat(row_node->index(), it.link().outer().node().index());
        }
    }

    return mat;
}

template <class Tree>
std::vector<double> node_distance_vector (
    const Tree& tree,
    const typename Tree::NodeType* node
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
            + it.edge().data.branch_length;
    }

    return vec;
}

template <class Tree>
utils::Matrix<double>      edge_distance_matrix (
    const Tree& tree
) {
    // Result matrix that will be returned.
    utils::Matrix<double> mat (tree.edge_count(), tree.edge_count());

    // For calculating the distance between edges, we use the distances between nodes and for every
    // pair of edged find the nodes at the ends of the edges that are closest to each other. This
    // is then the shortest distance between the two edged.
    // There is probably a way to get this distance via some tree traversal, which would save us
    // some lookups and calculation of the min, but be more complex and error prone.
    // For now, this version should be fast enough.
    auto node_dist_mat = node_distance_matrix(tree);

    for (auto row_it = tree.begin_edges(); row_it != tree.end_edges(); ++row_it) {
        const auto row_edge = row_it->get();

        for (auto col_it = tree.begin_edges(); col_it != tree.end_edges(); ++col_it) {
            const auto col_edge = col_it->get();

            // Set the diagonal element of the matrix. We don't need to compare nodes in this case,
            // and particularly want to skip the part where we add half the branch lengths to the
            // distance. After all, the distance between and element and itself should always be 0!
            if (row_edge->index() == col_edge->index()) {
                mat(row_edge->index(), row_edge->index()) = 0.0;
                continue;
            }

            // primary-primary case
            double pp = node_dist_mat(
                row_edge->primary_node().index(),
                col_edge->primary_node().index()
            );

            // primary-secondary case
            double ps = node_dist_mat(
                row_edge->primary_node().index(),
                col_edge->secondary_node().index()
            );

            // secondary-primary case
            double sp = node_dist_mat(
                row_edge->secondary_node().index(),
                col_edge->primary_node().index()
            );

            // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
            // (if this ever happens, the tree is broken).
            double dist = std::min(pp, std::min(ps, sp));
            assert(dist <= node_dist_mat(
                row_edge->secondary_node().index(),
                col_edge->secondary_node().index()
            ));

            // Store in matrix, with halves of the branch lengths.
            mat(row_edge->index(), col_edge->index())
                = (dist)
                + (row_edge->data.branch_length / 2)
                + (col_edge->data.branch_length / 2)
            ;
        }
    }

    return mat;
}

template <class Tree>
std::vector<double> edge_distance_vector (
    const Tree& tree,
    const typename Tree::EdgeType* edge
) {
    std::vector<double> vec (tree.edge_count());

    // Works similar to edge_distance_matrix(). See there for a description of the implementation.

    // We just need two rows of the distance matrix - let's take the vectors instead for speed.
    auto p_node_dist = node_distance_vector(tree, edge->primary_node());
    auto s_node_dist = node_distance_vector(tree, edge->secondary_node());

    for (auto col_it = tree.begin_edges(); col_it != tree.end_edges(); ++col_it) {
        const auto col_edge = col_it->get();

        if (edge->index() == col_edge->index()) {
            vec(edge->index()) = 0.0;
            continue;
        }

        // primary-primary case
        double pp = p_node_dist(col_edge->primary_node().index());

        // primary-secondary case
        double ps = p_node_dist(col_edge->secondary_node().index());

        // secondary-primary case
        double sp = s_node_dist(col_edge->primary_node().index());

        // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
        // (if this ever happens, the tree is broken).
        double dist = std::min(pp, std::min(ps, sp));
        assert(dist <= s_node_dist(col_edge->secondary_node().index()));

        // Store in vector, with halves of the branch lengths.
        vec(col_edge->index())
            = (dist)
            + (edge->data.branch_length / 2)
            + (col_edge->data.branch_length / 2)
        ;
    }

    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

template <class Tree>
std::vector<std::pair<const typename Tree::NodeType*, double>> closest_leaf_distance_vector(
    const Tree& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector<std::pair<const typename Tree::NodeType*, double>> vec;
    vec.resize(tree.node_count(), {nullptr, 0.0});

    // we need the pairwise distances between all nodes, so we can do quick lookups.
    auto node_distances = node_distance_matrix(tree);

    // fill the vector for every node.
    // there is probably a faster way of doing this: preorder traversal with pruning. but for now,
    // this simple O(n^2) version works.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        typename Tree::NodeType* min_node = nullptr;
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
