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
 * @brief Implementation of Tree distance methods.
 *
 * @file
 * @ingroup tree
 */

#include "tree/function/distances.hpp"

#include "tree/function/operators.hpp"
#include "tree/tree.hpp"
#include "tree/iterator/levelorder.hpp"

#include "utils/core/logging.hpp"

#include <algorithm>
#include <assert.h>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Distance Measures
// =================================================================================================

/**
 * @brief Return a matrix containing the pairwise depth of all nodes of the tree.
 *
 * See @link node_path_length_vector( Tree const& tree, TreeNode const& node ) node_path_length_vector(...)@endlink
 * for more information.
 *
 * The vector is indexed using the node().index() for every node.
 */
utils::Matrix<size_t> node_path_length_matrix(
    Tree const& tree
) {
    auto max_val = std::numeric_limits<size_t>::max();
    utils::Matrix<size_t> mat( tree.node_count(), tree.node_count(), max_val );

    // Fill every row of the matrix.
    for( auto const& row_node : tree.nodes() ) {

        // Set the diagonal element of the matrix.
        mat( row_node->index(), row_node->index() ) = 0;

        // The columns are filled using a levelorder traversal. This makes sure that for every node
        // we know how to calculate the distance to the current row node.
        // Unfortunately, this prevents us from simply calculating the upper triangle of the matrix
        // and copying it (distance is symmetric), because we do not really know which nodes are in
        // which half during a levelorder traversal...
        for( auto it : levelorder( row_node->link() )) {
            // Skip the diagonal of the matrix.
            if (it.is_first_iteration()) {
                assert( it.node().index() == row_node->index() );
                continue;
            }

            // Make sure we don't have touched the current position, but have calculated
            // the needed dependency already.
            assert( mat(row_node->index(), it.node().index()) == max_val );
            assert( mat(row_node->index(), it.link().outer().node().index()) != max_val );

            // The distance to the current row node is one more than the distance from the other
            // end of that branch to the row node.
            mat( row_node->index(), it.node().index() )
                = 1 + mat(row_node->index(), it.link().outer().node().index());
        }
    }

    return mat;
}

/**
 * @brief Return a vector containing the depth of all nodes with respect to the given start node.
 *
 * The vector is indexed using the node().index() for every node. Its elements give the depth of
 * each node with respect to the given start node. The depth is the number of edges visited on the
 * path between two nodes (0 for itself, 1 for immediate neighbours, etc).
 */
std::vector<size_t> node_path_length_vector(
    Tree const& tree,
    TreeNode const& node
) {
    if( ! belongs_to( tree, node )) {
        throw std::runtime_error(
            "Cannot caluclate node_path_length_vector, as the given Node does not belong to the Tree."
        );
    }
    auto max_val = std::numeric_limits<size_t>::max();

    // store the distance from each node to the given node.
    std::vector<size_t> vec;
    vec.resize(tree.node_count(), max_val);
    vec[ node.index() ] = 0;

    // calculate the distance vector via levelorder iteration.
    for( auto it : levelorder( node ) ) {
        // skip the starting node (it is already set to 0).
        if (it.is_first_iteration()) {
            continue;
        }

        // we do not have the distance of the current node, but the one of its "parent" (the one in
        // direction of the starting node)!
        assert( vec[it.node().index()] == max_val );
        assert( vec[it.link().outer().node().index()] != max_val );

        // the distance is the distance from the "parent" node (the next one in direction towards
        // the given node) plus 1.
        vec[it.node().index()] = 1 + vec[it.link().outer().node().index()];
    }

    return vec;
}

/**
 * @brief Return a vector containing the depth of all nodes with respect to the root node.
 *
 * This function calls and returns the value of
 * @link node_path_length_vector( Tree const& tree, TreeNode const& node ) node_path_length_vector(...)@endlink
 * using the root node of the tree.
 */
std::vector<size_t> node_path_length_vector(
    Tree const& tree
) {
    return node_path_length_vector( tree, tree.root_node() );
}

utils::Matrix<size_t> edge_path_length_matrix(
    Tree const& tree
) {
    // Result matrix that will be returned.
    utils::Matrix<size_t> mat (tree.edge_count(), tree.edge_count());

    // For calculating the distance between edges, we use the distances between nodes and for every
    // pair of edged find the nodes at the ends of the edges that are closest to each other. This
    // is then the shortest distance between the two edges.
    auto node_depth_mat = node_path_length_matrix(tree);

    for( auto const& row_edge : tree.edges() ) {
        for( auto const& col_edge : tree.edges() ) {

            // Set the diagonal element of the matrix. We don't need to compare nodes in this case.
            if (row_edge->index() == col_edge->index()) {
                mat(row_edge->index(), row_edge->index()) = 0;
                continue;
            }

            // primary-primary case
            auto pp = node_depth_mat(
                row_edge->primary_node().index(),
                col_edge->primary_node().index()
            );

            // primary-secondary case
            auto ps = node_depth_mat(
                row_edge->primary_node().index(),
                col_edge->secondary_node().index()
            );

            // secondary-primary case
            auto sp = node_depth_mat(
                row_edge->secondary_node().index(),
                col_edge->primary_node().index()
            );

            // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
            // (if this ever happens, the tree is broken).
            auto dist = std::min(pp, std::min(ps, sp));
            assert( dist <= node_depth_mat(
                row_edge->secondary_node().index(),
                col_edge->secondary_node().index()
            ));

            // Store in matrix.
            mat( row_edge->index(), col_edge->index() ) = dist + 1;
        }
    }

    return mat;
}

std::vector<size_t> edge_path_length_vector(
    Tree const& tree,
    TreeEdge const& edge
) {
    if( ! belongs_to( tree, edge )) {
        throw std::runtime_error(
            "Cannot caluclate node_path_length_vector, as the given Edge does not belong to the Tree."
        );
    }

    auto max_val = std::numeric_limits<size_t>::max();
    std::vector<size_t> vec( tree.edge_count(), max_val );

    // We just need two rows of the distance matrix - let's take the vectors instead for speed.
    auto p_node_dist = node_path_length_vector(tree, edge.primary_node());
    auto s_node_dist = node_path_length_vector(tree, edge.secondary_node());

    for( auto const& col_edge : tree.edges() ) {

        if( edge.index() == col_edge->index() ) {
            vec[ edge.index() ] = 0;
            continue;
        }

        // primary-primary case
        double pp = p_node_dist[ col_edge->primary_node().index() ];

        // primary-secondary case
        double ps = p_node_dist[ col_edge->secondary_node().index() ];

        // secondary-primary case
        double sp = s_node_dist[ col_edge->primary_node().index() ];

        // Find min. Make sure that the fourth case "secondary-secondary" is not shorter
        // (if this ever happens, the tree is broken).
        double dist = std::min(pp, std::min(ps, sp));
        assert(dist <= s_node_dist[ col_edge->secondary_node().index() ]);

        // Store in vector.
        vec[ col_edge->index() ] = dist + 1;
    }

    return vec;
}

// =================================================================================================
//     Complex Distance Methods
// =================================================================================================

/**
 * @brief Returns a vector containing the closest leaf node for each node, measured in number of
 * edges between them and its depth (number of edges between them).
 *
 * The vector is indexed using the node().index() for every node. Its value contains an std::pair,
 * where the first element is a NodeType* to the closest leaf node (with respect to its depth) and
 * the second element its depth with respect to the node at the given index of the vector. The depth
 * is the number of edges visited on the path between two nodes (0 for itself, 1 for immediate
 * neighbours, etc).
 *
 * Thus, leaf nodes will have a pointer to themselves and a depth value of 0, and for all other
 * nodes the depth will be the number of edges between it and the closest leaf node.
 *
 * There might be more than one leaf with the same depth to a given node. In this case, an
 * arbitrary one is used.
 */
std::vector< std::pair< TreeNode const*, size_t >> closest_leaf_depth_vector (
    const Tree& tree
) {
    // prepare a result vector with the size of number of nodes.
    std::vector< std::pair< TreeNode const*, size_t >> vec;
    vec.resize(tree.node_count(), {nullptr, 0});

    // fill the vector for every node.
    // this could be speed up by doing a postorder traversal followed by some sort of inside-out
    // traversal (preorder might do the job). but for now, this simple O(n^2) version works, too.
    for (auto node_it = tree.begin_nodes(); node_it != tree.end_nodes(); ++node_it) {
        auto node = node_it->get();

        // we have not visited this node. assertion holds as long as the indices are correct.
        assert(vec[node->index()].first == nullptr);

        // look for closest leaf node by doing a levelorder traversal.
        for( auto it : levelorder( *node ) ) {
            // if we find a leaf, leave the loop.
            if (it.node().is_leaf()) {
                vec[node->index()].first  = &it.node();
                vec[node->index()].second =  it.depth();
                break;
            }
        }
    }

    return vec;
}

} // namespace tree
} // namespace genesis
